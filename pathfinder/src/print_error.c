#include "pathfinder.h"

void print_error(enum e_error error, const void *data) {
    switch (error) {
        case FILE_DOES_NOT_EXIST:
            mx_printerr("error: file ");
            mx_printerr((char *) data);
            mx_printerr(" does not exist\n");
            break;
        case EMPTY_FILE:
            mx_printerr("error: file ");
            mx_printerr((char *) data);
            mx_printerr(" is empty\n");
            break;
        case INVALID_LINE: {
            char *line_number_str = mx_itoa(*(int *) data);
            mx_printerr("error: line ");
            mx_printerr(line_number_str);
            mx_printerr(" is not valid\n");
            mx_strdel(&line_number_str);
            break;
        }
        case INVALID_GIVEN_NUMBER_OF_ISLANDS:
            mx_printerr("error: invalid number of islands\n");
            break;
        case BRIDGES_ARE_DUPLICATED:
            mx_printerr("error: duplicate bridges\n");
            break;
        case INVALID_BRIDGES_SUM:
            mx_printerr("error: sum of bridges lengths is too big\n");
            break;
    }
}
