/* ***********************************************
 * Hash functions:
 * This file define several hash functions as
 * classes.
 *
 * If DEBUG is defined it asserts that initialization
 * is done properly.
 * ***********************************************/

#ifndef _HASHING_H_
#define _HASHING_H_

#include <vector>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <type_traits>

// #include <boost/multiprecision/cpp_int.hpp>

#ifdef DEBUG
#include <cassert>
#endif

#ifdef SEEDED_RANDOM
#include "randomgen.h"
#else
#include <random>
#endif

#include "blake3/blake3.h"
#include "MurmurHash3.cpp"

using namespace std;

// using uint256_t = boost::multiprecision::uint256_t;

/* ***********************************************
 * Multiply-shift hashing a la Dietzfelbinger
 * ***********************************************/
template<const bool sixtyfour=false>
class multishift {
#ifdef DEBUG
    bool hasInit;
#endif
    using uint_large_t = typename conditional<sixtyfour, __uint128_t, uint64_t>::type;
    using uint_t = typename conditional<sixtyfour, uint64_t, uint32_t>::type;

    uint_large_t m_a, m_b;

public:
    multishift();
    uint_t operator()(uint_t x);
    void init();
};

template<const bool sixtyfour>
multishift<sixtyfour>::multishift() {
#ifdef DEBUG
    hasInit=false;
#endif
}

template<>
void multishift<false>::init() {
#ifdef SEEDED_RANDOM
    m_a = getRandomUInt64();
    m_b = getRandomUInt64();
#else
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<uint64_t> dist;
    m_a = dist(rng);
    m_b = dist(rng);
#endif
#ifdef DEBUG
    hasInit=true;
#endif
}


template<>
void multishift<true>::init() {
#ifdef SEEDED_RANDOM
    m_a = getRandomUInt128();
    m_b = getRandomUInt128();
#else
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<__uint128_t> dist;
    m_a = dist(rng);
    m_b = dist(rng);
#endif
#ifdef DEBUG
    hasInit=true;
#endif
}

template<>
uint32_t multishift<false>::operator()(uint32_t x) {
#ifdef DEBUG
    assert(hasInit);
#endif
    return (m_a * (uint64_t)x + m_b) >> 32;
}

template<>
uint64_t multishift<true>::operator()(uint64_t x) {
#ifdef DEBUG
    assert(hasInit);
#endif
    return (m_a * (__uint128_t)x + m_b) >> 64;
}

using multishift_32 = multishift<false>;
using multishift_64 = multishift<true>;

class __uint256_type {
public:
    __uint128_t m_a;
    __uint128_t m_b;

    __uint256_type(__uint128_t b) : m_a(0), m_b(b) {};
    __uint256_type(__uint128_t a, __uint128_t b) : m_a(a), m_b(b) {};

    __uint256_type operator+(__uint128_t v);
    __uint256_type operator+(__uint256_type v);
    
    __uint256_type operator*(uint64_t v);

    __uint256_type operator>>(uint32_t v);

    __uint256_type operator&(__uint128_t v);
};

__uint256_type __uint256_type::operator+(__uint256_type v) {
    __uint128_t a = m_a >> 64;
    __uint128_t b = m_a & ((uint64_t) -1);
    __uint128_t c = m_b >> 64;
    __uint128_t d = m_b & ((uint64_t) -1);

    __uint128_t va = v.m_a >> 64;
    __uint128_t vb = v.m_a & ((uint64_t) -1);
    __uint128_t vc = v.m_b >> 64;
    __uint128_t vd = v.m_b & ((uint64_t) -1);

    __uint128_t low_b = d + vd;
    __uint128_t high_b = c + vc + (low_b >> 64);

    __uint128_t low_a = b + vb + (high_b >> 64);
    __uint128_t high_a = a + va + (low_a >> 64);

    __uint128_t real_b = (high_b << 64) + (low_b & ((uint64_t) -1));
    __uint128_t real_a = (high_a << 64) + (low_a & ((uint64_t) -1));

    return __uint256_type(real_a, real_b);
}

