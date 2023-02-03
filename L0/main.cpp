//
// Created by mhq on 02/02/23.
//
#include <cstddef>
#include <ostream>
#include <random>
#include <fstream>
#include "sstream"
#include <iostream>
#include <map>
#include <algorithm>
#include <bitset>

using freq_t = size_t;
using FrequencyMap = std::map<char, freq_t>;

void generate_file(const std::string& alphabet,
                   size_t characters_count,
                   std::ostream& os,
                   std::mt19937& generator) {
    std::uniform_int_distribution<size_t> distribution(0, alphabet.size() - 1);
    for (int i = 0; i < characters_count; ++i) {
        os << alphabet[distribution(generator)];
    }
}

FrequencyMap count_characters(std::istream& is) {
    FrequencyMap res{};
    char c;
    while (is >> c) {
        ++res[c];
    }
    return res;
}

std::vector<char> alphabet_sorted(const FrequencyMap& frequencies) {
    std::vector<char> res{};
    res.reserve(frequencies.size());
    for (auto [c, _] : frequencies) {
        res.push_back(c);
    }
    std::sort(res.begin(), res.end(), [&](auto left, auto right) {
        return frequencies.at(left) > frequencies.at(right);
    });
    return res;
}

constexpr size_t L = 24; // max code length for alphabet of 25 chars

struct Code {
    unsigned int value: L;
    unsigned char length;
    void set(size_t position, bool val = true) {
        if (val) {
            value |= 1u << position;
        } else {
            value &= ~(1u << position);
        }
    }
};

static_assert(sizeof(Code) == 4);

std::vector<Code> Huffman(std::vector<freq_t> probabilities) {
    size_t n = probabilities.size();
    std::vector<Code> codes{n};
    auto Up = [&](size_t n, freq_t q) -> size_t {
        size_t j = 0;
        for (int i = n - 2; i >= 1; --i) {
            if (probabilities[i - 1] < q) {
                probabilities[i] = probabilities[i - 1];
            } else {
                j = i;
                break;
            }
        }
        probabilities[j] = q;
        probabilities.pop_back();
        // pop_back, pop_back, insert into lower_bound/upper_bound & return index
        return j;
    };
    auto Down = [&](size_t n, size_t j) {
        Code c = codes[j];
        for (int i = j; i < n - 1; ++i) {
            codes[i] = codes[i + 1];
        }
        codes[n - 2] = codes[n - 1] = c;
        codes[n - 2].set(c.length, false);
        codes[n - 1].set(c.length, true);
        ++codes[n - 2].length;
        ++codes[n - 1].length;
    };
    auto MyHuffman = [&](size_t n, auto&& my_huffman) -> void {
        if (n == 2) {
            codes[0].set(0, false);
            codes[0].length = 1;
            codes[1].set(0, true);
            codes[1].length = 1;
            return;
        }
        freq_t q = probabilities[n-2] + probabilities[n-1];
        size_t j = Up(n, q);
        my_huffman(n - 1, my_huffman);
        Down(n, j);
    };
    MyHuffman(n, MyHuffman);
    return codes;
}

int main() {
    std::mt19937 gen{std::random_device{}()};
    std::fstream out{"10k.in", std::ios::out};
    std::string alphabet{(std::ostringstream{} << std::fstream {"alphabet.txt"}.rdbuf()).str()};
    generate_file(alphabet, 10'000,out,gen);
//    - B — Алгоритм Хаффмена
    auto codes = Huffman({31, 24, 17, 11, 9, 5, 2, 1});
    return 0;
}