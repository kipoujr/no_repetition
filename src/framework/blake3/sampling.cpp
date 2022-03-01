// Compile with: g++ --std=c++11 -o bin/a.exe -Wl,--stack,33554432 -O3 src/test.cpp
// Mac compile with: clang++ --std=c++11 -Wl,-stack_size -Wl,0x1000000 -O3 -o a.out src/test.cpp 

#include <iostream>
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

int main(int argc, char *argv[]) {
  ofstream myfile;
  myfile.open ("tests.txt");
    init_randomness();
    uint32_t k0 = 8192; //theoretical for e=0.01 according to the survey (our reasoning gives 80,000)
    uint32_t n, k, nExp;
    if (argc < 3) {
      n = 5000000;
      k = 100*k0;
      nExp = 50000;
    } else {
      n = atoi(argv[1]);
      k = atoi(argv[2]) * k0;
      nExp = atoi(argv[3]);
    }

#ifdef MYHASH1_4
    tab1perm4_64 hash;
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
    polyhash_32<100> hash;
#endif
#ifdef MYHASH10
    blake3 hash;
#endif
    
    uint32_t fail = 0;
    uint32_t counter;
    uint32_t counter2;
    uint64_t currNum=0;
    __uint128_t n128=(__uint128_t)n, thresholdRightHand = (((__uint128_t)1)<<64)*k;
    hash.init();
    for(uint32_t exp=1; exp<=nExp; ++exp) {
      #if defined MYHASH2 || defined MYHASH3 || defined MYHASH4 || defined MYHASH8 || defined MYHASH10
      hash.init();
      currNum = 0;
      #else
      if (currNum + n > (1L<<35)) { //feel free to change constant
        hash.init();
        currNum=0;
      }
      #endif
        
      counter = 0;
      for(uint32_t i=1; i<=n; ++i) {
        ++currNum;
        // sample if hash(currNum)/2^64 <= k/n
        if (n128*hash(currNum) <= thresholdRightHand) {
          ++counter;
        }
      }
      myfile << (uint32_t)(1.0*counter*n/k) << '\n';

      if(exp % (nExp/20) == 0) {
        printf("%d%% - Finished experiment %d out of %d\n", 100*exp/nExp, exp, nExp);
      }

#ifdef DEBUG
      if ( abs( 1.0*counter*n/k-n ) > 0.03*n) {
        //printf("Failed value--- %d\n", (uint32_t)(1.0*counter*n/k));
        ++fail;
      }      
#endif
    }
#ifdef DEBUG
    cout << "Failures (worse than 0.01): " << fail << "/" << nExp << '\n';
    cout << "Random bytes used: " << usedBytes << endl;
#endif
    
    return 0;
}
