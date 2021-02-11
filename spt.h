// This header declares two functions to find the SPT of a directed, weighted graph
// They implement Bellman-Ford's algorithm (spt_l) and Dijkstra's algorithm (spt_s)
// They both take as input a graph and return two arrays representing the shortest
// path tree on that graph (if more than one optimal solution exists, only one of
// these is found)

/*
 * spt.h
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

#ifndef SPT_H_DEFINED
#define SPT_H_DEFINED

// my functions to handle graph reading
#include "glib-graph.h"

#include <glib.h> // Glib header for data structures (GList, GQueue, ...)

/*
 * Definition: Bellman condition
 * Given two vertices i and j in the directed graph G = (N, A) such that
 * d_i + c_ij >= d_j, where d_x is the cost of the path in the current SPT
 * to go from the root to vertex x and c_ij is the cost of edge i -> j in the graph.
 * If the above inequality holds, then the bellman condition is said to be satisfied,
 * otherwise it's not satisfied.
 */

#define NO_LOWER_BOUND -1 // if the instance has no lower bound, spt_l returns this value

// runs the Bellman-Ford algorithm (SPT.L) on G
// returns the number of iterations needed on success
int spt_l(
  Graph *G,
  GArray *roots,
  float max_path,
  float *labels,
  int *predecessors
);

// runs Dijkstra's algorithm (SPT.S) on G
// returns the number of iterations needed on success
int spt_s(
  Graph *G,
  GArray *roots,
  float max_path,
  float *labels,
  int *predecessors
);

#endif
