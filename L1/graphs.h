//
// Created by mhq on 25/03/23.
//
#pragma once
#include "polya_dist.h"
#include <vector>
#include <cstddef>

namespace graphs {
    template<typename T=size_t>
    using adjacency_matrix = std::vector<std::vector<T>>;

    using Vertex = size_t;

    adjacency_matrix<size_t> generate(size_t nVertices, std::mt19937& gen);

    std::vector<size_t> out_degrees(size_t nVertices, std::mt19937& gen);

    adjacency_matrix<size_t> from_degrees(std::vector<size_t> vertex_degrees, std::mt19937& gen);

    adjacency_matrix<size_t> min_path_lengths(const adjacency_matrix<size_t>& that, size_t path_length);

    adjacency_matrix<size_t> max_path_lengths(const adjacency_matrix<size_t>& that, size_t path_length);

    // https://neerc.ifmo.ru/wiki/index.php?title=Задача_о_числе_путей_в_ациклическом_графе
    struct count_paths {
        count_paths(const adjacency_matrix<size_t>& graph, Vertex start)
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
        adjacency_matrix<size_t> graph;
        Vertex start; // стартовая вершина
        std::vector<bool> visited; // visited[v] = true, если ответ для вершины v уже посчитан
        std::vector<size_t> d; // d[v] — число путей из вершины `start` до вершины v
    };

    // lab2
    struct dijkstra_result_t {
        // array of path lengths from given vertex to all other
        std::vector<int> distances;
        // array of vertexes,
        // precedents[i] is immediately before vertex `i` in the shortest path from start vertex to `i`
        std::vector<Vertex> precedents;
        // iterations count
        size_t iterations;
    };

    constexpr auto INF = INT32_MAX;
    constexpr Vertex NO_VERTEX = -1u;

    dijkstra_result_t min_path_distances_dijkstra(const adjacency_matrix<int>& g, Vertex start_vertex);

    dijkstra_result_t min_path_distances_bellman_ford(const adjacency_matrix<int>& g, Vertex start_vertex);

    std::vector<Vertex> reconstruct_path(const std::vector<Vertex>& precedents, Vertex from, Vertex to);
}
