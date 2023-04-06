//
// Created by mhq on 25/03/23.
//
#include "graphs.h"
#include <algorithm>
#include <cassert>

using adjacency_matrix = graphs::adjacency_matrix<size_t>;

namespace graphs {
    template<typename Func>
    adjacency_matrix<size_t> matrix_multiply(const adjacency_matrix<size_t>& lhs, const adjacency_matrix<size_t>& rhs, Func extrem);

    // матрица m[i][j] из длин минимальных (или максимальных) путей от вершины i до j
    template<typename Func>
    adjacency_matrix<size_t> matrix_power_shimbell(const adjacency_matrix<size_t>& that, size_t power, Func extrem);
}

std::vector<size_t> graphs::out_degrees(size_t nVertices, std::mt19937 &gen) {
    if (nVertices == 0) {
        return std::vector<size_t> {};
    }
    if (nVertices == 1) {
        return std::vector<size_t>{ 0 };
    }
    // степень вершины может быть в диапазоне [1 .. n-1]
    // распределение выдает числа в диапазоне [0 .. n-2]
    auto dis = polya_1<int>(2, 20, 1, static_cast<int>(nVertices) - 3);
    std::vector<size_t> result(nVertices);
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

adjacency_matrix graphs::from_degrees(std::vector<size_t> vertex_degrees, std::mt19937 &gen) {
    auto nVertices = vertex_degrees.size();
    adjacency_matrix<size_t> result(nVertices, std::vector<size_t>(nVertices));
    auto dis = polya_1<size_t>(4, 8, 3, 50 - 1);
    for (int i = 0; i < nVertices; ++i) {
        for (int j = i + 1; j < vertex_degrees[i] + i + 1; ++j) {
            result[i][j] = dis(gen) + 1;
            assert(result[i][j] > 0);
        }
        if (vertex_degrees[i] == nVertices - i - 1) {
            continue;
        }
        // degrees[i] of ones, other are zeros.
        // nVertices - i - 1 total
        std::shuffle(result[i].begin() + i + 1, result[i].end(), gen);
    }
    return result;
}

adjacency_matrix graphs::min_path_lengths(const adjacency_matrix<> &that, size_t path_length) {
    if (that.empty()) {
        return {};
    }
    const size_t & (*fn)(const unsigned long &,const unsigned long &)  = &std::min<size_t>;
    return matrix_power_shimbell(that, path_length, fn);
}

adjacency_matrix graphs::max_path_lengths(const adjacency_matrix<> &that, size_t path_length) {
    if (that.empty()) {
        return {};
    }
    const size_t & (*fn)(const unsigned long &,const unsigned long &)  = &std::max<size_t>;
    return matrix_power_shimbell(that, path_length, fn);
}

adjacency_matrix graphs::generate(size_t nVertices, std::mt19937 &gen) {
    return from_degrees(out_degrees(nVertices, gen), gen);
}

template<typename Func>
adjacency_matrix graphs::matrix_power_shimbell(const adjacency_matrix<> &that, size_t power, Func extrem) {
    assert(power > 0);
    adjacency_matrix<size_t> result = that;
    for (int i = 1; i < power; ++i) {
        result = matrix_multiply<>(result, that, extrem);
    }
    return result;
}

template<typename Func>
adjacency_matrix
graphs::matrix_multiply(const adjacency_matrix<> &lhs, const adjacency_matrix<> &rhs, Func extrem) {
    assert(lhs.size() == lhs[0].size()); // квадратная
    assert(rhs.size() == rhs[0].size()); // квадратная
    assert(lhs.size() == rhs.size());
    size_t n = lhs.size();
    adjacency_matrix<size_t> result = lhs;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            auto& res = result[i][j];
            res = 0;
            for (int k = 0; k < n; ++k) {
                if (lhs[i][k] == 0 || rhs[k][j] == 0) {
                    continue;
                }
                res = res ? extrem(res, lhs[i][k] + rhs[k][j]) : lhs[i][k] + rhs[k][j];
            }
        }
    }
    return result;
}

size_t graphs::count_paths::operator()(size_t v) {
    if (visited[v]) {
        return d[v];
    }
    size_t result = 0;
    visited[v] = true;
    for (int i = 0; i < graph.size(); ++i) {
        if (!graph[i][v]) {
            continue;
        }
        // ∃ ребро из i в v
        result += operator()(i);
    }
    return d[v] = result;
}
