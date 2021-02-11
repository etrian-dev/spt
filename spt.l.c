// Finds (one of) the SPT in a directed graph G = (V, E) using the Bellman-Ford algorithm
// The time complexity is O(|V|*|E|)

/*
 * spt.l.c
 * This file is part of spt
 *
 * Copyright (C) 2021 - etrian-dev
 *
 * spt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * spt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with spt. If not, see <http://www.gnu.org/licenses/>.
 */

// my functions to handle graph reading
#include "glib-graph.h"
// the header file where this function is declared
#include "spt.h"

#include <glib.h> // Glib header for data structures (GList, GQueue, ...)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // to use boolean constants from C99, could be avoided

// spt_l applies Bellman-Ford on the graph G based on the list of roots
// and outputs the labels and predecessors arrays that represent (one of)
// the shortest paths tree
int spt_l(
  Graph *G,
  GArray *roots,
  float max_path,
  float *labels,
  int *predecessors
) {
    // The algorithm supports multiple roots, by adding a node connected to all
    // the roots with weigth 0 and applying the procedure on the modified graph
    // and the new node as the root of the spt. Afterwards, the node is removed
    int root;
    if(roots->len > 1) {
      root = graph_add_hyper_root(G, roots);
    }
    else {
      root = g_array_index(roots, int, 0);
    }

#ifdef DEBUG // prints the modified graph
    puts("GRAPH");
    print_graph(stdout, *G);
#endif

    // creates an empty queue (FIFO list) to store nodes that violate Bellman conditions
    GQueue *Q = g_queue_new();

    // An internal array to store how many times a node has been removed from Q
    // When any node reaches n insertions (and subsequent extractions),
    // then it's proved that the graph contains a cycle with total weight < 0
    // =>
    // The given graph's optimal solution has no lower bound (-inf)
    int *count_rm = (int *)calloc(G->order, sizeof(int));

    // An initial tree is needed to start the algorithm; a simple way to obtain such
    // a tree is to connect all nodes to the root with max_w as their edge weight
    // so that this edge will always violate Bellman conditions
    int i;
    for (i = 0; i < G->order; i++) {
        if (i != root) {
            labels[i] = max_path;
        }
        else {
            labels[i] = 0;
        }
        predecessors[i] = root;
    }

    // The tail nodes of those edges who violate Bellman conditions
    // must be inserted in Q. In this case, only the root is violating them,
    // because of how the initial tree has been built
    g_queue_push_tail(Q, GINT_TO_POINTER(root));
    // An element in Q must be stored as a gpointer, so the macro converts it from int

    // Counts the number of iterations made by the algorithm
    int count_it = 0;
    // Flag that signals the presence of cycles whose total cost is negative
    bool neg_cycle = false;
    // Other dummy variables
    Edge *e = NULL;
    GSList *adjlist = NULL;
    GList *dummy_list = NULL;

    // iterate while Q is not empty and a negative cycle hasn't been found
    while (!(g_queue_is_empty(Q) || neg_cycle)) {
        count_it++;

        i = GPOINTER_TO_INT(g_queue_pop_head(Q));
        // data in Q is a gpointer, the macro converts it back to int

        // check if there's a negative cycle (a node has been removed |V| times)
        count_rm[i]++;
        if(count_rm[i] == G->order) {
            neg_cycle = true;
        }

        // Check bellman conditions of the forward edges from i

        // get i's adjacency list
        // TODO: Replace with the correct call to g_list_find_custom to find
        // the i-th vertex's adjacency list
        dummy_list = G->nodes;
        while(dummy_list && ((Node *)dummy_list->data)->vertex != i) {
          dummy_list = dummy_list->next;
        }
        adjlist = ((Node *)dummy_list->data)->adjacent;

#ifdef DEBUG // prints the adjacency list of node i
        g_print("Node %d\'s adjacency list:\n[\n", ((Node *)dummy_list->data)->vertex);
        GSList *ss = adjlist;
        while(ss) {
          Edge *ee = ss->data;
          g_print("\t{dest = %d, weight = %.3f} ->\n", ee->destination, ee->weight);
          ss = ss->next;
        }
        g_print("\tNULL\n]\n");
#endif

        // Iterate over all the elements in the list
        while (adjlist != NULL) {
            e = (Edge *)adjlist->data;

            if (labels[e->destination] > labels[i] + e->weight) {
                printf("(%d, %d) violates Bellman\n", i, e->destination);
                printf("d_%d\t+\tc_%d_%d\t<\td_%d\n", i, i, e->destination, e->destination);
                printf("%.3f\t+\t%.3f\t<\t%.3f\n", labels[i], e->weight, labels[e->destination]);

                // update the label of e->destination; delays update on the subtree
                // to subsequent iteration to speed up the execution
                labels[e->destination] = labels[i] + e->weight;

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
    free(count_rm);

    // If there was more than one root (the algorithm ran on a hyper-root)
    // perform cleanup by removing it from the graph and updating the outputs accordingly
    for(i = 0; i < G->order; i++) {
      if(predecessors[i] == root) {
        predecessors[i] = i;
      }
    }
    graph_remove_hyper_root(G);

    // then returns to the caller the number of iterations performed
    if(neg_cycle) {
      return NO_LOWER_BOUND; // special value returned to signal no lower bound
    }
    return count_it;
}
