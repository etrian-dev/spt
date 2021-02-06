## Shortest Path Tree on a graph
This repository contains a simple implementation of two algorithms for solving the shortest paths tree problem (SPT) on a directed weighted graph G = (N, E).
The file `spt.l.c` contains the implementation of the Bellman-Ford algorithm, whereas the file `spt.s.c` contains the implementation of Dijkstra's algorithm. The latter is more efficient on graphs whose weights are all positive, especially if they are dense (that is, E is closer to O(n^2), rather than O(n)). Bellman-Ford, moreover, detects negative cycles in the graph and exits (no lower bound exists on these instances). Otherwise, the algorithms find one optimal solution and ouput in as a predecessor array and labels array.
### Specifying input
The algoritms read a graph from standard input; the format is the one supplied in the tests directory: an integer representing the number of nodes in the graph (its order) and a series of lines, representing a node's adjacency list.  
A few remarks:
* Nodes with an empty adjacency list (degree 0) are represented by empty lines (\n)
* Nodes are referred to as integers ranging from 0 to n-1, where n is the graph's order
* To specify the (directed) edge i -> j one should write in i's adjacency list (the i-th line) `j:weight`, where weight can be any real number, and then a blank
* The algorithm asks for a root node, the nodes where all paths start from, so one should include it as the last line of the input as well. Obviously root must follow the above rules for vertices'indexes
### License
GPLv3.0, provided in COPYING