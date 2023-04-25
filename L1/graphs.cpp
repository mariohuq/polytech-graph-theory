//
// Created by mhq on 25/03/23.
//
#include "graphs.h"
#include <algorithm>
#include <cassert>
#include <queue>
#include <utility>
#include <map>
#include <set>

using namespace graphs;

template<typename Func>
adjacency_matrix<>
matrix_multiply(const adjacency_matrix<> &lhs, const adjacency_matrix<> &rhs, Func extrem);

// матрица m[i][j] из длин минимальных (или максимальных) путей от вершины i до j
template<typename Func>
adjacency_matrix<> matrix_power_shimbell(const adjacency_matrix<> &that, size_t power, Func extrem);


std::vector<size_t> graphs::out_degrees(size_t nVertices, std::mt19937 &gen) {
    if (nVertices == 0) {
        return std::vector<size_t>{};
    }
    if (nVertices == 1) {
        return std::vector<size_t>{0};
    }
    // степень вершины может быть в диапазоне [1 .. n-1]
    // распределение выдает числа в диапазоне [0 .. n-2]
    auto dis = polya_1<int>(2, 20, 1, static_cast<int>(nVertices) - 3);
    std::vector<size_t> result(nVertices);
    for (size_t &x: result) {
        x = dis(gen) + 1;
    }
    std::sort(result.rbegin(), result.rend());

    // чтобы обеспечить отсутствие циклов
    for (size_t i = 0; i < nVertices; ++i) {
        result[i] = std::min(nVertices - i - 1, result[i]);
    }
    return result;
}

adjacency_matrix<> graphs::from_degrees(std::vector<size_t> vertex_degrees, std::mt19937 &gen) {
    auto nVertices = vertex_degrees.size();
    adjacency_matrix<> result(nVertices, std::vector<int>(nVertices));
    auto dis = polya_1<size_t>(4, 8, 3, 50 - 1);
    for (int i = 0; i < nVertices; ++i) {
        for (int j = i + 1; j < vertex_degrees[i] + i + 1; ++j) {
            result[i][j] = dis(gen) + 1;
            assert(result[i][j] > 0);
        }
        if (vertex_degrees[i] == nVertices - i - 1) {
            continue;
        }
        // degrees[i] of ones, other are zeros.
        // nVertices - i - 1 total
        std::shuffle(result[i].begin() + i + 1, result[i].end(), gen);
    }
    return result;
}

adjacency_matrix<> graphs::min_path_lengths(const adjacency_matrix<> &that, size_t path_length) {
    if (that.empty()) {
        return {};
    }
    const size_t &(*fn)(const unsigned long &, const unsigned long &) = &std::min<size_t>;
    return matrix_power_shimbell(that, path_length, fn);
}

adjacency_matrix<> graphs::max_path_lengths(const adjacency_matrix<> &that, size_t path_length) {
    if (that.empty()) {
        return {};
    }
    const size_t &(*fn)(const unsigned long &, const unsigned long &) = &std::max<size_t>;
    return matrix_power_shimbell(that, path_length, fn);
}

adjacency_matrix<> graphs::generate(size_t nVertices, std::mt19937 &gen) {
    return from_degrees(out_degrees(nVertices, gen), gen);
}

dijkstra_result_t
graphs::min_path_distances_dijkstra(const adjacency_matrix<> &g, Vertex start_vertex) {
    // https://github.com/okwedook/olymp/blob/master/code/graph/Dijkstra.hpp
    std::vector<Vertex> precedents(g.size(), NO_VERTEX);
    std::vector<bool> visited(g.size(), false);
    size_t iterations{};

    std::vector<int> distances(g.size(), INF);
    auto cmp = [&](Vertex i, Vertex j) {
        return distances[i] > distances[j];
    };
    std::priority_queue<Vertex, std::vector<Vertex>, decltype(cmp)> qu(cmp);
    distances[start_vertex] = 0;
    qu.push(start_vertex);
    while (!qu.empty()) {
        auto nearest_vertex = qu.top();
        qu.pop();
        if (visited[nearest_vertex]) {
            continue;
        }
        visited[nearest_vertex] = true;
        for (Vertex v{}; v < g.size(); ++v) {
            if (g[nearest_vertex][v] == 0) {
                continue;
            }
            auto distance_via_nearest = distances[nearest_vertex] + g[nearest_vertex][v];
            if (distance_via_nearest < distances[v]) {
                distances[v] = distance_via_nearest;
                qu.push(v);
                visited[v] = false;
                precedents[v] = nearest_vertex;
            }
            iterations++;
        }
    }
    return {
        .distances = distances,
        .precedents = precedents,
        .iterations = iterations
    };
}

