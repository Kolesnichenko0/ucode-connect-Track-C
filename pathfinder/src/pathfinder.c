#include "pathfinder.h"

static t_path_vertex *create_vertices(t_graph *graph, long starting_index) {
    t_path_vertex *vertices = (t_path_vertex *) malloc(sizeof(t_path_vertex) * graph->island_count);
    for (long i = 0; i < graph->island_count; i++) {
        vertices[i].isVisited = false;
        vertices[i].distance_from_initial = -1;
        vertices[i].island_index = i;
        vertices[i].previous = NULL;
    }
    vertices[starting_index].distance_from_initial = 0;
    return vertices;
}

void clear_vertices(t_path_vertex *vertices, long island_count) {
    for (long i = 0; i < island_count; i++)
        mx_clear_list(&vertices[i].previous);
    free(vertices);
}

static long find_min_vertex(t_graph *graph, t_path_vertex *vertices) {
    long min_vertex_index = -1;
    for (long i = 0; i < graph->island_count; i++)
        if (!vertices[i].isVisited && vertices[i].distance_from_initial != -1)
            if (min_vertex_index == -1 ||
                vertices[i].distance_from_initial < vertices[min_vertex_index].distance_from_initial)
                min_vertex_index = i;
    return min_vertex_index;
}

t_path_vertex *find_path(t_graph *graph, long starting_index) {
    t_path_vertex *vertices = create_vertices(graph, starting_index);
    long current_index = starting_index;
    while (current_index != -1) {
        vertices[current_index].isVisited = true;
        for (long i = 0; i < graph->island_count; i++) {
            if (!vertices[i].isVisited && graph->bridges[current_index][i] != -1) {
                int new_distance = vertices[current_index].distance_from_initial + graph->bridges[current_index][i];

                if (vertices[i].distance_from_initial == -1 || new_distance <= vertices[i].distance_from_initial) {
                    //Relaxation
                    if (new_distance < vertices[i].distance_from_initial)
                        mx_clear_list(&vertices[i].previous);
                    vertices[i].distance_from_initial = new_distance;
                    mx_push_back(&vertices[i].previous, &vertices[current_index]);
                }
            }
        }
        current_index = find_min_vertex(graph, vertices);
    }
    return vertices;
}

t_list *parse_all_paths_to(t_path_vertex *vertex) {
    t_list *paths = NULL;
    if (vertex->distance_from_initial == 0)
        return mx_create_node(mx_create_node(vertex));
    t_list *previous = vertex->previous;
    while (previous != NULL) {
        t_list *temp = parse_all_paths_to((t_path_vertex *) previous->data);
        t_list *current = temp;
        while (current != NULL) {
            mx_push_back((t_list **) & current->data, vertex);
            mx_push_back(&paths, current->data);
            current = current->next;
        }
        mx_clear_list(&temp);
        previous = previous->next;
    }

    return paths;
}
