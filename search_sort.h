#ifndef SEARCH_SORT_H
#define SEARCH_SORT_H

#include "inventory.h"

// Benchmark Metrics Result Structure
typedef struct BenchmarkResult {
    char algorithm_name[40];
    int element_count;
    double time_taken_ms;
    long comparisons;
    long swaps;
} BenchmarkResult;

// 1. Searching Algorithms
int linear_search_array(Item arr[], int size, int target_id, long* comparisons);
int binary_search_array(Item arr[], int size, int target_id, long* comparisons);

// 2. Sorting Algorithms (Sort by Price, Stock, or ID)
// Sort Types: 0 = Sort by ID, 1 = Sort by Price, 2 = Sort by Quantity
void quick_sort_items(Item arr[], int low, int high, int sort_key, long* comparisons, long* swaps);
void merge_sort_items(Item arr[], int l, int r, int sort_key, long* comparisons);
void insertion_sort_items(Item arr[], int size, int sort_key, long* comparisons, long* swaps);

// 3. Performance Benchmark Comparison Suite
void run_algorithm_benchmark(Item arr[], int size, BenchmarkResult results[], int* num_results);

#endif // SEARCH_SORT_H
