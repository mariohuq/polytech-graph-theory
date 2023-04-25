//
// Created by mhq on 25/03/23.
//
#pragma once
#include "polya_dist.h"
#include <vector>
#include <cstddef>

namespace graphs {
    template<typename T=int>
    using adjacency_matrix = std::vector<std::vector<T>>;

    using Vertex = size_t;

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

    std::vector<Vertex> reconstruct_path(const std::vector<Vertex>& precedents, Vertex from, Vertex to);

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
        int cost;
        adjacency_matrix<> flow;
    };

    min_cost_flow_result_t
    min_cost_flow(const flow_graph_t& g, int desired_flow);
}
