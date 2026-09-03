#ifndef LINEAR_DS_H
#define LINEAR_DS_H

#include "inventory.h"

// 1. Linked List Node for Inventory
typedef struct ItemNode {
    Item data;
    struct ItemNode* next;
    struct ItemNode* prev; // Doubly linked list
} ItemNode;

// Linked List Management API
ItemNode* create_item_node(Item item);
void insert_item_head(ItemNode** head, Item item);
void insert_item_tail(ItemNode** head, Item item);
bool delete_item_by_id(ItemNode** head, int id, Item* deleted_out);
ItemNode* search_item_by_id(ItemNode* head, int id);
void update_item_quantity(ItemNode* head, int id, int new_qty);
void traverse_inventory(ItemNode* head);
int get_inventory_count(ItemNode* head);
void free_inventory_list(ItemNode** head);

// 2. Stack (Using Linked List) for Action Undo/History
typedef struct StackNode {
    ActionRecord record;
    struct StackNode* next;
} StackNode;

typedef struct ActionStack {
    StackNode* top;
    int size;
} ActionStack;

ActionStack* create_stack();
void push_action(ActionStack* stack, ActionRecord record);
bool pop_action(ActionStack* stack, ActionRecord* out_record);
bool peek_action(ActionStack* stack, ActionRecord* out_record);
void free_stack(ActionStack* stack);

// 3. Queue (Using Linked List) for Order Processing
typedef struct QueueNode {
    Order order;
    struct QueueNode* next;
} QueueNode;

typedef struct OrderQueue {
    QueueNode* front;
    QueueNode* rear;
    int count;
} OrderQueue;

OrderQueue* create_queue();
void enqueue_order(OrderQueue* queue, Order order);
bool dequeue_order(OrderQueue* queue, Order* out_order);
bool peek_order(OrderQueue* queue, Order* out_order);
void print_order_queue(OrderQueue* queue);
void free_queue(OrderQueue* queue);

// 4. Stack Application: Simple Expression Evaluator for Pricing & Discounts
// Evaluates simple arithmetic expressions like "100 * 0.90 + 5" using stack
double evaluate_price_expression(const char* expr);

#endif // LINEAR_DS_H
