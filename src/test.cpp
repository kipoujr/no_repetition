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
#include <iomanip>

// #include <sys/resource.h>

// #define DEBUG
#define SEEDED_RANDOM

#include "framework/randomgen.h"
// #include "framework/hashing.h"
#include "framework/fast_hashing.h"
#include "framework/algorithms.h"
#include "framework/data.h"

#ifdef MYHASH1_4
  using HASH = tab1perm4_64;
  #define SKIP_MEDIAN
#endif
#ifdef MYHASH1_8
  using HASH = tab1perm8_64;
  #define SKIP_MEDIAN
#endif
#ifdef MYHASH2
  using HASH = polytwo_64;
#endif
#ifdef MYHASH3
  using HASH = polyhash_64<3>;
#endif
#ifdef MYHASH4
  using HASH = multishift_64;
#endif
#ifdef MYHASH5_4
  using HASH = simpletab4_64;
#endif
#ifdef MYHASH5_8
  using HASH = simpletab8_64;
#endif
#ifdef MYHASH6_4
  using HASH = twisttab4_64;
#endif
#ifdef MYHASH6_8
  using HASH = twisttab8_64;
#endif
#ifdef MYHASH7
  using HASH = mixedtab8_64;
  #define SKIP_MEDIAN
#endif
#ifdef MYHASH7_1
  using HASH = mixedtab8_64_1;
  #define SKIP_MEDIAN
#endif
#ifdef MYHASH8
  using HASH = murmurHash3;
#endif
#ifdef MYHASH9
  using HASH = polyhash_64<100>;
#endif
#ifdef MYHASH10
  using HASH = blake3;
#endif

using namespace std;

int main(int argc, char *argv[]) {
  ofstream myfile;
  myfile.open ("tests.txt");

#if TIME_INDIVIDUAL
  ofstream time_file;
  time_file.open("timesPerExperiment");
#endif

  init_randomness();
  uint32_t k0 = 8192; //theoretical for e=0.01 according to the survey (our reasoning gives 80,000)
  uint32_t n, k, nExp;
  if (argc < 3) {
    n = 5000000;
    k = 100*k0;
    nExp = 50000;
  } else {
    n = atoi(argv[1]);
    k = atoi(argv[2]); // * k0;
    nExp = atoi(argv[3]);
  }

#if defined STRUCTURED_DATA
  StructuredData data(1L<<35, 1); // feel free to change constant, depending on how much reinitialization of the tables you want
#elif defined RANDOM_DATA
  vector<uint64_t> data(n);
  polyhash_64<100> rand_gen;
  rand_gen.init();
  for (uint64_t i = 0; i < n; ++i) {
    data[i] = rand_gen(i);
  }
#elif defined REAL1_DATA
  vector<uint64_t> data(n);
  ifstream data_file;
  data_file.open("dataset.txt");
  for (uint64_t i = 0; i < n; ++i) {
    data_file >> data[i];
  }
  data_file.close();
#elif defined REAL2_DATA
  vector<uint64_t> data(n);
  ifstream data_file;
  data_file.open("newDataset.txt");
  for (uint64_t i = 0; i < n; ++i) {
    data_file >> data[i];
  }
  data_file.close();
#elif defined ATMOSPHERIC_DATA
  vector<uint64_t> data(n);
  ifstream data_file;
  data_file.open("atmospheric.txt");
  for (uint64_t i = 0; i < n; ++i) {
    data_file >> data[i];
  }
  data_file.close();
#elif defined REAL_PROCESSED_DATA
  vector<uint64_t> data(n);
  ifstream data_file;
  data_file.open("dataset_processed.txt");
  for (uint64_t i = 0; i < n; ++i) {
    data_file >> data[i];
  }
  data_file.close();
#endif

#if COUNT_TIME
  vector<uint64_t> times;
#endif

#if MEDIAN
  uint32_t t = 5; // Currently hardcoded but can be changed.
  k = k / t; // We assume that t | k
  #ifndef SKIP_MEDIAN
  vector<HASH> hashes(t);
  #else 
  HASH hash;
  #endif
#else
  HASH hash;
#endif

  for(uint32_t exp=1; exp<=nExp; ++exp) {
#if MEDIAN && !defined SKIP_MEDIAN
    for(uint32_t i = 0; i < t; ++i) {
      hashes[i].init();
    }
#else
    hash.init();
#endif

#if COUNT_TIME
    auto start = chrono::high_resolution_clock::now();
#endif

    uint64_t res;
#if defined ALG_SAMPLING
  #if MEDIAN
    #ifndef SKIP_MEDIAN
      res = sampling_median(hashes, t, k, n, data.begin());
    #else
      res = sampling(hash, k * t, n, data.begin());
    #endif
  #else
    res = sampling(hash, k, n, data.begin());
  #endif
#elif defined ALG_BOTTOM_K
  #if MEDIAN
    #ifndef SKIP_MEDIAN
      res = bottom_k_median(hashes, t, k, n, data.begin());
    #else
      res = bottom_k(hash, k * t, n, data.begin());
    #endif
  #else
    res = bottom_k(hash, k, n, data.begin());
  #endif
#endif

#if COUNT_TIME
    auto end = chrono::high_resolution_clock::now();
    times.push_back(chrono::duration_cast<chrono::microseconds>(end - start).count());
  #if TIME_INDIVIDUAL
    time_file << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
  #endif
#endif

myfile << res << '\n';

// #ifdef DEBUG
    if(nExp >= 20 && exp % (nExp/20) == 0) {
      fprintf(stderr, "%d%% - Finished experiment %d out of %d\n", 100*exp/nExp, exp, nExp);
    }
// #endif

  }

#if COUNT_TIME
    long double mean = 0;
    for (uint32_t i = 0; i < nExp; ++i) {
        mean += times[i];
    }
    mean = mean/nExp;

    myfile << fixed << setprecision(0);
    myfile << endl << "Time: " << mean << endl;
  #ifdef DEBUG
    cerr << fixed << setprecision(0) << mean << endl; 
  #endif
    cout << fixed << setprecision(0) << mean << endl;
#endif


#ifdef DEBUG
  cerr << "Random bytes used: " << usedBytes << endl;
#endif
    
  return 0;
}
