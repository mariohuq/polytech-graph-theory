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
#include <cassert>

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

    Code() = default;

    bool operator==(const Code other) const {
        return std::tie(value, length) == std::tie(other.value, other.length);
    }

    Code with_zero() const {
        return { value,
                static_cast<decltype(length)>(length + 1u)};
    }

    Code with_one() const {
        return { value | (1u << length),
                static_cast<decltype(length)>(length + 1u)};
    }
};

static_assert(sizeof(Code) == 4);

std::vector<Code> Huffman(std::vector<freq_t> probabilities) {
    std::vector<Code> codes;
    codes.reserve(probabilities.size());
    auto Up = [&](freq_t q) -> size_t {
        probabilities.pop_back();
        probabilities.pop_back();
        auto where = std::lower_bound(probabilities.rbegin(), probabilities.rend(), q).base();
        return probabilities.insert(where, q) - probabilities.begin();
    };
    auto Down = [&](size_t j) {
        Code current = codes[j];
        codes.erase(codes.begin() + j);
        codes.push_back(current.with_zero());
        codes.push_back(current.with_one());
    };
    auto MyHuffman = [&](auto&& my_huffman) -> void {
        size_t n = probabilities.size();
        if (n == 2) {
            codes.push_back(Code{}.with_zero());
            codes.push_back(Code{}.with_one());
            return;
        }
        size_t j = Up(probabilities[n-2] + probabilities[n-1]);
        my_huffman(my_huffman);
        Down(j);
    };
    MyHuffman(MyHuffman);
    return codes;
}

int main() {
    std::mt19937 gen{std::random_device{}()};
    std::fstream out{"10k.in", std::ios::out};
    std::string alphabet{(std::ostringstream{} << std::fstream {"alphabet.txt"}.rdbuf()).str()};
    generate_file(alphabet, 10'000,out,gen);
    return 0;
}

void test_haffman() {
    std::vector<Code> expected{
        {0b00, 2},
        {0b01, 2},
        {0b11, 2},
        {0b110, 3},
        {0b0010, 4},
        {0b01010, 5},
        {0b011010, 6},
        {0b111010, 6}};
    std::vector<Code> actual = Huffman({31, 24, 17, 11, 9, 5, 2, 1});
    assert(actual == expected);
}