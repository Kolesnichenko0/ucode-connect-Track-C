#include "uls_utils.h"

bool is_ignored(const char *name, t_ignore_type ignore_type) {
    if (ignore_type == HIDDEN && name[0] == '.')
        return true;
    if (ignore_type == DOTS && (mx_strcmp(name, ".") == 0 || mx_strcmp(name, "..") == 0))
        return true;
    return false;
}

bool get_dir_entries(t_list **entries, const char *name, t_configs *configs) {
    DIR *dir = opendir(name);
    if (dir == NULL)
        return false;

    struct dirent *entry = NULL;

    while ((entry = readdir(dir)) != NULL)
        if (!is_ignored(entry->d_name, configs->ignore_type))
            mx_push_back(entries, get_file_details(name, entry->d_name, configs));

    closedir(dir);
    return true;
}

blkcnt_t calculate_count_blocks(t_list *files) {
    t_list *temp = files;
    blkcnt_t blocks_count = 0;

    while (temp != NULL) {
        t_file_details *file_details = temp->data;
        blocks_count += file_details->stat.st_blocks;
        temp = temp->next;
    }
    return blocks_count;
}

bool sort_print_dir(t_file_details *file_details, t_configs *configs, bool must_print_name) {
    if (must_print_name) {
        mx_printstr(file_details->name);
        mx_printstr(":\n");
    }

    t_list *files_details = NULL;
    bool result_status = get_dir_entries(&files_details, file_details->path, configs);

    if (configs->format == DETAILED && files_details != NULL) {
        mx_printstr("total ");
        mx_printint(calculate_count_blocks(files_details));
        mx_printchar('\n');
    }

    if (!result_status) {
        mx_printerr("uls: ");
        mx_printerr(file_details->name);
        mx_printerr(": ");
        mx_printerr(strerror(errno));
        mx_printerr("\n");
        return false;
    }

    sort_files_details(files_details, configs->sort_type, configs->sort_reverse);
    print_files_details(files_details, configs);

    if (configs->use_recursion) {
        t_list *dirs = NULL;
        while (files_details != NULL) {
            t_file_details *entry = files_details->data;
            if (S_ISDIR(entry->stat.st_mode) && !is_ignored(entry->name, DOTS)) {
                free(entry->name);
                entry->name = mx_strdup(entry->path);
                mx_push_back(&dirs, entry);
            }
            files_details = files_details->next;
        }

        if (dirs != NULL)
            mx_printchar('\n');
        if (!sort_print_dirs(dirs, configs, true))
            result_status = false;
        mx_clear_list(&dirs);
    }

    free_files_details(files_details);
    return result_status;
}

bool sort_print_dirs(t_list *dirs, t_configs *configs, bool must_print_names) {
    bool result = true;

    sort_files_details(dirs, configs->sort_type, configs->sort_reverse);
    while (dirs != NULL) {
        t_file_details *file_details = dirs->data;
        if (!sort_print_dir(file_details, configs, must_print_names))
            result = false;
        if (dirs->next != NULL)
            mx_printchar('\n');
        dirs = dirs->next;
    }
    return result;
}
