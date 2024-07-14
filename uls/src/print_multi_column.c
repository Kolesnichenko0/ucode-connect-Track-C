#include "uls_utils.h"

static int calculate_max_width(t_list *files_details) {
    int max_width = 0;
    while (files_details != NULL) {
        t_file_details *file_details = files_details->data;
        int length = mx_strlen(file_details->name);
        if (max_width < length)
            max_width = length;
        files_details = files_details->next;
    }
    return max_width;
}

static t_file_details **list_to_file_details_array(t_list *files_details, int *file_number) {
    *file_number = mx_list_size(files_details);
    if (file_number == 0)
        return NULL;

    t_file_details **file_details_array = malloc(sizeof(t_file_details * ) * (*file_number));
    for (int i = 0; i < *file_number; i++) {
        file_details_array[i] = files_details->data;
        files_details = files_details->next;
    }

    return file_details_array;
}

void print_multi_column(t_list *files_details, t_configs *configs) {
    int tabwidth = 8;
    if (configs->use_colors)
        tabwidth = 1;

    int width = calculate_max_width(files_details);
    if (configs->classify || configs->add_only_slash_to_directories)
        width++;
    width = (width + tabwidth) & ~(tabwidth - 1);
    int terminal_width = 80;
    if (isatty(1)) {
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        terminal_width = w.ws_col;
    }

    int column_number = terminal_width / width;
    if (column_number <= 1) {
        print_one_column(files_details, configs);
        return;
    }

    int file_number;
    t_file_details **array = list_to_file_details_array(files_details, &file_number);
    int row_number = (file_number + column_number - 1) / column_number;

    int index = 0;
    for (int i = 0; i < row_number; i++) {
        if (!configs->sort_horizontally)
            index = i;
        for (int j = 0; j < column_number; j++) {
            int printed = print_file_details(array[index], configs);
            if (configs->sort_horizontally)
                index++;
            else
                index += row_number;
            if (index >= file_number)
                break;
            int tabs = (width - printed + tabwidth - 1) / tabwidth;
            for (int i = 0; i < tabs; i++) {
                if (tabwidth == 1)
                    mx_printchar(' ');
                else
                    mx_printchar('\t');
            }
        }
        mx_printchar('\n');
    }
    if (array != NULL)
        free(array);
}