dijkstra_result_t
graphs::min_path_distances_bellman_ford(const adjacency_matrix<> &g, Vertex start_vertex) {
    std::vector<Vertex> precedents(g.size(), NO_VERTEX);
    size_t iterations{};

    std::vector<int> distances(g.size(), INF);
    distances[start_vertex] = 0;

    for (int _ = 1; _ < g.size(); ++_) {
        for (Vertex u{}; u < g.size(); ++u) {
            for (Vertex v{}; v < g.size(); ++v) {
                if (g[u][v] == 0) {
                    continue;
                }
                auto candidate = distances[u] + g[u][v];
                if (distances[u] != INF && candidate < distances[v]) {
                    // новый путь короче
                    distances[v] = candidate;
                    precedents[v] = u;
                }
                iterations++;
            }
        }
    }
    // проверка на отрицательные контуры
    for (Vertex u{}; u < g.size(); ++u) {
        for (Vertex v{}; v < g.size(); ++v) {
            if (g[u][v] == 0) {
                continue;
            }
            // найден контур отрицательного веса
            if (distances[u] != INF && distances[v] > distances[u] + g[u][v]) {
                return {
                    .iterations = iterations
                };
            }
        }
    }

    return {
        .distances = distances,
        .precedents = precedents,
        .iterations = iterations
    };
}

floyd_warshall_result_t
graphs::min_path_distances_floyd_warshall(const adjacency_matrix<> &g) {
    std::vector<std::vector<Vertex>> precedents(g.size(), std::vector<Vertex>(g.size(), NO_VERTEX));
    size_t iterations{};
    std::vector<std::vector<int>> distances(g.size(), std::vector<int>(g.size(), INF));

    for (Vertex i{}; i < g.size(); ++i) {
        for (Vertex j{}; j < g.size(); ++j) {
            if (i == j) {
                distances[i][j] = 0;
            }
            if (g[i][j] == 0) {
                continue;
            }
            distances[i][j] = g[i][j];
            precedents[i][j] = i;
        }
    }

    for (Vertex i{}; i < g.size(); ++i) {
        for (Vertex j{}; j < g.size(); ++j) {
            if (j == i || distances[j][i] == INF) {
                continue;
            }
            for (Vertex k{}; k < g.size(); ++k) {
                iterations++;
                if (k == i || distances[i][k] == INF) {
                    continue;
                }
                auto candidate = distances[j][i] + distances[i][k];
                if (distances[j][k] == INF || distances[j][k] > candidate) {
                    distances[j][k] = candidate;
                    precedents[j][k] = i;
                }
            }
        }
        for (Vertex j{}; j < g.size(); ++j) {
            if (distances[j][j] < 0) {
                // Узел `j` входит в отрицательный контур
                return {
                    .iterations = iterations
                };
            }
        }
    }

    return {
        .distances = distances,
        .precedents = precedents,
        .iterations = iterations
    };
}

std::vector<Vertex>
graphs::reconstruct_path(const std::vector<Vertex> &precedents, Vertex from, Vertex to) {
    std::vector<Vertex> result;
    result.reserve(precedents.size());
    result.push_back(to);
    while (to != from && precedents[to] != NO_VERTEX) {
        to = precedents[to];
        result.push_back(to);
    }
    if (to != from) {
        return {};
    }
    std::reverse(result.begin(), result.end());
    return result;
}

adjacency_matrix<> graphs::generate_costs(const adjacency_matrix<> &g, std::mt19937 &gen) {
    auto dis = polya_1<int>(4, 8, 3, 30 - 1);
    auto copy = g;
    for (auto &row: copy) {
        for (auto &item: row) {
            if (item == 0) {
                continue;
            }
            item = 1 + dis(gen);
            assert(item > 0);
        }
    }
    return copy;
}

