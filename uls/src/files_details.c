#include "uls_utils.h"

static char *get_user(uid_t uid, bool numeric_ID) {
    struct passwd *passwd = getpwuid(uid);
    if (passwd == NULL || numeric_ID)
        return mx_lltoa(uid);
    return mx_strdup(passwd->pw_name);
}

static char *get_group(gid_t gid, bool numeric_ID) {
    struct group *group = getgrgid(gid);
    if (group == NULL || numeric_ID)
        return mx_lltoa(gid);
    return mx_strdup(group->gr_name);
}

static struct timespec get_timespec(struct stat *stat, t_time_type time_type) {
    switch (time_type) {
        case LAST_MODIFICATION:
            return stat->st_mtimespec;
        case LAST_METADATA_CHANGE:
            return stat->st_ctimespec;
        case LAST_ACCESS:
            return stat->st_atimespec;
        case LAST_CREATION:
            return stat->st_birthtimespec;
    }
}

void free_file_details(t_file_details *file_details) {
    free(file_details->path);
    free(file_details->name);
    mx_strdel(&file_details->user);
    mx_strdel(&file_details->link);
    mx_strdel(&file_details->group);
    mx_del_strarr(&file_details->xattr_keys);
    acl_free(file_details->acl);
    free(file_details);
}

void free_files_details(t_list *files_details) {
    t_list *temp = files_details;

    while (temp != NULL) {
        t_file_details *file_details = temp->data;
        free_file_details(file_details);
        temp = temp->next;
    }

    mx_clear_list(&files_details);
}

t_file_details *get_file_details(const char *dir, const char *name, t_configs *configs) {
    t_file_details *file_details = malloc(sizeof(t_file_details));

    file_details->path = mx_delim_strjoin(dir, name, '/');
    file_details->name = mx_strdup(name);
    file_details->user = NULL;
    file_details->link = NULL;
    file_details->group = NULL;
    file_details->xattr_keys = NULL;
    file_details->acl = NULL;

    int processing_status;
    if (dir == NULL && configs->follow_symbolic_links)
        processing_status = stat(file_details->path, &file_details->stat);
    else
        processing_status = lstat(file_details->path, &file_details->stat);
    if (processing_status != 0) {
        free_file_details(file_details);
        return NULL;
    }

    if (configs->format == DETAILED) {
        file_details->user = get_user(file_details->stat.st_uid, configs->display_numeric_IDs);
        file_details->group = get_group(file_details->stat.st_gid, configs->display_numeric_IDs);

        if (S_ISLNK(file_details->stat.st_mode)) {
            file_details->link = malloc(PATH_MAX);
            ssize_t size = readlink(file_details->path, file_details->link, PATH_MAX);
            file_details->link[size] = '\0';
        } else {
            file_details->link = NULL;
            file_details->acl = acl_get_file(file_details->path, ACL_TYPE_EXTENDED);
        }
        char buffer[1024] = {'\0'};
        ssize_t size = listxattr(file_details->path, buffer, sizeof(buffer), XATTR_NOFOLLOW);
        for (int i = 0; i < size - 1; i++)
            if (buffer[i] == '\0')
                buffer[i] = ':';
        if (size > 0)
            file_details->xattr_keys = mx_strsplit(buffer, ':');
        else
            file_details->xattr_keys = NULL;
    }
    file_details->timespec = get_timespec(&file_details->stat, configs->time_type);
    return file_details;
}
