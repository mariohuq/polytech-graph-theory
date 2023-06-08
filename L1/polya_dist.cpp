//
// Created by mhq on 22/03/23.
//
#include "polya_dist.h"
#include <map>
#include <iostream>

void test_polya() {
    auto dis = polya_1<>(20, 20, 1, 5);
    std::mt19937 gen{std::random_device{}()};
    std::map<int, int> map;

    for (int n = 0; n < 1e5; ++n)
        ++map[dis(gen)];

    for(const auto& [num, count] : map)
        std::cout << num << "\t" << count << "\n";
}

// start snippet polya_1_histograms
void polya_1_histograms() {
    std::mt19937 gen{std::random_device{}()};
    constexpr auto N = 10000; // итераций
    constexpr auto n = 5;
    for (auto b : {3, 140, 12, 20}) {
        auto distribution = polya_1(b, 20, 1, n);
        std::vector<size_t> data(n + 1);
        std::cout << "b = " << b << '\n';
        for (int i = 0; i < N; ++i) {
            data[distribution(gen)]++;
        }
        std::cout << 'x' << '\t' << "p(x)" << '\n';
        for (int i = 0; i <= n; ++i) {
            std::cout << i << '\t' << static_cast<double>(data[i]) / N << '\n';
        }
    }
}
// end snippet polya_1_histograms