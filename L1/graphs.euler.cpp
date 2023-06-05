//
// Created by mhq on 16/05/23.
//
#include "graphs.h"
#include <stack>
#include <cassert>
#include <algorithm>
#include <iostream>

using namespace graphs;

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<T>>& g) {
    for (const auto& row : g) {
        for (const auto &elem: row) {
            os << elem << ' ';
        }
        os << '\n';
    }
    return os;
}

adjacency_matrix<> graphs::unoriented(adjacency_matrix<> g) {
    for (Vertex i = 0; i < g.size(); ++i) {
        for (Vertex j = i + 1; j < g.size(); ++j) {
            if (g[i][j] != 0) {
                g[j][i] = g[i][j];
            }
            if (g[j][i] != 0) {
                g[i][j] = g[j][i];
            }
        }
    }
    return g;
}

// ensures upper triangular matrix
adjacency_matrix<> graphs::oriented(adjacency_matrix<> g) {
    for (Vertex i = 0; i < g.size(); ++i) {
        for (Vertex j = i + 1; j < g.size(); ++j) {
            if (g[i][j] != 0) {
                g[j][i] = 0;
                continue;
            }
            // under diagonal
            if (g[j][i] != 0) {
                g[i][j] = g[j][i];
                g[j][i] = 0;
            }
        }
    }
    return g;
}

std::vector<int> graphs::degrees_of(const adjacency_matrix<>& g) {
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

// start snippet is_eulerian
bool graphs::is_eulerian(const adjacency_matrix<>& g) {
    size_t nVertices = g.size();
    assert(nVertices > 2);
    auto degree = degrees_of(unoriented(g));
    return std::all_of(degree.begin(), degree.end(),
                       [](int d) { return d % 2 == 0;});
}
// end snippet is_eulerian

// start snippet euler_cycle
path_t graphs::euler_cycle(adjacency_matrix<> g) {
    size_t nVertices = g.size();
    g = unoriented(g);
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
// end snippet euler_cycle

// start snippet eulerize
graph_change_t graphs::eulerize(const adjacency_matrix<>& g_original) {
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
    size_t nVertices = g_original.size();
    if (nVertices <= 2) {
        // There are 1/2 vertices in the graph! Euler cycle is impossible!
        return {};
    }
    auto g = unoriented(g_original);
    std::vector<edge_t> added, removed;
    std::vector<int> degree;
    std::vector<Vertex> odd_vertices = odd_only(degrees_of(g));

    constexpr auto add = [](decltype(added)& added, decltype(g)& g, Vertex from, Vertex to, int weight = 1) {
        std::tie(from, to) = std::minmax({from, to});
        assert(!g[from][to]);
        assert(from != to);
        g[from][to] = 1;
        g[to][from] = 1;
        added.emplace_back(from, to, weight);
    };
    constexpr auto remove = [](decltype(removed)& removed, decltype(g)& g, Vertex from, Vertex to) {
        std::tie(from, to) = std::minmax({from, to}); // {,} -- see https://stackoverflow.com/a/56739991/9385971
        removed.emplace_back(from, to, g[from][to]);
        g[from][to] = 0;
        g[to][from] = 0;
    };

    // minimal cost matching by cost from given boolean adj matrix

    // step 1. add edges we can add with greed.
    auto used = std::vector<bool>(odd_vertices.size());
    for (size_t i = 0; i < odd_vertices.size(); ++i) {
        if (used[i]) { continue;}
        for (size_t j = i + 1; j < odd_vertices.size(); ++j) {
            if (used[j]) { continue;}
            auto from = odd_vertices[i], to = odd_vertices[j];
            if (g[from][to] == 0) {
                used[i] = used[j] = true;
                add(added, g, from, to);
                break;
            }
        }
    }
    // if used all odd vertexes => nothing else needed
    if (std::all_of(used.begin(), used.end(), [](auto x){return x;})) {
        return {
            .changed = oriented(g),
            .added = added,
            .removed = removed
        };
    }
    // step 2.1. remove matched vertexes
    for (int i = odd_vertices.size() - 1; i >= 0; --i) {
        if (used[i]) {
            odd_vertices.erase(odd_vertices.begin() + static_cast<decltype(odd_vertices)::difference_type>(i));
        }
    }
    // step 2.2. remove edges
    used = std::vector<bool>(odd_vertices.size());
    std::vector<edge_t> cut_edges;
    for (size_t i = 0; i < odd_vertices.size(); ++i) {
        if (used[i]) { continue;}
        for (size_t j = i + 1; j < odd_vertices.size(); ++j) {
            if (used[j]) { continue;}
            auto from = odd_vertices[i], to = odd_vertices[j];
            if (g[from][to] != 0) {
                used[i] = used[j] = true;
                remove(removed, g, from, to);
                // check if there still exists path between `from` and `to`.
                auto dij = min_path_distances_dijkstra(g, from);
                // If not, we have cut the bridge and should fix that!
                if (dij.distances[to] == INF) {
                    cut_edges.push_back(removed.back());
                }
                break; // go to next i iteration
            }
        }
    }
    if (cut_edges.size() > 1) {
        // we have >=3 convex components HOORAY!
        // step 3. choose 1 vertex from each component and join them into cycle.
        assert(("we found several cut edges and didn't manage to connect them greedily on step 1. Awful.", false)); // need to check
    }
    else if (cut_edges.size() == 1) {
        auto [from, to, _] = cut_edges.front();
        auto dij = min_path_distances_dijkstra(g, from);

        // recover that edge (undo removing)
        add(added, g, from, to);
        added.pop_back();
        removed.pop_back();

        // find vertex i in first or second component, and use it to recover connection
        Vertex i;
        for (i = 0; i < g.size(); ++i) {
            if (g[from][i] != 0 && i != to) {
                remove(removed, g, from, i);
                add(added, g, i, to);
                break;
            }
            if (g[to][i] != 0 && i != from) {
                remove(removed, g, to, i);
                add(added, g, i, from);
                break;
            }
        }
        assert(("we did't found such vertex. Awfuuuuul!", i != g.size()));
    }
    assert(is_eulerian(g));
    return {
        .changed = oriented(g),
        .added = added,
        .removed = removed
    };
}
// end snippet eulerize
