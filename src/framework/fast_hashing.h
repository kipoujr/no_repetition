/* ***********************************************
 * Hash functions:
 * This file define several hash functions as
 * classes.
 *
 * If DEBUG is defined it asserts that initialization
 * is done properly.
 * ***********************************************/

#ifndef _FAST_HASHING_H_
#define _FAST_HASHING_H_

#include <cstdio>
#include <vector>
#include <cstdint>
#include <type_traits>

// #include <boost/multiprecision/cpp_int.hpp>

#include "hashing.h"

#ifdef DEBUG
#include <cassert>
#endif

#include "randomgen.h"

using namespace std;

// using uint256_t = boost::multiprecision::uint256_t;

/* ***************************************************
 * 2-wise independent hashing using polyhash.
 * ***************************************************/

class polytwo_32 {
    uint64_t m_a;
    uint64_t m_b;

    constexpr static uint64_t m_p = ((uint64_t) 1 << 61) - 1;
public:
    void init();
    uint32_t operator()(uint32_t x);
};

void polytwo_32::init() {
    m_a = getRandomUInt64() >> 3;
    m_b = getRandomUInt64() >> 3;
}

uint32_t polytwo_32::operator()(uint32_t x) {
    __uint128_t h = (__uint128_t)m_a * x + m_b;
    // return (uint32_t)((h + ((h + 1 + ((h + 1) >> 61)) >> 61)) & m_p);

    h = (h & m_p) + (h >> 61);
    h = (h & m_p) + (h >> 61);
    return (uint32_t)h;
}


class polytwo_64 {
    __uint128_t m_a;
    __uint128_t m_b;

    constexpr static __uint128_t m_p = ((__uint128_t) 1 << 89) - 1;
public:
    void init();
    uint64_t operator()(uint64_t x);
};

void polytwo_64::init() {
    m_a = getRandomUInt128() >> 39;
    m_b = getRandomUInt128() >> 39;
}

uint64_t polytwo_64::operator()(uint64_t x) {
    __uint128_t h = fast_large_mult_mod(m_a, m_b, x);
    h = (h & m_p) + (h >> 89);

    return (uint64_t)h;
}

/* ***************************************************
 * Simple Tabulation
 * ***************************************************/

class simpletab2_32 {
    uint32_t m_T[1 << 16][2];

public:
    void init();
    uint32_t operator()(uint32_t x);
};

void simpletab2_32::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_32<100> h;
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 2; ++i) {
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_T[j][i] = h(x++);
        }
    }
}

uint32_t simpletab2_32::operator()(uint32_t x) {
    return m_T[(uint16_t)x][0] ^ m_T[(uint16_t)(x >> 16)][1];
}

class simpletab4_32 {
    uint32_t m_T[256][4];

public:
    void init();
    uint32_t operator()(uint32_t x);
};

void simpletab4_32::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_32<100> h;      
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 1 << 8; ++j) {
            m_T[j][i] = h(x++);
        }
    }
}

uint32_t simpletab4_32::operator()(uint32_t x) {
    return m_T[(uint8_t)x][0] ^ m_T[(uint8_t)(x >> 8)][1] ^ m_T[(uint8_t)(x >> 16)][2] ^ m_T[(uint8_t)(x >> 24)][3];
}


class simpletab4_64 {
    uint64_t m_T[1 << 16][4];

public:
    void init();
    uint64_t operator()(uint64_t x);
};

void simpletab4_64::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_64<100> h;  
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_T[j][i] = h(x++);
        }
    }
}

uint64_t simpletab4_64::operator()(uint64_t x) {
    return m_T[(uint16_t)x][0] ^ m_T[(uint16_t)(x >> 16)][1] ^ m_T[(uint16_t)(x >> 32)][2] ^ m_T[(uint16_t)(x >> 48)][3];
}

class simpletab8_64 {
    uint64_t m_T[256][8];

public:
    void init();
    uint64_t operator()(uint64_t x);
};

void simpletab8_64::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_64<100> h;    
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 8; ++i) {
        for (uint32_t j = 0; j < 1 << 8; ++j) {
            m_T[j][i] = h(x++);
        }
    }
}

uint64_t simpletab8_64::operator()(uint64_t x) {
    return m_T[(uint8_t)x][0] ^ m_T[(uint8_t)(x >> 8)][1] ^ m_T[(uint8_t)(x >> 16)][2] ^ m_T[(uint8_t)(x >> 24)][3]
        ^ m_T[(uint8_t)(x >> 32)][4] ^ m_T[(uint8_t)(x >> 40)][5] ^ m_T[(uint8_t)(x >> 48)][6] ^ m_T[(uint8_t)(x >> 56)][7];
}


