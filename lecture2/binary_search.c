#include <stdio.h>
#include <stdlib.h>

int binary_search(int K, int m, int n){
  int target[49] = {};
  // int* target = new int[50];
  for(int a = 0; a < 50; a++) {
    target[a] = a;
  }
  int i, j, p;
  int times = 0;
  i = m; j = n;
  while (i <= j){
    times++;
    p = (i + j)/2;
    if (K < target[p]) j = p-1;
    else if (K > target[p]) i = p+1;
    else if (K == target[p]) return times;
  }
  return -1;
}

int main() {
  int result = binary_search(4, 0, 50);
  printf("Found (step = %d)", result);
}