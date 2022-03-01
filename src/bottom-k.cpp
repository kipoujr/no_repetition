// Compile with: g++ --std=c++11 -o bin/a.exe -Wl,--stack,33554432 -O3 src/test.cpp
// Mac compile with: clang++ --std=c++11 -Wl,-stack_size -Wl,0x1000000 -O3 -o a.out src/test.cpp 

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <fstream>
#include <chrono>
#include <vector>
#include <set>

// #include <sys/resource.h>

#define DEBUG
#define SEEDED_RANDOM
#define p64 pair<uint64_t,uint64_t>

#include "framework/randomgen.h"
#include "framework/hashing.h"
#include "framework/fast_hashing.h"

using namespace std;

int main(int argc, char *argv[]) {
  ofstream myFile;
  ofstream myTime;  
  myFile.open ("tests.txt");
  myTime.open ("timesPerExperiment");
  init_randomness();
  uint32_t k0 = 700; //theoretical for e=0.03 according to the survey (the simple reasoning gives more)
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
    polyhash_64<100> hash;
#endif
#ifdef MYHASH10
    blake3 hash;
#endif
    hash.init();    
    
    uint32_t counter, t, i;
    uint64_t bigValue = 1L<<35; //feel free to change constant, depending on how much reinitialization of the tables you want
    uint64_t currNum=bigValue;
    set< p64 > S;
    set< p64 >::iterator pos;    
    p64 maxim, tmp;

    for(uint32_t exp=1; exp<=nExp; ++exp) {
      #if defined MYHASH2 || defined MYHASH3 || defined MYHASH4 || defined MYHASH8 || defined MYHASH10
      hash.init();
      currNum = bigValue;
      #else
      if (currNum < n) {
        #ifndef COUNT_TIME
        hash.init();
        #endif
        currNum=bigValue;
      }
      #endif
      S.clear();
      t = clock();      

      for(i=1; S.size() < k && i<=n; ++i) {
        --currNum;
        S.insert( {hash(currNum),currNum} );
      }
      pos = S.end(); --pos; maxim = *pos;
      
      for(;i<=n; ++i) {
        --currNum;
        tmp = {hash(currNum),currNum};
        if (tmp < maxim) {
          //Insert tmp
          S.insert(tmp);

          //Remove max element
          pos = S.end(); --pos; S.erase(pos);

          //Update max
          pos = S.end(); --pos; maxim = *pos;          
        }
      }

      t = clock() - t;
      uint32_t res = (uint32_t)((((__uint128_t)1)<<64)*(k-1) / maxim.first);      
      myTime << t << '\n';
      myFile << res << '\n';

      if( (exp % (nExp/20)) == 0) {
        printf("%d%% - Finished experiment %d out of %d\n", 100*exp/nExp, exp, nExp);
      }
    }
#ifdef DEBUG
    cout << "Random bytes used: " << usedBytes << endl;
#endif
    
    return 0;
}