__uint256_type __uint256_type::operator+(__uint128_t v) {
    return *this + __uint256_type(v);
}

__uint256_type __uint256_type::operator*(uint64_t v) {
    __uint128_t a = m_a >> 64;
    __uint128_t b = m_a & ((uint64_t) -1);
    __uint128_t c = m_b >> 64;
    __uint128_t d = m_b & ((uint64_t) -1);

    __uint128_t low_b  = d*(__uint128_t)v;
    __uint128_t high_b = c*(__uint128_t)v + (low_b >> 64);

    __uint128_t low_a  = b*(__uint128_t)v + (high_b >> 64);
    __uint128_t high_a = a*(__uint128_t)v + (low_a >> 64);

    __uint128_t real_b = (high_b << 64) + (low_b & ((uint64_t) -1));
    __uint128_t real_a = (high_a << 64) + (low_a & ((uint64_t) -1));
    
    return __uint256_type(real_a, real_b);
}

__uint256_type __uint256_type::operator>>(uint32_t v) {
    __uint128_t real_a = m_a >> v;
    __uint128_t real_b = ((m_a & ((1 << v) - 1)) << (128 - v)) + (m_b >> v);

    return __uint256_type(real_a, real_b);
}

__uint256_type __uint256_type::operator&(__uint128_t v) {
    return __uint256_type(m_b & v);
}

template<typename FROM, typename TO>
inline TO convert_int_type(FROM val);

template<>
inline uint64_t convert_int_type(__uint256_type a) {
    return (uint64_t) a.m_b;
}

template<typename FROM, typename TO>
inline TO convert_int_type(FROM val) {
    return (TO)val;
}

/* ***************************************************
 * Poly hashing with large degree a la Carter & Wegman
 * ***************************************************/

template<const uint32_t degree>
class polyhash_32 {
#ifdef DEBUG
    bool hasInit;
#endif

    uint64_t m_coef[degree];
    // Large mersenne prime (2^89 - 1) or (2^61 - 1)
    constexpr static uint32_t mersenne_size = 61;
    constexpr static uint64_t m_p = ((uint64_t) 1 << mersenne_size) - 1;
    //sixtyfour ? (((__uint128_t) 1 << 89) - 1) : (((uint64_t) 1 << 61) - 1);

public:
    polyhash_32();
    void init();
    uint32_t operator()(uint32_t x);
};


template<const uint32_t degree>
polyhash_32<degree>::polyhash_32() {
#ifdef DEBUG
    hasInit=false;
#endif
}

template<const uint32_t degree>
void polyhash_32<degree>::init() {
#ifndef SEEDED_RANDOM
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<uint64_t> dist;
#endif

    for (uint32_t i = 0; i < degree; ++i) {
		do {
#ifdef SEEDED_RANDOM
            m_coef[i] = getRandomUInt64() >> 3;
#else
			m_coef[i] = (dist(rng)) >> 3;
#endif
#ifdef DEBUG
			assert(m_coef[i] <= m_p); //Since poly_p = 2^61-1.
#endif
		} while(m_coef[i] >= m_p);
    }
#ifdef DEBUG
    hasInit=true;
#endif
}

template<>
uint32_t polyhash_32<2>::operator()(uint32_t x) {
#ifdef DEBUG
    assert(hasInit);
#endif
    __int128 h = m_coef[1] * x + m_coef[0];
    h = (h & m_p) + (h >> 61);
    h = (h & m_p) + (h >> 61);
    return (uint32_t)h;
}

template<>
uint32_t polyhash_32<3>::operator()(uint32_t x) {
#ifdef DEBUG
    assert(hasInit);
#endif
    __int128 h = m_coef[2] * x + m_coef[1];
    h = (h & m_p) + (h >> 61);
    h = h*x + m_coef[0];
    h = (h & m_p) + (h >> 61);
    h = (h & m_p) + (h >> 61);
    return (uint32_t)h;
}


