//
// Created by mhq on 02/02/23.
//
#include "huffman.h"
#include "runlength.h"

#include <random>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>

void generate_file(const std::string &alphabet,
                   size_t characters_count,
                   std::ostream &os,
                   std::mt19937 &generator) {
    std::uniform_int_distribution<size_t> distribution(0, alphabet.size() - 1);
    for (int i = 0; i < characters_count; ++i) {
        os << alphabet[distribution(generator)];
    }
}

int main(int argc, const char *argv[]) {
    test_huffman();
    test_header();

    if (argc == 2 && strcmp(argv[1], "generate") == 0) {
        std::mt19937 gen{std::random_device{}()};
        std::string alphabet{(std::ostringstream{} << std::cin.rdbuf()).str()};
        generate_file(alphabet, 10'000, std::cout, gen);
        return EXIT_SUCCESS;
    }
    if (argc == 3 && strcmp(argv[1], "encode_huffman") == 0) {
        std::ifstream input{argv[2]};
        auto [apriori, stats] = huffman::encode(input, std::cout);
        std::cerr
                << "цена кодирования = "
                << static_cast<double>(apriori.body_size_bits) / static_cast<double>(apriori.message_length) << '\n'
                << "коэффициент сжатия = "
                << static_cast<double>(stats.output_size) / static_cast<double>(stats.input_size) << '\n';
        return EXIT_SUCCESS;
    }
    if (argc == 2 && strcmp(argv[1], "decode_huffman") == 0) {
        huffman::decode(std::cin, std::cout);
        return EXIT_SUCCESS;
    }
    if (argc == 2 && strcmp(argv[1], "encode_rle") == 0) {
        auto stats = rle::encode(std::cin, std::cout);
        std::cerr
            << "коэффициент сжатия = "
            << static_cast<double>(stats.output_size) / static_cast<double>(stats.input_size) << '\n';
        return EXIT_SUCCESS;
    }
    if (argc == 2 && strcmp(argv[1], "decode_rle") == 0) {
        rle::decode(std::cin, std::cout);
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}



