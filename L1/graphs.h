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
    // start snippet count_paths
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
    // end snippet count_paths

    // lab2

    // - array of path lengths from given vertex to all other
    // - array of vertexes,
    // precedents[i] is immediately before vertex `i` in the shortest path from start vertex to `i`;
    //               is NO_VERTEX if no path exists
    // - iterations count

    // start snippet dijkstra_result_t
    struct dijkstra_result_t {
        // массив длин путей от данной вершины ко всем остальным
        std::vector<int> distances;
        // массив вершин, precedents[i] непосредственно перед вершиной i в кратчайшем пути от начальной вершины до i
        // NO_VERTEX если такого пути нет.
        std::vector<Vertex> precedents;
        // количество итераций
        size_t iterations;
    };
    // end snippet dijkstra_result_t

    constexpr auto INF = INT32_MAX;
    constexpr Vertex NO_VERTEX = -1u;

    // https://github.com/okwedook/olymp/blob/master/code/graph/Dijkstra.hpp
    // поиск кратчайших путей от данной вершины `start_vertex` ко всем остальным алгоритмом Дейкстры
    dijkstra_result_t min_path_distances_dijkstra(const adjacency_matrix<>& g, Vertex start_vertex);

    // поиск кратчайших путей от данной вершины `start_vertex` ко всем остальным алгоритмом Беллмана-Форда
    dijkstra_result_t min_path_distances_bellman_ford(const adjacency_matrix<>& g, Vertex start_vertex);

    // - matrix of path lengths: `distances[i][j]` is min path from vertex `i` to `j`
    // - matrix of vertexes,
    // precedents[i][j] is immediately before vertex `j` in the shortest path from `i` to `j`;
    //                  is NO_VERTEX if no path exists
    // - iterations count

    // start snippet floyd_warshall_result_t
    struct floyd_warshall_result_t {
        // матрица длин путей между всеми парами вершин
        // `distances[i][j]` - длина кратчайшего пути от `i` к `j`
        std::vector<std::vector<int>> distances;
        // матрица вершин,
        // precedents[i][j] непосредственно предшествует `j` в кратчайшем пути от `i` к `j`;
        //                  =NO_VERTEX если пути нет
        std::vector<std::vector<Vertex>> precedents;
        // количество итераций
        size_t iterations;
    };
    // end snippet floyd_warshall_result_t
    
    // поиск кратчайших путей от каждой вершины к каждой.
    floyd_warshall_result_t min_path_distances_floyd_warshall(const adjacency_matrix<>& g);

    // восстановление пути по вектору предшествования от вершины от `from` к `to`;
    path_t reconstruct_path(const std::vector<Vertex>& precedents, Vertex from, Vertex to);

    // lab3

    // start snippet flow_graph_t
    struct flow_graph_t {
        // матрица пропускных способностей
        adjacency_matrix<> capacity;
        // матрица стоимостей
        // 0 значит ∞ стоимость
        adjacency_matrix<> cost;
        // вершина источник
        Vertex source;
        // вершина сток
        Vertex sink;
    };
    // end snippet flow_graph_t

    // g: weight edge matrix
    // returns: random costs matrix y, such that
    // g[i][j] == 0 ⇔ y[i][j] == 0 ∀i,j

    // случайно генерирует матрицу стоимостей на основе матрицы весов
    // причем g[i][j] == 0 ⇔ y[i][j] == 0 ∀i,j
    adjacency_matrix<> generate_costs(const adjacency_matrix<>& g, std::mt19937& gen);

    // adds to the graph supersource and supersink with ∞ edge capacities if needed,
    // returns new matrix and supersource & supersink vertexes

    // добавить к графу фиктивный исток и сток с бесконечной пропускной способностью
    // если стоков (истоков) несколько
    // возвращает новую матрицу и номера вершин истока и стока в новой матрице.
    flow_graph_t add_supersource_supersink(const adjacency_matrix<>& capacity, const adjacency_matrix<>& cost);

    // start snippet flow_result_t
    struct flow_result_t {
        int max_flow; // величина потока
        adjacency_matrix<> flow; // матрица потока
    };
    // end snippet flow_result_t

    // find maximim flow of given capacities matrix. Ignores g.cost.
    // найти максимальный поток по данной матрице пропускных способностей (игнорируя стоимости).
    flow_result_t max_flow_ford_fulkerson(const flow_graph_t& g);

    // start snippet min_cost_flow_result_t
    struct min_cost_flow_result_t {
        int flow_sum; // величина потока
        int cost; // стоимость
        adjacency_matrix<> flow; // матрица потока
    };
    // end snippet min_cost_flow_result_t

    // найти поток заданной стоимости с минимальной стоимостью в графе
    min_cost_flow_result_t
    min_cost_flow(const flow_graph_t& g, int desired_flow);

    // lab4

    // start snippet edge_t
    struct edge_t {
        Vertex from;
        Vertex to;
        int weight;

        // сравнение ребер по весу
        bool operator<(const edge_t& rhs) const {
            return std::tie(weight, from, to) < std::tie(rhs.weight, rhs.from, rhs.to);
        }
        bool operator==(const edge_t& rhs) const {
            return std::tie(from, to) == std::tie(rhs.from, rhs.to);
        }

        edge_t(Vertex from, Vertex to, int weight) : from(from), to(to), weight(weight) {}
    };
    // end snippet edge_t

    // start snippet min_st_result_t
    struct min_st_result_t {
        std::set<edge_t> spanning_tree;
        // sum of costs of edges
        size_t cost;
        // iterations count
        size_t iterations;
    };
    // end snippet min_st_result_t

    // найти минимальное остовное дерево алгоритмом Краскала
    // dsu_t: сжатие пути + ранговая эвристика
    // http://e-maxx.ru/algo/dsu
    min_st_result_t kruskal_mst(const adjacency_matrix<>& g);

    // найти минимальное остовное дерево алгоритмом Прима
    min_st_result_t prim_mst(const adjacency_matrix<>& g);

    // подсчитать количество остовных деревьев согласно теореме Кирхгофа
    size_t spanning_trees_count(const adjacency_matrix<>& g);

    namespace pruefer {
        // https://cp-algorithms.com/graph/pruefer_code.html#building-the-prufer-code-for-a-given-tree
        // g -> min spanning tree -> pruefer code with weights
        std::pair<std::vector<Vertex>, std::vector<int>>
        encode(const adjacency_matrix<>& g);

        // https://cp-algorithms.com/graph/pruefer_code.html#restoring-the-tree-using-the-prufer-code
        // generates upper triangular matrix. Each edge appears once in the matrix
        adjacency_matrix<>
        decode(const std::vector<Vertex>& code, const std::vector<int>& weights);
    }

    // lab5

    // является ли граф эйлеровым (неориентированный, полученный забыванием направлений дуг)
    bool is_eulerian(const adjacency_matrix<>& g);

    // start snippet graph_change_t
    struct graph_change_t {
        adjacency_matrix<> changed; // матрица смежности модифицированного графа
        std::vector<edge_t> added; // добавленные ребра
        std::vector<edge_t> removed; // удаленные ребра
        bool has_changed() const { return !added.empty() || !removed.empty(); }
    };
    // end snippet graph_change_t

    // модифицировать граф до эйлерова
    // std::tie = {,}... -- see https://stackoverflow.com/a/56739991/9385971
    graph_change_t eulerize(const adjacency_matrix<>& g_original);

    // получить неориентированный граф, полученный забыванием направлений дуг графа
    adjacency_matrix<> unoriented(adjacency_matrix<> g);
    // получить ориентированный граф с верхнетреугольной матрицей смежности из данного
    adjacency_matrix<> oriented(adjacency_matrix<> g);
    // найти степени вершин графа (выходные степени если ориетированный)
    std::vector<int> degrees_of(const adjacency_matrix<>& g);

    // makes euler cycle assuming graph is eulerian
    // найти эйлеров цикл, в предположении что граф эйлеров
    path_t euler_cycle(adjacency_matrix<> g);

    // проверить, что граф является гамильтоновым
    bool is_hamiltonian(adjacency_matrix<> g);

    // start snippet costed_path_t
    struct costed_path_t {
        path_t path;
        // суммарный вес пути
        size_t cost;
        // непустой ли путь
        bool exists() const {
            return !path.empty();
        }
        // сравнение путей по их весу
        bool operator<(const costed_path_t& rhs) const {
            return cost < rhs.cost;
        }
    };
    // end snippet costed_path_t

    // start snippet hamilton_cycles
    struct hamilton_cycles {
        // инициализация поиска гамильтоновых циклов
        explicit hamilton_cycles(const adjacency_matrix<> &g) : g{unoriented(g)} {
        }

        struct iterator {
            using iterator_category = std::input_iterator_tag;
            using value_type = costed_path_t;
            using difference_type = std::ptrdiff_t;
            using pointer = costed_path_t*;
            using reference = costed_path_t&;

            std::vector<std::deque<Vertex>> N;
            hamilton_cycles* container;
            path_t candidate;
            costed_path_t result;

            explicit iterator(hamilton_cycles* container) : container{container} { }
            bool operator==(const iterator& rhs) const { return candidate == rhs.candidate; }
            bool operator!=(const iterator& rhs) const { return !(*this == rhs); }
            costed_path_t operator*() const { return result; }
            iterator& operator++() {
                result = find_next();
                return *this;
            }

        private:
            // стоимость пути
            static size_t cost(adjacency_matrix<> &g, decltype(candidate) &cand);
            // найти следующий гамильтонов цикл
            costed_path_t find_next();
        };

        iterator begin();
        iterator end() { return iterator{this}; }

    private:
        // начальная вершина
        static const Vertex start;
        // матрица смежности
        adjacency_matrix<> g;
        // номера вершин смежных c `x`
        std::deque<Vertex> adj(Vertex x) const;
    };
    // end snippet hamilton_cycles

    // дополнить граф ребрами до гамильтонова
    graph_change_t hamiltonize(const adjacency_matrix<>& g);
}
