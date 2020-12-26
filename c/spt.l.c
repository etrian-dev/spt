// finds the SPT in a digraph G = (N, A)
// by using the Bellman-Ford algorithm (uses a queue)

// my utility for manipulating graphs
#include <glib-graph.h>
// libreadline for input
#include <readline/readline.h>
// Glib header for data structures (GList, GQueue, ...)
#include <glib.h>
// standard lib headers
#include <stdio.h>
#include <stdlib.h>

// main function: reads a digraph from stdin, then applies Bellman-Ford on it
// to find the SPT (Shortest Paths Tree)
int main(void) {

	float max_w; // stores the max edge weight in the graph
    // read the graph    
    Graph graph = new_graph(&max_w);

    // print the graph
    puts("GRAPH");
    print_graph(stdout, graph);

    // choose the root node
    int root;
    char *line;
    line = readline("Enter the root node: ");
    root = atoi(line); // node label is assumed to be an integer
    
    free(line);
    line = NULL;

    // fifo list Q, a GQueue from Glib to store the visited nodes
    GQueue* Q = g_queue_new();

    // each node i has a label (the cost of the path from the root to i)
    float* labels = (float*)malloc(graph.order * sizeof(float));
    // each node j has a predecessor i 
    // <=>
    // in the SPT there is an edge i -> j
    int* predecessors = (int*)malloc(graph.order * sizeof(int));
    // the most expensive path possible in the graph is used as a fake edge weight
    float max_path = (float)(graph.order - 1) * max_w + 1;
    // array to store how many times a node has been removed from Q
    // when any node reaches n rimotions, a negative cycle is found
    // => the instance of the problem has no lower limit
    int* count_rm = (int*)calloc(graph.order, sizeof(int)); // memory init to 0 by calloc
    
    int i;
    // build the initial tree, by putting all the labels to 1 + the max possile path in the graph
    for (i = 0; i < graph.order; i++) {
        if (i != root) {
            labels[i] = max_path;
        }
        else {
            labels[root] = 0; // path root ----> root is set to 0
        }
        predecessors[i] = root; // all nodes have root as their predecessor
    }
    // after initializing the tree, the tail elements of those edges who violate bellman conditions
    // must be inserted in Q. In this case, just the root is violating them, because of how the tree is built
    g_queue_push_tail(Q, GINT_TO_POINTER(root));
    //printf("inserted %d\n", root);

    // some dummy variables
    Edge *e = NULL;
    Node *n = NULL;
    GSList *adjlist = NULL;
    int u = 0;
    int count_it = 0; // just counts the iterations
    int neg_cycle = 0;

    // while Q is not empty, loop
    while (!g_queue_is_empty(Q) && !neg_cycle) {
        // updates the number of iterations
        count_it++;

        // in Bellman-Ford Q is FIFO and dequeued at each iteration
        u = GPOINTER_TO_INT(g_queue_pop_head(Q));
        
        // check if there's a negative cycle; if so, exits the loop
        count_rm[u]++;
        if(count_rm[u] == graph.order) {
        	neg_cycle = 1;
        	continue;
        }
        
        // get u's adjacency list in the graph
        n = g_list_nth_data(graph.nodes, u);
        adjlist = n->adjacent;

        // check each forward edge from u
        for (adjlist = n->adjacent; adjlist != NULL; adjlist = adjlist->next) {
            // get the edge
            e = adjlist->data;
            // check if the edge (u, i) violates the corresponding Bellman condition
            if (labels[e->destination] > labels[u] + e->weight) {
                printf("(%d, %d) violates Bellman\n", u, e->destination);
                printf("d_%d\t+\tc_%d_%d\t<\td_%d\n", u, u, e->destination, e->destination);
                printf("%.3f\t+\t%.3f\t<\t%.3f\n", labels[u], e->weight, labels[e->destination]);

                // update labels[i] (not its tree!) and its predecessor if it changes
                labels[e->destination] = labels[u] + e->weight;
                if (predecessors[e->destination] != u) {
                    predecessors[e->destination] = u;
                }
                // inserts the head of the edge (vertex i) in Q if it's not already in there
                if (g_queue_find(Q, GINT_TO_POINTER(e->destination)) == NULL) {
                    g_queue_push_tail(Q, GINT_TO_POINTER(e->destination));
                }
            }
        }
    }

    // free the queue (no dynamically alloc'd data stored)
    g_queue_free(Q);

    if(neg_cycle) {
    	puts("Negative cycle! No lower bound");
    }
    else {
		printf("After %d iterations, the SPT found by Bellman-Ford is\n", count_it);
		// the resulting spt is represented by labels & predecessors
		for (i = 0; i < graph.order; i++) {
		    printf("label[%d] = %.3f\tpred[%d] = %d\n", i, labels[i], i, predecessors[i]);
		}
	}
	
	// free the graph nodes
	g_list_free(graph.nodes);
	
	// free the various helper arrays
    free(labels);
    free(predecessors);
    free(count_rm);
}
