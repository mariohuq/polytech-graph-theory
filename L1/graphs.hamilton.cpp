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
    return hc.begin() != hc.end();
}
// end snippet is_hamiltonian

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

    int zero = (int)added.size();

    auto iterate = [&](auto& that, int level) {
        if (level == 0) {
            return is_hamiltonian(g);
        }
        for (auto e : no_edges) {
            if (std::find(added.begin() + zero, added.end(), e) != added.end()) {
                continue;
            }
            // Try with edge e
            g[e.from][e.to] = g[e.to][e.from] = e.weight;
            added.push_back(e);
            if (that(that, level - 1)) {
                return true;
            }
            // If it still doesn't work, remove edge e
            g[e.from][e.to] = g[e.to][e.from] = 0;
            added.pop_back();
        }
        return false;
    };
    for (int k = 1; k <= 3; ++k) {
        if (iterate(iterate, k)) {
            return result();
        }
    }
//    assert(is_hamiltonian(g));
    return result();
}
// end snippet hamiltonize

const Vertex graphs::hamilton_cycles::start = 0;

hamilton_cycles::iterator hamilton_cycles::begin() {
    // инициализация поиска гамильтоновых циклов
    auto it = iterator{this};
    it.N.resize(g.size());
    for (Vertex i = 0; i < g.size(); ++i) {
        it.N[i] = adj(i);
    }
    it.candidate.push_back(start);
    return ++it;
}

std::deque<Vertex> hamilton_cycles::adj(Vertex x) const {
    std::deque<Vertex> res;
    for (Vertex i = 0; i < g.size(); ++i) {
        if (g[x][i] == 0) {
            continue;
        }
        res.push_back(i);
    }
    return res;
}


// start snippet hamilton_cycles_op
costed_path_t hamilton_cycles::iterator::find_next() {
    while (!candidate.empty()) {
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
        N[y] = container->adj(y);
        if (candidate.size() != container->g.size()) {
            continue;
        }
        if (std::find(N[y].begin(), N[y].end(), start) == N[y].end()) {
            candidate.pop_back();
            continue;
        }
        auto res = candidate;
        candidate.pop_back();
        res.push_back(start);
        return {res, cost(container->g, res)};
    }
    return {};
}
// end snippet hamilton_cycles_op

size_t hamilton_cycles::iterator::cost(adjacency_matrix<> &g, path_t &cand) {
    size_t x = 0;
    for (size_t i = 1; i < cand.size(); ++i) {
        x += g[cand[i - 1]][cand[i]];
    }
    return x + g[cand.front()][cand.back()];
}
