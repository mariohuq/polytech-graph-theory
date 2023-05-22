//
// Created by mhq on 20/05/23.
//
#include "graphs.h"
#include <algorithm>

using namespace graphs;

bool graphs::is_hamiltonian(const adjacency_matrix<> &g) {
    if (g.size() <= 2) {
        return false;
    }
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
    // if !convex ->  true
    // ???
    return false;
}