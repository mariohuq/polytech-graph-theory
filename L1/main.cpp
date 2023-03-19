//
// Created by mhq on 13/03/23.
//

#include <iostream>
#include <map>
#include <cstdlib>
#include <cassert>
#include "polya_dist.h"

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

using adjacency_matrix = std::vector<std::vector<bool>>;
//Д. 1.3. Алгоритм построения случайного ориентированного бесконтурного графа, Хаг
adjacency_matrix generate_acyclic_convex(size_t nVertices, size_t nEdges) {
    auto dis = polya_1<int>(20, 20, 1, nVertices * (nVertices - 1) / 2 - 1);
    auto split = [](int k) -> div_t {
        int i = (-1 + sqrt(8 * k + 1)) / 2;
        return {k - i*(i+1)/2, i+1};
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    adjacency_matrix result{nVertices, std::vector<bool>(nVertices)};

    for (int k = 0; k < nEdges; k++) {
        // i < j by construction
        auto [i, j] = split(dis(gen));
        assert(i < j);
        assert(j < nVertices);
        if (result[i][j]) {
            k--;
            continue;
        }
        result[i][j] = true;
    }
    return result;
}

int main() {
    int n = 10;
    auto g = generate_acyclic_convex(10, 18);
    for (const auto& row : g) {
        for (const auto& elem: row) {
            std::cout << elem << ',';
        }
        std::cout << '\n';
    }
    return 0;
}

