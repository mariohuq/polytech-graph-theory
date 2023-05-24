//1
// Created by mhq on 20/05/23.
//
#include "graphs.h"
#include <algorithm>
#include <iostream>

using namespace graphs;

bool graphs::is_hamiltonian(adjacency_matrix<> g) {
    if (g.size() <= 2) {
        return false;
    }
    g = unoriented(g);
    auto ds = degrees_of(g);
    // Dirac's Theorem
    if (g.size() > 3 && std::all_of(ds.begin(), ds.end(),
                                             [&](auto d){ return 2 * d >= g.size(); })) {
        return true;
    }
    // necessary condition (easy)
    if (std::any_of(ds.begin(), ds.end(),
                                             [&](auto d){ return d <= 1; })) {
        return false;
    }
    hamilton_cycles hc{g};
    return hc().exists();
}

const Vertex graphs::hamilton_cycles::start = 0;

costed_path_t graphs::hamilton_cycles::operator()() {
    static constexpr auto cost = [](decltype(g)& g, decltype(candidate)& candidate) {
        size_t result = 0;
        for (size_t i = 1; i < candidate.size(); ++i) {
            result += g[candidate[i-1]][candidate[i]];
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
