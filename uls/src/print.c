#include "uls_utils.h"

static void apply_color(mode_t mode) {
    switch (mode & S_IFMT) {
        case S_IFBLK:
            mx_printstr("\033[34;46m");
            break;
        case S_IFCHR:
            mx_printstr("\033[34;43m");
            break;
        case S_IFDIR:
            if (mode & S_IWOTH) {
                if (mode & S_ISTXT) {
                    mx_printstr("\033[30;42m");
                } else {
                    mx_printstr("\033[30;43m");
                }
            } else {
                mx_printstr("\033[34m");
            }
            break;
        case S_IFIFO:
            mx_printstr("\033[33m");
            break;
        case S_IFLNK:
            mx_printstr("\033[35m");
            break;
        case S_IFSOCK:
            mx_printstr("\033[32m");
            break;
        default:
            if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
                if (mode & S_ISUID) {
                    mx_printstr("\033[30;41m");
                } else if (mode & S_ISGID) {
                    mx_printstr("\033[30;46m");
                } else {
                    mx_printstr("\033[31m");
                }
            }
            break;
    }
}

static int add_classifier(mode_t mode, bool slash_only) {
    char classifier = '\0';

    switch (mode & S_IFMT) {
        case S_IFDIR:
            classifier = '/';
            break;
        case S_IFIFO:
            classifier = '|';
            break;
        case S_IFLNK:
            classifier = '@';
            break;
        case S_IFSOCK:
            classifier = '=';
            break;
        case S_IFREG:
            if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
                classifier = '*';
            break;
    }

    if (classifier != '\0') {
        if (slash_only && classifier != '/')
            return 0;
        mx_printchar(classifier);
        return 1;
    }
    return 0;
}

static char *replace_non_printable_characters(const char *name) {
    char *temp = mx_strdup(name);
    for (int i = 0; temp[i] != '\0'; i++)
        if (temp[i] >= 0 && temp[i] <= 31)
            temp[i] = '?';
    return temp;
}

int print_file_details(t_file_details *file_details, t_configs *configs) {
    if (configs->use_colors)
        apply_color(file_details->stat.st_mode);
    if (configs->display_non_printable_characters) {
        char *str = replace_non_printable_characters(file_details->name);
        mx_printstr(str);
        free(str);
    } else
        mx_printstr(file_details->name);
    if (configs->use_colors)
        mx_printstr("\033[0m");

    int length = mx_strlen(file_details->name);
    if (configs->classify || configs->add_only_slash_to_directories)
        length += add_classifier(file_details->stat.st_mode, configs->add_only_slash_to_directories);
    return length;
}

void print_files_details(t_list *files_details, t_configs *configs) {
    switch (configs->format) {
        case ONE_COLUMN:
            print_one_column(files_details, configs);
            break;
        case MULTI_COLUMN:
            print_multi_column(files_details, configs);
            break;
        case STREAM:
            print_stream(files_details, configs);
            break;
        case DETAILED:
            print_detailed(files_details, configs);
            break;
        default:
            break;
    }
}
