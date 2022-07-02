#ifndef _ALGORITHMS_H_
#define _ALGORITHMS_H_

#include <cstdio>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <queue>

#ifdef DEBUG
#include <cassert>
#endif

using namespace std;

/* ***************************************************
 * Sampling on numbers given in a vector.
 * ***************************************************/

template<typename T, typename S>
uint64_t sampling(T& hash, uint64_t k, uint64_t n, S data_stream) {
  uint64_t counter = 0;

  for(uint64_t i=1; i<=n; ++i) {
    uint64_t currNum = *(data_stream++);

    // sample if hash(currNum)/2^64 <= k/n <=> hash(currNum)*n <= k*2^64
    if ((__uint128_t)n*hash(currNum) <= ((__uint128_t)k << 64)) {
      ++counter;
    }
  }

  return (uint64_t)((long double)counter * n / k);
}

/* ***************************************************
 * Sampling t times and taking the median on numbers given in a vector.
 * It assume that t is an odd integer.
 * ***************************************************/

template<typename T, typename S>
uint64_t sampling_median(vector<T>& hashes, uint64_t t, uint32_t k, uint64_t n, S data_stream) {
#ifdef DEBUG
  assert(t % 2 == 1);
#endif

  vector<uint64_t> counters(t, 0);

  for(uint64_t i=1; i<=n; ++i) {
    uint64_t currNum = *data_stream++;

    for (uint32_t j = 0; j < t; ++j) {
      // sample if hash(currNum)/2^64 <= k/n <=> hash(currNum)*n <= k*2^64
      if ((__uint128_t)n*hashes[j](currNum) <= ((__uint128_t)k << 64)) {
        ++counters[j];
      }
    }
  }
  
  sort(counters.begin(), counters.end());

  return (uint64_t)((long double)counters[t/2] * n / k);
}

/* ***************************************************
 * Uses bottom-k sampling on numbers given in a vector.
 * It assume that t is an odd integer.
 * ***************************************************/

template<typename T, typename S>
uint64_t bottom_k(T& hash, uint64_t k, uint64_t n, S data_stream) {
  priority_queue<tuple<uint64_t, uint64_t> > que;

  uint64_t i = 0;
  for (; i < k && i < n; ++i) {
    uint64_t currNum = *data_stream++;
    que.push({hash(currNum), currNum});
  }

  tuple<uint64_t, uint64_t> maxVal = que.top();

  for (; i < n; ++i) {
    uint64_t currNum = *data_stream++;

    tuple<uint64_t, uint64_t> currVal = {hash(currNum), currNum};

    if (currVal < maxVal) {
      // Insert the new element
      que.push(currVal);
      
      // Remove max element
      que.pop();

      // Update max element
      maxVal = que.top();
    }
  }

  return  (uint64_t)((((__uint128_t)1)<<64)*(k-1) / get<0>(maxVal));
}

/* ***************************************************
 * Uses bottom-k sampling t times on numbers given in a vector.
 * It assume that t is an odd integer.
 * ***************************************************/

template<typename T, typename S>
uint64_t bottom_k_median(vector<T>& hashes, uint32_t t, uint64_t k, uint64_t n, S data_stream) {
#ifdef DEBUG
  assert(t % 2 == 1);
#endif

  vector<priority_queue<tuple<uint64_t, uint64_t> > > ques(t);

  uint64_t i = 0;
  for (; i < k && i < n; ++i) {
    uint64_t currNum = *data_stream++;
    for (uint32_t j = 0; j < t; ++j) {
      ques[j].push({hashes[j](currNum), currNum});
    }
  }

  vector<tuple<uint64_t, uint64_t> > maxVals(t);
  for (uint32_t j = 0; j < t; ++j) {
    maxVals[j] = ques[j].top();
  }

  for (; i < n; ++i) {
    uint64_t currNum = *data_stream++;

    for (uint32_t j = 0; j < t; ++j) {
      // cerr << i << " " << j << endl;
      tuple<uint64_t, uint64_t> currVal = {hashes[j](currNum), currNum};

      if (currVal < maxVals[j]) {
        // Insert the new element
        ques[j].push(currVal);
        
        // Remove max element
        ques[j].pop();

        // Update max element
        maxVals[j] = ques[j].top();
      }
    }
  }

  vector<uint64_t> estimates(t);
  for (uint32_t j = 0; j < t; ++j) {
    estimates[j] = (uint64_t)((((__uint128_t)1)<<64)*(k-1) / get<0>(maxVals[j]));
  }

  sort(estimates.begin(), estimates.end());

  return estimates[t/2];
}

#endif // _ALGORITHMS_H_
