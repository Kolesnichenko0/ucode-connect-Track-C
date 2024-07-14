#include "pathfinder.h"

void validate_file(const char *filename) {
    char buf;
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        print_error(FILE_DOES_NOT_EXIST, filename);
        exit(1);
    }
    int n_read = read(fd, &buf, 1);
    close(fd);
    if (n_read == 0) {
        print_error(EMPTY_FILE, filename);
        exit(1);
    }
}

static long parse_first_line(int fd) {
    const int line_number = 1;
    char *line = NULL;
    int size_result = mx_read_line(&line, 1, '\n', fd);
    if (size_result < 0) {
        print_error(INVALID_LINE, &line_number);
        exit(1);
    }
    int plus_length = 0;
    if (line[0] == '+')
        plus_length = 1;
    for (int i = plus_length; line[i] != '\0'; i++)
        if (!mx_isdigit(line[i])) {
            mx_strdel(&line);
            print_error(INVALID_LINE, &line_number);
            exit(1);
        }
    long givenNumberOfIslands = mx_atol(line);
    if (givenNumberOfIslands <= 0) {
        mx_strdel(&line);
        print_error(INVALID_LINE, &line_number);
        exit(1);
    }
    mx_strdel(&line);
    return givenNumberOfIslands;
}

static bool validate_length_str(char *length_of_bridge_str) {
    int plus_length = 0;
    if (length_of_bridge_str[0] == '+')
        plus_length = 1;
    for (int i = plus_length; length_of_bridge_str[i] != '\0'; i++)
        if (!mx_isdigit(length_of_bridge_str[i]))
            return false;
    return true;
}

static bool validate_island(char *island) {
    int island_length = mx_strlen(island);
    if (island_length == 0)
        return false;
    for (int i = 0; i < island_length; i++) {
        if (!mx_isalpha(island[i]))
            return false;
    }
    return true;
}

static bool validate_last_newline(const char *filename) {
    char *file_str = mx_file_to_str(filename);
    mx_str_reverse(file_str);
    if (file_str[0] != '\n') {
        return false;
    }
    mx_strdel(&file_str);
    return true;
}

static bool parse_nth_line(int fd, char **island1, char **island2,
                           int *length_of_bridge, t_graph *graph) {
    int size_result = mx_read_line(island1, 1, '-', fd);
    if (size_result < 0)
        return false;

    size_result = mx_read_line(island2, 1, ',', fd);
    if (size_result < 0) {
        mx_strdel(island1);
        graph->mustClear = true;
        return false;
    }
    char *length_of_bridge_str = NULL;
    size_result = mx_read_line(&length_of_bridge_str, 1, '\n', fd);
    if (size_result < 0) {
        mx_strdel(island1);
        mx_strdel(island2);
        graph->mustClear = true;
        return false;
    }
    if (!validate_length_str(length_of_bridge_str)) {
        mx_strdel(island1);
        mx_strdel(island2);
        mx_strdel(&length_of_bridge_str);
        graph->mustClear = true;
        return false;
    }
    *length_of_bridge = mx_atoi(length_of_bridge_str);
    mx_strdel(&length_of_bridge_str);
    if (!validate_island(*island1)
        || !validate_island(*island2)
        || mx_strcmp(*island1, *island2) == 0
        || *length_of_bridge <= 0) {
        mx_strdel(island1);
        mx_strdel(island2);
        graph->mustClear = true;
        return false;
    }
    return true;
}

static bool insert_nth_line(char *island1, char *island2, int length_of_bridge,
                            t_graph *graph, long givenNumberOfIslands) {
    long island_indexes[2] = {get_island_index(graph, island1), get_island_index(graph, island2)};
    char *islands[2] = {island1, island2};
    for (int i = 0; i < 2; i++)
        if (island_indexes[i] == -1 && !graph->isFull) {
            insert_island(graph, islands[i]);
            island_indexes[i] = graph->island_count - 1;
            if (graph->island_count == givenNumberOfIslands)
                graph->isFull = true;
        }
    for (int i = 0; i < 2; i++) {
        if (island_indexes[i] == -1) {
            print_error(INVALID_GIVEN_NUMBER_OF_ISLANDS, NULL);
            graph->mustClear = true;
            return false;
        }
    }
    if (!insert_length_of_bridge(graph, island_indexes[0], island_indexes[1], length_of_bridge))
        graph->isDuplicateBridges = true;
    return true;
}

t_graph *parse_file(int fd, const char *filename) {
    long givenNumberOfIslands = parse_first_line(fd);
    t_graph *graph = create_graph(givenNumberOfIslands);
    int current_line_number = 1;
    bool mustContinue = true;
    while (true) {
        current_line_number++;
        char *island1 = NULL;
        char *island2 = NULL;
        int length_of_bridge = 0;

        mustContinue = parse_nth_line(fd, &island1, &island2, &length_of_bridge, graph);
        if (graph->mustClear)
            print_error(INVALID_LINE, &current_line_number);
        if (!mustContinue) {
            if (!graph->mustClear && !validate_last_newline(filename)) {
                current_line_number--;
                graph->mustClear = true;
                print_error(INVALID_LINE, &current_line_number);
            }
            break;
        }

        mustContinue = insert_nth_line(island1, island2, length_of_bridge, graph, givenNumberOfIslands);
        mx_strdel(&island1);
        mx_strdel(&island2);
        if (!mustContinue)
            break;
    }
    if(!graph->mustClear) {
        graph->mustClear = true;
        if (!graph->isFull)
            print_error(INVALID_GIVEN_NUMBER_OF_ISLANDS, NULL);
        else if(graph->isDuplicateBridges)
            print_error(BRIDGES_ARE_DUPLICATED, NULL);
        else if (!validate_bridges_sum(graph))
            print_error(INVALID_BRIDGES_SUM, NULL);
        else
            graph->mustClear = false;
    }

    if (graph->mustClear) {
        clear_graph(graph, givenNumberOfIslands);
        exit(1);
    }

    return graph;
}
