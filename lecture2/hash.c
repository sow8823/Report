#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define bucket_size (10)
int value = 0;
int delta = 3;
int default_value = -9;

int hash_table[bucket_size];

void
init_hash_table()
{
  for (int i = 0; i < bucket_size; i++)
    hash_table[i] = default_value;

  for (int i = 0; i < bucket_size; i++) {
    int value = value + rand() % 9;
    int rem = value % bucket_size;

    while (hash_table[rem] != default_value) {
      rem += delta;
      if (rem >= bucket_size) {
          rem -= bucket_size;
      }
    }
    hash_table[rem] = value;
  }
  for (int i = 0; i < bucket_size; i++) {
    printf("%d ", hash_table[i]);
  }
  printf("\n");
}


void
hash_search(int key)
{
  int rem = key % bucket_size;
  int step = 0;
  while (true) {
    step++;
    if (hash_table[rem] == default_value) {
      break;
    }
    else if (hash_table[rem] == key) {
      printf("Found (Step = %d)\n", step);
      break;
    }
    else rem += delta;
    if (rem >= bucket_size) rem -= bucket_size;
  }
}

int main() {
  int key;
  
  init_hash_table();
  printf("Key? ");
  scanf("%d", &key);
  hash_search(key);
}