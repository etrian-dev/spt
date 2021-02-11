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

// define an array of function pointers to choose the algorithm to run on G at runtime
#define N_IMPLEMENTED 2
int (*algorithms[N_IMPLEMENTED])(Graph *, GArray *, float, float *, int *) = {
  spt_s, spt_l // spt.s has index 0, spt.l has index 1
};

void print_spt(char *algorithm, GArray *roots, float *labels, int *predecessors, const int iterations, const int graph_order) {
  // the resulting spt is represented by labels & predecessors
  float spt_cost = 0.0;
  int i;
  printf("After %d iterations, the SPT with root(s) [ ", iterations);
  for(i = 0; i < roots->len - 1; i++) {
    printf("%d, ", g_array_index(roots, int, i));
  }
  printf("%d ] found by %s is:\n", g_array_index(roots, int, roots->len - 1), algorithm);
  for (i = 0; i < graph_order; i++) {
    printf("label[%d] = %.3f\tpred[%d] = %d\n", i, labels[i], i, predecessors[i]);
    spt_cost += labels[i]; // computes the SPT's cost: the sum of all the labels
  }
  printf("Total cost of the SPT: %f\n", spt_cost);
}

// Main function

int main() {
  float max_w, min_w;
  // reads the graph using the library glib-graph
  Graph *graph = new_graph(&min_w, &max_w);
  // stores in max_w and min_w the maximum and minimum weights of edges in the graph

  // The most expensive path in the graph is |N|*max_weight
  // Adding 1.0 to that gives the value used as a fake edge weigth for the initial tree
  float max_path = (float)(graph->order) * max_w + 1.0;

#ifdef DEBUG // the graph is printed to stdout
  puts("GRAPH");
  print_graph(stdout, *graph);
#endif

  // Read the root nodes: this can be a list of any vertices in the graph
  int root;
  char *token, *line;
  // The data structure that stores the root list is a GArray
  GArray *spt_rootlist = g_array_new(FALSE, FALSE, sizeof(int));

  line = readline("Set the root(s): ");
  // tokenizes the input line to get the vertices
  token = strtok(line, " ");
  while(token != NULL) {
    root = atoi(token); // assumes that it's an integer
    if(root >= 0 && root < graph->order) {
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

#ifdef DEBUG // print the spt_rootlist
  g_print("ROOTLIST: [");
  for(int i = 0; i < spt_rootlist->len; i++) {
    g_print("%d, ", g_array_index(spt_rootlist, int, i));
  }
  g_print("]\n");
#endif

  int num_labels = graph->order;
  if(spt_rootlist->len > 1) {
    // since a new hyper-root will be added, the number of labels must be one more
    num_labels++;
  }

  // each node has a label: the cost of the shortest path from root to i
  float *spt_labels = (float *)malloc(num_labels * sizeof(float));
  // a node j has a predecessor i in the SPT <=> in the SPT there is an edge i -> j
  int *spt_pred = (int *)malloc(num_labels * sizeof(int));

  // Applies the chosen algorithm on G
  // If the min weight is less than 0.0, suggests using spt.l
  if(min_w < 0) {
    g_warning("There is a negative edge in the graph: using SPT.L is strongly suggested");
  }

  line = readline("Choose an algorithm to apply on G\nDijkstra (SPT.S) [0]\nBellman-Ford (SPT.L) [1]\n");
  long int choice = strtol(line, NULL, 10);
  if (choice < 0 || choice >= N_IMPLEMENTED)
  {
    g_error("Sorry, this algorithm has not been implemented yet");
  }
  char *chosen_algo = (choice == 0 ? "Dijkstra" : "Bellman-Ford");
  g_print("Run %s...\n", chosen_algo);
  // choose the algorithm from an array of function pointers
  int iterations = (*algorithms[choice])(graph, spt_rootlist, max_path, spt_labels, spt_pred);

  // Print the resulting SPT
  if(iterations == NO_LOWER_BOUND) {
      puts("Negative cycle! No lower bound.");
  }
  else {
    print_spt(chosen_algo, spt_rootlist, spt_labels, spt_pred, iterations, graph->order);
  }

  // freeing all the memory before exiting
  free(spt_labels);
  free(spt_pred);
  free(line);
  g_array_free(spt_rootlist, TRUE);
  graph_free(graph);

  return 0;
}
