//
// Created by mhq on 22/03/23.
//

#pragma once
#include "polya_dist.h"
#include <algorithm>

template <typename T>
using adjacency_matrix = std::vector<std::vector<T>>;

using vertex_degrees = std::vector<size_t>;

vertex_degrees out_degrees(size_t nVertices, std::mt19937& gen) {
    // степень вершины может быть в диапазоне [1 .. n-1]
    // распределение выдает числа в диапазоне [0 .. n-2]
    auto dis = polya_1<size_t>(2, 20, 1, nVertices - 3);
    vertex_degrees result(nVertices);
    for (size_t& x : result) {
        x = dis(gen) + 1;
    }
    std::sort(result.rbegin(), result.rend());

    // чтобы обеспечить отсутствие циклов
    for (size_t i = 0; i < nVertices; ++i) {
        result[i] = std::min(nVertices - i - 1, result[i]);
    }
    return result;
}

adjacency_matrix<bool> from_degrees(vertex_degrees degrees, std::mt19937& gen) {
    auto nVertices = degrees.size();
    adjacency_matrix<bool> result(nVertices, std::vector<bool>(nVertices));

    for (int i = 0; i < nVertices; ++i) {
        for (int j = i + 1; j < degrees[i] + i + 1; ++j) {
            result[i][j] = true;
        }
        if (degrees[i] == nVertices - i - 1) {
            continue;
        }
        // degrees[i] of 1
        // nVertices - i - 1 all
        std::shuffle(result[i].begin() + i + 1, result[i].end(), gen);
    }
    return result;
}
