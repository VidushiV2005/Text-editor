#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Node for linked list
typedef struct Node {
    char data;
    struct Node *next;
} Node;

// Stack Node
typedef struct StackNode {
    char operation;  // 'i' for insert, 'd' for delete
    char data;       // Character involved
    int position;    // Position in the linked list
    struct StackNode *next;
} StackNode;

// Global Variables
Node *head = NULL;
StackNode *undoStack = NULL, *redoStack = NULL;
int changes = 0; // Change counter

// Linked List Operations
void insertChar(int position, char ch);
void deleteChar(int position);
void displayText();
void saveToFile(const char *filename);

// Stack Operations
void push(StackNode **stack, char operation, char data, int position);
StackNode *pop(StackNode **stack);

// Editor Operations
void undo();
void redo();

int main() {
    int choice, position;
    char ch;
    char filename[100];

    while (1) {
        printf("\n--- Text Editor ---\n");
        printf("1. Insert Character\n");
        printf("2. Delete Character\n");
        printf("3. Undo\n");
        printf("4. Redo\n");
        printf("5. Display Text\n");
        printf("6. Save to File\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            printf("Enter position to insert (0-based): ");
            scanf("%d", &position);
            printf("Enter character to insert: ");
            scanf(" %c", &ch);
            insertChar(position, ch);
            changes++;
            break;
        case 2:
            printf("Enter position to delete (0-based): ");
            scanf("%d", &position);
            deleteChar(position);
            changes++;
            break;
        case 3:
            undo();
            break;
        case 4:
            redo();
            break;
        case 5:
            displayText();
            break;
        case 6:
            printf("Enter filename to save: ");
            scanf("%s", filename);
            saveToFile(filename);
            printf("File saved successfully.\n");
            break;
        case 7:
            printf("Exiting editor. Total changes made: %d\n", changes);
            exit(0);
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

// Insert a character at a given position
void insertChar(int position, char ch) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = ch;
    newNode->next = NULL;

    // Insert at the beginning
    if (position == 0) {
        newNode->next = head;
        head = newNode;
        push(&undoStack, 'i', ch, position);
        return;
    }

    Node *temp = head;
    int count = 0;

    // Traverse to the position
    while (temp != NULL && count < position - 1) {
        temp = temp->next;
        count++;
    }

    // Insert at the position
    if (temp != NULL) {
        newNode->next = temp->next;
        temp->next = newNode;
        push(&undoStack, 'i', ch, position);
    } else {
        printf("Invalid position.\n");
        free(newNode);
    }
}

// Delete a character at a given position
void deleteChar(int position) {
    if (head == NULL) {
        printf("Nothing to delete.\n");
        return;
    }

    Node *temp = head;

    // If deleting at the head (position 0)
    if (position == 0) {
        head = temp->next;
        push(&undoStack, 'd', temp->data, position);
        free(temp);
        return;
    }

    int count = 0;
    Node *prev = NULL;

    // Traverse to the node to delete
    while (temp != NULL && count < position) {
        prev = temp;
        temp = temp->next;
        count++;
    }

    // If the position is valid
    if (temp != NULL) {
        prev->next = temp->next;
        push(&undoStack, 'd', temp->data, position);
        free(temp);
    } else {
        printf("Invalid position.\n");
    }
}

// Display the current text in the editor
void displayText() {
    Node *temp = head;
    printf("Current Text: ");
    while (temp) {
        printf("%c", temp->data);
        temp = temp->next;
    }
    printf("\n");
}

// Save the current text to a file
void saveToFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Unable to save file.\n");
        return;
    }
    Node *temp = head;
    while (temp) {
        fputc(temp->data, file);
        temp = temp->next;
    }
    fclose(file);
}

// Push an action onto the stack (undo/redo)
void push(StackNode **stack, char operation, char data, int position) {
    StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
    newNode->operation = operation;
    newNode->data = data;
    newNode->position = position;
    newNode->next = *stack;
    *stack = newNode;
}

// Pop an action from the stack
StackNode *pop(StackNode **stack) {
    if (!*stack) return NULL;
    StackNode *temp = *stack;
    *stack = (*stack)->next;
    return temp;
}

// Undo the last action
void undo() {
    StackNode *topUndo = pop(&undoStack);
    if (!topUndo) {
        printf("No actions to undo.\n");
        return;
    }

    if (topUndo->operation == 'i') {
        // Undo insert - delete the character
        deleteChar(topUndo->position);
    } else if (topUndo->operation == 'd') {
        // Undo delete - insert the character
        insertChar(topUndo->position, topUndo->data);
    }

    // Push the action onto the redo stack
    push(&redoStack, topUndo->operation, topUndo->data, topUndo->position);
    free(topUndo);
}

// Redo the last undone action
void redo() {
    StackNode *topRedo = pop(&redoStack);
    if (!topRedo) {
        printf("No actions to redo.\n");
        return;
    }

    if (topRedo->operation == 'i') {
        // Redo insert
        insertChar(topRedo->position, topRedo->data);
    } else if (topRedo->operation == 'd') {
        // Redo delete
        deleteChar(topRedo->position);
    }

    // Push the action back onto the undo stack
    push(&undoStack, topRedo->operation, topRedo->data, topRedo->position);
    free(topRedo);
}
