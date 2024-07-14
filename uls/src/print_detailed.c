#include "uls_utils.h"

static void print_permissions(mode_t mode) {
    mx_printchar((mode & S_IRUSR) ? 'r' : '-');
    mx_printchar((mode & S_IWUSR) ? 'w' : '-');
    if (mode & S_IXUSR)
        mx_printchar((mode & S_ISUID) ? 's' : 'x');
    else
        mx_printchar((mode & S_ISUID) ? 'S' : '-');
    mx_printchar((mode & S_IRGRP) ? 'r' : '-');
    mx_printchar((mode & S_IWGRP) ? 'w' : '-');
    if (mode & S_IXGRP)
        mx_printchar((mode & S_ISGID) ? 's' : 'x');
    else
        mx_printchar((mode & S_ISGID) ? 'S' : '-');
    mx_printchar((mode & S_IROTH) ? 'r' : '-');
    mx_printchar((mode & S_IWOTH) ? 'w' : '-');
    if (mode & S_IXOTH)
        mx_printchar((mode & S_ISTXT) ? 't' : 'x');
    else
        mx_printchar((mode & S_ISTXT) ? 'T' : '-');
}

static void print_filetype(mode_t mode) {
    switch (mode & S_IFMT) {
        case S_IFBLK:
            mx_printchar('b');
            break;
        case S_IFCHR:
            mx_printchar('c');
            break;
        case S_IFDIR:
            mx_printchar('d');
            break;
        case S_IFIFO:
            mx_printchar('p');
            break;
        case S_IFLNK:
            mx_printchar('l');
            break;
        case S_IFSOCK:
            mx_printchar('s');
            break;
        default:
            mx_printchar('-');
            break;
    }
}

static void print_aligned(char *str, int width, bool align_right) {
    if (!align_right)
        mx_printstr(str);
    for (int i = 0; i < width - mx_strlen(str); i++)
        mx_printchar(' ');
    if (align_right)
        mx_printstr(str);
}

static void print_number_aligned(long long number, int width) {
    char *str_number = mx_lltoa(number);
    print_aligned(str_number, width, true);
    free(str_number);
}

static void print_date_and_time(time_t ptime, bool full_time_info) {
    char *time_str = ctime(&ptime);
    char **array = mx_strsplit(time_str, ' ');
    time_t now = time(NULL);
    time_t six_months_sec = (365 / 2) * 24 * 60 * 60;

    array[4][4] = '\0';
    if (full_time_info) {
        for (int i = 1; array[i] != NULL; i++) {
            print_aligned(array[i], 2, true);
            if (array[i + 1] != NULL)
                mx_printchar(' ');
        }
    } else if (ptime + six_months_sec <= now || ptime >= now + six_months_sec) {
        mx_printstr(array[1]);
        mx_printchar(' ');
        print_aligned(array[2], 2, true);
        mx_printstr("  ");
        mx_printstr(array[4]);
    } else {
        mx_printstr(array[1]);
        mx_printchar(' ');
        print_aligned(array[2], 2, true);
        mx_printchar(' ');
        char **arr_time = mx_strsplit(array[3], ':');
        mx_printstr(arr_time[0]);
        mx_printchar(':');
        mx_printstr(arr_time[1]);
        mx_del_strarr(&arr_time);
    }
    mx_del_strarr(&array);
}

static double round_size(double number) {
    return (long) (number + 0.5);
}

static void print_size(off_t size, int width) {
    const char *measurement_units[] = {"B", "K", "M", "G", "T", "P"};
    double file_size = size;
    int unit_index = 0;

    while (file_size >= 1000) {
        file_size /= 1024;
        unit_index++;
    }

    double rounded_size = round_size(file_size * 10) / 10;
    char buf[5] = {'\0'};
    if (rounded_size >= 10 || unit_index == 0) {
        char *str = mx_lltoa(round_size(file_size));
        mx_strcat(buf, str);
        free(str);
    } else {
        char *str = mx_lltoa(rounded_size);
        mx_strcat(buf, str);
        free(str);
        mx_strcat(buf, ".");
        str = mx_lltoa((long long) (rounded_size * 10) % 10);
        mx_strcat(buf, str);
        free(str);
    }
    mx_strcat(buf, measurement_units[unit_index]);
    print_aligned(buf, width, true);
}

static void print_xattrs(t_file_details *file_details, bool human_readable) {
    for (char **ptr = file_details->xattr_keys; *ptr != NULL; ptr++) {
        mx_printchar('\t');
        mx_printstr(*ptr);
        mx_printchar('\t');
        ssize_t value_size = getxattr(file_details->path, *ptr, NULL, 0, 0, XATTR_NOFOLLOW);
        if (human_readable)
            print_size(value_size, 5);
        else
            print_number_aligned(value_size, 4);
        mx_printstr(" \n");
    }
}

