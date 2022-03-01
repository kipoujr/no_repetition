// Compile with: g++ --std=c++11 -o bin/a.exe -Wl,--stack,33554432 -O3 src/test.cpp
// Mac compile with: clang++ --std=c++11 -Wl,-stack_size -Wl,0x1000000 -O3 -o a.out src/test.cpp 

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <unistd.h>
#include <fstream>
#include <chrono>
#include <vector>

// #include <sys/resource.h>

#define DEBUG
#define SEEDED_RANDOM

#include "framework/randomgen.h"
#include "framework/hashing.h"
#include "framework/fast_hashing.h"

using namespace std;
uint64_t A[2000005];
int main(int argc, char *argv[]) {
  uint32_t tim[10];
  ifstream data;
  ofstream myfile;
  data.open("dataset.txt");
  myfile.open ("tests.txt");
    init_randomness();
    uint32_t k0 = 700; 
    uint32_t n, k, nExp;
    if (argc < 3) {
      n = 1815365;
      k = 100*k0;
      nExp = 50000;
    } else {
      n = atoi(argv[1]);
      k = atoi(argv[2]) * k0;
      nExp = atoi(argv[3]);
    }
    for(int i=1; i<=n; ++i) {
      data >> A[i];
    }
    data.close();

#ifdef MYHASH1_4
    tab1perm4_64 hash;
    k *= 5;
#endif
#ifdef MYHASH1_8
    tab1perm8_64 hash;
#endif
#ifdef MYHASH2
    polytwo_64 hash;
#endif
#ifdef MYHASH3
    polyhash_64<3> hash;
#endif
#ifdef MYHASH4
    multishift_64 hash;
#endif
#ifdef MYHASH5_4
    simpletab4_64 hash;
#endif
#ifdef MYHASH5_8
    simpletab8_64 hash;
#endif
#ifdef MYHASH6_4
    twisttab4_64 hash;
#endif
#ifdef MYHASH6_8
    twisttab8_64 hash;
#endif
#ifdef MYHASH7
    mixedtab8_64 hash;
#endif
#ifdef MYHASH8
    murmurHash3 hash;
#endif
#ifdef MYHASH9
    polyhash_64<100> hash;
#endif
#ifdef MYHASH10
    blake3 hash;
#endif
    
    hash.init();    
    uint32_t counter;
    __uint128_t n128=(__uint128_t)n, thresholdRightHand = (((__uint128_t)1)<<64)*k;
    for(uint32_t exp=1; exp<=nExp; ++exp) {
#if defined MYHASH2 || defined MYHASH4
      for(int run=1; run<=5; ++run) {      
#endif      
      #if defined MYHASH2 || defined MYHASH3 || defined MYHASH4 || defined MYHASH8 || defined MYHASH10
      hash.init();
      #endif
#ifndef COUNT_TIME       
      hash.init(); //its the same input again and again
#endif

      counter = 0;
      for(uint32_t i=1; i<=n; ++i) {
        // sample if hash(currNum)/2^64 <= k/n
        if (n128*hash(A[i]) <= thresholdRightHand) {
          ++counter;
        }
      }
#if defined MYHASH2 || defined MYHASH4
      tim[run] = (uint32_t)(1.0*counter*n/k);
      }
      sort(tim+1, tim+6);
      myfile << tim[3] << '\n';      
#endif
#ifdef MYHASH1_4
      myfile << (uint32_t)(1.0*counter*n/k) << '\n';
#endif

      if(exp % (nExp/20) == 0) {
        cout << 100*exp/nExp << "% - Finished experiment " << exp << " out of " << nExp << '\n';
      }

    }
#ifdef DEBUG
    cout << "Random bytes used: " << usedBytes << endl;
#endif
    
    return 0;
}
