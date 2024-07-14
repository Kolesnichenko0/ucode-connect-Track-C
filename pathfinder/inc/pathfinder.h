#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "libmx.h"

typedef struct s_graph {
    char** islands;
    int** bridges;
    long island_count;
    bool mustClear;
    bool isFull;
    bool isDuplicateBridges;
}       t_graph;

typedef struct s_path_vertex {
    bool isVisited;
    int distance_from_initial;
    long island_index;
    t_list *previous;
}              t_path_vertex;

enum e_error {
    FILE_DOES_NOT_EXIST,
    EMPTY_FILE,
    INVALID_LINE,
    INVALID_GIVEN_NUMBER_OF_ISLANDS,
    BRIDGES_ARE_DUPLICATED,
    INVALID_BRIDGES_SUM,
};

//File handling and parsing
void validate_file(const char* filename);
t_graph* parse_file(int fd, const char* filename);

//Graph
t_graph *create_graph(long givenNumberOfIslands);
void clear_graph(t_graph *graph, long givenNumberOfIslands);
long get_island_index(t_graph *graph, char *island);
void insert_island(t_graph *graph, char *island);
bool insert_length_of_bridge(t_graph *graph, long island1_index, long island2_index, int length_of_bridge);
bool validate_bridges_sum(t_graph *graph);

//Dijkstra's algorithm
t_path_vertex *find_path(t_graph *graph, long starting_index);
t_list *parse_all_paths_to(t_path_vertex *vertex);
void clear_vertices(t_path_vertex *vertices, long island_count);

//Output
void print_paths_from(t_graph *graph, t_path_vertex *vertices, long starting_index);

//Error handling
void print_error(enum e_error error, const void *data);

#endif
