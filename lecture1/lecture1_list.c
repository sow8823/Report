#include <stdio.h>
#include <stdlib.h>

struct list {
  struct list *next;
  int data;
};

int main() {
  struct list *head = NULL;
  struct list *current = NULL;

  for (int i = 1; i <= 100; i++) {
    struct list *new = malloc(sizeof(struct list));
    new->data = i;
    new->next = NULL;

    if (head == NULL) {
      head = new;
      current = new;
    } else {
      current->next = new;
      current = new;
    }
  }

  current = head;
  while (current != NULL) {
    printf("%d ", current->data);
    current = current->next;
  }

  current = head;
  while (current != NULL) {
    struct list *tes = current;
    current = current->next;
    free(tes);
  }
}