// Finds the SPT of a weighted directed graph G = (N, A), by using
// Dijkstra's algorithm (adapted for finding the shortest path tree).
// The time complexity is T(|V|, |E|) = O(|V|^2) if and only if all weights are
// positive (integers or floating point), exponential otherwise (in the worst case).
// It may not terminate at all if there are negative cycles in the graph (see spt.l)

/*
 * spt.s.c
 * This file is part of spt.s
 *
 * Copyright (C) 2021 - etrian-dev
 *
 * spt.s is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * spt.s is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with spt.s. If not, see <http://www.gnu.org/licenses/>.
 */

// my functions to handle graph reading
#include "glib-graph.h"
// the header file for this function
#include "spt.h"

#include <glib.h> // Glib header for data structures (GList, GQueue, ...)

#include <stdio.h>
#include <stdlib.h>

// A queue element is the program's representation of a node in the SPT:
struct q_element
{
    int vertex;      // the node's identifier (int because it's simple and efficient)
    int predecessor; // the node's predecessor in the current SPT
    float label;     // the current shortest path from root to the node
};
typedef struct q_element *Element;

// glib-style comparison function to compute the priority of Elements
// the priority in this context is given by its label field
// user_data is just ignored
gint smallest_label(gconstpointer a, gconstpointer b, gpointer user_data)
{
    Element pa = (Element)a;
    Element pb = (Element)b;
    if (pa->label < pb->label)
    {
        return -1;
    }
    if (pa->label > pb->label)
    {
        return 1;
    }
    return 0;
}

// The program reads a graph and asks for a root node, then computes the SPT
int spt_s(
  Graph G,
  GArray *roots,
  float max_path,
  float *labels,
  int *predecessors
)
{
  int root;
  // If there's more than one root, then the graph is modified
  // A new node is added (super_root) and connected with edges
  // of weight 0 to the roots; the algorithm is then executed
  // on the modified graph
  if(roots->len > 1) {
    root = graph_add_hyper_root(&G, roots);
  }
  else {
    root = g_array_index(roots, int, 0);
  }
    // The new graph now has only one root either way
    // SPT.S implements the set Q as a priority queue
    // a list ordered by the smallest label of its elements
    GQueue *Q = g_queue_new();

    int i;
    // alloc an array of elements:
    // any vertex of the |N| vertices can be inserted in the queue,
    // so it's handy to reserve space in advance
    Element *vertices = (Element *)malloc(G.order * sizeof(Element));
    if (vertices == NULL)
    {
        g_error("Failed to alloc elements array");
    }
    for (i = 0; i < G.order; i++)
    {
        vertices[i] = (struct q_element *)malloc(sizeof(struct q_element));
        if (vertices[i] == NULL)
        {
            g_error("Failed to alloc queue element");
        }

        // build the initial tree, by setting all the labels (except root's)
    	// to max_path and all predecessors as the root of the tree
        if (i != root)
        {
            vertices[i]->label = max_path;
        }
        else
        {
            vertices[root]->label = 0.0; // root's label is set to 0 as obvious
        }
        vertices[i]->vertex = i;
        vertices[i]->predecessor = root; // all nodes have root as their predecessor
    }

    // Q is initialized with all the tail nodes of those edges who violate bellman conditions
    // BELLMAN: d_u + c_ui >= d_i => OK, otherwise the condition is violated
    // In this case, only the root is violating them, because of how the tree is built
    g_queue_push_head(Q, (void *)vertices[root]);
    // the data must be stored as a gpointer (just a fancy void*)

#ifdef DEBUG
    g_print("Put\n\tvertex: %d\n\tlabel: %f\n\tpred: %d\n",
            vertices[root]->vertex,
            vertices[root]->label,
            vertices[root]->predecessor);
#endif

    // some dummy variables
    Edge *e = NULL;
    //Node *n = NULL;
    GSList *adjlist = NULL;
    Element u = 0;
    int count_it = 0; // just counts the iterations of the algorithm

    // while Q is not empty, iterate
    while (!g_queue_is_empty(Q))
    {
        count_it++;

        // in Dijkstra (SPT.S) Q is a priority queue, so the element with the highest
        // priority is the head of the list at each iteration.
        u = (Element)g_queue_pop_head(Q);
        // the cast is needed to convert from gpointer

#ifdef DEBUG
        g_print("Extracted\n\tvertex: %d\n\tlabel: %f\n\tpred: %d\n",
                u->vertex,
                u->label,
                u->predecessor);
#endif

        // CHECKS BELLMAN CONDITIONS ON THE FORWARD EDGES FROM U

        // get u's adjacency list in the graph (cast to Node* from gpointer [void *])
        adjlist = ((Node *)g_list_nth_data(G.nodes, u->vertex))->adjacent;

        // check each forward edge from u
        while (adjlist != NULL)
        {
            // the explicit cast is not strictly necessary, but useful to understand what's going on
            e = (Edge *)adjlist->data;
            // check if the edge e violates the Bellman condition
            // BELLMAN: d_u + c_ui >= d_i => OK, otherwise the condition is violated
            if (vertices[e->destination]->label > vertices[u->vertex]->label + e->weight)
            {
                printf("(%d, %d) violates Bellman\n", u->vertex, e->destination);
                printf("d_%d\t+\tc_%d_%d\t<\td_%d\n", u->vertex, u->vertex, e->destination, e->destination);
                printf("%.3f\t+\t%.3f\t<\t%.3f\n", vertices[u->vertex]->label, e->weight, vertices[e->destination]->label);

                // update the head node's label (not its subtree)
                vertices[e->destination]->label = vertices[u->vertex]->label + e->weight;
                // if the predecessor changes, update it
                if (vertices[e->destination]->predecessor != u->vertex)
                {
                    vertices[e->destination]->predecessor = u->vertex;
                }
                // if the vertex e->destination is not in the prioqueue
                // inserts it while maintaining the queue sorted by lowest label
                if (g_queue_find(Q, (void *)vertices[e->destination]) == NULL)
                {
                    // inserts sorted by using the smallest_label compare function
                    g_queue_insert_sorted(Q, (void *)vertices[e->destination], smallest_label, NULL);

#ifdef DEBUG
                    g_print("Put\n\tvertex: %d\n\tlabel: %f\n\tpred: %d\n",
                            vertices[e->destination]->vertex,
                            vertices[e->destination]->label,
                            vertices[e->destination]->predecessor);
#endif
                }
            }

            // get the next edge
            adjlist = adjlist->next;
        }
    }

    printf("After %d iterations, the SPT found by Dijkstra is\n", count_it);
    // the resulting spt is represented by labels & predecessors
    float spt_cost = 0.0;
    for (i = 0; i < G.order; i++)
    {
        printf("label[%d] = %.3f\tpred[%d] = %d\n", i, vertices[i]->label, i, vertices[i]->predecessor);
        spt_cost += vertices[i]->label;
    }
    // the total cost is also printed to stdout
    printf("Total cost of the SPT: %f\n", spt_cost);

	// all the necessary frees
    for (i = 0; i < G.order; i++)
    {
        free(vertices[i]);
    }
    free(vertices);
    g_queue_free(Q);

    return 0;
}
