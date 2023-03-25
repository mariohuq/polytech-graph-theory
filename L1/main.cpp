//
// Created by mhq on 13/03/23.
//

#include "polya_dist.h"
#include "graphs.h"

#include <iostream>
#include <cstdlib>
#include <cassert>

using namespace graphs;

//Д. 1.3. Алгоритм построения случайного ориентированного бесконтурного графа, Хаг
adjacency_matrix<bool> generate_acyclic_convex(size_t nVertices, size_t nEdges) {
    auto dis = polya_1<int>(20, 20, 1, nVertices * (nVertices - 1) / 2 - 1);
    auto split = [](int k) -> div_t {
        int i = (-1 + sqrt(8 * k + 1)) / 2;
        return {k - i*(i+1)/2, i+1};
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    adjacency_matrix<bool> result{nVertices, std::vector<bool>(nVertices)};

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
    size_t n = 10;
    std::mt19937 gen(std::random_device{}());
    auto degs = out_degrees(n, gen);
    for (auto d: degs) {
        std::cout << d << ' ';
    }
    std::cout << '\n';
    auto g = from_degrees(degs, gen);
    for (const auto& row : g) {
        for (const auto& elem: row) {
            std::cout << elem << ',';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
    auto g3 = min_path_lengths(g, 3);
    for (const auto& row : g3) {
        for (const auto& elem: row) {
            std::cout << elem << ',';
        }
        std::cout << '\n';
    }
    auto count = count_paths(g,0);
    size_t to;
    while (std::cin >> to) {
        std::cout << count(to);
    }
    return 0;
}