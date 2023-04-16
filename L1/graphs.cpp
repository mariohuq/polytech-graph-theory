//
// Created by mhq on 25/03/23.
//
#include "graphs.h"
#include <algorithm>
#include <cassert>
#include <queue>

using namespace graphs;

template<typename Func>
adjacency_matrix<>
matrix_multiply(const adjacency_matrix<> &lhs, const adjacency_matrix<> &rhs, Func extrem);
// матрица m[i][j] из длин минимальных (или максимальных) путей от вершины i до j
template<typename Func>
adjacency_matrix<> matrix_power_shimbell(const adjacency_matrix<> &that, size_t power, Func extrem);


std::vector<size_t> graphs::out_degrees(size_t nVertices, std::mt19937 &gen) {
    if (nVertices == 0) {
        return std::vector<size_t>{};
    }
    if (nVertices == 1) {
        return std::vector<size_t>{0};
    }
    // степень вершины может быть в диапазоне [1 .. n-1]
    // распределение выдает числа в диапазоне [0 .. n-2]
    auto dis = polya_1<int>(2, 20, 1, static_cast<int>(nVertices) - 3);
    std::vector<size_t> result(nVertices);
    for (size_t &x: result) {
        x = dis(gen) + 1;
    }
    std::sort(result.rbegin(), result.rend());

    // чтобы обеспечить отсутствие циклов
    for (size_t i = 0; i < nVertices; ++i) {
        result[i] = std::min(nVertices - i - 1, result[i]);
    }
    return result;
}