flow_graph_t graphs::add_supersource_supersink(const adjacency_matrix<> &capacity, const adjacency_matrix<> &cost) {
    // identify sources and sinks
    std::vector<bool> is_source(capacity.size(), true);
    std::vector<bool> is_sink(capacity.size(), true);
    for (Vertex i{}; i < capacity.size(); ++i) {
        for (Vertex j{}; j < capacity.size(); ++j) {
            if (capacity[i][j] != 0) {
                is_source[j] = false;
                is_sink[i] = false;
            }
        }
    }
    std::vector<Vertex> sources;
    std::vector<Vertex> sinks;
    for (Vertex i{}; i < capacity.size(); ++i) {
        if (is_sink[i]) {
            sinks.push_back(i);
            continue;
        }
        if (is_source[i]) {
            sources.push_back(i);
        }
    }
    // fix matrices if needed
    flow_graph_t result{
        .capacity = capacity,
        .cost = cost,
    };

    assert(!sources.empty());
    if (sources.size() == 1) {
        result.source = sources.front();
    } else {
        result.source = capacity.size();
        for (auto &row: result.cost) {
            row.emplace_back();
        }
        for (auto &row: result.capacity) {
            row.emplace_back();
        }
        result.cost.emplace_back(result.source + 1, 0u);
        result.capacity.emplace_back(result.source + 1, 0u);
        for (auto v: sources) {
            result.capacity[result.source][v] = INF;
        }
    }

    assert(!sinks.empty());
    if (sinks.size() == 1) {
        result.sink = sinks.front();
    } else {
        result.sink = capacity.size() + (sources.size() > 1);
        for (auto &row: result.cost) {
            row.emplace_back();
        }
        for (auto &row: result.capacity) {
            row.emplace_back();
        }
        result.cost.emplace_back(result.sink + 1, 0u);
        result.capacity.emplace_back(result.sink + 1, 0u);
        for (auto v: sinks) {
            result.capacity[v][result.sink] = INF;
        }
    }
    return result;
}

