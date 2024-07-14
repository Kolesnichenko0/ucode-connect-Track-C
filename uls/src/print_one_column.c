#include "uls_utils.h"

void print_one_column(t_list *files_details, t_configs *configs) {
    while (files_details != NULL) {
        print_file_details(files_details->data, configs);
        mx_printchar('\n');
        files_details = files_details->next;
    }
}
