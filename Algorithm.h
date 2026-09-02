#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "inventory.h"
#include "hierarchical_ds.h"

// Knapsack Packing Result Structure
typedef struct KnapsackResult {
    double max_value;
    double total_weight;
    int selected_item_ids[50];
    int selected_count;
    char strategy_used[30]; // "DP 0/1" or "Greedy Fractional"
} KnapsackResult;

// Dijkstra Shortest Path Result Structure
typedef struct ShortestPathResult {
    int src_zone;
    int dest_zone;
    int total_distance;
    int path[MAX_GRAPH_NODES];
    int path_length;
} ShortestPathResult;

// 1. 0/1 Knapsack using Dynamic Programming
KnapsackResult solve_knapsack_dp(Item items[], int n, double max_capacity);

// 2. Fractional Knapsack using Greedy Algorithm
KnapsackResult solve_knapsack_greedy(Item items[], int n, double max_capacity);

// 3. Dijkstra Shortest Path on Warehouse Graph
ShortestPathResult find_shortest_picking_route(WarehouseGraph* graph, int src, int dest);

#endif // ALGORITHMS_H
