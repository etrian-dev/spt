// This file contains the main file of the spt program, to determine one spt on a
// given graph G = (N, E)

/*
 * main.c
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
// the header declaring the functions that implement the algorithms
#include "spt.h"

#include <readline/readline.h> // for reading input lines
#include <glib.h> // Glib header for data structures (GList, GQueue, ...)

// standard library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_IMPLEMENTED 2
int (*algorithms[N_IMPLEMENTED])(Graph, GArray *, float, float *, int *) = {
    spt_s, spt_l};

int main() {
  float max_w, min_w;
  // reads the graph using the library glib-graph
  // Returns the maximum and minimum weights in the graph
  Graph graph = new_graph(&min_w, &max_w);

  // the most expensive path possible in the graph |N|*max_weight + 1.0
  // is used as a fake edge weigth for the initial tree
  float max_path = (float)(graph.order) * max_w + 1.0;

#ifdef DEBUG
  // the graph is printed to stdout
  puts("GRAPH");
  print_graph(stdout, graph);
#endif

  // read the root nodes: this can be a list of any vertices in the graph
  int root;
  char *token, *line;
  line = readline("Set the root(s): ");

  // The data structure that stores the roots is a GArray
  GArray *spt_rootlist = g_array_new(FALSE, FALSE, sizeof(int));

  token = strtok(line, " ");
  while(token != NULL) {
    root = atoi(token); // assumes that it's an integer
    if(root >= 0 && root < graph.order) {
      // appends root and returns the new array:
      spt_rootlist = g_array_append_val(spt_rootlist, root);
    }
    else {
      g_warning("The root %d in the rootlist is not a valid vertex", root);
    }

    token = strtok(NULL, " ");
  }
  free(line);
  line = token = NULL;

#ifdef DEBUG
  // print the spt_rootlist
  g_print("ROOTLIST: [");
  for(int i = 0; i < spt_rootlist->len; i++) {
    g_print("%d, ", g_array_index(spt_rootlist, int, i));
  }
  g_print("]\n");
#endif

  float *spt_labels = NULL;
  int *spt_pred = NULL;

  // applies the chosen spt algorithm on G
  // If the min weight is less than 0.0, then suggests using spt.l
  if(min_w < 0) {
    g_warning("There is a negative edge in the graph: using SPT.L is strongly suggested");
  }

  line = readline("Choose an algorithm to apply on G\nDijkstra (SPT.S) [0]\nBellman-Ford (SPT.L) [1]\n");
  long int choice = strtol(line, NULL, 10);
  if (choice < 0 || choice >= N_IMPLEMENTED)
  {
    g_error("Sorry, this algorithm has not been implemented yet");
  }
  (*algorithms[choice])(graph, spt_rootlist, max_path, spt_labels, spt_pred);

  // freeing all the memory before exiting
  graph_free(graph);
  g_array_free(spt_rootlist, TRUE);
  free(spt_labels);
  free(spt_pred);
  free(line);
}
