//
// Created by mhq on 25/03/23.
//
#pragma once
#include "polya_dist.h"
#include <vector>
#include <set>
#include <cstddef>
#include <deque>

namespace graphs {
    template<typename T=int>
    using adjacency_matrix = std::vector<std::vector<T>>;

    using Vertex = size_t;

    using path_t = std::vector<Vertex>;

    adjacency_matrix<> generate(size_t nVertices, std::mt19937& gen);

    std::vector<size_t> out_degrees(size_t nVertices, std::mt19937& gen);

    adjacency_matrix<> from_degrees(std::vector<size_t> vertex_degrees, std::mt19937& gen);

    adjacency_matrix<> min_path_lengths(const adjacency_matrix<>& that, size_t path_length);

    adjacency_matrix<> max_path_lengths(const adjacency_matrix<>& that, size_t path_length);

    // https://neerc.ifmo.ru/wiki/index.php?title=Задача_о_числе_путей_в_ациклическом_графе
    struct count_paths {
        count_paths(const adjacency_matrix<>& graph, Vertex start)
            : graph{graph}
            , start{start}
            , visited(graph.size())
            , d(graph.size())
        {
            visited[start] = true;
            d[start] = 1;
        }
        size_t operator()(Vertex v);
    private:
        adjacency_matrix<> graph;
        Vertex start; // стартовая вершина
        std::vector<bool> visited; // visited[v] = true, если ответ для вершины v уже посчитан
        std::vector<size_t> d; // d[v] — число путей из вершины `start` до вершины v
    };

    // lab2
    struct dijkstra_result_t {
        // array of path lengths from given vertex to all other
        std::vector<int> distances;
        // array of vertexes,
        // precedents[i] is immediately before vertex `i` in the shortest path from start vertex to `i`;
        //               is NO_VERTEX if no path exists
        std::vector<Vertex> precedents;
        // iterations count
        size_t iterations;
    };

    constexpr auto INF = INT32_MAX;
    constexpr Vertex NO_VERTEX = -1u;

    dijkstra_result_t min_path_distances_dijkstra(const adjacency_matrix<>& g, Vertex start_vertex);

    dijkstra_result_t min_path_distances_bellman_ford(const adjacency_matrix<>& g, Vertex start_vertex);

    struct floyd_warshall_result_t {
        // matrix of path lengths: `distances[i][j]` is min path from vertex `i` to `j`
        std::vector<std::vector<int>> distances;
        // matrix of vertexes,
        // precedents[i][j] is immediately before vertex `j` in the shortest path from `i` to `j`;
        //                  is NO_VERTEX if no path exists
        std::vector<std::vector<Vertex>> precedents;
        // iterations count
        size_t iterations;
    };

    floyd_warshall_result_t min_path_distances_floyd_warshall(const adjacency_matrix<>& g);

    path_t reconstruct_path(const std::vector<Vertex>& precedents, Vertex from, Vertex to);

    // lab3

    struct flow_graph_t {
        adjacency_matrix<> capacity;
        adjacency_matrix<> cost; // 0 means ∞ cost
        Vertex source;
        Vertex sink;
    };

    // g: weight edge matrix
    // returns: random costs matrix y, such that
    // g[i][j] == 0 ⇔ y[i][j] == 0 ∀i,j
    adjacency_matrix<> generate_costs(const adjacency_matrix<>& g, std::mt19937& gen);

    // adds to the graph supersource and supersink with ∞ edge capacities if needed,
    // returns new matrix and supersource & supersink vertexes
    flow_graph_t add_supersource_supersink(const adjacency_matrix<>& capacity, const adjacency_matrix<>& cost);

    struct flow_result_t {
        int max_flow;
        adjacency_matrix<> flow;
    };

    // find maximim flow of given capacities matrix. Ignores g.cost.
    flow_result_t max_flow_ford_fulkerson(const flow_graph_t& g);

    struct min_cost_flow_result_t {
        int flow_sum;
        int cost;
        adjacency_matrix<> flow;
    };

    min_cost_flow_result_t
    min_cost_flow(const flow_graph_t& g, int desired_flow);

    // lab4

    struct edge_t {
        Vertex from;
        Vertex to;
        int weight;

        bool operator<(const edge_t& rhs) const {
            return std::tie(weight, from, to) < std::tie(rhs.weight, rhs.from, rhs.to);
        }

        edge_t(Vertex from, Vertex to, int weight) : from(from), to(to), weight(weight) {}
    };

    struct min_st_result_t {
        std::set<edge_t> spanning_tree;
        // sum of costs of edges
        size_t cost;
        // iterations count
        size_t iterations;
    };

    min_st_result_t kruskal_mst(const adjacency_matrix<>& g);

    min_st_result_t prim_mst(const adjacency_matrix<>& g);

    size_t spanning_trees_count(const adjacency_matrix<>& g);

    namespace pruefer {
        // g -> min spanning tree -> pruefer code with weights
        std::pair<std::vector<Vertex>, std::vector<int>>
        encode(const adjacency_matrix<>& g);

        // generates upper triangular matrix. Each edge appears once in the matrix
        adjacency_matrix<>
        decode(const std::vector<Vertex>& code, const std::vector<int>& weights);
    }

    // lab5

    bool is_eulerian(const adjacency_matrix<>& g);

    struct graph_change_t {
        adjacency_matrix<> changed;
        std::vector<edge_t> added;
        std::vector<edge_t> removed;
        bool has_changed() const { return !added.empty() || !removed.empty(); }
    };

    graph_change_t eulerize(const adjacency_matrix<>& g_original);

    adjacency_matrix<> unoriented(adjacency_matrix<> g);
    adjacency_matrix<> oriented(adjacency_matrix<> g);

    // makes euler cycle assuming graph is eulerian
    path_t euler_cycle(adjacency_matrix<> g);

    std::vector<int> degrees_of(const adjacency_matrix<>& g);

    bool is_hamiltonian(adjacency_matrix<> g);

    struct costed_path_t {
        path_t path;
        size_t cost;
        bool exists() const {
            return !path.empty();
        }
        bool operator<(const costed_path_t& rhs) const {
            return cost < rhs.cost;
        }
    };

    struct hamilton_cycles {
        hamilton_cycles(const adjacency_matrix<> &g) : g{unoriented(g)}, N(g.size()) {
            for (Vertex i = 0; i < g.size(); ++i) {
                N[i] = adj(i);
            }
            candidate.push_back(start);
        }
        costed_path_t operator()(); // next hamilton cycle
        bool has_next() { // true for last iteration. What to do?
            return !candidate.empty();
        }
    private:
        static const Vertex start;
        adjacency_matrix<> g;
        std::vector<std::deque<Vertex>> N;
        path_t candidate;
        std::deque<Vertex> adj(Vertex x);
    };

    graph_change_t hamiltonize(const adjacency_matrix<>& g);
}
