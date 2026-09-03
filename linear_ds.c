#include "../include/linear_ds.h"
#include <ctype.h>
#include <math.h>

// --- 1. LINKED LIST IMPLEMENTATION ---

ItemNode* create_item_node(Item item) {
    ItemNode* node = (ItemNode*)malloc(sizeof(ItemNode));
    if (!node) {
        perror("Failed to allocate memory for ItemNode");
        return NULL;
    }
    node->data = item;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void insert_item_head(ItemNode** head, Item item) {
    ItemNode* node = create_item_node(item);
    if (!node) return;
    if (*head != NULL) {
        node->next = *head;
        (*head)->prev = node;
    }
    *head = node;
}

void insert_item_tail(ItemNode** head, Item item) {
    ItemNode* node = create_item_node(item);
    if (!node) return;
    if (*head == NULL) {
        *head = node;
        return;
    }
    ItemNode* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = node;
    node->prev = temp;
}

bool delete_item_by_id(ItemNode** head, int id, Item* deleted_out) {
    if (!head || !*head) return false;
    ItemNode* temp = *head;

    while (temp != NULL && temp->data.id != id) {
        temp = temp->next;
    }

    if (temp == NULL) return false; // Not found

    if (deleted_out) {
        *deleted_out = temp->data;
    }

    if (temp->prev != NULL) {
        temp->prev->next = temp->next;
    } else {
        *head = temp->next; // Deleted head
    }

    if (temp->next != NULL) {
        temp->next->prev = temp->prev;
    }

    free(temp);
    return true;
}

ItemNode* search_item_by_id(ItemNode* head, int id) {
    ItemNode* temp = head;
    while (temp != NULL) {
        if (temp->data.id == id) return temp;
        temp = temp->next;
    }
    return NULL;
}

void update_item_quantity(ItemNode* head, int id, int new_qty) {
    ItemNode* node = search_item_by_id(head, id);
    if (node) {
        node->data.quantity = new_qty;
        node->data.value = node->data.price * new_qty;
    }
}

void traverse_inventory(ItemNode* head) {
    printf("\n=== INVENTORY RECORDS (LINKED LIST TRAVERSAL) ===\n");
    printf("%-6s %-25s %-15s %-8s %-10s %-8s %-6s\n", 
           "ID", "Name", "Category", "Stock", "Price ($)", "Weight", "Aisle");
    printf("-----------------------------------------------------------------------------\n");
    ItemNode* curr = head;
    while (curr != NULL) {
        printf("%-6d %-25s %-15s %-8d $%-9.2f %-8.1f Aisle %d\n",
               curr->data.id, curr->data.name, curr->data.category,
               curr->data.quantity, curr->data.price, curr->data.weight, curr->data.aisle_id);
        curr = curr->next;
    }
    printf("-----------------------------------------------------------------------------\n");
}

int get_inventory_count(ItemNode* head) {
    int count = 0;
    ItemNode* curr = head;
    while (curr != NULL) {
        count++;
        curr = curr->next;
    }
    return count;
}

void free_inventory_list(ItemNode** head) {
    ItemNode* curr = *head;
    while (curr != NULL) {
        ItemNode* next = curr->next;
        free(curr);
        curr = next;
    }
    *head = NULL;
}


// --- 2. STACK IMPLEMENTATION (ACTION UNDO LOGS) ---

ActionStack* create_stack() {
    ActionStack* stack = (ActionStack*)malloc(sizeof(ActionStack));
    stack->top = NULL;
    stack->size = 0;
    return stack;
}

void push_action(ActionStack* stack, ActionRecord record) {
    StackNode* node = (StackNode*)malloc(sizeof(StackNode));
    node->record = record;
    node->next = stack->top;
    stack->top = node;
    stack->size++;
}

bool pop_action(ActionStack* stack, ActionRecord* out_record) {
    if (!stack || stack->top == NULL) return false;
    StackNode* temp = stack->top;
    if (out_record) {
        *out_record = temp->record;
    }
    stack->top = temp->next;
    free(temp);
    stack->size--;
    return true;
}

bool peek_action(ActionStack* stack, ActionRecord* out_record) {
    if (!stack || stack->top == NULL) return false;
    if (out_record) {
        *out_record = stack->top->record;
    }
    return true;
}

void free_stack(ActionStack* stack) {
    if (!stack) return;
    ActionRecord rec;
    while (pop_action(stack, &rec));
    free(stack);
}


// --- 3. QUEUE IMPLEMENTATION (ORDER DISPATCH PROCESSING) ---

OrderQueue* create_queue() {
    OrderQueue* queue = (OrderQueue*)malloc(sizeof(OrderQueue));
    queue->front = NULL;
    queue->rear = NULL;
    queue->count = 0;
    return queue;
}

void enqueue_order(OrderQueue* queue, Order order) {
    QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
    node->order = order;
    node->next = NULL;

    if (queue->rear == NULL) {
        queue->front = node;
        queue->rear = node;
    } else {
        queue->rear->next = node;
        queue->rear = node;
    }
    queue->count++;
}

bool dequeue_order(OrderQueue* queue, Order* out_order) {
    if (!queue || queue->front == NULL) return false;
    QueueNode* temp = queue->front;
    if (out_order) {
        *out_order = temp->order;
    }
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);
    queue->count--;
    return true;
}

