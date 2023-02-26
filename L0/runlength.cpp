//
// Created by mhq on 27/02/23.
//
#include "runlength.h"
#include <cassert>

struct Code {
    char byte;
    uint8_t count;
};

rle::EncodingStats rle::encode(std::istream& is, std::ostream& os) {
    Code current;
    rle::EncodingStats stats{};
    while (is.get(current.byte)) {
        current.count = 1;
        while (is.peek() == current.byte) {
            current.count++;
            is.get();
        }
        os.write(reinterpret_cast<char*>(&current), sizeof(current));
        stats.input_size += current.count;
        stats.output_size += 2;
    }
    return stats;
}

void rle::decode(std::istream& is, std::ostream& os) {
    Code current;
    while(is.read(reinterpret_cast<char*>(&current), sizeof(current))) {
        assert(is.gcount() == sizeof(current));
        for (int i = 0; i < current.count; ++i) {
            os.put(current.byte);
        }
    }
}