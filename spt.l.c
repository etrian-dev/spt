// finds the SPT in a digraph G = (N, A)
// by using the Bellman-Ford algorithm (uses a queue)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Glib headers for the queue and the list
#include <glib.h>

// readline is used to read the input graph
#include <readline/readline.h>

/* The graph node */
typedef struct node_t {
    int destination;
    float weight;
} Node;

void destroy_node(gpointer data) {
    free(data);
}

void print_graph(GSList* graph, const int order) {
    int i, j;
    GSList* adjlist = NULL;
    Node* n = NULL;

    // prints the graph
    for (i = 0; i < order; i++) {
        adjlist = g_slist_nth(graph, i);

        printf("%d -> ", i);

        // gets the element at position j in the list (dest vertex and edge weight)
        Node* el = NULL;
        for (j = 0; j < g_slist_length(graph) - 1; j++) {
            el = g_slist_nth_data(graph, j);
            if (!el) {
                puts("Something went wrong while reading the graph");
                exit(1);
            }
            printf("%d (%f), ", el->destination, el->weight);
        }
        el = g_slist_nth_data(graph, j);
        if (!el) {
            puts("Something went wrong while reading the graph");
            exit(1);
        }
        printf("%d (%f)\n", el->destination, el->weight);
    }
    putchar('\n');
}

int main(void) {
    // reads the vertices and edges, then the graph
    int vertices;
    float max_w = -INFINITY;  // store smallest value, gets compared lt to any float

    printf("Enter the number of vertices and edges: ");
    scanf("%d", &vertices);

    // The graph is an adjacency list (list of lists)
    GSList* graph = NULL;       // initialized as the empty list

    // dummy list and other variables
    GSList* adjlist = NULL;
    int dest = -1;
    float weight = 0;
    Node* n = NULL;
    char* line = NULL;
    char* token = NULL;
    int i;

    for (i = 0; i < vertices; i++) {
        // reads the adjacency list of vertex i
        line = readline("");

        // tokenizes it: tokens are separated by " "
        token = strtok(line, " ");

        while (token) {
            // parses the token in the destination vertex and the edge's weight
            sscanf(token, "%d:%f", &dest, &weight);
            // allocates memory for the pointer to the Node and fills its records
            n = (Node*)malloc(sizeof(Node));
            if (n == NULL) {
                puts("Failed to alloc Node");
                exit(1);
            }
            n->destination = dest;
            n->weight = weight;
            // prepends the Node n to the list
            adjlist = g_slist_prepend(adjlist, n);

            //update the max weight if greater than current
            if (max_w < weight) {
                max_w = weight;
            }

            // get the next token
            token = strtok(NULL, " ");
        }
        // prepend the adjacency list to the graph
        // TODO: prepend and then reverse once built to improve performance
        graph = g_slist_append(graph, adjlist);

        // then the dummy list is cleared but its elements are not freed
        g_clear_slist(&adjlist, destroy_node);
        adjlist = NULL;

        // input line freed
        free(line);
    }
    // input line freed
    free(line);
    line = NULL;
    // then the list is cleared and its head pointer set to NULL to be safe
    g_clear_slist(&adjlist, destroy_node);
    adjlist = NULL;

    // prints the graph
    puts("GRAPH");
    print_graph(graph, vertices);

    // choose the root node
    int root;
    printf("Enter the root node [0, %d]: ", vertices - 1);
    scanf("%d", &root);

    // fifo list Q    
    // defines and initializes a queue
    GQueue* Q = g_queue_new();

    // each node i has a label (the cost of the path from the root to i)
    float* labels = (float*)malloc(vertices * sizeof(float));
    // each node j has a predecessor i: in the tree there is the edge i -> j
    int* predecessors = (int*)malloc(vertices * sizeof(int));

    float max_path = (float)(vertices - 1) * max_w + 1;
    // build the initial tree, by putting all the labels to 1 + the max possile path in the graph
    for (i = 0; i < vertices; i++) {
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
    printf("inserted %d\n", root);

    //Element* u = NULL;
    int u = 0;
    int count_it = 0; // just counts the iterations

    // while Q is not empty, loop
    while (!g_queue_is_empty(Q)) {
        // updates the number of iterations
        count_it++;

        // in Bellman-Ford Q is FIFO and dequeued at each iteration
        u = GPOINTER_TO_INT(g_queue_pop_head(Q));
        // get the u's adjacency list in the graph
        adjlist = g_slist_nth(graph, u);

        // check each forward edge from u
        for (i = 0; i < g_slist_length(adjlist); i++) {
            n = g_slist_nth_data(adjlist, i);
            // check if the edge (u, i) violates the corresponding Bellman condition
            if (labels[n->destination] > labels[u] + n->weight) {
                printf("(%d, %d) violates Bellman\n", u, n->destination);
                printf("d_%d\t+\tc_%d_%d\t<\td_%d\n", u, u, n->destination, n->destination);
                printf("%f\t+\t%f\t<\t%f\n", labels[u], n->weight, labels[n->destination]);

                // update labels[i] (not its tree!) and its predecessor if it changes
                labels[n->destination] = labels[u] + n->weight;
                if (predecessors[n->destination] != u) {
                    predecessors[n->destination] = u;
                }
                // inserts the head of the edge (vertex i) in Q if it's not already in there
                if (g_queue_find(Q, GINT_TO_POINTER(n->destination)) == NULL) {
                    g_queue_push_tail(Q, GINT_TO_POINTER(n->destination));
                }
            }
        }
    }

    // free the queue (no dynamically alloc'd data stored
    g_queue_free(Q);

    printf("After %d iterations, the SPT found by Bellman-Ford is\n", count_it);
    // the resulting spt is represented by labels & predecessors
    for (i = 0; i < vertices; i++) {
        printf("label[%d] = %f\tpred[%d] = %d\n", i, labels[i], i, predecessors[i]);
    }
}