/* ***************************************************
 * Twisted Tabulation
 * ***************************************************/

class twisttab2_32 {
    uint64_t m_T[1 << 16][2];

public:
    void init();
    uint32_t operator()(uint32_t x);
};

void twisttab2_32::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_32<100> h;      
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 2; ++i) {
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_T[j][i] = ((uint64_t)h(x++) << 32) + h(x++);
        }
    }
}

uint32_t twisttab2_32::operator()(uint32_t x) {
    uint64_t tmp = m_T[(uint16_t)x][0];
    return ((uint32_t) tmp) ^ ((uint32_t)m_T[((uint16_t)(x >> 16)) ^ ((uint16_t)(tmp >> 32))][1]);
}

class twisttab4_32 {
    uint64_t m_T[1 << 8][4];

public:
    void init();
    uint32_t operator()(uint32_t x);
};

void twisttab4_32::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_32<100> h;        
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 1 << 8; ++j) {
            m_T[j][i] = ((uint64_t)h(x++) << 32) + h(x++);
        }
    }
}

uint32_t twisttab4_32::operator()(uint32_t x) {
    uint64_t tmp = m_T[(uint8_t)x][0] ^ m_T[(uint8_t)(x >> 8)][1] ^ m_T[(uint8_t)(x >> 16)][2];
    return ((uint32_t) tmp) ^ ((uint32_t)m_T[((uint8_t)(x >> 24)) ^ ((uint8_t)(tmp >> 32))][3]);
}

class twisttab4_64 {
    __uint128_t m_T[1 << 16][4];

public:
    void init();
    uint64_t operator()(uint64_t x);
};

void twisttab4_64::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_64<100> h;            
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_T[j][i] = ((uint64_t)h(x++) << 32) + h(x++);
        }
    }
}

uint64_t twisttab4_64::operator()(uint64_t x) {
    __uint128_t tmp = m_T[(uint16_t)x][0] ^ m_T[(uint16_t)(x >> 16)][1] ^ m_T[(uint16_t)(x >> 32)][2];
    return ((uint64_t) tmp) ^ ((uint64_t)m_T[((uint16_t)(x >> 48)) ^ ((uint16_t)(tmp >> 64))][3]);
}

class twisttab8_64 {
    __uint128_t m_T[1 << 8][8];

public:
    void init();
    uint64_t operator()(uint64_t x);
};

void twisttab8_64::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_64<100> h;          
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 8; ++i) {
        for (uint32_t j = 0; j < 1 << 8; ++j) {
            m_T[j][i] = ((uint64_t)h(x++) << 32) + h(x++);
        }
    }
}

uint64_t twisttab8_64::operator()(uint64_t x) {
    __uint128_t tmp = m_T[(uint8_t)x][0] ^ m_T[(uint8_t)(x >> 8)][1] ^ m_T[(uint8_t)(x >> 16)][2]
        ^ m_T[(uint8_t)(x >> 24)][3] ^ m_T[(uint8_t)(x >> 32)][4] ^ m_T[(uint8_t)(x >> 40)][5] ^ m_T[(uint8_t)(x >> 48)][6];
    return ((uint64_t) tmp) ^ ((uint64_t)m_T[((uint8_t)(x >> 56)) ^ ((uint8_t)(tmp >> 64))][7]);
}


/* ***************************************************
 * Tabulation-Permutation
 * ***************************************************/

class permtab2_32 {
    uint32_t m_T[1 << 16][2];
    uint32_t m_P[1 << 16][2];

public:
    void init();
    uint32_t operator()(uint32_t x);
};

void permtab2_32::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_32<100> h;            
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 2; ++i) {
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_T[j][i] = h(x++);
        }
    }

    
    for (uint32_t i = 0; i < 2; ++i) {
        // Initialize table
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_P[j][i] = j << (i*16);
        }

        // Shuffle the table with Fisher-Yates shuffle
        for (uint64_t j = (1 << 16) - 1; j > 0; --j) {
            uint64_t val = h(x++);
            swap(m_P[j][i], m_P[(j*val)>>32][i]);
        }
    }
}

uint32_t permtab2_32::operator()(uint32_t x) {
    uint32_t tmp = m_T[(uint16_t)x][0] ^ m_T[(uint16_t)(x >> 16)][1];
    return m_P[(uint16_t)tmp][0] | m_P[(uint16_t)(tmp >> 16)][1];
}

