// finds the SPT in a digraph G = (N, A)
// by using Dijkstra's algorithm (uses a binary max heap)
// O(n^2) time if all weights are positive, exponential otherwise

// my utility for manipulating graphs
#include <glib-graph.h>
// libreadline for input
#include <readline/readline.h>
// Glib header for data structures (GList, GQueue, ...)
#include <glib.h>
// standard lib headers
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// a queue element is the program's representation of a node:
// it contains the vertex, the predecessor in the SPT and a label
// that is, the distance from the root node
struct q_element {
    int vertex;
    int predecessor;
    float label;
};
typedef struct q_element *Element;

gint smallest_label(gconstpointer a, gconstpointer b, gpointer user_data) {
    Element pa = (Element)a;
    Element pb = (Element)b;
    if(pa->label < pb->label) {
        return -1;
    }
    else if(pa->label > pb->label) {
        return 1;
    }
    return 0;
}

int main(void) {
    float max_w;
    Graph graph = new_graph(&max_w);

    puts("GRAPH");
    print_graph(stdout, graph);

    // choose the root node
    int root;
    char *line;
    line = readline("Enter the root node: ");
    // node label is assumed to be an integer that can be converted using atoi
    root = atoi(line);

    // mem free
    free(line);
    line = NULL;

    // SPT.S implements the set Q as a priority queue
    GQueue *Q = g_queue_new();

    int i;
    // alloc |N| vertices, just to reserve the memory for all the possible elements in the queue
    Element *vertices = (Element *)malloc(graph.order * sizeof(Element));
    if(vertices == NULL) {
        puts("Failed to alloc queue elements vector");
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < graph.order; i++) {
        vertices[i] = (struct q_element *)malloc(sizeof(struct q_element));
        if(vertices[i] == NULL) {
            puts("Failed to alloc queue element");
            exit(EXIT_FAILURE);
        }
    }

    // the most expensive path possible in the graph is used as a fake edge weight
    float max_path = (float)(graph.order - 1) * max_w + 1;

    // build the initial tree, by setting all the labels to max_path + 1.0
    for (i = 0; i < graph.order; i++) {
        if (i != root) {
            vertices[i]->label = max_path + 1.0;
        }
        else {
            vertices[root]->label = 0.0; // the trivial path root ----> root is set to 0
        }
        vertices[i]->vertex = i;
        vertices[i]->predecessor = root; // all nodes have root as their predecessor
    }

    // the tail nodes of those edges who violate bellman conditions
    // must be inserted in Q. In this case, only the root is violating them,
    // because of how the tree is built
    g_queue_push_head(Q, (void *)vertices[root]);
    /*
    printf("Put\n\tvertex: %d\n\tlabel: %f\n\tpred: %d\n",
    	vertices[root]->vertex,
    	vertices[root]->label,
    	vertices[root]->predecessor
    );
    */
    // the data must be stored as a gpointer (just a fancy void*)

    // some dummy variables
    Edge *e = NULL;
    //Node *n = NULL;
    GSList *adjlist = NULL;
    Element u = 0;
    int count_it = 0; // just counts the iterations of the algorithm

    // while Q is not empty and a negative cycle hasn't been found
    while (!g_queue_is_empty(Q)) {
        // updates the number of iterations
        count_it++;

        // in Dijkstra (SPT.S) Q is a prioqueue and, so the elem with the biggest precedence
        // is maintained at the head at each iteration
        u = (Element)g_queue_pop_head(Q);
        /*
        printf("Extracted\n\tvertex: %d\n\tlabel: %f\n\tpred: %d\n",
        	u->vertex,
        	u->label,
        	u->predecessor
        );
        */
        // data in Q is a gpointer, the macro cast back to Element [struct q_element *]

        // CHECKS BELLMAN CONDITIONS ON THE FORWARD EDGES FROM U

        // get u's adjacency list in the graph
        adjlist = ((Node *)g_list_nth_data(graph.nodes, u->vertex))->adjacent;

        // check each forward edge from u
        while (adjlist != NULL) {
            // get the edge
            // the casting is implicit, but useful to understand what's going on
            e = (Edge *)(adjlist->data);
            // check if the edge (u, i) violates the Bellman condition
            // BELLMAN: d_u + c_ui >= d_i => OK, otherwise the condition is violated
            if (vertices[e->destination]->label > vertices[u->vertex]->label + e->weight) {
                printf("(%d, %d) violates Bellman\n", u->vertex, e->destination);
                printf("d_%d\t+\tc_%d_%d\t<\td_%d\n", u->vertex, u->vertex, e->destination, e->destination);
                printf("%.3f\t+\t%.3f\t<\t%.3f\n", vertices[u->vertex]->label, e->weight, vertices[e->destination]->label);

                // update labels[i] (not its subtree) and change the predecessor if needed
                vertices[e->destination]->label = vertices[u->vertex]->label + e->weight;
                if (vertices[e->destination]->predecessor != u->vertex) {
                    vertices[e->destination]->predecessor = u->vertex;
                }
                // if the vertex e is not in the prioqueue, inserts it maintaining the
                // queue sorted by minimum label
                if (g_queue_find(Q, (void *)vertices[e->destination]) == NULL) {
                    // inserts sorted by using the smallest_label compare function
                    g_queue_insert_sorted(Q, (void *)vertices[e->destination], smallest_label, NULL);
                    /*
                    printf("Put\n\tvertex: %d\n\tlabel: %f\n\tpred: %d\n",
                    	vertices[e->destination]->vertex,
                    	vertices[e->destination]->label,
                    	vertices[e->destination]->predecessor
                    );
                    */
                }
            }

            // get the next edge
            adjlist = adjlist->next;
        }
    }

    // free the empty queue
    g_queue_free(Q);

    printf("After %d iterations, the SPT found by Dijkstra is\n", count_it);
    // the resulting spt is represented by labels & predecessors
    float spt_cost = 0.0;
    for (i = 0; i < graph.order; i++) {
        printf("label[%d] = %.3f\tpred[%d] = %d\n", i, vertices[i]->label, i, vertices[i]->predecessor);
        spt_cost += vertices[i]->label;
    }
    // the total cost is also printed to stdout
    printf("Total cost of the SPT: %f\n", spt_cost);

    // free the elements array
    for(i = 0; i < graph.order; i++) {
        free(vertices[i]);
    }
    free(vertices);

    // free the graph nodes
    g_list_free(graph.nodes);
}
