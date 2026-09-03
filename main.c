#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/inventory.h"
#include "../include/linear_ds.h"
#include "../include/hierarchical_ds.h"
#include "../include/search_sort.h"
#include "../include/algorithms.h"

// Global Data Structures
ItemNode* g_inventory_head = NULL;
BSTNode* g_bst_root = NULL;
ActionStack* g_undo_stack = NULL;
OrderQueue* g_order_queue = NULL;
WarehouseGraph* g_warehouse_graph = NULL;

// Helper: Seed Default Sample Warehouse Inventory Data
void seed_sample_inventory() {
    Item items[] = {
        {101, "Mechanical Keyboard", "Peripherals", 35, 1599.99, 0.9, 55999.65, 0},
        {102, "Wireless Mouse", "Peripherals", 60, 799.50, 0.4, 47970.00, 1},
        {103, "Laser Printer", "Office", 18, 2490.00, 6.5, 44820.00, 2},
        {104, "4K Monitor", "Display", 12, 18999.00, 3.2, 227988.00, 1},
        {105, "UPS Battery", "Power", 22, 4500.00, 5.8, 99000.00, 3},
        {106, "Laptop Stand", "Accessories", 40, 1499.00, 1.2, 59960.00, 4},
        {107, "Webcam", "Accessories", 28, 2499.00, 0.7, 69972.00, 1},
        {108, "Bluetooth Speaker", "Audio", 16, 3299.00, 1.1, 52784.00, 2}
    };

    int n = sizeof(items) / sizeof(items[0]);
    for (int i = 0; i < n; i++) {
        insert_item_tail(&g_inventory_head, items[i]);
        g_bst_root = insert_bst(g_bst_root, items[i]);
    }

    // Seed sample city graph vertices (Metro hubs)
    g_warehouse_graph = create_warehouse_graph(5);
    snprintf(g_warehouse_graph->vertices[0].name, 40, "Bangalore");
    snprintf(g_warehouse_graph->vertices[1].name, 40, "Chennai");
    snprintf(g_warehouse_graph->vertices[2].name, 40, "Kolkata");
    snprintf(g_warehouse_graph->vertices[3].name, 40, "Delhi");
    snprintf(g_warehouse_graph->vertices[4].name, 40, "Mumbai");

    add_warehouse_edge(g_warehouse_graph, 0, 1, 15);
    add_warehouse_edge(g_warehouse_graph, 0, 2, 25);
    add_warehouse_edge(g_warehouse_graph, 1, 3, 10);
    add_warehouse_edge(g_warehouse_graph, 2, 3, 20);
    add_warehouse_edge(g_warehouse_graph, 3, 4, 12);
    add_warehouse_edge(g_warehouse_graph, 1, 4, 30);

    // Seed sample order queue
    Order o1 = {501, "Acme Logistics", 102, "Barcode Scanner Pro", 4, 598.00, "PENDING"};
    Order o2 = {502, "RoboTech Inc", 105, "Automated Guided Vehicle", 1, 8500.00, "PENDING"};
    enqueue_order(g_order_queue, o1);
    enqueue_order(g_order_queue, o2);
}