class permtab4_32 {
    uint32_t m_T[1 << 8][4];
    uint32_t m_P[1 << 8][4];

public:
    void init();
    uint32_t operator()(uint32_t x);
};


void permtab4_32::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_32<100> h;              
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 1 << 8; ++j) {
            m_T[j][i] = h(x++);
        }
    }

    for (uint32_t i = 0; i < 4; ++i) {
        // Initialize table
        for (uint32_t j = 0; j < 1 << 8; ++j) {
            m_P[j][i] = j << (i*8);
        }

        // Shuffle the table with Fisher-Yates shuffle
        for (uint64_t j = (1 << 8) - 1; j > 0; --j) {
            uint64_t val = h(x++);
            swap(m_P[j][i], m_P[(j*val)>>32][i]);
        }
    }
}

uint32_t permtab4_32::operator()(uint32_t x) {
    uint32_t tmp = m_T[(uint8_t)x][0] ^ m_T[(uint8_t)(x >> 8)][1] ^ m_T[(uint8_t)(x >> 16)][2] ^ m_T[(uint8_t)(x >> 24)][3];
    return m_P[(uint8_t)tmp][0] | m_P[(uint8_t)(tmp >> 8)][1] | m_P[(uint8_t)(tmp >> 16)][2] | m_P[(uint8_t)(tmp >> 24)][3];
}


class permtab4_64 {
    uint64_t m_T[1 << 16][4];
    uint64_t m_P[1 << 16][4];

public:
    void init();
    uint64_t operator()(uint64_t x);
};

void permtab4_64::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_64<100> h;                
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_T[j][i] = h(x++);
        }
    }

    
    for (uint32_t i = 0; i < 4; ++i) {
        // Initialize table
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_P[j][i] = j << (i*16);
        }

        // Shuffle the table with Fisher-Yates shuffle
        for (uint64_t j = (1 << 16) - 1; j > 0; --j) {
            uint64_t val = h(x++);
            swap(m_P[j][i], m_P[(__uint128_t)(j*val)>>64][i]);
        }
    }
}

uint64_t permtab4_64::operator()(uint64_t x) {
    uint64_t tmp = m_T[(uint16_t)x][0] ^ m_T[(uint16_t)(x >> 16)][1] ^ m_T[(uint16_t)(x >> 32)][2] ^ m_T[(uint16_t)(x >> 48)][3];
    return m_P[(uint16_t)tmp][0] | m_P[(uint16_t)(tmp >> 16)][1] | m_P[(uint16_t)(tmp >> 32)][2] | m_P[(uint16_t)(tmp >> 48)][3];
}

class permtab8_64 {
    uint32_t m_T[1 << 8][8];
    uint32_t m_P[1 << 8][8];

public:
    void init();
    uint64_t operator()(uint64_t x);
};


void permtab8_64::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_64<100> h;                    
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 8; ++i) {
        for (uint32_t j = 0; j < 1 << 8; ++j) {
            m_T[j][i] = h(x++);
        }
    }

    for (uint32_t i = 0; i < 8; ++i) {
        // Initialize table
        for (uint32_t j = 0; j < 1 << 8; ++j) {
            m_P[j][i] = j << (i*8);
        }

        // Shuffle the table with Fisher-Yates shuffle
        for (uint64_t j = (1 << 8) - 1; j > 0; --j) {
            uint64_t val = h(x++);
            swap(m_P[j][i], m_P[(__uint128_t)(j*val)>>64][i]);
        }
    }
}

uint64_t permtab8_64::operator()(uint64_t x) {
    uint64_t tmp = m_T[(uint8_t)x][0] ^ m_T[(uint8_t)(x >> 8)][1] ^ m_T[(uint8_t)(x >> 16)][2] ^ m_T[(uint8_t)(x >> 24)][3]
        ^ m_T[(uint8_t)(x >> 32)][4] ^ m_T[(uint8_t)(x >> 40)][5] ^ m_T[(uint8_t)(x >> 48)][6] ^ m_T[(uint8_t)(x >> 56)][7];
    return m_P[(uint8_t)tmp][0] | m_P[(uint8_t)(tmp >> 8)][1] | m_P[(uint8_t)(tmp >> 16)][2] | m_P[(uint8_t)(tmp >> 24)][3]
        | m_P[(uint8_t)(tmp >> 32)][4] | m_P[(uint8_t)(tmp >> 40)][5] | m_P[(uint8_t)(tmp >> 48)][6] | m_P[(uint8_t)(tmp >> 56)][7];
}


/* ***************************************************
 * Tabulation-1Permutation
 * ***************************************************/

