#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

void print_graph(Graph G, const int order) {
    int i, j;
    for (i = 0; i < order; i++) {
        printf("%d -> ", i);
        for (j = 0; j < G[i].out_degree - 1; j++) {
            printf("%d (%d), ", G[i].adjacent[j], G[i].weights[j]);
        }
        printf("%d (%d)\n", G[i].adjacent[j], G[i].weights[j]);
    }
    putchar('\n');
}

// read a weighted digraph in a matrix N x N, keeps track of max edge in the graph
Graph read_graph(const int vertices, int* max_edge) {
    Graph G = NULL; // the graph is just an array

    // formatting rules
    puts("Format: out-deg to_1:w_1 to_2:w2 ...");

    // allocates memory for the adjacency list of the graph
    G = (Graph)calloc(vertices, sizeof(Node));

    int out_deg, i, j, to, w;
    for (i = 0; i < vertices; i++) {
        // read the out-deg of node i
        scanf("%d", &out_deg);
        G[i].out_degree = out_deg;
        // alloc some memory to store them
        G[i].adjacent = (int*)calloc(out_deg, sizeof(int));
        G[i].weights = (int*)calloc(out_deg, sizeof(int));

        // reads edge from -> to of weight w (weights are integers at the moment)
        for (j = 0; j < G[i].out_degree; j++) {
            scanf("%d:%d", &to, &w);
            G[i].adjacent[j] = to;
            G[i].weights[j] = w;

            // update max edge weight in the graph if necessary
            if (w > *max_edge) {
                *max_edge = w;
            }
        }
    }

    return G;
}