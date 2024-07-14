#include "pathfinder.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        mx_printerr("usage: ./pathfinder [filename]\n");
        exit(1);
    }
    validate_file(argv[1]);
    int fd = open(argv[1], O_RDONLY);
    t_graph *graph = parse_file(fd, argv[1]);
    for (long i = 0; i < graph->island_count; i++) {
        t_path_vertex *vertices = find_path(graph, i);
        print_paths_from(graph, vertices, i);
        clear_vertices(vertices, graph->island_count);
    }
    clear_graph(graph, graph->island_count);
    return 0;
}
