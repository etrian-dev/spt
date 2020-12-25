#include "glib-graph.h"

void destroy_edge(gpointer data) {
    free(data);
}
void print_graph(GList* graph, const int order) {
    // Dummy variables to explore the graphs
    Node* n = NULL;
    Edge* e = NULL;
    int j;

    GList* iter = NULL;
    GSList* adj = NULL;
    for (iter = graph; iter != NULL; iter = iter->next) {
        n = iter->data;
        printf("%d -> ", n->vertex);
        for (adj = n->adjacent; adj != NULL; adj = adj->next) {
            e = adj->data;
            printf("%d (%.3f), ", e->destination, e->weight);
        }
        putchar('\n');
    }
}