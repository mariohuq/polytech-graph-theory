//
// Created by mhq on 13/03/23.
//

#include "polya_dist.h"
#include "graphs.h"

#include <iostream>
#include <cstdlib>
#include <cassert>

using namespace graphs;

std::mt19937 gen{std::random_device{}()};

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

//Д. 1.3. Алгоритм построения случайного ориентированного бесконтурного графа, Хаг
adjacency_matrix<bool> generate_acyclic_convex(size_t nVertices, size_t nEdges) {
    auto dis = polya_1<int>(20, 20, 1, nVertices * (nVertices - 1) / 2 - 1);
    auto split = [](int k) -> div_t {
        int i = static_cast<int>((-1 + sqrt(8 * k + 1)) / 2);
        return {k - i*(i+1)/2, i+1};
    };

    adjacency_matrix<bool> result{nVertices, std::vector<bool>(nVertices)};

    for (int k = 0; k < nEdges; k++) {
        // i < j by construction
        auto [i, j] = split(dis(gen));
        assert(i < j);
        assert(j < nVertices);
        if (result[i][j]) {
            k--;
            continue;
        }
        result[i][j] = true;
    }
    return result;
}

void max_flow_test();
void min_max_flow_test();
void mst_test();
void mst2_test();
void spanning_trees_count_test();

int main() {
//    max_flow_test();
//min_max_flow_test();
//    mst2_test();
    spanning_trees_count_test();
    return 0;
}

void test_1_vertex() {
    auto g = generate(1, gen);
}

void test_2_vertexes() {
    auto g = generate(2, gen);
    std::cout << g;
}

void sample_usage() {
    size_t n = 10;
    auto degs = out_degrees(n, gen);
    for (auto d: degs) {
        std::cout << d << ' ';
    }
    std::cout << '\n';
    auto g = from_degrees(degs, gen);
    std::cout << g << '\n';
    auto g3 = min_path_lengths(g, 3);
    std::cout << g3;
    auto count = count_paths(g,0);
    size_t to;
    while (std::cin >> to) {
        std::cout << count(to);
    }
}

void max_flow_test() {
    {
        auto g = flow_graph_t{
            .capacity{
                {0, 4},
                {0, 0},
            },
            .source=0,
            .sink=1
        };
        auto [max_flow, _] = max_flow_ford_fulkerson(g);
        assert(4 == max_flow);
    }
    {
        auto g = flow_graph_t{
            .capacity{
                {0,5,6,0,0},
                {0,0,0,4,1},
                {0,0,0,1,5},
                {0,0,0,0,1},
                {0,0,0,0,0},
            },
            .source=0,
            .sink=4,
        };
        auto [max_flow, _] = max_flow_ford_fulkerson(g);
        assert(7 == max_flow);
    }
    {
        auto capacity = generate(6, gen);
        auto cost = generate_costs(capacity, gen);
        auto g = add_supersource_supersink(capacity, cost);
        auto [max_flow, flow] = max_flow_ford_fulkerson(g);
        std::cout << max_flow << '\n';
        std::cout << "Flow:\n";
        std::cout << flow;
    }
}

void min_max_flow_test() {
    auto g = flow_graph_t{
        .capacity{
            {0,5,6,0,0},
            {0,0,0,4,1},
            {0,0,0,1,5},
            {0,0,0,0,1},
            {0,0,0,0,0},
        },
        .source=0,
        .sink=4,
    };
    g.cost = generate_costs(g.capacity, gen);
    std::cout << "Capacities:\n" << g.capacity << '\n';
    std::cout << "Costs:\n" << g.cost << '\n';
    auto [flow_sum, cost, flow] = min_cost_flow(g, 7 * 2 / 3);
    std::cout << "Cost = "<< cost << '\n';
    std::cout << "Flow:\n" << flow << '\n';
}

std::ostream& operator<<(std::ostream& os, const graphs::edge_t& e) {
    return os << "{" << e.from << " → " << e.to << ": " << e.weight << "}";
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& s) {
    for (const auto &elem: s) {
        os << elem << ' ';
    }
    return os;
}

void mst_test() {
    auto g = adjacency_matrix<>{
        {0, 9, 3, 4},
        {9, 0, 2, 0},
        {3, 2, 0, 1},
        {4, 0, 1, 0},
    };
    std::cout << "Kruskal.\n";
    {
        auto [spanning_tree, cost, iterations] = kruskal_mst(g);
        std::cout << "tree: " << spanning_tree << "\n"
            << "cost: " << cost << "\n"
            << "it: " << iterations;
    }
    std::cout << "Prim.\n";
    {
        auto [spanning_tree, cost, iterations] = prim_mst(g);
        std::cout << "tree: " << spanning_tree << "\n"
            << "cost: " << cost << "\n"
            << "it: " << iterations;
    }
}

void mst2_test() {
    auto g = adjacency_matrix<>{
        {0, 9, 3, 4},
        {0, 0, 2, 0},
        {0, 0, 0, 1},
        {0, 0, 0, 0},
    };
    std::cout << "Kruskal.\n";
    {
        auto [spanning_tree, cost, iterations] = kruskal_mst(g);
        std::cout << "tree: " << spanning_tree << "\n"
            << "cost: " << cost << "\n"
            << "it: " << iterations;
    }
    std::cout << "Prim.\n";
    {
        auto [spanning_tree, cost, iterations] = prim_mst(g);
        std::cout << "tree: " << spanning_tree << "\n"
            << "cost: " << cost << "\n"
            << "it: " << iterations;
    }
}

void spanning_trees_count_test() {
    auto g = adjacency_matrix<>{
        {0, 9, 3, 4},
        {0, 0, 2, 0},
        {0, 0, 0, 1},
        {0, 0, 0, 0},
    };
    std::cout << "STC = " << spanning_trees_count(g) << "\n";
}