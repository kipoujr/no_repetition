#include "blake3.h"
#include <stdio.h>
#include <unistd.h>

int main() {
  // Initialize the hasher.
  blake3_hasher hasher;
  uint8_t output[BLAKE3_OUT_LEN];  
  int n = 50000000;
  uint64_t kaiKala = 0;
  
  for(uint64_t i=(1L<<35); i<(1L<<35)+n; ++i) {
    blake3_hasher_init(&hasher);    
    blake3_hasher_update(&hasher, &i, 8);
  }

  blake3_hasher_finalize(&hasher, output, BLAKE3_OUT_LEN);
  for (size_t i = 0; i < BLAKE3_OUT_LEN; i++) {
    kaiKala += (uint64_t)output[i];
  }  
  printf("%lld %d\n", (long long) kaiKala, BLAKE3_OUT_LEN);
  return 0;
}
