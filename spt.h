// Defines the functions that implement Bellman-Ford and Dijkstra's algorithms
// for finding the shortest path tree (and related functions)

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

// runs the Bellman-Ford algorithm (SPT.L) on G
// returns 0 on success
int spt_l(
  Graph G,
  GArray *roots,
  float max_path,
  float *labels,
  int *predecessors
);

// runs Dijkstra's algorithm (SPT.S) on G
// returns 0 on success
int spt_s(
  Graph G,
  GArray *roots,
  float max_path,
  float *labels,
  int *predecessors
);

#endif
