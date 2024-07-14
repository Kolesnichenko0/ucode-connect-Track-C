#include "pathfinder.h"

static void print_separating_line() {
    for (int i = 0; i < 40; i++) {
        mx_printstr("=");
    }
    mx_printstr("\n");
}

static void print_route(t_graph *graph, t_list *path) {
    while (path != NULL) {
        t_path_vertex *vertex = (t_path_vertex *) path->data;
        if (vertex->distance_from_initial != 0)
            mx_printstr(" -> ");
        mx_printstr(graph->islands[vertex->island_index]);
        path = path->next;
    }
}

static void print_distance(t_list *path) {
    while (path != NULL && path->next != NULL) {
        t_path_vertex *vertex = (t_path_vertex *) path->data;
        t_path_vertex *next_vertex = (t_path_vertex *) path->next->data;
        if (vertex->distance_from_initial != 0)
            mx_printstr(" + ");
        mx_printint(next_vertex->distance_from_initial - vertex->distance_from_initial);
        if (vertex->distance_from_initial != 0 && path->next->next == NULL) {
            mx_printstr(" = ");
            mx_printint(next_vertex->distance_from_initial);
        }
        path = path->next;
    }
}

static void print_path(t_graph *graph, t_list *path, long starting_index, long ending_index) {
    print_separating_line();
    mx_printstr("Path: ");
    mx_printstr(graph->islands[starting_index]);
    mx_printstr(" -> ");
    mx_printstr(graph->islands[ending_index]);
    mx_printstr("\n");
    mx_printstr("Route: ");
    print_route(graph, path);
    mx_printstr("\n");
    mx_printstr("Distance: ");
    print_distance(path);
    mx_printstr("\n");
    print_separating_line();
}

static bool compare_island(void *path1, void *path2) {
    path1 = ((t_list*)path1)->next;
    path2 = ((t_list*)path2)->next;
    while((t_list*)path1 != NULL && (t_list*)path2 != NULL ) {
        t_path_vertex *vertex1 = (t_path_vertex *)(((t_list*)path1)->data);
        t_path_vertex *vertex2 = (t_path_vertex *)(((t_list*)path2)->data);
        if(vertex1->island_index > vertex2->island_index)
            return true;
        if(vertex1->island_index != vertex2->island_index)
            return false;
        path1 = ((t_list*)path1)->next;
        path2 = ((t_list*)path2)->next;
    }
    return false;
}

void print_paths_from(t_graph *graph, t_path_vertex *vertices, long starting_index) {
    for (long ending_index = starting_index + 1; ending_index < graph->island_count; ending_index++) {
        t_list *paths_to_ending = parse_all_paths_to(&vertices[ending_index]);
        mx_sort_list(paths_to_ending, compare_island);
        t_list *current_path = paths_to_ending;
        while (current_path != NULL) {
            t_list *path = (t_list *) current_path->data;
            print_path(graph, path, starting_index, ending_index);
            mx_clear_list(&path);
            current_path = current_path->next;
        }
        mx_clear_list(&paths_to_ending);
    }
}
