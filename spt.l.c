// finds the shortest paths tree in a graph G = (N, A)
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct node_t {
    int value;
    struct node_t *next;
} Node;
typedef Node* Queue;

void enqueue(Queue *head, Queue *tail, const int v) {
    Node *new = (Node*) malloc(sizeof(Node));
    new->value = v;
    new->next = NULL;
    if(*head == NULL) {
        *head = new;
        *tail = new;
    }
    else {
        (*tail)->next = new;
        *tail = new;
    }
}
Node* dequeue(Queue *head, Queue *tail) {
    if(*head) {
        Node *tmp = *head;
        *head = (*head)->next;
        if(*head == NULL) {
            *tail = NULL;
        }
        return tmp;
    }
    else {
        return NULL;
    }
}

int isIn(Queue head, const int key) {
    int found = 0;
    while((found == 0) && (head != NULL)) {
        if(head->value == key) {
            found = 1;
        }
        head = head->next;
    }
    return found;
}

void print_graph(int **g, const int order) {
    int i, j;
    for(i = 0; i < order; i++) {
        printf("%d -> ", i);
        for(j = 0; j < order; j++) {
            if(g[i][j] != 0) {
                printf("%d [%d], ", j, g[i][j]);
            }
        }
        putchar('\n');
    }
    putchar('\n');
}

// read a weighted digraph in a matrix N x N, keeps track of max edge in the graph
int** read_graph(const int vertices, const int edges, int *max_edge) {
    int **adj_matrix;
    int i, from, to, w;
    // allocates memory for the matrix
    adj_matrix = (int**) calloc(vertices, sizeof(int*));
    for(i = 0; i < vertices; i++) {
        adj_matrix[i] = (int*) calloc(vertices, sizeof(int));
    }
    printf("To enter directed edge i -> j of weight w: i j w\n");
    // reads edge from -> to, weight (weights are integers)
    for(i = 0; i < edges; i++) {
        scanf("%d %d %d", &from, &to, &w);
        adj_matrix[from][to] = w;
        if(w > *max_edge) {
            *max_edge = w;
        }
    }

    // prints the matrix
    puts("GRAPH");
    print_graph(adj_matrix, vertices);

    return adj_matrix;
}

int main(void) {
    // reads the vertices and edges, then the graph
    int vertices, edges, max_w = INT_MIN;
    printf("Enter the number of vertices and edges: ");
    scanf("%d %d", &vertices, &edges);
    int **graph = read_graph(vertices, edges, &max_w);

    // choose the root node
    int root, i;
    printf("Enter the root node [0, %d]: ", vertices-1);
    scanf("%d", &root);

    // fifo list Q
    Queue Qhead = NULL;
    Queue Qtail = NULL;

    // each node i has a label (the cost of the path from the root to i)
    int *labels = (int*) malloc(vertices * sizeof(int));
    // each node j has a predecessor i: in the tree there is the edge i -> j
    int *predecessors = (int*) malloc(vertices * sizeof(int));
    
    for(i = 0; i < vertices; i++) {
        if(i != root) {
            labels[i] = (vertices-1) * max_w + 1;
        }
        predecessors[i] = root;
    }
    // after initializing the tree, the tail nodes of those edges who violate bellman conditions
    // must be inserted in Q. In this case, just root is surely violating them, by contstruction
    enqueue(&Qhead, &Qtail, root);
    
    Node *u = NULL;
    int count_it = 0;
    // while Q is not empty, loop
    while(Qhead != NULL) {
        count_it++;
        // in Bellman-Ford Q is FIFO and at each iteration Q is dequeued
        u = dequeue(&Qhead, &Qtail);

        for(i = 0; i < vertices; i++) {
            // check if there's an edge in a lame way between u->value and i
            // then checks is the edge violates Bellman conditions
            if((graph[u->value][i] != 0) && (labels[u->value] + graph[u->value][i] < labels[i])) {
                // update labels[i] (not its tree!) and its predecessor if it changes

                printf("(%d, %d) violates Bellman\n", u->value, i);
                printf("d_%d\t+\tc_%d%d\t<\td_%d\n", u->value, u->value, i, i);
                printf("%d\t+\t%d\t<\t%d\n", labels[u->value], graph[u->value][i], labels[i]);

                labels[i] = labels[u->value] + graph[u->value][i];
                if(predecessors[i] != u->value) {
                    predecessors[i] = u->value;
                }
                // inserts i in Q if it's not already in there
                if(!isIn(Qhead, i)) {
                    enqueue(&Qhead, &Qtail, i);
                }
            }
        }
        // frees the node
        free(u);
    }

    printf("After %d iterations, the SPT found by Bellman-Ford is\n", count_it);
    // the resulting spt is represented by labels & predecessors
    for(i = 0; i< vertices; i++) {
        printf("label[%d] = %d\tpred[%d] = %d\n", i, labels[i], i, predecessors[i]);
    }
}