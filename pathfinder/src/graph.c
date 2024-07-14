#include "pathfinder.h"

t_graph *create_graph(long givenNumberOfIslands) {
    t_graph *graph = (t_graph *) malloc(sizeof(t_graph));
    graph->mustClear = false;
    graph->isFull = false;
    graph->isDuplicateBridges = false;
    graph->island_count = 0;
    graph->islands = (char **) malloc(sizeof(char *) * givenNumberOfIslands);
    graph->bridges = (int **) malloc(sizeof(int *) * givenNumberOfIslands);
    for (long i = 0; i < givenNumberOfIslands; i++) {
        graph->islands[i] = NULL;
        graph->bridges[i] = (int *) malloc(sizeof(int) * givenNumberOfIslands);
        for (long j = 0; j < givenNumberOfIslands; j++)
            graph->bridges[i][j] = -1;
    }

    return graph;
}

void clear_graph(t_graph *graph, long givenNumberOfIslands) {
    for (long i = 0; i < givenNumberOfIslands; i++) {
        mx_strdel(&graph->islands[i]);
        free(graph->bridges[i]);
    }
    free(graph->islands);
    free(graph->bridges);
    free(graph);
}

long get_island_index(t_graph *graph, char *island) {
    for (long i = 0; i < graph->island_count; i++) {
        if (mx_strcmp(island, graph->islands[i]) == 0) {
            return i;
        }
    }
    return -1;
}

void insert_island(t_graph *graph, char *island) {
    graph->islands[graph->island_count] = mx_strdup(island);
    graph->bridges[graph->island_count][graph->island_count] = 0;
    graph->island_count++;
}

bool insert_length_of_bridge(t_graph *graph, long island1_index, long island2_index, int length_of_bridge) {
    if (graph->bridges[island1_index][island2_index] != -1)
        return false;

    graph->bridges[island1_index][island2_index] = length_of_bridge;
    graph->bridges[island2_index][island1_index] = length_of_bridge;
    return true;
}

bool validate_bridges_sum(t_graph *graph) {
    unsigned long sum = 0;
    for (long i = 0; i < graph->island_count; i++)
        for (long j = 0; j < i; j++)
            if (graph->bridges[i][j] != -1) {
                sum += graph->bridges[i][j];
                if (sum > LONG_MAX)
                    return false;
            }
    if (sum > INT_MAX)
        return false;
    return true;
}
