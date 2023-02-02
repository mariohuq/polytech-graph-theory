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

using FrequencyMap = std::map<char, size_t>;

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

int main() {
    std::mt19937 gen{std::random_device{}()};
    std::fstream out{"10k.in", std::ios::out};
    std::string alphabet{(std::ostringstream{} << std::fstream {"alphabet.txt"}.rdbuf()).str()};
    generate_file(alphabet, 10'000,out,gen);
//    - B — Алгоритм Хаффмена
    return 0;
}