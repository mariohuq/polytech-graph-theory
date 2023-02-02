//
// Created by mhq on 02/02/23.
//
#include <cstddef>
#include <ostream>
#include <random>
#include <fstream>
#include <iostream>

void generate_file(const std::vector<std::string>& alphabet,
                   size_t characters_count,
                   std::ostream& os,
                   std::mt19937& generator) {
    std::uniform_int_distribution<size_t> distribution(0, alphabet.size() - 1);
    for (int i = 0; i < characters_count; ++i) {
        os << alphabet[distribution(generator)];
    }
}

int main() {
    std::mt19937 gen{std::random_device{}()};
    std::fstream out{"10k.in", std::ios::out};
    generate_file({"С", "М", "И", "Ъ", "Е", "Т",
                   "с", "м", "и", "ъ", "е", "т",
                   " ",
                   "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                   "<", "."}, 10'000,out,gen);
//    - B — Алгоритм Хаффмена
    return 0;
}