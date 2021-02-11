// Finds the SPT of a weighted directed graph G = (V, E) using Dijkstra's algorithm
// The time complexity is O(|V|^2) if and only if all weights are positive
// It will NOT terminate if there are negative cycles in the graph (use spt_l)

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
// the header file where this function is declared
#include "spt.h"

#include <glib.h> // Glib header for data structures (GList, GQueue, ...)

#include <stdio.h>
#include <stdlib.h>

// A q_element is the function's representation of a node in the SPT:
struct q_element
{
    int vertex;      // the node's identifier
    int predecessor; // the node's predecessor in the current SPT
    float label;     // the cost of the shortest path from root to vertex
};
typedef struct q_element *Element;

// GComparisonFunc to compute the relative priority of Elements
// the priority in this context is given by its label field: an element with lower
// label must come before another with a higher label
// The user_data parameter is just ignored
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

// spt_l applies Dijkstra on the graph G based on the list of roots
// and outputs the labels and predecessors arrays that represent (one of)
// the shortest paths tree
int spt_s(
  Graph *G,
  GArray *roots,
  float max_path,
  float *labels,
  int *predecessors
)
{
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

  // SPT.S implements the set Q as a priority queue
  // a list ordered by the smallest label of its elements
  GQueue *Q = g_queue_new();

  int i;
  // alloc an array of elements:
  // any vertex of the |V| vertices can be inserted in the queue,
  // so it's handy to reserve space in advance
  Element *vertices = (Element *)malloc(G->order * sizeof(Element));
  if (vertices == NULL)
  {
      g_error("Failed to alloc elements array");
  }
  for (i = 0; i < G->order; i++)
  {
      vertices[i] = (struct q_element *)malloc(sizeof(struct q_element));
      if (vertices[i] == NULL)
      {
          g_error("Failed to alloc queue element");
      }

      // An initial tree is needed to start the algorithm; a simple way to obtain such
      // a tree is to connect all nodes to the root with max_w as their edge weight
      // so that this edge will always violate Bellman conditions
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

  // Q is initialized with all the tail nodes of those edges violating
  // bellman conditions; only root meets these conditions at initialization
  g_queue_push_head(Q, (void *)vertices[root]);
  // the data must be stored as a gpointer (just a fancy void*)

#ifdef DEBUG // prints the insertion of root in Q
  g_print("Put\n\tvertex: %d\n\tlabel: %f\n\tpred: %d\n",
          vertices[root]->vertex,
          vertices[root]->label,
          vertices[root]->predecessor);
#endif

  // Counts the number of iterations made by the algorithm
  int count_it = 0;
  // Other dummy variables
  Edge *e = NULL;
  GList *dummy_list = NULL;
  GSList *adjlist = NULL;
  Element u = 0;

  // while Q is not empty, iterate
  while (!g_queue_is_empty(Q))
  {
    count_it++;

    // in Dijkstra (SPT.S) Q is a priority queue, so the element with the highest
    // priority is the head of the list at each iteration.
    u = (Element)g_queue_pop_head(Q);
    // the cast is needed to convert from gpointer

#ifdef DEBUG // prints the extacted vertex
    g_print("Extracted\n\tvertex: %d\n\tlabel: %f\n\tpred: %d\n",
            u->vertex,
            u->label,
            u->predecessor);
#endif

    // Check bellman conditions of the forward edges from u

    // get i's adjacency list
    // TODO: Replace with the correct call to g_list_find_custom to find
    // the i-th vertex's adjacency list
    dummy_list = G->nodes;
    while(dummy_list && ((Node *)dummy_list->data)->vertex != u->vertex) {
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
    while (adjlist != NULL)
    {
      e = (Edge *)adjlist->data;
      // edge (u, e->destination) satisfies the Bellman condition?
      if (vertices[u->vertex]->label + e->weight < vertices[e->destination]->label)
      {
        printf("(%d, %d) violates Bellman\n", u->vertex, e->destination);
        printf("d_%d\t+\tc_%d_%d\t<\td_%d\n", u->vertex, u->vertex, e->destination, e->destination);
        printf("%.3f\t+\t%.3f\t<\t%.3f\n", vertices[u->vertex]->label, e->weight, vertices[e->destination]->label);

        // update the label of e->destination; delays update on the subtree
        // to subsequent iteration to speed up the execution
        vertices[e->destination]->label = vertices[u->vertex]->label + e->weight;

        if (vertices[e->destination]->predecessor != u->vertex)
        {
          vertices[e->destination]->predecessor = u->vertex;
        }
        // if the vertex e->destination is not in the prioqueue
        // inserts it while maintaining the queue sorted by lowest label
        if (g_queue_find(Q, (void *)vertices[e->destination]) == NULL)
        {
          // inserts maintaining sorting by priority (calls smallest_label to compare)
          g_queue_insert_sorted(Q, (void *)vertices[e->destination], smallest_label, NULL);

#ifdef DEBUG
          g_print("Put\n\tvertex: %d\n\tlabel: %f\n\tpred: %d\n",
                  vertices[e->destination]->vertex,
                  vertices[e->destination]->label,
                  vertices[e->destination]->predecessor);
#endif
        }
      }

      // gets the next edge in this node's adjacency list
      adjlist = adjlist->next;
    }
  }
  g_queue_free(Q);

  // If there was more than one root (the algorithm ran on a hyper-root)
  // perform cleanup by removing it from the graph and updating the outputs accordingly
  for(i = 0; i < G->order; i++) {
    if(vertices[i]->predecessor == root) {
      vertices[i]->predecessor = i;
    }
    
    // Copy the resulting spt in the given arrays as well
    labels[i] = vertices[i]->label;
    predecessors[i] = vertices[i]->predecessor;
  }
  graph_remove_hyper_root(G);

  // and then free the array of Elements
  for (i = 0; i < G->order; i++)
  {
    free(vertices[i]);
  }
  free(vertices);

  // then returns to the caller the number of iterations needed to find the SPT
  return count_it;
}
