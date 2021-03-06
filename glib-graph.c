// Graph reading utilities using Glib data structures
/*
 * glib-graph.c
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

// the header containing the declarations
#include "glib-graph.h"

// std lib  header for INFINITY: remember to link with -lm when compiling
#include <math.h>

/*
  Reads the graph from standard input with readline
  Must be stored in an adjacency list format
*/
Graph* new_graph(float *min_weight, float *max_weight) {
  // The graph is an adjacency list (doubly linked)
  // of nodes, which in turn contain a singly linked edge list
  Graph *g = (Graph *)malloc(sizeof(struct graph_t));
  if(!g) {
    g_error("Graph can't be alloc'd");
  }
  g->order = 0;
  g->nodes = NULL;

  // reads the order (number of vertices), then the graph
  char* line = NULL;
  line = readline("Enter the number of vertices: ");
  g->order = atoi(line); // assuming a valid integer

  free(line);
  line = NULL;

  // dummy list and other variables
  GSList* adjlist = NULL;
  int dest = -1;
  float weight = 0;
  *min_weight = INFINITY; // store biggest value: greater than any float
  *max_weight = -INFINITY; // store smallest value: less than any float
  Node* n = NULL;
  Edge* e = NULL;
  char* token = NULL;
  int i;

  for (i = 0; i < g->order; i++) {
      // reads a line containing the adjacency list of vertex i (no prompt)
      line = readline(NULL);

      // tokenizes the line: tokens are separated by " "
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
          if(*max_weight < weight) {
              *max_weight = weight;
          }
          if(*min_weight > weight) {
            *min_weight = weight;
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
      g->nodes = g_list_append(g->nodes, n);

      // then the adjacency list is resetted without deallocating anything
      adjlist = NULL;

  }
  // dummy list set to NULL just to be safe
  adjlist = NULL;

  return g;
}

void print_graph(FILE *target, Graph g) {
    // Dummy variables to explore the graphs
    Node* n = NULL;
    Edge* e = NULL;
    GList* iter = NULL;
    GSList* adj = NULL;
    for (iter = g.nodes; iter != NULL; iter = iter->next) {
        n = iter->data;
        fprintf(target, "%d -> ", n->vertex);
        for (adj = n->adjacent; adj != NULL; adj = adj->next) {
            e = adj->data;
            fprintf(target, "%d (%.3f), ", e->destination, e->weight);
        }
        fputc('\n', target);
    }
}

// a new node is added as a super root, connecting it with edges
// of weight 0 to all the nodes in the supplied GArray
// !!The node is predended to the list of nodes!!
int graph_add_hyper_root(Graph *G, GArray *roots) {
  // first build the adjacency list of the super-root
  int i, lenght = roots->len;
  GSList *adjlist = NULL;
  Edge *e = NULL;
  for(i = 0; i < lenght; i++) {
    e = (Edge *)malloc(sizeof(Edge));
    if(!e) {
      g_error("Failed to alloc Edge");
    }
    // set the edge's head to the i-th root with edge weight 0
    e->destination = g_array_index(roots, int, i);
    e->weight = 0.0;
    // prepend the edge to the adjacency list of the node
    adjlist = g_slist_prepend(adjlist, e);
  }

  Node *n = (Node *)malloc(sizeof(Node));
  if(!n) {
    g_error("Failed to alloc Node");
  }
  // then set the node's adjacency list to the one created in the loop
  n->vertex = G->order; // the new super-root is set to be the next available integer
  n->adjacent = adjlist;
  // then add the node to the graph and update it consequently
  G->order += 1;
  G->nodes = g_list_prepend(G->nodes, n);

  return G->order - 1;
}

void graph_remove_hyper_root(Graph *g) {
    // the hyper-root is the first node in the GList and must be removed
    // First get a link to it
    GList *hyroot = g->nodes;
    // then free the data contained in that GList node
    node_free(hyroot->data);
    // then delete (remove + free) the list's node
    // NOTE: do not swap this and the line above: data becomes unreachable,
    // even though it doesn't leak either way (see valgrind output)
    g->nodes = g_list_delete_link(g->nodes, hyroot);
}

void edge_free(gpointer data) {
  if(data != NULL) {
    free((Edge *)data);
  }
}

void node_free(gpointer data) {
  // just frees the adjacency list of this node
  g_slist_free_full(((Node *)data)->adjacent, edge_free);
  // then the node itself is freed
  free((Node *)data);
}

void graph_free(Graph *g) {
  // frees the nodes list
  g_list_free_full(g->nodes, node_free);
  // and then the graph itseff
  free(g);
}
