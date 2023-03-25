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

    std::vector<size_t> out_degrees(size_t nVertices, std::mt19937& gen);

    adjacency_matrix<size_t> from_degrees(std::vector<size_t> vertex_degrees, std::mt19937& gen);

    adjacency_matrix<size_t> min_path_lengths(const adjacency_matrix<size_t>& that, size_t path_length);

    adjacency_matrix<size_t> max_path_lengths(const adjacency_matrix<size_t>& that, size_t path_length);

    // https://neerc.ifmo.ru/wiki/index.php?title=Задача_о_числе_путей_в_ациклическом_графе
    struct count_paths {
        count_paths(const adjacency_matrix<size_t>& graph, size_t start)
            : graph{graph}
            , start{start}
            , visited(graph.size())
            , d(graph.size())
        {
            visited[start] = true;
            d[start] = 1;
        }
        size_t operator()(size_t v);
    private:
        adjacency_matrix<size_t> graph;
        size_t start; // стартовая вершина
        std::vector<bool> visited; // visited[v] = true, если ответ для вершины v уже посчитан
        std::vector<size_t> d; // d[v] — число путей из вершины `start` до вершины v
    };

}
