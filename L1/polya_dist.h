//
// Created by mhq on 13/03/23.
//

#pragma once
#include <random>
#include <tuple>

template <class IntType = int>
std::discrete_distribution<IntType> polya_1(IntType black_n, IntType red_n, IntType c, IntType size) {
    std::vector<double> probabilities;
    probabilities.reserve(size + 1);
    double p0 = 1;
    for (int i = 0; i < size; i++) {
        p0 *= (red_n + i * c);
        p0 /= (black_n + red_n + i * c);
    }
    double p = p0;
    probabilities.push_back(p);
    for (int x = 1; x <= size; ++x) {
        p *= (size - (x - 1)) * (black_n + (x - 1) * c);
        p /= x * (red_n + (size - x) * c);
        probabilities.push_back(p);
    }
    return { probabilities.begin(), probabilities.end() };
}

void test_polya();