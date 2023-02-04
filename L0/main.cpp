//
// Created by mhq on 02/02/23.
//
#include <cstddef>
#include <ostream>
#include <random>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <bitset>
#include <cassert>
#include <numeric>

using freq_t = size_t;
using FrequencyMap = std::map<char, freq_t>;

void generate_file(const std::string &alphabet,
                   size_t characters_count,
                   std::ostream &os,
                   std::mt19937 &generator) {
    std::uniform_int_distribution<size_t> distribution(0, alphabet.size() - 1);
    for (int i = 0; i < characters_count; ++i) {
        os << alphabet[distribution(generator)];
    }
}

FrequencyMap count_characters(std::istream &is) {
    FrequencyMap res{};
    char c;
    while (is.get(c)) {
        ++res[c];
    }
    return res;
}

std::vector<char> alphabet_sorted(const FrequencyMap &frequencies) {
    std::vector<char> res{};
    res.reserve(frequencies.size());
    for (auto [c, _]: frequencies) {
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
        return {value,
                static_cast<decltype(length)>(length + 1u)};
    }

    Code with_one() const {
        return {value | (1u << length),
                static_cast<decltype(length)>(length + 1u)};
    }
};

static_assert(sizeof(Code) == 4);

using AlphabetCoding = std::map<char, Code>;

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
    auto MyHuffman = [&](auto &&my_huffman) -> void {
        size_t n = probabilities.size();
        if (n == 2) {
            codes.push_back(Code{}.with_zero());
            codes.push_back(Code{}.with_one());
            return;
        }
        size_t j = Up(probabilities[n - 2] + probabilities[n - 1]);
        my_huffman(my_huffman);
        Down(j);
    };
    MyHuffman(MyHuffman);
    return codes;
}

struct EncodingStats {
    size_t input_size;
    size_t output_size;
};

double coding_price(const std::vector<Code>& codes, const std::vector<freq_t>& frequencies) {
    size_t res = std::inner_product(codes.begin(), codes.end(), frequencies.begin(),
                                    size_t{}, std::plus<size_t>{}, [](Code code, freq_t freq) {
        return code.length * freq;
    });
    size_t one = std::accumulate(frequencies.begin(), frequencies.end(), size_t{});
    return static_cast<double>(res) / static_cast<double>(one);
}

EncodingStats encode(std::istream& is, std::ostream& os, const AlphabetCoding& coding) {
    // Header
    size_t alphabet_size = coding.size();
    os.write(reinterpret_cast<char*>(&alphabet_size), sizeof(alphabet_size));
    for (auto [character, code]: coding) {
        os.write(&character, sizeof(char));
        os.write(reinterpret_cast<char*>(&code), sizeof(Code));
    }
    EncodingStats result{};
    result.output_size = sizeof(alphabet_size) + alphabet_size * (sizeof(char) + sizeof(Code));
    char input_buffer;
    size_t nbits = 0;
    char current_byte = '\0';

    auto bitout = [&](Code code) {
        for (; code.length > 0; code.length--, code.value >>= 1) {
            /* shift current byte left one */
            current_byte <<= 1;
            /* put a one on the end of this byte if b is '1' */
            current_byte |= code.value & 1;
            /* one more bit */
            nbits++;
            /* enough bits?  write out the byte */
            if (nbits == 8) {
                os.put(current_byte);
                result.output_size++;
                nbits = 0;
                current_byte = '\0';
            }
        }
    };
    result.input_size = 0;
    while (is.get(input_buffer)) {
        bitout(coding.at(input_buffer));
        result.input_size++;
    }
    if (nbits != 0) {
        bitout(Code{0, static_cast<unsigned char>(8 - nbits)});
    }
    return result;
}


void test_haffman();

int main(int argc, const char *argv[]) {
    test_haffman();
    // ./lab0 generate <alphabet.txt >10k.in
    if (argc == 2 && strcmp(argv[1], "generate") == 0) {
        std::mt19937 gen{std::random_device{}()};
        std::string alphabet{(std::ostringstream{} << std::cin.rdbuf()).str()};
        generate_file(alphabet, 10'000, std::cout, gen);
        return EXIT_SUCCESS;
    }
    if (argc == 3 && strcmp(argv[1], "encode_huffman") == 0) {
        std::ifstream input{argv[2]};
        auto freqs = count_characters(input);
        auto alphabet = alphabet_sorted(freqs);
        std::vector<freq_t> frequencies;
        frequencies.reserve(alphabet.size());
        std::transform(alphabet.begin(), alphabet.end(), std::back_inserter(frequencies),
                       [&](char alpha){
            return freqs[alpha];
        });
        auto codes = Huffman(frequencies);
        std::cerr << "цена кодирования = " << coding_price(codes, frequencies) << '\n';
        std::map<char, Code> encoding{};
        std::transform(alphabet.begin(), alphabet.end(), codes.begin(),
                       std::inserter(encoding, encoding.end()),
                       std::make_pair<const char&, const Code&>);
        input.clear();
        input.seekg(0, std::ios::beg); // rewind
        auto stats = encode(input, std::cout, encoding);
        std::cerr
            << "коэффициент сжатия = "
            << static_cast<double>(stats.output_size) / static_cast<double>(stats.input_size) << '\n';
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

void test_haffman() {
    std::vector<Code> expected{
            {0b00,     2},
            {0b01,     2},
            {0b11,     2},
            {0b110,    3},
            {0b0010,   4},
            {0b01010,  5},
            {0b011010, 6},
            {0b111010, 6}};
    std::vector<Code> actual = Huffman({31, 24, 17, 11, 9, 5, 2, 1});
    assert(actual == expected);
}