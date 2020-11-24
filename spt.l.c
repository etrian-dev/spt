// finds the SPT in a digraph G = (N, A)
// by using the Bellman-Ford algorithm (uses a queue)

// my headers
#include "graph.h"

//Glib headers for the queue and the list
#include <glib.h>

// readline is used to read the input graph
#include <readline/readline.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* The graph node */
typedef struct node_t {
    int destination;
    float weight;
} Node;

void destroy_node(gpointer data) {
    free(data);
}

void print_graph(GSlist *graph, const int order) {
    int i, j;
    for (i = 0; i < order; i++) {
        printf("%d -> ", i);
        
        for (j = 0; j < g_slist_length() - 1; j++) {
            printf("%d (%d), ", G[i].adjacent[j], G[i].weights[j]);
        }
        printf("%d (%d)\n", G[i].adjacent[j], G[i].weights[j]);
    }
    putchar('\n');
}

int main(void) {
    // reads the vertices and edges, then the graph
    int vertices, edges;
    int max_w = INT_MIN; // initializes max weight to the minimum integer
    printf("Enter the number of vertices and edges: ");
    scanf("%d %d", &vertices, &edges);

    // The graph is an adjacency matrix (list of lists)
    //Graph G = read_graph(vertices, &max_w);
    GSlist *graph = NULL;   // initialized as the empty list
    // read the vertex's adjacency list using readline()
    char *line = readline(NULL);
    GSlist *adjlist = NULL;
    int dest = -1;
    float weight = 0;
    Node *n = NULL;
    while(line) {
        // parse the adjacency list in a list
        while(scanf(" %d:%f", &dest, &weight) == 2) {
            // allocates memory for the pointer to the Node and fills its records
            n = (Node*)malloc(sizeof(Node));
            if(n == NULL) {
                puts("Failed to alloc Node");
                exit(1);
            }
            n->destination = dest;
            n->weight = weight;
            // prepends the Node n to the list
            adjlist = g_slist_prepend(adjlist, n);
        }
        graph = g_slist_prepend(graph, adjlist);
        
        // then the list is cleared and its head pointer set to NULL to be safe
        g_clear_slist(&adjlist, destroy_node);
        adjlist = NULL;
        
        // input line freed
        free(line);
        // a new line is then read from input
        line = readline(NULL);
    }
    // input line freed
    free(line);
    // then the list is cleared and its head pointer set to NULL to be safe
    g_clear_slist(&adjlist, destroy_node);
    adjlist = NULL;

    // prints the graph
    puts("GRAPH");
    print_graph(graph, vertices);

    // choose the root node
    int root, i;
    printf("Enter the root node [0, %d]: ", vertices-1);
    scanf("%d", &root);

    // fifo list Q
    //Queue Qhead = NULL;
    //Queue Qtail = NULL;
    
    // defines and initializes a queue
    GQueue *Q = g_queue_new();

    // each node i has a label (the cost of the path from the root to i)
    int *labels = (int*) malloc(vertices * sizeof(int));
    // each node j has a predecessor i: in the tree there is the edge i -> j
    int *predecessors = (int*) malloc(vertices * sizeof(int));

    int max_path = (vertices - 1) * max_w + 1;
    // build the initial tree, by putting all the labels to 1 + the max possile path in the graph
    for(i = 0; i < vertices; i++) {
        if(i != root) {
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
    printf("inserted %d\n", root);

    //Element* u = NULL;
    int u = 0;
    int count_it = 0; // just counts the iterations
    
    // while Q is not empty, loop
    while(!g_queue_is_empty(Q)) {
        // updates the number of iterations
        count_it++;
        
        // in Bellman-Ford Q is FIFO and dequeued at each iteration
        u = GPOINTER_TO_INT(g_queue_pop_head(Q));
        u = g_slist_find(graph, 

        // check each forward edge from u
        for (i = 0; i < G[u].out_degree; i++) {

            // check if the edge (u, i) violates the corresponding Bellman condition
            if (labels[G[u].adjacent[i]] > labels[u] + G[u].weights[i]) {
                printf("(%d, %d) violates Bellman\n", u, G[u].adjacent[i]);
                printf("d_%d\t+\tc_%d_%d\t<\td_%d\n", u, u, G[u].adjacent[i], G[u].adjacent[i]);
                printf("%d\t+\t%d\t<\t%d\n", labels[u], G[u].weights[i], labels[G[u].adjacent[i]]);

                // update labels[i] (not its tree!) and its predecessor if it changes
                labels[G[u].adjacent[i]] = labels[u] + G[u].weights[i];
                if (predecessors[G[u].adjacent[i]] != u) {
                    predecessors[G[u].adjacent[i]] = u;
                }
                // inserts G[u].adjacent[i] in Q if it's not already in there
                if (g_queue_find(Q, GINT_TO_POINTER(G[u].adjacent[i])) == NULL) {
                    g_queue_push_tail(Q, GINT_TO_POINTER(G[u].adjacent[i]));
                }
            }
        }
    }
    
    // free the queue (no dynamically alloc'd data stored
    g_queue_free(Q);

    printf("After %d iterations, the SPT found by Bellman-Ford is\n", count_it);
    // the resulting spt is represented by labels & predecessors
    for(i = 0; i< vertices; i++) {
        printf("label[%d] = %d\tpred[%d] = %d\n", i, labels[i], i, predecessors[i]);
    }
}