template<>
uint32_t polyhash_32<4>::operator()(uint32_t x) {
#ifdef DEBUG
    assert(hasInit);
#endif
    __int128 h = m_coef[3] * x + m_coef[2];
    h = (h & m_p) + (h >> 61);
    h = h*x + m_coef[1];
    h = (h & m_p) + (h >> 61);
    h = h*x + m_coef[0];
    h = (h & m_p) + (h >> 61);
    h = (h & m_p) + (h >> 61);
    return (uint32_t)h;
}

template<const uint32_t degree>
uint32_t polyhash_32<degree>::operator()(uint32_t x) {
#ifdef DEBUG
    assert(hasInit);
#endif
    __uint128_t h(m_coef[degree-1]);
    for (int32_t i = degree - 2; i >= 0; --i) {
		h = h * x + m_coef[i];
		h = (h & m_p) + (h >> mersenne_size);
    }
	h = (h & m_p) + (h >> mersenne_size);
    return (uint32_t)h;
}


template<const uint32_t degree>
class polyhash_64 {
#ifdef DEBUG
    bool hasInit;
#endif

    __uint128_t m_coef[degree];
    // Large mersenne prime (2^89 - 1) or (2^61 - 1)
    constexpr static uint32_t mersenne_size = 89;
    constexpr static __uint128_t m_p = ((__uint128_t) 1 << mersenne_size) - 1;
    //sixtyfour ? (((__uint128_t) 1 << 89) - 1) : (((uint64_t) 1 << 61) - 1);

public:
    polyhash_64();
    void init();
    uint64_t operator()(uint64_t x);
};


template<const uint32_t degree>
polyhash_64<degree>::polyhash_64() {
#ifdef DEBUG
    hasInit=false;
#endif
}

template<const uint32_t degree>
void polyhash_64<degree>::init() {
#ifndef SEEDED_RANDOM
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<uint64_t> dist;
#endif

    for (uint32_t i = 0; i < degree; ++i) {
		do {
#ifdef SEEDED_RANDOM
            m_coef[i] = getRandomUInt128() >> (128 - mersenne_size);
#else
			m_coef[i] = (dist(rng)) >> 3;
#endif
#ifdef DEBUG
			assert(m_coef[i] <= m_p); //Since poly_p = 2^61-1.
#endif
		} while(m_coef[i] >= m_p);
    }
#ifdef DEBUG
    hasInit=true;
#endif
}

__uint128_t fast_large_mult_mod(__uint128_t a, __uint128_t b, uint64_t x) {
    __uint128_t fst_a = a >> 64;
    __uint128_t scd_a = (uint64_t)a;

    __uint128_t fst_b = b >> 64;    
    __uint128_t scd_b = (uint64_t)b;

    __uint128_t c = scd_a * (__uint128_t)x + scd_b;
    __uint128_t d = fst_a * (__uint128_t)x + fst_b + (c >> 64);

    return ((d & (((__uint128_t) 1 << 25) - 1)) << 64) + (__uint128_t)(c & ((uint64_t) - 1)) + (d >> 25);
}

template<const uint32_t degree>
uint64_t polyhash_64<degree>::operator()(uint64_t x) {
#ifdef DEBUG
    assert(hasInit);
#endif
    __uint128_t h(m_coef[degree-1]);
    for (int32_t i = degree - 2; i >= 0; --i) {
		h = fast_large_mult_mod(h, m_coef[i], x);
    }
	h = (h & m_p) + (h >> mersenne_size);
    return (uint64_t)h;
}

/* ***************************************************
 * Mixed Tabulation a la Dahlgaard et al.
 * ***************************************************/

class mixedtab {
#ifdef DEBUG
    bool hasInit;
#endif
    // Use 4 characters + 4 derived characters.
    // TODO: Parameterize
    uint64_t mt_T1[256][4];
    uint32_t mt_T2[256][4];

public:
    mixedtab();
    void init();
    uint32_t operator()(uint32_t x);
};

mixedtab::mixedtab() {
#ifdef DEBUG
    hasInit = false;
#endif
}

void mixedtab::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_32<100> h;
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 256; ++j) {
            mt_T1[j][i] = h(x++);
            mt_T1[j][i] <<= 32;
            mt_T1[j][i] += h(x++);
            mt_T2[j][i] = h(x++);
        }
    }
