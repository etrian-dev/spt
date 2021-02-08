// Graph reading utilities using Glib data structures (header file)
/*
 * glib-graph.h
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

#ifndef GLIB_GRAPH_DEFINED
#define GLIB_GRAPH_DEFINED

// Glib headers (both GList and GSList are used)
#include <glib.h>
// readline is used to read the input graph
#include <readline/readline.h>

// Edge structure
typedef struct edge_t {
    int destination; // the destination of the edge
    float weight;
} Edge;
// Node structure: a list of Edges coming out of vertex
typedef struct node_t {
    int vertex;
    GSList* adjacent;
} Node;
// The graph is just a list of nodes
typedef struct graph_t {
  int order;
  GList *nodes;
} Graph;

/* 	Generates a new graph
	The parameter is a pointer to the max edge weight in the graph
	that gets overwritten inside the function.
	It's useful in algorithms such as SPT.L or SPT.S
*/
Graph new_graph(float *min_weight, float *max_weight);
void edge_free(gpointer edge);
void node_free(gpointer node);
void graph_free(Graph g);
/* Prints the graph to target, where target can be any open file descriptor */
void print_graph(FILE *target, Graph g);
// a new node is added as a super root, connecting it with edges
// of weight 0 to all the nodes in the supplied GArray
// Returns the index of the newly created node
int graph_add_hyper_root(Graph *g, GArray *roots);
// removes the hyper-root from the graph
void graph_remove_hyper_root(Graph *g);

#endif
