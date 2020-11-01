// finds the SPT in a digraph G = (N, A)
// by using the Bellman-Ford algorithm (uses a queue)

// my headers
#include "queue.h"
#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(void) {
    // reads the vertices and edges, then the graph
    int vertices, edges;
    int max_w = INT_MIN; // initializes max weight to the minimum integer
    printf("Enter the number of vertices and edges: ");
    scanf("%d %d", &vertices, &edges);

    Graph G = read_graph(vertices, &max_w);

    // prints the graph
    puts("GRAPH");
    print_graph(G, vertices);

    // choose the root node
    int root, i;
    printf("Enter the root node [0, %d]: ", vertices-1);
    scanf("%d", &root);

    // fifo list Q
    Queue Qhead = NULL;
    Queue Qtail = NULL;

    // each node i has a label (the cost of the path from the root to i)
    int *labels = (int*) malloc(vertices * sizeof(int));
    // each node j has a predecessor i: in the tree there is the edge i -> j
    int *predecessors = (int*) malloc(vertices * sizeof(int));

    int max_path = (vertices - 1) * max_w + 1;
    // build the initial tree, by putting all the labels to 1 + the max possile path in the graph
    for(i = 0; i < vertices; i++) {
        if(i != root) {
            labels[i] = max_path;
        }
        else {
            labels[root] = 0; // path root ----> root is set to 0
        }
        predecessors[i] = root; // all nodes have root as their predecessor
    }
    // after initializing the tree, the tail elements of those edges who violate bellman conditions
    // must be inserted in Q. In this case, just the root is violating them, because of how the tree is built
    enqueue(&Qhead, &Qtail, root);

    Element* u = NULL;
    int count_it = 0; // just counts the iterations
    // while Q is not empty, loop
    while(Qhead != NULL) {
        count_it++;
        // in Bellman-Ford Q is FIFO and at each iteration Q is dequeued
        u = dequeue(&Qhead, &Qtail);

        // check each forward edge from u->value
        for (i = 0; i < G[u->value].out_degree; i++) {

            // check if the edge (u->value, i) violates the corresponding Bellman condition
            if (labels[G[u->value].adjacent[i]] > labels[u->value] + G[u->value].weights[i]) {
                printf("(%d, %d) violates Bellman\n", u->value, G[u->value].adjacent[i]);
                printf("d_%d\t+\tc_%d_%d\t<\td_%d\n", u->value, u->value, G[u->value].adjacent[i], G[u->value].adjacent[i]);
                printf("%d\t+\t%d\t<\t%d\n", labels[u->value], G[u->value].weights[i], labels[G[u->value].adjacent[i]]);

                // update labels[i] (not its tree!) and its predecessor if it changes
                labels[G[u->value].adjacent[i]] = labels[u->value] + G[u->value].weights[i];
                if (predecessors[G[u->value].adjacent[i]] != u->value) {
                    predecessors[G[u->value].adjacent[i]] = u->value;
                }
                // inserts i in Q if it's not already in there
                if (!isIn(Qhead, G[u->value].adjacent[i])) {
                    enqueue(&Qhead, &Qtail, G[u->value].adjacent[i]);
                }
            }
        }
        // frees the element extracted from the queue
        free(u);
    }

    printf("After %d iterations, the SPT found by Bellman-Ford is\n", count_it);
    // the resulting spt is represented by labels & predecessors
    for(i = 0; i< vertices; i++) {
        printf("label[%d] = %d\tpred[%d] = %d\n", i, labels[i], i, predecessors[i]);
    }
}
