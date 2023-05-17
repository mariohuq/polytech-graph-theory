//
// Created by mhq on 16/05/23.
//
#include <stack>
#include <cassert>
#include <algorithm>
#include "graphs.h"

using namespace graphs;

adjacency_matrix<> unoriented(adjacency_matrix<> g) {
    for (Vertex i = 0; i < g.size(); ++i) {
        for (Vertex j = i + 1; j < g.size(); ++j) {
            if (g[i][j] != 0) {
                g[j][i] = g[i][j];
            }
        }
    }
    return g;
}

std::vector<int> degrees_of(const adjacency_matrix<>& g) {
    std::vector<int> result;
    result.reserve(g.size());
    for (const auto& row: g) {
        int sst = 0;
        for (auto x: row) {
            if (x == 0) continue;
            ++sst;
        }
        result.push_back(sst);
    }
    return result;
}

bool graphs::is_eulerian(const adjacency_matrix<>& g) {
    size_t nVertices = g.size();
    assert(nVertices > 2);
    auto degree = degrees_of(unoriented(g));
    return std::all_of(degree.begin(), degree.end(),
                       [](int d) { return !(d == 0 || d % 2 != 0);});
}

std::vector<Vertex> euler_cycle(adjacency_matrix<> g) {
    size_t nVertices = g.size();
    std::vector<Vertex> result;
    result.reserve(nVertices);
    std::stack<Vertex> s;
    auto degree = degrees_of(g);
    s.push(std::min_element(degree.begin(), degree.end()) - degree.begin());
    while (!s.empty()) {
        Vertex i = s.top();
        if (degree[i] == 0) {
            s.pop();
            result.push_back(i);
            continue;
        }
        for (int j = 0; j < nVertices; j++) {
            if (g[i][j] == 0) continue;
            g[i][j] = 0;
            g[j][i] = 0;
            degree[i] -= 1;
            degree[j] -= 1;
            s.push(j);
            break;
        }
    }
    return result;
}

euler_change_t graphs::eulerize(adjacency_matrix<> g) {
    static const auto odd_only = [](const std::vector<int>& degree){
        std::vector<Vertex> result;
        result.reserve(degree.size() / 2);
        for (int i = 0; i < degree.size(); ++i) {
            if (degree[i] % 2 != 0) {
                result.push_back(i);
            }
        }
        assert(result.size() % 2 == 0 && "Number of vertices with odd degree should be even");
        return result;
    };
    size_t nVertices = g.size();
    if (nVertices <= 2) {
        // There are 1/2 vertices in the graph! Euler cycle is impossible!
        return {};
    }
    g = unoriented(g);
    std::vector<edge_t> added, removed;
    std::vector<int> degree;
    std::vector<Vertex> odd_vertices;

    constexpr auto add = [](decltype(added)& added, decltype(g)& g, Vertex from, Vertex to, int weight = 1) {
        assert(!g[from][to]);
        assert(from != to);
        g[from][to] = 1;
        g[to][from] = 1;
        added.emplace_back(from, to, weight);
    };
    constexpr auto remove = [](decltype(removed)& removed, decltype(g)& g, Vertex from, Vertex to) {
        removed.emplace_back(from, to, g[from][to]);
        g[from][to] = 0;
        g[to][from] = 0;
    };
    static const auto try_change = [&]() {
        for (Vertex i{}; i < odd_vertices.size(); i++) {
            for (Vertex j = i + 1; j < odd_vertices.size(); j++) {
                Vertex from = odd_vertices[i];
                Vertex to = odd_vertices[j];
                if (g[from][to] == 0) {
                    add(added, g, from, to);
                    return;
                }
                remove(removed, g, from, to);
                if (degree[from] == 1 && degree[to] % 2 != 0 && degree[to] != 1) {
                    if (from != nVertices - 1) {
                        add(added, g, from, nVertices - 1);
                    } else {
                        add(added, g, 0, from);
                    }
                    if (from != nVertices - 2) {
                        add(added, g, from, nVertices - 2);
                    } else {
                        add(added, g, 0, from);
                    }
                }
                assert(!(degree[to] == 1 && degree[from] % 2 != 0 && degree[from] != 1));
                /*if (degree[to] == 1 && degree[from] % 2 != 0 && degree[from] != 1) {
                    if (to != nVertices - 1 && g[from][to] == 0) {
                        add(to, nVertices - 1);
                    } else {
                        add(0, to);
                    }
                    if (to != nVertices - 2 && g[from][to] == 0) {
                        add(to, nVertices - 2);
                    } else {
                        add(0, to);
                    }
                }*/
                return;
            }
        }
    };
    while (!is_eulerian(g)) {
        degree = degrees_of(g);
        if (std::any_of(degree.begin(), degree.end(), [](auto x){return x == 0;})){
            odd_vertices.clear();
            odd_vertices.reserve(nVertices);
            for (int i = 0; i < nVertices; ++i) {
                odd_vertices.push_back(i);
            }
        } else {
            odd_vertices = odd_only(degree);
        }
        try_change();
    }
    return {
        .eulerian = g,
        .added = added,
        .removed = removed,
        .euler_cycle = euler_cycle(g)
    };
}