#ifdef DEBUG
    hasInit = true;
#endif
}

uint32_t mixedtab::operator()(uint32_t x) {
#ifdef DEBUG
    assert(hasInit);
#endif
    uint64_t h=0; // Final hash value
    for (uint32_t i = 0; i < 4; ++i, x >>= 8)
        h ^= mt_T1[(uint8_t)x][i];
    uint32_t drv=h >> 32;
    for (uint32_t i = 0; i < 4; ++i, drv >>= 8)
        h ^= mt_T2[(uint8_t)drv][i];
    return (uint32_t)h;
}

/* ***************************************************
 * Simple Tabulation
 * ***************************************************/

template <const uint32_t C, const bool sixtyfour=false>
class simpletab {
    using uint_t = typename conditional<sixtyfour, uint64_t, uint32_t>::type;

    constexpr static uint32_t max_bits = (sixtyfour ? 64 : 32);

    constexpr static uint_t nr_bits = max_bits/C;
    constexpr static uint_t bit_mask = (1 << nr_bits) - 1;
    constexpr static uint32_t nr_values = 1 << nr_bits;

    uint_t m_T[nr_values][C];

public:
    simpletab();
    void init();
    uint_t operator()(uint_t x);
};

template <const uint32_t C, const bool sixtyfour>
simpletab<C, sixtyfour>::simpletab() {
}

template <const uint32_t C, const bool sixtyfour>
void simpletab<C, sixtyfour>::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_64<100> h;
    h.init();

	uint_t x = 0;
    for (uint32_t i = 0; i < C; ++i) {
        for (uint32_t j = 0; j < nr_values; ++j) {
            m_T[j][i] = (uint_t)h(x++);
        }
    }
}

template <const uint32_t C, const bool sixtyfour>
typename simpletab<C, sixtyfour>::uint_t simpletab<C, sixtyfour>::operator()(uint_t x) {
    uint_t h = 0; // Final hash value
    for (uint32_t i = 0; i < C; ++i, x >>= nr_bits) {
        h ^= m_T[bit_mask & x][i];
    }
    return h;
}

/* ***************************************************
 * Twisted Tabulation
 * ***************************************************/

template <const uint32_t C, const bool sixtyfour=false>
class twisttab {
    using uint_t      = typename conditional<sixtyfour, uint64_t,    uint32_t>::type;
    using uintlarge_t = typename conditional<sixtyfour, __uint128_t, uint64_t>::type;

    constexpr static uint32_t max_bits = (sixtyfour ? 64 : 32);

    constexpr static uint_t nr_bits = max_bits/C;
    constexpr static uint_t bit_mask = (1 << nr_bits) - 1;
    constexpr static uint_t nr_values = 1 << nr_bits;

    uintlarge_t mt_T1[nr_values][C];

public:
    twisttab();
    void init();
    uint_t operator()(uint_t x);
};

template <const uint32_t C, const bool sixtyfour>
twisttab<C, sixtyfour>::twisttab() {
}

template <const uint32_t C, const bool sixtyfour>
void twisttab<C, sixtyfour>::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_64<100> h;
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < C; ++i) {
        for (uint32_t j = 0; j < nr_values; ++j) {
            mt_T1[j][i] = (uint_t)h(x++);
            mt_T1[j][i] <<= max_bits;
            mt_T1[j][i] += (uint_t)h(x++);
        }
    }
}

template <const uint32_t C, const bool sixtyfour>
typename twisttab<C, sixtyfour>::uint_t twisttab<C, sixtyfour>::operator()(uint_t x) {
    uintlarge_t h = 0; // Final hash value
    for (uint32_t i = 0; i < C-1; ++i, x >>= nr_bits) {
        h ^= mt_T1[bit_mask & x][i];
    }
    uint_t drv = h >> max_bits;
    h ^= mt_T1[bit_mask & (x^drv)][C-1];
    return (uint_t)h;
}


/* ***************************************************
 * Permuted Tabulation (General)
 * ***************************************************/