flow_result_t graphs::max_flow_ford_fulkerson(const flow_graph_t &g) {
    const auto s = g.source;
    const auto t = g.sink;

    auto capacity = g.capacity;
    const auto n = g.capacity.size();
    auto parent = std::vector<Vertex>(g.capacity.size(), -1u);

    const auto bfs = [n,s,t,&parent,&capacity = std::as_const(capacity)]() -> bool {
        std::vector<bool> visited(n, false);
        std::deque<Vertex> queue{s};
        visited[s] = true;
        parent[s] = s;
        while (!(queue.empty())) {
            Vertex u = queue.front();
            queue.pop_front();
            for (Vertex v{}; v < n; v++) {
                if (visited[v] || capacity[u][v] <= 0) {
                    continue;
                }
                queue.push_back(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
        return visited[t];
    };

    flow_result_t result = {
        .max_flow = 0,
        .flow = adjacency_matrix<>(g.capacity.size(), std::vector<int>(g.capacity.size()))
    };
    while (bfs()) {
        int path_flow = INF;
        for (Vertex v = t; v != s; v = parent[v]) {
            path_flow = std::min(path_flow, capacity[parent[v]][v]);
        }
        for (Vertex v = t; v != s; v = parent[v]) {
            Vertex u = parent[v];
            capacity[u][v] -= path_flow;
            capacity[v][u] += path_flow;
            result.flow[u][v] += path_flow;
        }
        result.max_flow += path_flow;
    }
    return result;
}

min_cost_flow_result_t min_cost_flow(const flow_graph_t &g, int desired_flow) {

    int maxFlow = 0;
    int flow = 0;
    int sigma = INFINITY;
    int result = 0;
    int** modifiedCostArr;

    auto numberVert = g.capacity.size();

    std::map<int, std::pair<std::vector<int>, int>> tempMap;
    std::map<std::vector<int>, std::pair<int, int>> temporary;
    std::set<std::pair<int, int>> resultVert;
    std::vector<std::vector<int>> f(numberVert);
    std::vector<int> k;

    //auto& AdjacencyMatrixInf = g.capacity;
    auto& CostMatrix = g.cost;
    auto& CapacityMatrix = g.capacity;
    auto b = desired_flow;

    modifiedCostArr = new int* [numberVert];
    for (int i = 0; i < numberVert; i++)
    {
        modifiedCostArr[i] = new int[numberVert];
        for (int j = 0; j < numberVert; j++)
        {
            f.at(i).push_back(0);
            modifiedCostArr[i][j] = CostMatrix[i][j];
        }
    }

    while (maxFlow < b)
    {
        std::vector<int> path = BellmanFordPath(modifiedCostArr);
        for (int i = 0; i < path.size() - 1; i++)
        {
            sigma = std::min(sigma, CapacityMatrix[path.at(i)][path.at(i + 1)]);
            if (resultVert.find({path.at(i), path.at(i + 1)}) == resultVert.end())
            {
                resultVert.insert({path.at(i), path.at(i + 1)});
            }
        }

        flow = std::min(sigma, b - maxFlow);
        k.push_back(flow);
        int tempCost = 0;

        for (int i = 0; i < path.size() - 1; i++)
        {
            tempCost += CostMatrix[path.at(i)][path.at(i + 1)];
        }

        if (temporary.find(path) == temporary.end())
        {
            temporary[path] = {flow, tempCost};
        }
        else
        {
            temporary[path].first += flow;
        }

        for (int i = 0; i < path.size() - 1; i++)
        {
            f.at(path[i]).at(path[i + 1]) += flow;
            if (f.at(path[i]).at(path[i + 1]) == CapacityMatrix[path[i]][path[i + 1]])
            {
                modifiedCostArr[path[i]][path[i + 1]] = INFINITY;
                modifiedCostArr[path[i + 1]][path[i]] = INFINITY;
            }
            else
            {
                if (f.at(path[i]).at(path[i + 1]) >= 0)
                {
                    modifiedCostArr[path[i]][path[i + 1]] = CostMatrix[path[i]][path[i + 1]];
                    modifiedCostArr[path[i + 1]][path[i]] = -modifiedCostArr[path[i]][path[i + 1]];
                }
            }
        }
        maxFlow += flow;
    }
    int min = INFINITY;
    std::vector<std::pair<int, std::pair<std::vector<int>, int>>> tempVect;

    for (auto & i : temporary)
    {
        tempVect.push_back({i.second.second, {i.first, i.second.first}});
    }

    for (int i = 0; i < tempVect.size() - 1; i++)
    {
        for (int j = 0; j < tempVect.size() - 1; j++)
        {
            if (tempVect[j].first > tempVect[j + 1].first)
                swap(tempVect[j], tempVect[j + 1]);
        }
    }

    std::vector<int> vecPotok;
    std::vector<int> vecMatr;
    int ur = 0;

    for (int i = 0; i < tempVect.size(); i++)
    {
    //	printf("\n\nFlow path: ");
        for (int j = 0; j < tempVect[i].second.first.size() - 1; j++)
        {
            //printf("%d - ", tempVect[i].second.first[j] + 1);
            vecMatr.push_back(tempVect[i].second.first[j]);
        }

        //printf("%d", tempVect[i].second.first[tempVect[i].second.first.size() - 1] + 1);
        vecMatr.push_back(tempVect[i].second.first[tempVect[i].second.first.size() - 1]);

        //printf("\n\nFlow throught the path found: %d\n", tempVect[i].second.second);
        for (int l = 0; l < vecMatr.size() - 1; l++)
        {
            if (vecMatr[l] < vecMatr[l + 1] && matrPotok[vecMatr[l]][vecMatr[l + 1]] == 0)
                matrPotok[vecMatr[l]][vecMatr[l + 1]] = tempVect[i].second.second;
        }
        vecPotok.push_back(tempVect[i].second.second);
    }

    for_each(resultVert.begin(), resultVert.end(), [&result, &f](pair<int, int>x) {result += f.at(x.first).at(x.second) * CostMatrix[x.first][x.second]; });


    return {};
}

template<typename Func>
adjacency_matrix<>
matrix_power_shimbell(const adjacency_matrix<> &that, size_t power, Func extrem) {
    assert(power > 0);
    adjacency_matrix<> result = that;
    for (int i = 1; i < power; ++i) {
        result = matrix_multiply<>(result, that, extrem);
    }
    return result;
}

template<typename Func>
adjacency_matrix<>
matrix_multiply(const adjacency_matrix<> &lhs, const adjacency_matrix<> &rhs, Func extrem) {
    assert(lhs.size() == lhs[0].size()); // квадратная
    assert(rhs.size() == rhs[0].size()); // квадратная
    assert(lhs.size() == rhs.size());
    size_t n = lhs.size();
    adjacency_matrix<> result = lhs;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            auto &res = result[i][j];
            res = 0;
            for (int k = 0; k < n; ++k) {
                if (lhs[i][k] == 0 || rhs[k][j] == 0) {
                    continue;
                }
                res = res ? extrem(res, lhs[i][k] + rhs[k][j]) : lhs[i][k] + rhs[k][j];
            }
        }
    }
    return result;
}

size_t graphs::count_paths::operator()(size_t v) {
    if (visited[v]) {
        return d[v];
    }
    size_t result = 0;
    visited[v] = true;
    for (int i = 0; i < graph.size(); ++i) {
        if (!graph[i][v]) {
            continue;
        }
        // ∃ ребро из i в v
        result += operator()(i);
    }
    return d[v] = result;
}
