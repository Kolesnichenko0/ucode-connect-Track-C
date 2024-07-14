#include "uls_utils.h"

t_configs *parse_configs(int argc, char *argv[]) {
    t_configs *configs = malloc(sizeof(t_configs));
    mx_memset(configs, 0, sizeof(t_configs));
    if (isatty(1))
        configs->format = MULTI_COLUMN;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-')
            break;
        for (char *str = argv[i] + 1; *str != '\0'; str++) {
            switch (*str) {
                case 'R':
                    configs->use_recursion = true;
                    break;
                case 'A':
                    configs->ignore_type = DOTS;
                    break;
                case 'a':
                    configs->ignore_type = NOT_IGNORED;
                    break;
                case 'G':
                    configs->use_colors = true;
                    break;
                case 'F':
                    configs->classify = true;
                    configs->add_only_slash_to_directories = false;
                    break;
                case 'p':
                    configs->add_only_slash_to_directories = true;
                    configs->classify = false;
                    break;
                case 'r':
                    configs->sort_reverse = true;
                    break;
                case 'h':
                    configs->display_human_readable_size = true;
                    break;
                case 'e':
                    configs->acl = true;
                    break;
                case 'o':
                    configs->hide_group_information = true;
                    break;
                case 'T':
                    configs->full_time_info = true;
                    break;
                case '@':
                    configs->extended_attributes = true;
                    break;
                case 'g':
                    configs->hide_owner_information = true;
                    break;
                case 'x':
                    configs->sort_horizontally = true;
                    break;
                case 'n':
                    configs->display_numeric_IDs = true;
                    break;
                case 'q':
                    configs->display_non_printable_characters = true;
                    break;
                case 'H':
                    configs->follow_symbolic_links = true;
                    break;
                case '1':
                    configs->format = ONE_COLUMN;
                    break;
                case 'C':
                    configs->format = MULTI_COLUMN;
                    break;
                case 'c':
                    configs->time_type = LAST_METADATA_CHANGE;
                    break;
                case 'm':
                    configs->format = STREAM;
                    break;
                case 'l':
                    configs->format = DETAILED;
                    break;
                case 'u':
                    configs->time_type = LAST_ACCESS;
                    break;
                case 'U':
                    configs->time_type = LAST_CREATION;
                    break;
                case 't':
                    configs->sort_type = BY_TIME;
                    break;
                case 'S':
                    configs->sort_type = BY_SIZE;
                    break;
                case 'f':
                    configs->sort_type = UNSORTED;
                    configs->ignore_type = NOT_IGNORED;
                    break;
                default:
                    mx_printerr("uls: illegal option -- ");
                    write(2, str, 1);
                    write(2, "\n", 1);
                    mx_printerr("usage: uls [-@ACFGHRSTUacefghlmnopqrtux1] [file ...]\n");
                    exit(1);
            }
        }
    }

    if (isatty(1))
        configs->display_non_printable_characters = true;
    else
        configs->use_colors = false;

    if (!configs->follow_symbolic_links)
        configs->follow_symbolic_links = configs->format != DETAILED && !configs->use_colors && !configs->classify;

    if (configs->sort_type == UNSORTED)
        configs->ignore_type = NOT_IGNORED;

    return configs;
}

int main(int argc, char *argv[]) {
    t_list *given_files_and_dirs = NULL;
    for (int i = 1; i < argc; i++)
        if (argv[i][0] != '-' || given_files_and_dirs != NULL)
            mx_push_back(&given_files_and_dirs, argv[i]);
    if (given_files_and_dirs == NULL)
        mx_push_back(&given_files_and_dirs, ".");

    t_configs *configs = parse_configs(argc, argv);

    t_list *files = NULL;
    t_list *dirs = NULL;
    bool must_print_names = mx_list_size(given_files_and_dirs) > 1;
    int result_status = 0;

    sort_filenames(given_files_and_dirs, configs->sort_type);
    while (given_files_and_dirs != NULL) {
        char *file_or_dir = given_files_and_dirs->data;
        given_files_and_dirs = given_files_and_dirs->next;

        t_file_details *file_details = get_file_details(NULL, file_or_dir, configs);
        if (file_details == NULL) {
            mx_printerr("uls: ");
            mx_printerr(file_or_dir);
            mx_printerr(": ");
            mx_printerr(strerror(errno));
            mx_printerr("\n");
            result_status = 1;
            continue;
        }

        if (S_ISDIR(file_details->stat.st_mode))
            mx_push_back(&dirs, file_details);
        else
            mx_push_back(&files, file_details);
    }
    if (files != NULL) {
        sort_files_details(files, configs->sort_type, configs->sort_reverse);
        print_files_details(files, configs);
    }
    if (files != NULL && dirs != NULL)
        mx_printchar('\n');
    if (!sort_print_dirs(dirs, configs, must_print_names))
        result_status = 1;
    free_files_details(files);
    free_files_details(dirs);

    mx_clear_list(&given_files_and_dirs);
    free(configs);
    return result_status;
}
