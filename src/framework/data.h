#ifndef _DATA_H_
#define _DATA_H_

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
 * An iterator returning the numbers start, start + a, ...
 * ***************************************************/

class StructuredData
{
    uint64_t curr;
    uint64_t a;

public:
    using iterator_category = std::output_iterator_tag;
    using value_type = uint64_t;

    StructuredData operator++(int) {
        StructuredData temp(curr, a);
        curr += a;
        return temp;
    }

    uint64_t operator*() {
        return curr;
    }

    StructuredData begin() {
        return StructuredData(curr, a);
    }

    StructuredData(uint64_t start, uint64_t a) : curr(start), a(a) {}
};

// template<typename T>
// class RandomData
// {
//     T rand_gen;
//     uint64_t i;
//     uint64_t curr;

// public:
//     using iterator_category = std::output_iterator_tag;
//     using value_type = void; // crap
//     using difference_type = void;
//     using pointer = void;
//     using reference = void;

//     RandomData operator++(int) {
//         auto temp(*this);
//         curr = rand_gen(i++);
//         return temp;
//     }

//     uint64_t operator*() {
//         return curr;
//     }

// };

#endif // _DATA_H_
