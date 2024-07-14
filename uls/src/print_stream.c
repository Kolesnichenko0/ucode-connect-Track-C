#include "uls_utils.h"

void print_stream(t_list *files_details, t_configs *configs) {
    while (files_details != NULL) {
        print_file_details(files_details->data, configs);
        if (files_details->next != NULL)
            mx_printstr(", ");
        files_details = files_details->next;
    }
    mx_printchar('\n');
}
