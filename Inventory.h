#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME 50
#define MAX_CATEGORY 30
#define MAX_AISLES 10

// Core Item Record Structure
typedef struct Item {
    int id;
    char name[MAX_NAME];
    char category[MAX_CATEGORY];
    int quantity;
    double price;
    double weight; // For Knapsack algorithm
    double value;  // For Knapsack algorithm (price * quantity or strategic value)
    int aisle_id;  // Warehouse location aisle (0 to MAX_AISLES - 1)
} Item;

// Action Types for Undo Stack
typedef enum {
    ACTION_ADD_ITEM,
    ACTION_DELETE_ITEM,
    ACTION_UPDATE_STOCK,
    ACTION_DISPATCH_ORDER
} ActionType;

// Action Log for Undo Stack
typedef struct ActionRecord {
    ActionType type;
    Item item;
    int prev_quantity;
    char timestamp[30];
} ActionRecord;

// Order Structure for Queue
typedef struct Order {
    int order_id;
    char customer[MAX_NAME];
    int item_id;
    char item_name[MAX_NAME];
    int quantity;
    double total_price;
    char status[20]; // "PENDING", "DISPATCHED", "CANCELLED"
} Order;

#endif // INVENTORY_H
