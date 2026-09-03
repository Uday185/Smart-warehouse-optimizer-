#include "../include/search_sort.h"

// Helper function to compare items based on sort key
// sort_key: 0 = ID, 1 = Price, 2 = Quantity
static int compare_items(Item a, Item b, int sort_key) {
    if (sort_key == 1) { // Price
        if (a.price < b.price) return -1;
        if (a.price > b.price) return 1;
        return 0;
    } else if (sort_key == 2) { // Quantity
        if (a.quantity < b.quantity) return -1;
        if (a.quantity > b.quantity) return 1;
        return 0;
    } else { // ID
        if (a.id < b.id) return -1;
        if (a.id > b.id) return 1;
        return 0;
    }
}

static void swap_items(Item* a, Item* b) {
    Item temp = *a;
    *a = *b;
    *b = temp;
}

// --- 1. SEARCHING ALGORITHMS ---

int linear_search_array(Item arr[], int size, int target_id, long* comparisons) {
    if (comparisons) *comparisons = 0;
    for (int i = 0; i < size; i++) {
        if (comparisons) (*comparisons)++;
        if (arr[i].id == target_id) {
            return i; // Found at index i
        }
    }
    return -1; // Not found
}

int binary_search_array(Item arr[], int size, int target_id, long* comparisons) {
    if (comparisons) *comparisons = 0;
    int low = 0, high = size - 1;

    while (low <= high) {
        if (comparisons) (*comparisons)++;
        int mid = low + (high - low) / 2;

        if (arr[mid].id == target_id) return mid;
        if (arr[mid].id < target_id) low = mid + 1;
        else high = mid - 1;
    }

    return -1;
}

// --- 2. SORTING ALGORITHMS ---

// Insertion Sort
void insertion_sort_items(Item arr[], int size, int sort_key, long* comparisons, long* swaps) {
    if (comparisons) *comparisons = 0;
    if (swaps) *swaps = 0;

    for (int i = 1; i < size; i++) {
        Item key = arr[i];
        int j = i - 1;

        while (j >= 0) {
            if (comparisons) (*comparisons)++;
            if (compare_items(arr[j], key, sort_key) > 0) {
                arr[j + 1] = arr[j];
                if (swaps) (*swaps)++;
                j--;
            } else {
                break;
            }
        }
        arr[j + 1] = key;
    }
}

// QuickSort Partition
static int partition(Item arr[], int low, int high, int sort_key, long* comparisons, long* swaps) {
    Item pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (comparisons) (*comparisons)++;
        if (compare_items(arr[j], pivot, sort_key) <= 0) {
            i++;
            swap_items(&arr[i], &arr[j]);
            if (swaps) (*swaps)++;
        }
    }
    swap_items(&arr[i + 1], &arr[high]);
    if (swaps) (*swaps)++;
    return (i + 1);
}

void quick_sort_items(Item arr[], int low, int high, int sort_key, long* comparisons, long* swaps) {
    if (low < high) {
        int pi = partition(arr, low, high, sort_key, comparisons, swaps);
        quick_sort_items(arr, low, pi - 1, sort_key, comparisons, swaps);
        quick_sort_items(arr, pi + 1, high, sort_key, comparisons, swaps);
    }
}

// MergeSort Merge helper
static void merge(Item arr[], int l, int m, int r, int sort_key, long* comparisons) {
    int n1 = m - l + 1;
    int n2 = r - m;

    Item* L = (Item*)malloc(n1 * sizeof(Item));
    Item* R = (Item*)malloc(n2 * sizeof(Item));

    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (comparisons) (*comparisons)++;
        if (compare_items(L[i], R[j], sort_key) <= 0) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

void merge_sort_items(Item arr[], int l, int r, int sort_key, long* comparisons) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort_items(arr, l, m, sort_key, comparisons);
        merge_sort_items(arr, m + 1, r, sort_key, comparisons);
        merge(arr, l, m, r, sort_key, comparisons);
    }
}


// --- 3. BENCHMARK COMPARISON SUITE ---

void run_algorithm_benchmark(Item arr[], int size, BenchmarkResult results[], int* num_results) {
    if (size <= 0) return;

    int idx = 0;
    Item* copy = (Item*)malloc(size * sizeof(Item));

    // 1. QuickSort Benchmark
    memcpy(copy, arr, size * sizeof(Item));
    long comp_qs = 0, swap_qs = 0;
    clock_t start = clock();
    quick_sort_items(copy, 0, size - 1, 1, &comp_qs, &swap_qs);
    clock_t end = clock();

    snprintf(results[idx].algorithm_name, sizeof(results[idx].algorithm_name), "Fast Product Sort (O(N log N))");
    results[idx].element_count = size;
    results[idx].time_taken_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    results[idx].comparisons = comp_qs;
    results[idx].swaps = swap_qs;
    idx++;

    // 2. MergeSort Benchmark
    memcpy(copy, arr, size * sizeof(Item));
    long comp_ms = 0;
    start = clock();
    merge_sort_items(copy, 0, size - 1, 1, &comp_ms);
    end = clock();

    snprintf(results[idx].algorithm_name, sizeof(results[idx].algorithm_name), "City Stock Merge (O(N log N))");
    results[idx].element_count = size;
    results[idx].time_taken_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    results[idx].comparisons = comp_ms;
    results[idx].swaps = 0;
    idx++;

    // 3. InsertionSort Benchmark
    memcpy(copy, arr, size * sizeof(Item));
    long comp_is = 0, swap_is = 0;
    start = clock();
    insertion_sort_items(copy, size, 1, &comp_is, &swap_is);
    end = clock();

    snprintf(results[idx].algorithm_name, sizeof(results[idx].algorithm_name), "Daily Sales Insert (O(N^2))");
    results[idx].element_count = size;
    results[idx].time_taken_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    results[idx].comparisons = comp_is;
    results[idx].swaps = swap_is;
    idx++;

    // 4. Linear Search Benchmark
    int target_id = arr[size - 1].id; // worst case
    long comp_ls = 0;
    start = clock();
    linear_search_array(arr, size, target_id, &comp_ls);
    end = clock();

    snprintf(results[idx].algorithm_name, sizeof(results[idx].algorithm_name), "Store Item Search (O(N))");
    results[idx].element_count = size;
    results[idx].time_taken_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    results[idx].comparisons = comp_ls;
    results[idx].swaps = 0;
    idx++;

    // 5. Binary Search Benchmark (requires sorted array)
    quick_sort_items(copy, 0, size - 1, 0, NULL, NULL); // Sort by ID
    long comp_bs = 0;
    start = clock();
    binary_search_array(copy, size, target_id, &comp_bs);
    end = clock();

    snprintf(results[idx].algorithm_name, sizeof(results[idx].algorithm_name), "Quick Item Lookup (O(log N))");
    results[idx].element_count = size;
    results[idx].time_taken_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    results[idx].comparisons = comp_bs;
    results[idx].swaps = 0;
    idx++;

    *num_results = idx;
    free(copy);
}