adjacency_matrix<> graphs::from_degrees(std::vector<size_t> vertex_degrees, std::mt19937 &gen) {
    auto nVertices = vertex_degrees.size();
    adjacency_matrix<> result(nVertices, std::vector<int>(nVertices));
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

adjacency_matrix<> graphs::min_path_lengths(const adjacency_matrix<> &that, size_t path_length) {
    if (that.empty()) {
        return {};
    }
    const size_t &(*fn)(const unsigned long &, const unsigned long &) = &std::min<size_t>;
    return matrix_power_shimbell(that, path_length, fn);
}
adjacency_matrix<> graphs::max_path_lengths(const adjacency_matrix<> &that, size_t path_length) {
    if (that.empty()) {
        return {};
    }
    const size_t &(*fn)(const unsigned long &, const unsigned long &) = &std::max<size_t>;
    return matrix_power_shimbell(that, path_length, fn);
}
adjacency_matrix<> graphs::generate(size_t nVertices, std::mt19937 &gen) {
    return from_degrees(out_degrees(nVertices, gen), gen);
}

dijkstra_result_t
graphs::min_path_distances_dijkstra(const adjacency_matrix<> &g, Vertex start_vertex) {
    // https://github.com/okwedook/olymp/blob/master/code/graph/Dijkstra.hpp
    std::vector<Vertex> precedents(g.size(), NO_VERTEX);
    std::vector<bool> visited(g.size(), false);
    size_t iterations{};

    std::vector<int> distances(g.size(), INF);
    auto cmp = [&](Vertex i, Vertex j) {
        return distances[i] > distances[j];
    };
    std::priority_queue<Vertex, std::vector<Vertex>, decltype(cmp)> qu(cmp);
    distances[start_vertex] = 0;
    qu.push(start_vertex);
    while(!qu.empty()) {
        auto nearest_vertex = qu.top();
        qu.pop();
        if (visited[nearest_vertex]) {
            continue;
        }
        visited[nearest_vertex] = true;
        for (Vertex v{}; v < g.size(); ++v) {
            if (g[nearest_vertex][v] == 0) {
                continue;
            }
            auto distance_via_nearest = distances[nearest_vertex] + g[nearest_vertex][v];
            if (distance_via_nearest < distances[v]) {
                distances[v] = distance_via_nearest;
                qu.push(v);
                visited[v] = false;
                precedents[v] = nearest_vertex;
            }
            iterations++;
        }
    }
    return {
            .distances = distances,
            .precedents = precedents,
            .iterations = iterations
    };
}

dijkstra_result_t
graphs::min_path_distances_bellman_ford(const adjacency_matrix<> &g, Vertex start_vertex) {
    std::vector<Vertex> precedents(g.size(), NO_VERTEX);
    size_t iterations{};

    std::vector<int> distances(g.size(), INF);
    distances[start_vertex] = 0;

    for (int _{}; _ < g.size(); ++_) {
        for (Vertex u{}; u < g.size(); ++u) {
            for (Vertex v{}; v < g.size(); ++v) {
                if (g[u][v] == 0) {
                    continue;
                }
                auto candidate = distances[u] + g[u][v];
                if (candidate < distances[v]) {
                    // новый путь короче
                    distances[v] = candidate;
                    precedents[v] = u;
                }
                iterations++;
            }
        }
    }
    // проверка на отрицательные контуры
    for (Vertex u{}; u < g.size(); ++u) {
        for (Vertex v{}; v < g.size(); ++v) {
            if (g[u][v] == 0) {
                continue;
            }
            // найден контур отрицательного веса
            if (distances[v] > distances[u] + g[u][v]) {
                return {
                    .iterations = iterations
                };
            }
        }
    }

    return {
            .distances = distances,
            .precedents = precedents,
            .iterations = iterations
    };
}

floyd_warshall_result_t
graphs::min_path_distances_floyd_warshall(const adjacency_matrix<> &g) {
    std::vector<std::vector<Vertex>> precedents(g.size(), std::vector<Vertex>(g.size(), NO_VERTEX));
    size_t iterations{};
    std::vector<std::vector<int>> distances(g.size(), std::vector<int>(g.size(), INF));

    for (Vertex i{}; i < g.size(); ++i) {
        for (Vertex j{}; j < g.size(); ++j) {
            if (g[i][j] == 0) {
                continue;
            }
            distances[i][j] = g[i][j];
            precedents[i][j] = j;
        }
    }

    for (Vertex i{}; i < g.size(); ++i) {
        for (Vertex j{}; j < g.size(); ++j) {
            if (g[i][j] == 0) {
                continue;
            }
            if (i == j || distances[j][i] == INF) {
                continue;
            }
            for (Vertex k{}; k < g.size(); ++k) {
                if (i == k || distances[i][k] == INF) {
                    continue;
                }
                if (distances[j][k] == INF
                 || distances[j][k] > distances[j][i] + distances[i][k]) {
                    distances[j][k] = distances[j][i] + distances[i][k];
                    precedents[j][k] = precedents[j][i];
                }
            }
        }
        for (Vertex j{}; j < g.size(); ++j) {
            if (distances[j][j] < 0) {
                // Узел `j` входит в отрицательный контур
                return {
                    .iterations = iterations
                };
            }
        }
    }

    return {
        .distances = distances,
        .precedents = precedents,
        .iterations = iterations
    };
}

std::vector<Vertex>
graphs::reconstruct_path(const std::vector<Vertex> &precedents, Vertex from, Vertex to) {
    std::vector<Vertex> result;
    result.reserve(precedents.size());
    result.push_back(to);
    while(to != from && precedents[to] != NO_VERTEX) {
        to = precedents[to];
        result.push_back(to);
    }
    if (to != from) {
        return {};
    }
    std::reverse(result.begin(), result.end());
    return result;
}

template<typename Func>
adjacency_matrix<>
matrix_power_shimbell(const adjacency_matrix<> &that, size_t power, Func extrem) {
    assert(power > 0);
    adjacency_matrix<> result = that;
    for (int i = 1; i < power; ++i) {
        result = matrix_multiply<>(result, that, extrem);
    }
    return result;
}

template<typename Func>
adjacency_matrix<>
matrix_multiply(const adjacency_matrix<> &lhs, const adjacency_matrix<> &rhs, Func extrem) {
    assert(lhs.size() == lhs[0].size()); // квадратная
    assert(rhs.size() == rhs[0].size()); // квадратная
    assert(lhs.size() == rhs.size());
    size_t n = lhs.size();
    adjacency_matrix<> result = lhs;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            auto &res = result[i][j];
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
