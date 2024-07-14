#include "uls_utils.h"

static bool compare_strings(void *first_data, void *second_data) {
    return mx_strcmp(first_data, second_data) > 0;
}

static bool compare_by_name(void *first_data, void *second_data) {
    t_file_details *file_details1 = first_data;
    t_file_details *file_details2 = second_data;
    return mx_strcmp(file_details1->name, file_details2->name) > 0;
}

static bool compare_by_size(void *first_data, void *second_data) {
    t_file_details *file_details1 = first_data;
    t_file_details *file_details2 = second_data;
    if (file_details1->stat.st_size == file_details2->stat.st_size)
        return mx_strcmp(file_details1->name, file_details2->name) > 0;
    return file_details1->stat.st_size < file_details2->stat.st_size;
}

static bool compare_by_time(void *first_data, void *second_data) {
    t_file_details *file_details1 = first_data;
    t_file_details *file_details2 = second_data;
    if (file_details1->timespec.tv_sec == file_details2->timespec.tv_sec) {
        if (file_details1->timespec.tv_nsec == file_details2->timespec.tv_nsec)
            return mx_strcmp(file_details1->name, file_details2->name) > 0;
        return file_details1->timespec.tv_nsec < file_details2->timespec.tv_nsec;
    }
    return file_details1->timespec.tv_sec < file_details2->timespec.tv_sec;
}

void sort_filenames(t_list *filenames, t_sort_type sort_type) {
    if (sort_type != UNSORTED)
        mx_sort_list(filenames, compare_strings, false);
}

void sort_files_details(t_list *files_details, t_sort_type sort_type, bool isReversed) {
    switch (sort_type) {
        case BY_NAME:
            mx_sort_list(files_details, compare_by_name, isReversed);
            break;
        case BY_SIZE:
            mx_sort_list(files_details, compare_by_size, isReversed);
            break;
        case BY_TIME:
            mx_sort_list(files_details, compare_by_time, isReversed);
            break;
        default:
            break;
    }
}
