#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

typedef struct node_t {
    int out_degree;
    int* adjacent;
    int* weights;
} Node;
typedef Node* Graph;

void print_graph(Graph G, const int order);
Graph read_graph(const int vertices, int* max_edge);

#endif