template <const uint32_t C, const uint32_t D, const bool sixtyfour=false>
class permutedtab {
    using uint_t      = typename conditional<sixtyfour, uint64_t, uint32_t>::type;
    using uintlarge_t = typename conditional<sixtyfour, __uint128_t, uint64_t>::type;

    constexpr static uint32_t max_bits = (sixtyfour ? 64 : 32);

    constexpr static uint_t nr_bits = max_bits/C;
    constexpr static uint_t bit_mask = (1 << nr_bits) - 1;
    constexpr static uint_t bit_mask_lowBits = (uint_t)((1L << ((C-D)*nr_bits)) - 1);
    constexpr static uint_t nr_values = 1 << nr_bits;
    uint_t m_T[nr_values][C];
    uint_t m_P[nr_values][D];

public:
    permutedtab();
    void init();
    uint_t operator()(uint_t x);
};

template <const uint32_t C, const uint32_t D, const bool sixtyfour>
permutedtab<C, D, sixtyfour>::permutedtab() {
    #ifdef DEBUG
        assert(C >= D);
    #endif
}

template <const uint32_t C, const uint32_t D, const bool sixtyfour>
void permutedtab<C, D, sixtyfour>::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_64<100> h;
    h.init();

	uint_t x = 0;
    for (uint32_t i = 0; i < C; ++i) {
        for (uint32_t j = 0; j < nr_values; ++j) {
            m_T[j][i] = (uint_t)h(x++);
        }
    }

    for (uint32_t i = 0; i < D; ++i) {
        // Initialize table
        for (uint_t j = 0; j < nr_values; ++j) {
          m_P[j][i] = j << ((C-1-i)*nr_bits);
        }

        // Shuffle the table with Fisher-Yates shuffle
        for (uintlarge_t j = nr_values - 1; j > 0; --j) {
            uintlarge_t val = (uint_t)h(x++);
            swap(m_P[j][i], m_P[(j*val)>>max_bits][i]);
        }
    }
}

template <const uint32_t C, const uint32_t D, const bool sixtyfour>
typename permutedtab<C, D, sixtyfour>::uint_t permutedtab<C, D, sixtyfour>::operator()(uint_t x) {
    uint_t h=0; // Intermediate hash value
    for (uint32_t i = 0; i < C; ++i, x >>= nr_bits) {
        h ^= m_T[bit_mask & x][i];
    }
    
    //This was the previous line of code, but I think it only D permutations and sets everything else to 0
    //so it wouldn't work for tabulation-1permutation
    //uint_t res = 0; // Final hash value

    //my suggestion is
    uint_t res = h & bit_mask_lowBits;
    
    for (uint32_t i = 0; i < D; ++i, h >>= nr_bits) {
        res |= m_P[bit_mask & h][i];
    }

    return res;
}

/* ***************************************************
 * MurmurHash3
 * ***************************************************/

class murmurHash3 {
  uint32_t murmurSeed;

public:
  void init() {
    murmurSeed = (uint32_t)getRandomUInt64();
  }
  uint64_t operator()(uint64_t x) {
    __uint128_t res;
    MurmurHash3_x64_128(&x, 8, murmurSeed, &res);
    return (uint64_t)(res & (-1L));
  }
};


/* ***************************************************
 * Blake3
 * ***************************************************/

class blake3 {
  __uint128_t blakeSeed;
  blake3_hasher hasher;
  uint8_t output[BLAKE3_OUT_LEN];

public:
  void init() {
    blakeSeed = ((__uint128_t)getRandomUInt64()) << 64;
    blake3_hasher_init(&hasher);    
  }
  uint64_t operator()(uint64_t x) {
    uint64_t res=0;
    __uint128_t salted = blakeSeed + x;
    
    blake3_hasher_update(&hasher, &blakeSeed, 16);    
    blake3_hasher_finalize(&hasher, output, BLAKE3_OUT_LEN);
    for (size_t i = 0; i < BLAKE3_OUT_LEN; i++) {
      res = (res<<8) + output[i];
    }  

    return res;
  }
};


#endif // _HASHING_H_
