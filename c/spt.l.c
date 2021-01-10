// finds the SPT in a digraph G = (N, A)
// by using the Bellman-Ford algorithm (uses a queue)
// in O(mn) time

// my utility for manipulating graphs
#include <glib-graph.h>
// libreadline for input
#include <readline/readline.h>
// Glib header for data structures (GList, GQueue, ...)
#include <glib.h>
// standard lib headers
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// main function: reads a digraph from stdin, then applies Bellman-Ford on it
// to find the SPT (Shortest Paths Tree)
int main(void) {

    float max_w; // stores the max edge weight in the graph
    // create and read a new graph from stdin
    Graph graph = new_graph(&max_w);

    // print the graph to stdout
    puts("GRAPH");
    print_graph(stdout, graph);

    // choose the root node
    int root;
    char *line;
    line = readline("Enter the root node: ");
    // node label is assumed to be an integer that can be converted using atoi
    root = atoi(line);

    // mem free
    free(line);
    line = NULL;

    // create a new GQueue (defined by GLib), a fifo list to store visited vertices
    GQueue *Q = g_queue_new();

    // each node i has a label (the cost of the current path from root to i)
    float *labels = (float *)malloc(graph.order * sizeof(float));

    // a node j has a predecessor i <=> in the SPT there is an edge i -> j
    int *predecessors = (int *)malloc(graph.order * sizeof(int));

    // the most expensive path possible in the graph is used as a fake edge weight
    float max_path = (float)(graph.order - 1) * max_w + 1;

    // array to store how many times a node has been removed from Q
    // when any node reaches n insertions (and subsequent remotions),
    // then a negative cycle is found is present in the graph
    // => the instance of the problem has no lower limit
    int *count_rm = (int *)calloc(graph.order, sizeof(int));
    // memory set to 0 by calloc, no need to initialize manually

    // build the initial tree, by setting all the labels to max_path + 1.0
    int i;
    for (i = 0; i < graph.order; i++) {
        if (i != root) {
            labels[i] = max_path + 1.0;
        }
        else {
            labels[root] = 0; // the trivial path root ----> root is set to 0
        }
        predecessors[i] = root; // all nodes have root as their predecessor
    }

    // the tail nodes of those edges who violate bellman conditions
    // must be inserted in Q. In this case, only the root is violating them,
    // because of how the tree is built
    g_queue_push_tail(Q, GINT_TO_POINTER(root));
    // the data must be stored as a gpointer, so the macro converts to it from int

    // some dummy variables
    Edge *e = NULL;
    //Node *n = NULL;
    GSList *adjlist = NULL;
    int u = 0;
    int count_it = 0; // just counts the iterations of the algorithm
    bool neg_cycle = false; // set to true if a negative cycle is found, exiting the loop

    // while Q is not empty and a negative cycle hasn't been found
    while (!(g_queue_is_empty(Q) || neg_cycle)) {
        // updates the number of iterations
        count_it++;

        // in Bellman-Ford Q is FIFO and, so the head is popped at each iteration
        u = GPOINTER_TO_INT(g_queue_pop_head(Q));
        // data in Q is a gpointer, the macro converts back to integer

        // if there's a negative cycle (a node has been removed |N| == graph order times)
        // then the loop exits
        count_rm[u]++;
        if(count_rm[u] == graph.order) {
            neg_cycle = true;
            continue;
        }

        // CHECKS BELLMAN CONDITIONS ON THE FORWARD EDGES FROM U

        // get u's adjacency list in the graph
        adjlist = ((Node *)g_list_nth_data(graph.nodes, u))->adjacent;

        // check each forward edge from u
        while (adjlist != NULL) {
            // get the edge
            // the casting is implicit, but useful to understand what's going on
            e = (Edge *)(adjlist->data);
            // check if the edge (u, i) violates the Bellman condition
            // BELLMAN: d_u + c_ui >= d_i => OK, otherwise the condition is violated
            if (labels[e->destination] > labels[u] + e->weight) {
                printf("(%d, %d) violates Bellman\n", u, e->destination);
                printf("d_%d\t+\tc_%d_%d\t<\td_%d\n", u, u, e->destination, e->destination);
                printf("%.3f\t+\t%.3f\t<\t%.3f\n", labels[u], e->weight, labels[e->destination]);

                // update labels[i] (not its subtree) and change the predecessor if needed
                labels[e->destination] = labels[u] + e->weight;
                if (predecessors[e->destination] != u) {
                    predecessors[e->destination] = u;
                }
                // inserts in the queue the edge head in Q if absent
                if (g_queue_find(Q, GINT_TO_POINTER(e->destination)) == NULL) {
                    g_queue_push_tail(Q, GINT_TO_POINTER(e->destination));
                }
            }

            // get the next edge
            adjlist = adjlist->next;
        }
    }

    // free the empty queue
    g_queue_free(Q);

    if(neg_cycle) {
        puts("Negative cycle! No lower bound");
    }
    else {
        printf("After %d iterations, the SPT found by Bellman-Ford is\n", count_it);
        // the resulting spt is represented by labels & predecessors
        float spt_cost = 0.0;
        for (i = 0; i < graph.order; i++) {
            printf("label[%d] = %.3f\tpred[%d] = %d\n", i, labels[i], i, predecessors[i]);
            spt_cost += labels[i];
        }
        // the total cost is also printed to stdout
        printf("Total cost of the SPT: %f\n", spt_cost);
    }

    // free the graph nodes
    g_list_free(graph.nodes);

    // free the various helper arrays
    free(labels);
    free(predecessors);
    free(count_rm);
}