// JSON API Handler for Web Backend Integration
void handle_json_command(int argc, char* argv[]) {
    if (argc < 3) {
        printf("{\"error\":\"No command specified\"}\n");
        return;
    }

    const char* cmd = argv[2];

    if (strcmp(cmd, "get_inventory") == 0) {
        printf("{\"status\":\"success\", \"data\":[");
        ItemNode* curr = g_inventory_head;
        bool first = true;
        while (curr) {
            if (!first) printf(",");
            printf("{\"id\":%d,\"name\":\"%s\",\"category\":\"%s\",\"quantity\":%d,\"price\":%.2f,\"weight\":%.1f,\"aisle\":%d}",
                   curr->data.id, curr->data.name, curr->data.category,
                   curr->data.quantity, curr->data.price, curr->data.weight, curr->data.aisle_id);
            first = false;
            curr = curr->next;
        }
        printf("]}\n");
    } else if (strcmp(cmd, "benchmark") == 0) {
        int count = get_inventory_count(g_inventory_head);
        Item* items = (Item*)malloc(count * sizeof(Item));
        ItemNode* curr = g_inventory_head;
        for (int i = 0; i < count; i++) {
            items[i] = curr->data;
            curr = curr->next;
        }
        BenchmarkResult results[10];
        int num_res = 0;
        run_algorithm_benchmark(items, count, results, &num_res);

        printf("{\"status\":\"success\", \"results\":[");
        for (int i = 0; i < num_res; i++) {
            if (i > 0) printf(",");
            printf("{\"name\":\"%s\",\"elements\":%d,\"time_ms\":%.4f,\"comparisons\":%ld,\"swaps\":%ld}",
                   results[i].algorithm_name, results[i].element_count,
                   results[i].time_taken_ms, results[i].comparisons, results[i].swaps);
        }
        printf("]}\n");
        free(items);
    } else if (strcmp(cmd, "knapsack") == 0) {
        double capacity = (argc >= 4) ? atof(argv[3]) : 200.0;
        int count = get_inventory_count(g_inventory_head);
        Item* items = (Item*)malloc(count * sizeof(Item));
        ItemNode* curr = g_inventory_head;
        for (int i = 0; i < count; i++) {
            items[i] = curr->data;
            curr = curr->next;
        }

        KnapsackResult dp_res = solve_knapsack_dp(items, count, capacity);
        KnapsackResult gr_res = solve_knapsack_greedy(items, count, capacity);

        printf("{\"status\":\"success\",\"capacity\":%.1f,\"dp\":{\"value\":%.2f,\"weight\":%.1f,\"count\":%d},\"greedy\":{\"value\":%.2f,\"weight\":%.1f,\"count\":%d}}\n",
               capacity, dp_res.max_value, dp_res.total_weight, dp_res.selected_count,
               gr_res.max_value, gr_res.total_weight, gr_res.selected_count);
        free(items);
    } else if (strcmp(cmd, "shortest_path") == 0) {
        int src = (argc >= 4) ? atoi(argv[3]) : 0;
        int dest = (argc >= 5) ? atoi(argv[4]) : 4;

        ShortestPathResult path_res = find_shortest_picking_route(g_warehouse_graph, src, dest);

        printf("{\"status\":\"success\",\"src\":%d,\"dest\":%d,\"distance\":%d,\"path\":[", src, dest, path_res.total_distance);
        for (int i = 0; i < path_res.path_length; i++) {
            if (i > 0) printf(",");
            printf("%d", path_res.path[i]);
        }
        printf("]}\n");
    } else if (strcmp(cmd, "eval_expr") == 0) {
        const char* expr = (argc >= 4) ? argv[3] : "100 * 0.9 + 15";
        double result = evaluate_price_expression(expr);
        printf("{\"status\":\"success\",\"expression\":\"%s\",\"result\":%.2f}\n", expr, result);
    } else {
        printf("{\"error\":\"Unknown CLI command\"}\n");
    }
}