bool peek_order(OrderQueue* queue, Order* out_order) {
    if (!queue || queue->front == NULL) return false;
    if (out_order) {
        *out_order = queue->front->order;
    }
    return true;
}

void print_order_queue(OrderQueue* queue) {
    printf("\n=== PENDING ORDER QUEUE (FIFO) ===\n");
    printf("%-8s %-20s %-20s %-8s %-10s %-10s\n",
           "Order ID", "Customer", "Item", "Qty", "Total ($)", "Status");
    printf("-----------------------------------------------------------------------------\n");
    QueueNode* curr = queue->front;
    while (curr != NULL) {
        printf("%-8d %-20s %-20s %-8d $%-9.2f %-10s\n",
               curr->order.order_id, curr->order.customer, curr->order.item_name,
               curr->order.quantity, curr->order.total_price, curr->order.status);
        curr = curr->next;
    }
    printf("-----------------------------------------------------------------------------\n");
}

void free_queue(OrderQueue* queue) {
    if (!queue) return;
    Order ord;
    while (dequeue_order(queue, &ord));
    free(queue);
}


// --- 4. STACK EXPRESSION EVALUATOR (PRICING CALCULATION) ---

typedef struct DoubleStackNode {
    double data;
    struct DoubleStackNode* next;
} DoubleStackNode;

typedef struct CharStackNode {
    char data;
    struct CharStackNode* next;
} CharStackNode;

static void push_val(DoubleStackNode** top, double val) {
    DoubleStackNode* n = (DoubleStackNode*)malloc(sizeof(DoubleStackNode));
    n->data = val;
    n->next = *top;
    *top = n;
}

static double pop_val(DoubleStackNode** top) {
    if (!*top) return 0;
    DoubleStackNode* temp = *top;
    double val = temp->data;
    *top = temp->next;
    free(temp);
    return val;
}

static void push_op(CharStackNode** top, char op) {
    CharStackNode* n = (CharStackNode*)malloc(sizeof(CharStackNode));
    n->data = op;
    n->next = *top;
    *top = n;
}

static char pop_op(CharStackNode** top) {
    if (!*top) return 0;
    CharStackNode* temp = *top;
    char op = temp->data;
    *top = temp->next;
    free(temp);
    return op;
}

static int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

static double apply_op(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b != 0 ? a / b : 0;
    }
    return 0;
}

double evaluate_price_expression(const char* expr) {
    DoubleStackNode* valStack = NULL;
    CharStackNode* opStack = NULL;

    for (int i = 0; expr[i] != '\0'; i++) {
        if (isspace(expr[i])) continue;

        if (isdigit(expr[i]) || expr[i] == '.') {
            double val = 0;
            char num_str[32];
            int k = 0;
            while (isdigit(expr[i]) || expr[i] == '.') {
                num_str[k++] = expr[i++];
            }
            num_str[k] = '\0';
            i--; // Adjust index
            val = atof(num_str);
            push_val(&valStack, val);
        } else if (expr[i] == '(') {
            push_op(&opStack, expr[i]);
        } else if (expr[i] == ')') {
            while (opStack && opStack->data != '(') {
                double val2 = pop_val(&valStack);
                double val1 = pop_val(&valStack);
                char op = pop_op(&opStack);
                push_val(&valStack, apply_op(val1, val2, op));
            }
            if (opStack) pop_op(&opStack); // remove '('
        } else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
            while (opStack && precedence(opStack->data) >= precedence(expr[i])) {
                double val2 = pop_val(&valStack);
                double val1 = pop_val(&valStack);
                char op = pop_op(&opStack);
                push_val(&valStack, apply_op(val1, val2, op));
            }
            push_op(&opStack, expr[i]);
        }
    }

    while (opStack) {
        double val2 = pop_val(&valStack);
        double val1 = pop_val(&valStack);
        char op = pop_op(&opStack);
        push_val(&valStack, apply_op(val1, val2, op));
    }

    return valStack ? pop_val(&valStack) : 0.0;
}
