//1
// Created by mhq on 20/05/23.
//
#include "graphs.h"
#include <algorithm>
#include <iostream>
#include <cassert>

using namespace graphs;

// start snippet is_hamiltonian
bool graphs::is_hamiltonian(adjacency_matrix<> g) {
    if (g.size() <= 2) {
        return false;
    }
    g = unoriented(g);
    auto ds = degrees_of(g);
    // Dirac's Theorem
    if (g.size() > 3 && std::all_of(ds.begin(), ds.end(),
                                    [&](auto d) { return 2 * d >= g.size(); })) {
        return true;
    }
    // necessary condition (easy)
    if (std::any_of(ds.begin(), ds.end(),
                    [&](auto d) { return d <= 1; })) {
        return false;
    }
    hamilton_cycles hc{g};
    return hc().exists();
}
// end snippet is_hamiltonian

const Vertex graphs::hamilton_cycles::start = 0;

// start snippet hamilton_cycles_op
costed_path_t graphs::hamilton_cycles::operator()() {
    static constexpr auto cost = [](decltype(g) &g, decltype(candidate) &candidate) {
        size_t result = 0;
        for (size_t i = 1; i < candidate.size(); ++i) {
            result += g[candidate[i - 1]][candidate[i]];
        }
        return result + g[candidate.front()][candidate.back()];
    };
    while (has_next()) {
        auto x = candidate.back();
        if (N[x].empty()) {
            candidate.pop_back();
            continue;
        }
        auto y = N[x].back();
        N[x].pop_back();
        if (std::find(candidate.begin(), candidate.end(), y) != candidate.end()) {
            continue;
        }
        candidate.push_back(y);
        N[y] = adj(y);
        if (candidate.size() != g.size()) {
            continue;
        }
        if (std::find(N[y].begin(), N[y].end(), start) == N[y].end()) {
            candidate.pop_back();
            continue;
        }
        auto res = candidate;
        candidate.pop_back();
        res.push_back(start);
        return {res, cost(g, res)};
    }
    return {};
}
// end snippet hamilton_cycles_op

std::deque<Vertex> graphs::hamilton_cycles::adj(Vertex x) {
    std::deque<Vertex> res;
    for (Vertex i = 0; i < g.size(); ++i) {
        if (g[x][i] == 0) {
            continue;
        }
        res.push_back(i);
    }
    return res;
}

// start snippet hamiltonize
graph_change_t graphs::hamiltonize(const adjacency_matrix<> &g_orig) {
    auto g = unoriented(g_orig);
    auto added = std::vector<edge_t>{};
    {
        auto ds = degrees_of(g);
        auto leafs = std::deque<Vertex>{};
        // connect all leafs with a path
        for (Vertex i{}; i < g.size(); ++i) {
            if (ds[i] <= 1) {
                leafs.push_back(i);
            }
            if (leafs.size() > 1) {
                assert(g[leafs[0]][leafs[1]] == 0);
                g[leafs[1]][leafs[0]] = g[leafs[0]][leafs[1]] = 1;
                added.emplace_back(leafs[0], leafs[1], 1);
                leafs.pop_front();
            }
        }
    }
    auto result = [&]() -> graph_change_t {
        return {
            .changed = oriented(g),
            .added = added,
            .removed = {}
        };
    };
    if (is_hamiltonian(g)) {
        return result();
    }

    // Create list of no-edges
    auto no_edges = std::vector<edge_t>{};
    no_edges.reserve(g.size() * g.size() / 4);
    for (Vertex i = 0; i < g.size(); ++i) {
        for (Vertex j = i + 1; j < g.size(); ++j) {
            if (g[i][j] == 0) {
                no_edges.emplace_back(i, j, 1);
            }
        }
    }

    // Try with one edge
    for (auto [from1, to1, w]: no_edges) {
        g[from1][to1] = g[to1][from1] = w;
        if (is_hamiltonian(g)) {
            added.emplace_back(from1, to1, w);
            return result();
        }
        g[from1][to1] = g[to1][from1] = 0;
    }
    // Try adding two edges
    for (auto [from1, to1, w]: no_edges) {
        g[from1][to1] = g[to1][from1] = w;
        // adding a second edge
        for (auto [from2, to2, ww]: no_edges) {
            if (from2 == from1 && to2 == to1) {
                continue;
            }
            g[from2][to2] = g[to2][from2] = ww;
            if (is_hamiltonian(g)) {
                added.emplace_back(from2, to2, ww);
                return result();
            }
            // If it still doesn't work, remove the second edge
            g[from2][to2] = g[to2][from2] = 0;
        }
        // Otherwise, remove the first edge
        g[from1][to1] = g[to1][from1] = 0;
    }
    // Try adding three edges
    for (auto [from1, to1, w]: no_edges) {
        g[from1][to1] = g[to1][from1] = w;
        // adding a second edge
        for (auto [from2, to2, ww]: no_edges) {
            if (from2 == from1 && to2 == to1) {
                continue;
            }
            g[from2][to2] = g[to2][from2] = ww;
            if (is_hamiltonian(g)) {
                added.emplace_back(from2, to2, ww);
                return result();
            }
            // If it Still doesn't work, add the third edge
            for (auto [from3, to3, www]: no_edges) {
                if (from3 == from1 && to3 == to1) {
                    continue;
                }
                g[from3][to3] = g[to3][from3] = www;
                if (is_hamiltonian(g)) {
                    added.emplace_back(from3, to3, www);
                    return result();
                }
                // If it STILL doesn't work, remove the third edge
                g[from3][to3] = g[to3][from3] = 0;
            }
            // If it STILLL doesn't work, remove the second edge
            g[from2][to2] = g[to2][from2] = 0;
        }
        // Otherwise, remove the first edge
        g[from1][to1] = g[to1][from1] = 0;
    }
//    assert(is_hamiltonian(g));
    return result();
}
// end snippet hamiltonize
