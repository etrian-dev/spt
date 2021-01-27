// finds the SPT in a digraph G = (N, E) with the Bellman-Ford algorithm in O(|V|*|E|) time

/*
 * spt.l.c
 * This file is part of spt.l
 *
 * Copyright (C) 2021 - etrian-dev
 *
 * spt.l is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * spt.l is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with spt.l. If not, see <http://www.gnu.org/licenses/>.
 */

// my functions to handle graph reading
#include "glib-graph.h"

#include <readline/readline.h> // for reading input lines
#include <glib.h> // Glib header for data structures (GList, GQueue, ...)

// standard library headers
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 * Definition: Bellman condition
 * Given two vertices i and j in the directed graph G = (N, A) such that 
 * d_i + c_ij >= d_j, where d_x is the cost of the path in the current SPT
 * to go from the root to vertex x and c_ij is the cost of edge i -> j in the graph.
 * If the above inequality holds, then the bellman condition is said to be satisfied, 
 * otherwise it's not satisfied.
 */

// main function: reads a digraph from stdin, then applies Bellman-Ford on it
// to find the SPT (Shortest Paths Tree) with the given root node (also read from stdin)
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
    // node label is assumed to be an integer that can be converted safely using atoi
    root = atoi(line);

    free(line);
    line = NULL;

    // creates an empty queue (FIFO list) to store nodes
    GQueue *Q = g_queue_new();

    // each node has a label: the cost of the current shortest path from root to i
    float *labels = (float *)malloc(graph.order * sizeof(float));

    // a node j has a predecessor i in the SPT <=> in the SPT there is an edge i -> j
    int *predecessors = (int *)malloc(graph.order * sizeof(int));

    // the most expensive path (plus a constant) is used as an upper limit for an edge weight
    float max_path = (float)(graph.order - 1) * max_w + 1.0;

    // array to store how many times a node has been removed from Q
    // when any node reaches n insertions (and subsequent extractions),
    // then it's proven that the graph contains a cycle with negative weight
    // and thus the instance's optimal solution has no lower bound (-inf)
    int *count_rm = (int *)calloc(graph.order, sizeof(int));
    // the initial values are already zeroed by calloc, no need to initialize them

    // the initial tree is built from fake edges with maximum weight (max_path)
    int i;
    for (i = 0; i < graph.order; i++) {
        if (i != root) {
            labels[i] = max_path;
        }
        else {
            labels[root] = 0;
        }
        predecessors[i] = root;
    }

    // the tail nodes of those edges who violate bellman conditions
    // must be inserted in Q. In this case, only the root is violating them,
    // because of how the tree is built
    g_queue_push_tail(Q, GINT_TO_POINTER(root));
    // an element in Q must be stored as a gpointer, so the macro converts it from int

    Edge *e = NULL;
    GSList *adjlist = NULL;
    // just counts the number of iterations made by the algorithm
    int count_it = 0;
    // flag that signals the presence of cycles whose total cost is negative
    bool neg_cycle = false;

    // iterate while Q is not empty and a negative cycle hasn't been found
    while (!(g_queue_is_empty(Q) || neg_cycle)) {
        count_it++;

        i = GPOINTER_TO_INT(g_queue_pop_head(Q));
        // data in Q is a gpointer, the macro converts it back to int

        // check if there's a negative cycle (a node has been removed |N| times)
        count_rm[i]++;
        if(count_rm[i] == graph.order) {
            neg_cycle = true;
        }

        // checks bellman conditions of the forward edges from i

        // get i's adjacency list in the graph
        adjlist = ((Node *)g_list_nth_data(graph.nodes, i))->adjacent;
        while (adjlist != NULL) {
            // the cast to Edge* should't be needed, but clarifies what the code is doing
            e = (Edge *)(adjlist->data);
            // edge (i, e->destination) satisfies the Bellman condition?
            if (labels[e->destination] > labels[i] + e->weight) {
                printf("(%d, %d) violates Bellman\n", i, e->destination);
                printf("d_%d\t+\tc_%d_%d\t<\td_%d\n", i, i, e->destination, e->destination);
                printf("%.3f\t+\t%.3f\t<\t%.3f\n", labels[i], e->weight, labels[e->destination]);

                // update the label of e->destioation with the shorter path passing from i
                // its subtree's labels should be updated as well, but this can be delayed
                // (labels update delay) to speed up the execution
                labels[e->destination] = labels[i] + e->weight;
                // if necessary, change the predecessor of e->destination
                if (predecessors[e->destination] != i) {
                    predecessors[e->destination] = i;
                }
                // put e->destination in Q, since its forward edges can violate Bellman
                if (g_queue_find(Q, GINT_TO_POINTER(e->destination)) == NULL) {
                    g_queue_push_tail(Q, GINT_TO_POINTER(e->destination));
                }
            }
            // get the next edge in the list
            adjlist = adjlist->next;
        }
    }

    g_queue_free(Q);

    if(neg_cycle) {
        puts("Negative cycle! No lower bound.");
    }
    else {
    	// the resulting spt is represented by labels & predecessors
        printf("After %d iterations, the SPT found by Bellman-Ford is\n", count_it);
        float spt_cost = 0.0;
        for (i = 0; i < graph.order; i++) {
            printf("label[%d] = %.3f\tpred[%d] = %d\n", i, labels[i], i, predecessors[i]);
            spt_cost += labels[i]; // computes the SPT's cost: the sum of all the labels
        }
        printf("Total cost of the SPT: %f\n", spt_cost);
    }

	// all the necessary frees
    g_list_free(graph.nodes);
    free(labels);
    free(predecessors);
    free(count_rm);
}
