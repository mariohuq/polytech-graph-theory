//
// Created by mhq on 13/03/23.
//

#include "polya_dist.h"
#include "graphs.h"

#include <iostream>
#include <cstdlib>
#include <cassert>

using namespace graphs;

std::mt19937 gen{std::random_device{}()};

//Д. 1.3. Алгоритм построения случайного ориентированного бесконтурного графа, Хаг
adjacency_matrix<bool> generate_acyclic_convex(size_t nVertices, size_t nEdges) {
    auto dis = polya_1<int>(20, 20, 1, nVertices * (nVertices - 1) / 2 - 1);
    auto split = [](int k) -> div_t {
        int i = static_cast<int>((-1 + sqrt(8 * k + 1)) / 2);
        return {k - i*(i+1)/2, i+1};
    };

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

void max_flow_test();

int main() {
    max_flow_test();
    return 0;
}

void test_1_vertex() {
    auto g = generate(1, gen);
}

void test_2_vertexes() {
    auto g = generate(2, gen);
    for (const auto& row : g) {
        for (const auto& elem: row) {
            std::cout << elem << ' ';
        }
        std::cout << '\n';
    }
}

void sample_usage() {
    size_t n = 10;
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
}

void max_flow_test() {
    auto cost = generate(6, gen);
    auto capacity = generate_capacities(cost, gen);
    auto g = add_supersource_supersink(cost, capacity);
    auto [max_flow, flow] = max_flow_ford_fulkerson(g);
    std::cout << max_flow << '\n';
    std::cout << flow[g.source][1] << '\n';
}