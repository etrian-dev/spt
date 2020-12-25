/* Graph reading utilities with Glib */
//Glib headers for the queue and the list
#include <glib.h>

// readline is used to read the input graph
#include <readline/readline.h>

/* The graph node and edge*/
typedef struct node_t {
    int vertex;
    GSList* adjacent;
} Node;
typedef struct edge_t {
    int destination;
    float weight;
} Edge;

void destroy_edge(gpointer data);
void print_graph(GList* graph, const int order);