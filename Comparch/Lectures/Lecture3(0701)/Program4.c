/*
    Creating Linked List
 */
#include <stdlib.h>
#include <stdio.h>

// Initialize Node, head, and tail
struct Node {
    struct Node* next;
    int val;
};

struct Node* head = NULL;
struct Node* tail = NULL;

void InsertNode(int);
void PrintLinkedList();

int main(int argc, char** argv) {

    InsertNode(1);
    InsertNode(3);
    InsertNode(5);
    InsertNode(7);
    PrintLinkedList();

    return 0;
}

void InsertNode(int num) {
    struct Node * n = (struct Node *)malloc(sizeof(struct Node));
    if (n == NULL) {
        printf("Could not create node\n");
        exit(0);
    }
    n->next = NULL;
    n->val = num;

    if (tail == NULL) {
        // This is the first node!
        head = n;
        tail = n;
    }
    else {
        tail->next = n;
        tail = n;
    }
}

void PrintLinkedList() {
    struct Node* temp = head;
    while (temp != NULL) {
        printf("%d ", temp->val);
        temp = temp->next;
    }
    printf("\n");
}