class tab1perm2_32 {
    uint32_t m_T[1 << 16][2];
    uint32_t m_P[1 << 16];

public:
    void init();
    uint32_t operator()(uint32_t x);
};


void tab1perm2_32::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_32<100> h;                      
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 2; ++i) {
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_T[j][i] = h(x++);
        }
    }

    // Initialize table
    for (uint32_t j = 0; j < 1 << 16; ++j) {
        m_P[j] = j;
    }

    // Shuffle the table with Fisher-Yates shuffle
    for (uint64_t j = (1 << 16) - 1; j > 0; --j) {
        uint64_t val = h(x++);
        swap(m_P[j], m_P[(j*val)>>32]);
        m_P[j] = (m_P[j] ^ j) << 16;
    }
}

uint32_t tab1perm2_32::operator()(uint32_t x) {
    uint32_t tmp = m_T[(uint16_t)x][0] ^ m_T[(uint16_t)(x >> 16)][1];
    return tmp ^ m_P[(uint8_t)(tmp >> 16)];
}

class tab1perm4_32 {
    uint32_t m_T[1 << 8][4];
    uint32_t m_P[1 << 8];

public:
    void init();
    uint32_t operator()(uint32_t x);
};


void tab1perm4_32::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_32<100> h;                        
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 1 << 8; ++j) {
            m_T[j][i] = h(x++);
        }
    }

    // Initialize table
    for (uint32_t j = 0; j < 1 << 8; ++j) {
        m_P[j] = j;
    }

    // Shuffle the table with Fisher-Yates shuffle
    for (uint64_t j = (1 << 8) - 1; j > 0; --j) {
        uint64_t val = h(x++);
        swap(m_P[j], m_P[(j*val)>>32]);
        m_P[j] = (m_P[j] ^ j) << 24;
    }
}

uint32_t tab1perm4_32::operator()(uint32_t x) {
    uint32_t tmp = m_T[(uint8_t)x][0] ^ m_T[(uint8_t)(x >> 8)][1] ^ m_T[(uint8_t)(x >> 16)][2] ^ m_T[(uint8_t)(x >> 24)][3];
    return tmp ^ m_P[(uint8_t)(tmp >> 24)];
}


class tab1perm4_64 {
    uint64_t m_T[1 << 16][4];
    uint64_t m_P[1 << 16];

public:
    void init();
    uint64_t operator()(uint64_t x);
};


void tab1perm4_64::init() {
  //printf("Initializing tab1perm4_64...\n");
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_64<100> h;
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_T[j][i] = h(x++);
        }
    }

    // Initialize table
    for (uint32_t j = 0; j < 1 << 16; ++j) {
        m_P[j] = j;
    }

    // Shuffle the table with Fisher-Yates shuffle
    for (uint64_t j = (1 << 16) - 1; j > 0; --j) {
        uint64_t val = h(x++);
        swap(m_P[j], m_P[(__uint128_t)(j*val)>>64]);
        m_P[j] = (m_P[j] ^ j) << 48;
    }
}

uint64_t tab1perm4_64::operator()(uint64_t x) {
    uint64_t tmp = m_T[(uint16_t)x][0] ^ m_T[(uint16_t)(x >> 16)][1] ^ m_T[(uint16_t)(x >> 32)][2] ^ m_T[(uint16_t)(x >> 48)][3];
    return tmp ^ m_P[(uint8_t)(tmp >> 48)];
}

class tab1perm8_64 {
    uint64_t m_T[1 << 8][8];
    uint64_t m_P[1 << 8];

public:
    void init();
    uint64_t operator()(uint64_t x);
};

void tab1perm8_64::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_64<100> h;  
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 8; ++i) {
        for (uint32_t j = 0; j < 1 << 8; ++j) {
            m_T[j][i] = h(x++);
        }
    }

    // Initialize table
    for (uint32_t j = 0; j < 1 << 8; ++j) {
        m_P[j] = j;
    }

    // Shuffle the table with Fisher-Yates shuffle
    for (uint64_t j = (1 << 8) - 1; j > 0; --j) {
        uint64_t val = h(x++);
        swap(m_P[j], m_P[(__uint128_t)(j*val)>>64]);
        m_P[j] = (m_P[j] ^ j) << 56;
    }
}

