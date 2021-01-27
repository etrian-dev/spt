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
Graph new_graph(float *max_weight) {
  // The graph is an adjacency list (doubly linked)
  // of nodes, which in turn contain a singly linked edge list
  Graph g;
  g.order = 0;
  g.nodes = NULL;

  // reads the order (number of vertices), then the graph
  char* line = NULL;
  line = readline("Enter the number of vertices: ");
  g.order = atoi(line); // assuming a valid integer
  free(line);

  // dummy list and other variables
  GSList* adjlist = NULL;
  int dest = -1;
  float weight = 0;
  *max_weight = -INFINITY;  // store smallest value, gets compared lt to any float
  Node* n = NULL;
  Edge* e = NULL;
  char* token = NULL;
  int i;

  for (i = 0; i < g.order; i++) {
      // reads the adjacency list of vertex i (no prompt)
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
          if (*max_weight < weight) {
              *max_weight = weight;
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
      g.nodes = g_list_append(g.nodes, n);

      // then the adjacency list is resetted without deallocating anything
      adjlist = NULL;

  }
  // dummy list set to NULL just to be safe
  adjlist = NULL;

  return g;
}

void destroy_edge(gpointer edge) {
    free(edge);
}
void print_graph(FILE *target, Graph g) {
    // Dummy variables to explore the graphs
    Node* n = NULL;
    Edge* e = NULL;
    int j;

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