// CLI Interactive Menu Loop
void interactive_menu() {
    int choice = 0;
    while (1) {
        printf("\n=========================================================\n");
        printf("   SMART WAREHOUSE & INVENTORY MANAGEMENT SYSTEM (C CLI) \n");
        printf("=========================================================\n");
        printf(" 1. [Linear DS] Display Inventory (Linked List)\n");
        printf(" 2. [Linear DS] Add New Item\n");
        printf(" 3. [Linear DS] Delete Item & Push to Undo Stack\n");
        printf(" 4. [Linear DS] View Pending Order Queue\n");
        printf(" 5. [Linear DS] Evaluate Dynamic Pricing Expression\n");
        printf(" 6. [Tree DS]   Inorder Traversal (BST Sorted Catalog)\n");
        printf(" 7. [Graph DS]  BFS & DFS Warehouse Zone Traversals\n");
        printf(" 8. [Algorithm] QuickSort & MergeSort Benchmark Suite\n");
        printf(" 9. [Algorithm] Smart Truck Loading (0/1 Knapsack Solver)\n");
        printf("10. [Algorithm] Dijkstra Shortest Picking Route\n");
        printf(" 0. Exit System\n");
        printf("---------------------------------------------------------\n");
        printf("Enter your choice (0-10): ");

        if (scanf("%d", &choice) != 1) break;

        if (choice == 0) {
            printf("\nExiting Smart Warehouse System. Goodbye!\n");
            break;
        }

        switch (choice) {
            case 1:
                traverse_inventory(g_inventory_head);
                break;
            case 2: {
                Item newItem;
                printf("Enter Item ID: "); scanf("%d", &newItem.id);
                printf("Enter Name: "); scanf(" %[^\n]", newItem.name);
                printf("Enter Category: "); scanf(" %[^\n]", newItem.category);
                printf("Enter Quantity: "); scanf("%d", &newItem.quantity);
                printf("Enter Unit Price ($): "); scanf("%lf", &newItem.price);
                printf("Enter Unit Weight (kg): "); scanf("%lf", &newItem.weight);
                printf("Enter Aisle Zone (0-4): "); scanf("%d", &newItem.aisle_id);
                newItem.value = newItem.price * newItem.quantity;

                insert_item_tail(&g_inventory_head, newItem);
                g_bst_root = insert_bst(g_bst_root, newItem);
                printf("\n[SUCCESS] Item '%s' inserted into Linked List and BST.\n", newItem.name);
                break;
            }
            case 3: {
                int del_id;
                printf("Enter Item ID to delete: "); scanf("%d", &del_id);
                Item deleted;
                if (delete_item_by_id(&g_inventory_head, del_id, &deleted)) {
                    ActionRecord rec = {ACTION_DELETE_ITEM, deleted, deleted.quantity, "2026-08-19"};
                    push_action(g_undo_stack, rec);
                    printf("\n[SUCCESS] Deleted item ID %d (%s). Pushed to Undo Stack.\n", del_id, deleted.name);
                } else {
                    printf("\n[ERROR] Item ID %d not found.\n", del_id);
                }
                break;
            }
            case 4:
                print_order_queue(g_order_queue);
                break;
            case 5: {
                char expr[100];
                printf("Enter price expression (e.g. 250 * 0.85 + 10): ");
                scanf(" %[^\n]", expr);
                double val = evaluate_price_expression(expr);
                printf("\n[RESULT] Expression '%s' = $%.2f\n", expr, val);
                break;
            }
            case 6:
                printf("\n=== BST INORDER TRAVERSAL (SORTED CATALOG) ===\n");
                inorder_traversal(g_bst_root);
                break;
            case 7:
                bfs_traversal(g_warehouse_graph, 0);
                dfs_traversal(g_warehouse_graph, 0);
                break;
            case 8: {
                int count = get_inventory_count(g_inventory_head);
                Item* arr = (Item*)malloc(count * sizeof(Item));
                ItemNode* curr = g_inventory_head;
                for (int i = 0; i < count; i++) { arr[i] = curr->data; curr = curr->next; }

                BenchmarkResult res[10];
                int num_res = 0;
                run_algorithm_benchmark(arr, count, res, &num_res);

                printf("\n=== ALGORITHM PERFORMANCE BENCHMARK (%d items) ===\n", count);
                printf("%-26s %-12s %-14s %-10s\n", "Algorithm", "Time (ms)", "Comparisons", "Swaps");
                printf("---------------------------------------------------------------\n");
                for (int i = 0; i < num_res; i++) {
                    printf("%-26s %-12.4f %-14ld %-10ld\n",
                           res[i].algorithm_name, res[i].time_taken_ms, res[i].comparisons, res[i].swaps);
                }
                free(arr);
                break;
            }
            case 9: {
                double cap;
                printf("Enter truck loading weight capacity (kg): "); scanf("%lf", &cap);
                int count = get_inventory_count(g_inventory_head);
                Item* arr = (Item*)malloc(count * sizeof(Item));
                ItemNode* curr = g_inventory_head;
                for (int i = 0; i < count; i++) { arr[i] = curr->data; curr = curr->next; }

                KnapsackResult dp_res = solve_knapsack_dp(arr, count, cap);
                KnapsackResult gr_res = solve_knapsack_greedy(arr, count, cap);

                printf("\n=== 0/1 KNAPSACK TRUCK PACKING OPTIMIZATION ===\n");
                printf("1. Dynamic Programming 0/1: Max Value = $%.2f | Weight = %.1f kg | Items = %d\n",
                       dp_res.max_value, dp_res.total_weight, dp_res.selected_count);
                printf("2. Greedy Fractional      : Max Value = $%.2f | Weight = %.1f kg | Items = %d\n",
                       gr_res.max_value, gr_res.total_weight, gr_res.selected_count);
                free(arr);
                break;
            }
            case 10: {
                int src, dest;
                printf("Enter Start Zone ID (0-4): "); scanf("%d", &src);
                printf("Enter Target Zone ID (0-4): "); scanf("%d", &dest);

                ShortestPathResult p = find_shortest_picking_route(g_warehouse_graph, src, dest);
                printf("\n=== DIJKSTRA SHORTEST PICKING PATH ===\n");
                printf("Total Distance: %d meters\nRoute Path: ", p.total_distance);
                for (int i = 0; i < p.path_length; i++) {
                    printf("%s %s", g_warehouse_graph->vertices[p.path[i]].name, (i < p.path_length - 1) ? "-> " : "");
                }
                printf("\n");
                break;
            }
            default:
                printf("\nInvalid option. Try again.\n");
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize data structure containers
    g_undo_stack = create_stack();
    g_order_queue = create_queue();

    // Populate initial default warehouse dataset
    seed_sample_inventory();

    // Check if called with JSON RPC mode
    if (argc >= 2 && strcmp(argv[1], "--json") == 0) {
        handle_json_command(argc, argv);
    } else {
        interactive_menu();
    }

    // Cleanup memory before exit
    free_inventory_list(&g_inventory_head);
    free_bst(g_bst_root);
    free_stack(g_undo_stack);
    free_queue(g_order_queue);
    free_warehouse_graph(g_warehouse_graph);

    return 0;
}
