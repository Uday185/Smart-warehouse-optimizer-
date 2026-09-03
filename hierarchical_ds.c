#include "../include/hierarchical_ds.h"
#include <ctype.h>

static int case_insensitive_compare(const char* a, const char* b) {
    while (*a && *b) {
        unsigned char ca = (unsigned char)*a;
        unsigned char cb = (unsigned char)*b;
        int diff = tolower(ca) - tolower(cb);
        if (diff != 0) return diff;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

// --- 1. BINARY SEARCH TREE (BST) IMPLEMENTATION ---

BSTNode* create_bst_node(Item item) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    if (!node) return NULL;
    node->data = item;
    node->left = NULL;
    node->right = NULL;
    return node;
}

BSTNode* insert_bst(BSTNode* root, Item item) {
    if (root == NULL) {
        return create_bst_node(item);
    }

    if (item.id < root->data.id) {
        root->left = insert_bst(root->left, item);
    } else if (item.id > root->data.id) {
        root->right = insert_bst(root->right, item);
    } else {
        // ID already exists, update data
        root->data = item;
    }

    return root;
}

BSTNode* search_bst_by_id(BSTNode* root, int id) {
    if (root == NULL || root->data.id == id) {
        return root;
    }

    if (id < root->data.id) {
        return search_bst_by_id(root->left, id);
    }

    return search_bst_by_id(root->right, id);
}

BSTNode* search_bst_by_name(BSTNode* root, const char* name) {
    if (root == NULL) return NULL;

if (case_insensitive_compare(root->data.name, name) == 0) return root;

    BSTNode* left_res = search_bst_by_name(root->left, name);
    if (left_res) return left_res;

    return search_bst_by_name(root->right, name);
}

// Inorder Traversal (L -> Root -> R) - Sorted Catalog Output
void inorder_traversal(BSTNode* root) {
    if (root != NULL) {
        inorder_traversal(root->left);
        printf("ID: %-5d | Name: %-22s | Stock: %-4d | Price: $%-7.2f | Section: Aisle %d\n",
               root->data.id, root->data.name, root->data.quantity, root->data.price, root->data.aisle_id);
        inorder_traversal(root->right);
    }
}

// Preorder Traversal (Root -> L -> R) - Structural Backup / Tree Export
void preorder_traversal(BSTNode* root) {
    if (root != NULL) {
        printf("ID: %-5d | Name: %-22s | Stock: %-4d\n",
               root->data.id, root->data.name, root->data.quantity);
        preorder_traversal(root->left);
        preorder_traversal(root->right);
    }
}

// Postorder Traversal (L -> R -> Root) - Summary Audit & Cleanup
void postorder_traversal(BSTNode* root) {
    if (root != NULL) {
        postorder_traversal(root->left);
        postorder_traversal(root->right);
        printf("ID: %-5d | Name: %-22s | Val: $%.2f\n",
               root->data.id, root->data.name, root->data.value);
    }
}

void free_bst(BSTNode* root) {
    if (root != NULL) {
        free_bst(root->left);
        free_bst(root->right);
        free(root);
    }
}


// --- 2. GRAPH IMPLEMENTATION (WAREHOUSE MAP) ---

WarehouseGraph* create_warehouse_graph(int num_vertices) {
    if (num_vertices > MAX_GRAPH_NODES) num_vertices = MAX_GRAPH_NODES;

    WarehouseGraph* graph = (WarehouseGraph*)malloc(sizeof(WarehouseGraph));
    graph->num_vertices = num_vertices;

    for (int i = 0; i < num_vertices; i++) {
        graph->vertices[i].id = i;
        snprintf(graph->vertices[i].name, sizeof(graph->vertices[i].name), "Zone_%d", i);
        graph->adj_list[i] = NULL;
        for (int j = 0; j < num_vertices; j++) {
            graph->adj_matrix[i][j] = (i == j) ? 0 : 9999; // 9999 represents INF
        }
    }
    return graph;
}

void add_warehouse_edge(WarehouseGraph* graph, int src, int dest, int weight) {
    if (src < 0 || src >= graph->num_vertices || dest < 0 || dest >= graph->num_vertices) return;

    // Adjacency List (Undirected graph)
    GraphAdjNode* node1 = (GraphAdjNode*)malloc(sizeof(GraphAdjNode));
    node1->dest = dest;
    node1->weight = weight;
    node1->next = graph->adj_list[src];
    graph->adj_list[src] = node1;

    GraphAdjNode* node2 = (GraphAdjNode*)malloc(sizeof(GraphAdjNode));
    node2->dest = src;
    node2->weight = weight;
    node2->next = graph->adj_list[dest];
    graph->adj_list[dest] = node2;

    // Adjacency Matrix
    graph->adj_matrix[src][dest] = weight;
    graph->adj_matrix[dest][src] = weight;
}

// BFS Traversal
void bfs_traversal(WarehouseGraph* graph, int start_vertex) {
    if (start_vertex < 0 || start_vertex >= graph->num_vertices) return;

    bool visited[MAX_GRAPH_NODES] = { false };
    int queue[MAX_GRAPH_NODES];
    int front = 0, rear = 0;

    visited[start_vertex] = true;
    queue[rear++] = start_vertex;

    printf("\n--- BFS WAREHOUSE ZONE TRAVERSAL (Starting at %s) ---\n", graph->vertices[start_vertex].name);
    while (front < rear) {
        int current = queue[front++];
        printf("[%s] -> ", graph->vertices[current].name);

        GraphAdjNode* temp = graph->adj_list[current];
        while (temp != NULL) {
            int adj = temp->dest;
            if (!visited[adj]) {
                visited[adj] = true;
                queue[rear++] = adj;
            }
            temp = temp->next;
        }
    }
    printf("END\n");
}

// DFS Helper Function
static void dfs_util(WarehouseGraph* graph, int v, bool visited[]) {
    visited[v] = true;
    printf("[%s] -> ", graph->vertices[v].name);

    GraphAdjNode* temp = graph->adj_list[v];
    while (temp != NULL) {
        int adj = temp->dest;
        if (!visited[adj]) {
            dfs_util(graph, adj, visited);
        }
        temp = temp->next;
    }
}

// DFS Traversal
void dfs_traversal(WarehouseGraph* graph, int start_vertex) {
    if (start_vertex < 0 || start_vertex >= graph->num_vertices) return;

    bool visited[MAX_GRAPH_NODES] = { false };
    printf("\n--- DFS WAREHOUSE ZONE TRAVERSAL (Starting at %s) ---\n", graph->vertices[start_vertex].name);
    dfs_util(graph, start_vertex, visited);
    printf("END\n");
}

void free_warehouse_graph(WarehouseGraph* graph) {
    if (!graph) return;
    for (int i = 0; i < graph->num_vertices; i++) {
        GraphAdjNode* curr = graph->adj_list[i];
        while (curr != NULL) {
            GraphAdjNode* next = curr->next;
            free(curr);
            curr = next;
        }
    }
    free(graph);
}
