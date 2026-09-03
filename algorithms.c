#include "../include/algorithms.h"

// --- 1. 0/1 KNAPSACK USING DYNAMIC PROGRAMMING ---

static double max_val(double a, double b) {
    return (a > b) ? a : b;
}

KnapsackResult solve_knapsack_dp(Item items[], int n, double max_capacity) {
    KnapsackResult result;
    memset(&result, 0, sizeof(KnapsackResult));
    snprintf(result.strategy_used, sizeof(result.strategy_used), "Dynamic Programming 0/1");

    if (n <= 0 || max_capacity <= 0) return result;

    int W = (int)max_capacity; // Discretized capacity for 2D DP matrix

    // DP Table allocation: (n + 1) x (W + 1)
    double** dp = (double**)malloc((n + 1) * sizeof(double*));
    for (int i = 0; i <= n; i++) {
        dp[i] = (double*)malloc((W + 1) * sizeof(double));
        memset(dp[i], 0, (W + 1) * sizeof(double));
    }

    // Build DP table bottom-up
    for (int i = 1; i <= n; i++) {
        int wt = (int)items[i - 1].weight;
        double val = items[i - 1].value;

        for (int w = 0; w <= W; w++) {
            if (wt <= w) {
                dp[i][w] = max_val(val + dp[i - 1][w - wt], dp[i - 1][w]);
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    result.max_value = dp[n][W];

    // Backtrack to find selected items
    int w = W;
    for (int i = n; i > 0 && result.max_value > 0; i--) {
        if (dp[i][w] != dp[i - 1][w]) {
            result.selected_item_ids[result.selected_count++] = items[i - 1].id;
            result.total_weight += items[i - 1].weight;
            w -= (int)items[i - 1].weight;
        }
    }

    // Free DP table memory
    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);

    return result;
}


// --- 2. FRACTIONAL KNAPSACK USING GREEDY ALGORITHM ---

typedef struct ItemRatio {
    Item item;
    double ratio; // value / weight
} ItemRatio;

static int compare_ratios(const void* a, const void* b) {
    ItemRatio* r1 = (ItemRatio*)a;
    ItemRatio* r2 = (ItemRatio*)b;
    if (r2->ratio > r1->ratio) return 1;
    if (r2->ratio < r1->ratio) return -1;
    return 0;
}

KnapsackResult solve_knapsack_greedy(Item items[], int n, double max_capacity) {
    KnapsackResult result;
    memset(&result, 0, sizeof(KnapsackResult));
    snprintf(result.strategy_used, sizeof(result.strategy_used), "Greedy Fractional");

    if (n <= 0 || max_capacity <= 0) return result;

    ItemRatio* ratios = (ItemRatio*)malloc(n * sizeof(ItemRatio));
    for (int i = 0; i < n; i++) {
        ratios[i].item = items[i];
        ratios[i].ratio = (items[i].weight > 0) ? (items[i].value / items[i].weight) : 0;
    }

    // Sort by ratio descending
    qsort(ratios, n, sizeof(ItemRatio), compare_ratios);

    double current_weight = 0.0;
    for (int i = 0; i < n; i++) {
        if (current_weight + ratios[i].item.weight <= max_capacity) {
            current_weight += ratios[i].item.weight;
            result.max_value += ratios[i].item.value;
            result.selected_item_ids[result.selected_count++] = ratios[i].item.id;
        } else {
            // Take fraction of item
            double remain = max_capacity - current_weight;
            result.max_value += ratios[i].item.value * (remain / ratios[i].item.weight);
            current_weight += remain;
            result.selected_item_ids[result.selected_count++] = ratios[i].item.id;
            break;
        }
    }

    result.total_weight = current_weight;
    free(ratios);
    return result;
}


// --- 3. DIJKSTRA SHORTEST PATH ON WAREHOUSE GRAPH ---

ShortestPathResult find_shortest_picking_route(WarehouseGraph* graph, int src, int dest) {
    ShortestPathResult res;
    memset(&res, 0, sizeof(ShortestPathResult));
    res.src_zone = src;
    res.dest_zone = dest;

    if (!graph || src < 0 || src >= graph->num_vertices || dest < 0 || dest >= graph->num_vertices) {
        return res;
    }

    int n = graph->num_vertices;
    int dist[MAX_GRAPH_NODES];
    bool visited[MAX_GRAPH_NODES];
    int parent[MAX_GRAPH_NODES];

    for (int i = 0; i < n; i++) {
        dist[i] = 9999;
        visited[i] = false;
        parent[i] = -1;
    }

    dist[src] = 0;

    for (int count = 0; count < n - 1; count++) {
        // Find minimum distance vertex
        int min_dist = 9999, min_idx = -1;
        for (int v = 0; v < n; v++) {
            if (!visited[v] && dist[v] <= min_dist) {
                min_dist = dist[v];
                min_idx = v;
            }
        }

        if (min_idx == -1) break;
        visited[min_idx] = true;

        // Update distances of adjacent vertices
        for (int v = 0; v < n; v++) {
            int weight = graph->adj_matrix[min_idx][v];
            if (!visited[v] && weight != 9999 && dist[min_idx] != 9999
                && dist[min_idx] + weight < dist[v]) {
                dist[v] = dist[min_idx] + weight;
                parent[v] = min_idx;
            }
        }
    }

    res.total_distance = dist[dest];

    // Reconstruct path
    int temp_path[MAX_GRAPH_NODES];
    int path_len = 0;
    int curr = dest;

    while (curr != -1) {
        temp_path[path_len++] = curr;
        curr = parent[curr];
    }

    // Reverse path array
    res.path_length = path_len;
    for (int i = 0; i < path_len; i++) {
        res.path[i] = temp_path[path_len - 1 - i];
    }

    return res;
}