uint64_t tab1perm8_64::operator()(uint64_t x) {
    uint64_t tmp = m_T[(uint8_t)x][0] ^ m_T[(uint8_t)(x >> 8)][1] ^ m_T[(uint8_t)(x >> 16)][2] ^ m_T[(uint8_t)(x >> 24)][3]
        ^ m_T[(uint8_t)(x >> 32)][4] ^ m_T[(uint8_t)(x >> 40)][5] ^ m_T[(uint8_t)(x >> 48)][6] ^ m_T[(uint8_t)(x >> 56)][7];
    return tmp ^ m_P[(uint8_t)(tmp >> 56)];
}


/* ***************************************************
 * Mixed Tabulation a la Dahlgaard et al.
 * ***************************************************/

class mixedtab4_32 {
    // Use 4 characters + 4 derived characters.
    uint64_t mt_T1[256][4];
    uint32_t mt_T2[256][4];

public:
    void init();
    uint32_t operator()(uint32_t x);
};

void mixedtab4_32::init() {
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
}

uint32_t mixedtab4_32::operator()(uint32_t x) {
    uint64_t h=0; // Final hash value
    for (uint32_t i = 0; i < 4; ++i, x >>= 8)
        h ^= mt_T1[(uint8_t)x][i];
    uint32_t drv=h >> 32;
    for (uint32_t i = 0; i < 4; ++i, drv >>= 8)
        h ^= mt_T2[(uint8_t)drv][i];
    return (uint32_t)h;
}

class mixedtab8_64 {
    // Use 8 characters + 8 derived characters.
    __uint128_t mt_T1[256][8];
    uint64_t mt_T2[256][8];

public:
    void init();
    uint64_t operator()(uint64_t x);
};

void mixedtab8_64::init() {
    polyhash_64<100> h;  
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 8; ++i) {
        for (uint32_t j = 0; j < 256; ++j) {
            mt_T1[j][i] = h(x++);
            mt_T1[j][i] <<= 64;
            mt_T1[j][i] += h(x++);
            mt_T2[j][i] = h(x++);
        }
    }
}

uint64_t mixedtab8_64::operator()(uint64_t x) {
    __uint128_t h=0; // Final hash value
    for (uint32_t i = 0; i < 8; ++i, x >>= 8)
        h ^= mt_T1[(uint8_t)x][i];
    uint64_t drv=h >> 64;
    for (uint32_t i = 0; i < 8; ++i, drv >>= 8)
        h ^= mt_T2[(uint8_t)drv][i];
    return (uint64_t)h;
}


/* ***************************************************
 * Double Tabulation
 * ***************************************************/

class doubletab_32 {
    __uint128_t m_T[3][1 << 16][2];
    uint32_t m_T2[20][1 << 16];

public:
    void init();
    uint32_t operator()(uint32_t x);
};

void doubletab_32::init() {
    // Use a degree-100 polynomial to fill out the entries.
    polyhash_32<100> h;    
    h.init();

	uint32_t x = 0;
    for (uint32_t i = 0; i < 2; ++i) {
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_T[0][j][i] = ((__uint128_t)h(x++) << 96) + ((__uint128_t)h(x++) << 64) + ((__uint128_t)h(x++) << 32) + h(x++);
            m_T[1][j][i] = ((__uint128_t)h(x++) << 96) + ((__uint128_t)h(x++) << 64) + ((__uint128_t)h(x++) << 32) + h(x++);
            m_T[2][j][i] = ((__uint128_t)h(x++) << 96) + ((__uint128_t)h(x++) << 64) + ((__uint128_t)h(x++) << 32) + h(x++);
        }
    }

    for (uint32_t i = 0; i < 20; ++i) {
        for (uint32_t j = 0; j < 1 << 16; ++j) {
            m_T2[i][j] = h(x++);
        }
    }
}

uint32_t doubletab_32::operator()(uint32_t x) {
    __uint128_t derived[3];
    derived[0] = m_T[0][(uint16_t)x][0] ^ m_T[0][(uint16_t)(x >> 16)][1];
    derived[1] = m_T[1][(uint16_t)x][0] ^ m_T[1][(uint16_t)(x >> 16)][1];
    derived[2] = m_T[2][(uint16_t)x][0] ^ m_T[2][(uint16_t)(x >> 16)][1];

    uint32_t h = 0;
    for (uint32_t i = 0; i < 8; ++i, derived[0] >>= 16) {
        h ^= m_T2[i][(uint16_t)derived[0]];
    }
    
    for (uint32_t i = 0; i < 8; ++i, derived[1] >>= 16) {
        h ^= m_T2[8 + i][(uint16_t)derived[1]];
    }

    return h ^ m_T2[18][(uint16_t)derived[2]] ^ m_T2[19][(uint16_t)(derived[2] >> 16)];
}


#endif // _FAST_HASHING_H_
