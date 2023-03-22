//
// Created by mhq on 22/03/23.
//
#include "polya_dist.h"
#include <map>
#include <iostream>

void test_polya() {
    auto dis = polya_1<>(20, 20, 1, 5);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::map<int, int> map;

    for (int n = 0; n < 1e5; ++n)
        ++map[dis(gen)];

    for(const auto& [num, count] : map)
        std::cout << num << "\t" << count << "\n";
}