static void print_acl(acl_t acl) {
    char *acl_str = acl_to_text(acl, NULL);
    char **lines = mx_strsplit(acl_str, '\n');
    for (int i = 1; lines[i] != NULL; i++) {
        mx_printchar(' ');
        mx_printint(i - 1);
        mx_printstr(": ");
        char **parts_of_line = mx_strsplit(lines[i], ':');
        mx_printstr(parts_of_line[0]);
        mx_printchar(':');
        mx_printstr(parts_of_line[2]);
        mx_printchar(' ');
        mx_printstr(parts_of_line[4]);
        mx_printchar(' ');
        mx_printstr(parts_of_line[5]);
        mx_printchar('\n');
        mx_del_strarr(&parts_of_line);
    }
    mx_del_strarr(&lines);
    free(acl_str);
}

static void print_file_details_detailed(t_file_details *file_details, t_width *width, t_configs *configs) {
    print_filetype(file_details->stat.st_mode);
    print_permissions(file_details->stat.st_mode);

    if (file_details->xattr_keys != NULL)
        mx_printchar('@');
    else if (file_details->acl != NULL)
        mx_printchar('+');
    else
        mx_printchar(' ');

    mx_printchar(' ');
    print_number_aligned(file_details->stat.st_nlink, width->links);
    mx_printchar(' ');

    if (!configs->hide_owner_information) {
        print_aligned(file_details->user, width->user, false);
        mx_printstr("  ");
    }
    if (!configs->hide_group_information) {
        print_aligned(file_details->group, width->group, false);
        mx_printstr("  ");
    }

    if (configs->hide_owner_information && configs->hide_group_information)
        mx_printstr("  ");

    if (S_ISCHR(file_details->stat.st_mode) || S_ISBLK(file_details->stat.st_mode)) {
        char *hex_number = mx_nbr_to_hex(file_details->stat.st_rdev);
        char *str;
        if (file_details->stat.st_rdev == 0)
            str = mx_strdup(hex_number);
        else
            str = mx_strjoin("0x", hex_number);
        print_aligned(str, width->size, true);
        free(str);
        free(hex_number);
    } else if (configs->display_human_readable_size)
        print_size(file_details->stat.st_size, width->size);
    else
        print_number_aligned(file_details->stat.st_size, width->size);

    mx_printchar(' ');
    print_date_and_time(file_details->timespec.tv_sec, configs->full_time_info);
    mx_printchar(' ');
    print_file_details(file_details, configs);
    if (file_details->link != NULL) {
        mx_printstr(" -> ");
        mx_printstr(file_details->link);
    }
    mx_printchar('\n');

    if (configs->extended_attributes && file_details->xattr_keys != NULL)
        print_xattrs(file_details, configs->display_human_readable_size);

    if (configs->acl && file_details->acl != NULL)
        print_acl(file_details->acl);
}

static t_width calculate_max_width(t_list *files_details, t_configs *configs) {
    t_width width;
    width.links = 0;
    width.user = 0;
    width.group = 0;
    width.size = 0;

    while (files_details != NULL) {
        t_file_details *file_details = files_details->data;

        char *nlinks = mx_lltoa(file_details->stat.st_nlink);
        if (width.links < mx_strlen(nlinks))
            width.links = mx_strlen(nlinks);
        free(nlinks);
        if (width.user < mx_strlen(file_details->user))
            width.user = mx_strlen(file_details->user);
        if (width.group < mx_strlen(file_details->group))
            width.group = mx_strlen(file_details->group);

        int size_len = 5;
        if (S_ISCHR(file_details->stat.st_mode) || S_ISBLK(file_details->stat.st_mode)) {
            char *wsize = mx_nbr_to_hex(file_details->stat.st_rdev);
            size_len = mx_strlen(wsize) + 2;
            free(wsize);
        } else if (!configs->display_human_readable_size) {
            char *wsize = mx_lltoa(file_details->stat.st_size);
            size_len = mx_strlen(wsize);
            free(wsize);
        }
        if (width.size < size_len)
            width.size = size_len;

        files_details = files_details->next;
    }

    return width;
}

void print_detailed(t_list *files_details, t_configs *configs) {
    t_width width = calculate_max_width(files_details, configs);

    while (files_details != NULL) {
        print_file_details_detailed(files_details->data, &width, configs);
        files_details = files_details->next;
    }
}
