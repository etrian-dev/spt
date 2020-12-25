// finds the SPT in a digraph G = (N, A)
// by using the Bellman-Ford algorithm (uses a queue)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Glib headers for the queue and the list
#include <glib.h>

// readline is used to read the input graph
#include <readline/readline.h>

#include "glib-graph.h"

int main(void) {
    // reads the vertices and edges, then the graph
    int vertices;
    float max_w = -INFINITY;  // store smallest value, gets compared lt to any float
    char* line = NULL;

    line = readline("Enter the number of vertices: ");
    vertices = atoi(line);
    free(line);

    // The graph is an adjacency list (list of nodes) doubly linked
    // of nodes, which in turn contain a singly linked edge list
    GList* graph = NULL;

    // dummy list and other variables
    GSList* adjlist = NULL;
    int dest = -1;
    float weight = 0;
    Node* n = NULL;
    Edge* e = NULL;
    char* token = NULL;
    int i;

    for (i = 0; i < vertices; i++) {
        // reads the adjacency list of vertex i
        line = readline(NULL);

        // tokenizes it: tokens are separated by " "
        token = strtok(line, " ");

        while (token) {
            // parses the token in the destination vertex and the edge's weight
            sscanf(token, "%d:%f", &dest, &weight);
            // allocates memory for the edge
            e = (Edge*)malloc(sizeof(Edge));
            if (e == NULL) {
                puts("Failed to alloc Edge");
                exit(1);
            }
            e->destination = dest;
            e->weight = weight;
            // prepends the Edge e to the adjacency list
            adjlist = g_slist_prepend(adjlist, e);

            //update the max weight if greater than current
            if (max_w < weight) {
                max_w = weight;
            }

            // get the next token
            token = strtok(NULL, " ");
        }
        // input line freed
        free(line);

        // Alloc another node
        n = (Node*)malloc(sizeof(Node));
        if (!n) {
            puts("Failed to alloc Node");
            exit(1);
        }
        // Initialize the node with its label and the adjlist
        n->vertex = i;
        n->adjacent = adjlist;
        // Then append to the graph
        // TODO: maybe prepend then reverse at the end
        graph = g_list_append(graph, n);
        
        // then the adjacency list is resetted without deallocating anything
        adjlist = NULL; 
    
    }
    // dummy list set to NULL just to be safe
    adjlist = NULL;

    // prints the graph
    puts("GRAPH");
    print_graph(graph, vertices);

    // choose the root node
    int root;
    line = readline("Enter the root node: ");
    root = atoi(line);
    free(line);
    line = NULL;

    // fifo list Q    
    // defines and initializes a queue
    GQueue* Q = g_queue_new();

    // each node i has a label (the cost of the path from the root to i)
    float* labels = (float*)malloc(vertices * sizeof(float));
    // each node j has a predecessor i: in the tree there is the edge i -> j
    int* predecessors = (int*)malloc(vertices * sizeof(int));

    float max_path = (float)(vertices - 1) * max_w + 1;
    // build the initial tree, by putting all the labels to 1 + the max possile path in the graph
    for (i = 0; i < vertices; i++) {
        if (i != root) {
            labels[i] = max_path;
        }
        else {
            labels[root] = 0; // path root ----> root is set to 0
        }
        predecessors[i] = root; // all nodes have root as their predecessor
    }
    // after initializing the tree, the tail elements of those edges who violate bellman conditions
    // must be inserted in Q. In this case, just the root is violating them, because of how the tree is built
    g_queue_push_tail(Q, GINT_TO_POINTER(root));
    printf("inserted %d\n", root);

    //Element* u = NULL;
    int u = 0;
    int count_it = 0; // just counts the iterations

    // while Q is not empty, loop
    while (!g_queue_is_empty(Q)) {
        // updates the number of iterations
        count_it++;

        // in Bellman-Ford Q is FIFO and dequeued at each iteration
        u = GPOINTER_TO_INT(g_queue_pop_head(Q));
        // get u's adjacency list in the graph
        n = g_list_nth_data(graph, u);
        adjlist = n->adjacent;

        // check each forward edge from u
        for (adjlist = n->adjacent; adjlist != NULL; adjlist = adjlist->next) {
            // get the edge
            e = adjlist->data;
            // check if the edge (u, i) violates the corresponding Bellman condition
            if (labels[e->destination] > labels[u] + e->weight) {
                printf("(%d, %d) violates Bellman\n", u, e->destination);
                printf("d_%d\t+\tc_%d_%d\t<\td_%d\n", u, u, e->destination, e->destination);
                printf("%.3f\t+\t%.3f\t<\t%.3f\n", labels[u], e->weight, labels[e->destination]);

                // update labels[i] (not its tree!) and its predecessor if it changes
                labels[e->destination] = labels[u] + e->weight;
                if (predecessors[e->destination] != u) {
                    predecessors[e->destination] = u;
                }
                // inserts the head of the edge (vertex i) in Q if it's not already in there
                if (g_queue_find(Q, GINT_TO_POINTER(e->destination)) == NULL) {
                    g_queue_push_tail(Q, GINT_TO_POINTER(e->destination));
                }
            }
        }
    }

    // free the queue (no dynamically alloc'd data stored
    g_queue_free(Q);

    printf("After %d iterations, the SPT found by Bellman-Ford is\n", count_it);
    // the resulting spt is represented by labels & predecessors
    for (i = 0; i < vertices; i++) {
        printf("label[%d] = %.3f\tpred[%d] = %d\n", i, labels[i], i, predecessors[i]);
    }
}
