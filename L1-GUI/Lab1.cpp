//
// Created by mhq on 25/03/23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Lab1.h" resolved

#include <QOpenGLWidget>
#include "Lab1.h"
#include "ui_Lab1.h"
#include "GraphScene.h"
#include "ResizeFilter.h"
#include "MatrixModel.h"
#include "NodesModel.h"
#include "VectorModel.h"

Lab1::Lab1(QWidget *parent)
        : QWidget(parent)
        , ui{new Ui::Lab1}
        , gen{std::random_device{}()}
        , m_flow_source{}
        , m_flow_sink{} {
    ui->setupUi(this);

    auto matrixModel = new MatrixModel{this};
    auto costMatrixModel = new MatrixModel{this};
    auto shimbelModel = new MatrixModel{this};
    auto distances2dModel = new MatrixModel{this};
    auto flowModel = new MatrixModel{this};
    auto distances1dModel = new VectorModel{this};

    auto graphScene = new GraphScene{{}, ui->graphicsView};
    auto nodesModel = new NodesModel{this, 10};

    connect(ui->nVertices, qOverload<int>(&QSpinBox::valueChanged), nodesModel, &NodesModel::setSize);

    connect(ui->generate, &QPushButton::pressed, [=]() {
        auto adjacency = graphs::generate(ui->nVertices->value(), gen);
        matrixModel->setMatrix(adjacency);
        shimbelModel->setMatrix({});
        costMatrixModel->setMatrix({});
        graphScene->reset();
        for (int i = 0; i < adjacency.size(); ++i) {
            graphScene->updateNode(i, "ellipse");
        }
        for (int i = 0; i < adjacency.size(); ++i) {
            for (int j = 0; j < adjacency.size(); ++j) {
                if (!adjacency[i][j]) {
                    continue;
                }
                graphScene->addEdge(i, j, adjacency[i][j]);
            }
        }
        m_flow_sink = 0;
        m_flow_source = 0;
    });
    // Shimbel method
    {
        const auto act = [=](bool minChosen, int edgesCount) {
            if (edgesCount == 0) {
                return;
            }
            shimbelModel->setMatrix(
                    (minChosen ? graphs::min_path_lengths : graphs::max_path_lengths)
                            (matrixModel->matrix(), edgesCount)
            );
        };
        connect(ui->edgesCountInPath, qOverload<int>(&QSpinBox::valueChanged), [=](int value) {
            act(ui->chooseExtremum->currentIndex() == 0, value);
        });
        connect(ui->chooseExtremum, qOverload<int>(&QComboBox::currentIndexChanged), [=](int index) {
            act(index == 0, ui->edgesCountInPath->value());
        });
    }
    connect(ui->countPaths, &QPushButton::pressed, [=]() {
        if (matrixModel->rowCount({}) == 0) {
            return;
        }
        graphs::Vertex start = ui->startBox->currentData(Qt::UserRole).toUInt();
        graphs::Vertex end = ui->finishBox->currentData(Qt::UserRole).toUInt();
        auto res = graphs::count_paths{matrixModel->matrix(), start}(end);
        ui->pathsCountOut->setText(QString::number(res));
    });
    // allow edit adjMatrix -> that changes graph
    connect(matrixModel, &MatrixModel::dataChanged, [=](const QModelIndex &topLeft, const QModelIndex &bottomRight){
        Q_ASSERT(topLeft == bottomRight);
        int i = topLeft.row();
        int j = topLeft.column();
        graphScene->updateEdge(i, j, topLeft.data().toUInt());
    });
    {
        auto f = [=](decltype(graphs::min_path_distances_dijkstra) method) {
            return [=]() {
                if (matrixModel->rowCount({}) == 0) {
                    return;
                }
                graphs::Vertex start = ui->startBox_2->currentData(Qt::UserRole).toUInt();
                graphs::Vertex end = ui->finishBox_2->currentData(Qt::UserRole).toUInt();
                auto [
                        distances,
                        precedents,
                        iterations
                ] = method(matrixModel->matrix(), start);
                distances1dModel->setFrom(start);
                distances1dModel->setVector(distances);
                if (ui->distMatrix->model() != distances1dModel) {
                    ui->distMatrix->setModel(distances1dModel);
                }
                QStringList list;
                for (auto v: graphs::reconstruct_path(precedents, start, end)) {
                    list.append(QString{static_cast<char>('a' + v)});
                }
                ui->pathOut->setText(list.empty()
                                     ? "Такого пути нет"
                                     : list.join("→")
                );
                ui->iterationsOut->setText(QString::number(iterations));
            };
        };
        connect(ui->dijkstra, &QPushButton::pressed, f(graphs::min_path_distances_dijkstra));
        connect(ui->bellman,&QPushButton::pressed, f(graphs::min_path_distances_bellman_ford));
    }
    connect(ui->floyd, &QPushButton::pressed, [=]{
        if (matrixModel->rowCount({}) == 0) {
            return;
        }
        graphs::Vertex start = ui->startBox_2->currentData(Qt::UserRole).toUInt();
        graphs::Vertex end = ui->finishBox_2->currentData(Qt::UserRole).toUInt();

        auto [
                distances,
                precedents,
                iterations
        ] = graphs::min_path_distances_floyd_warshall(matrixModel->matrix());
        distances2dModel->setMatrix(distances);
        if (ui->distMatrix->model() != distances2dModel) {
            ui->distMatrix->setModel(distances2dModel);
        }
        QStringList list;
        for (auto v: graphs::reconstruct_path(precedents[start], start, end)) {
            list.append(QString{static_cast<char>('a' + v)});
        }
        ui->pathOut->setText(list.empty()
                             ? "Такого пути нет"
                             : list.join("→")
        );
        ui->iterationsOut->setText(QString::number(iterations));
    });
    connect(ui->addMinuses, &QPushButton::pressed, [=](){
        auto m = matrixModel->matrix();
        std::uniform_int_distribution<int> dis{0, 1};
        for (graphs::Vertex i{}; i < m.size(); ++i) {
            for (graphs::Vertex j{}; j < m.size(); ++j) {
                if (dis(gen)) {
                    m[i][j] *= -1;
                    if (m[i][j] == 0) {
                        continue;
                    }
                    graphScene->updateEdge(i, j, m[i][j]);
                }
            }
        }
        matrixModel->setMatrix(m);
    });
    // Lab 3
    connect(ui->generate_costs, &QPushButton::pressed, [=](){
        costMatrixModel->setMatrix(graphs::generate_costs(matrixModel->matrix(), gen));
    });
    connect(ui->identify_source_sink, &QPushButton::pressed, [=](){
        if (matrixModel->matrix().empty()) {
            return;
        }
        auto g = graphs::add_supersource_supersink(matrixModel->matrix(), costMatrixModel->matrix());
        ui->sourceText->setText({static_cast<char>('a' + g.source)});
        m_flow_source = g.source;
        ui->sinkText->setText({static_cast<char>('a' + g.sink)});
        m_flow_sink = g.sink;
        if (matrixModel->matrix().size() == g.capacity.size()) {
            return;
        }
        ui->nVertices->setValue(g.capacity.size());
        matrixModel->setMatrix(g.capacity);
        for (graphs::Vertex i = std::min(g.source, g.sink); i < g.capacity.size(); ++i) {
            graphScene->updateNode(i, "ellipse");
        }
        for (graphs::Vertex i{}; i < g.capacity.size(); i++) {
            if (g.capacity[g.source][i]) {
                graphScene->addEdge(g.source, i, g.capacity[g.source][i]);
            }
            if (g.capacity[i][g.sink]) {
                graphScene->addEdge(i, g.sink, g.capacity[i][g.sink]);
            }
        }
        if (costMatrixModel->matrix().empty()) {
            return;
        }
        costMatrixModel->setMatrix(g.cost);
    });
    connect(ui->fulkerson, &QPushButton::pressed, [=](){
        if (m_flow_source == m_flow_sink) {
            return;
        }
        auto [max_flow, flow_matrix] = graphs::max_flow_ford_fulkerson({
            .capacity = matrixModel->matrix(),
            .source = m_flow_source,
            .sink = m_flow_sink
        });
        ui->flowText->setText(QString::number(max_flow));
        flowModel->setMatrix(flow_matrix);
    });
    connect(ui->minFlow, &QPushButton::pressed, [=](){
        if (ui->flowText->text().isEmpty() || m_flow_source == m_flow_sink) {
            return;
        }
        auto [flow_sum, cost, flow_matrix] = graphs::min_cost_flow({
            .capacity = matrixModel->matrix(),
            .cost = costMatrixModel->matrix(),
            .source = m_flow_source,
            .sink = m_flow_sink
        }, ui->flowText->text().toInt() * 2 / 3);
        ui->costText->setText(cost != -1 ? QString::number(cost) : "—");
        ui->minFlowText->setText(QString::number(flow_sum));
        flowModel->setMatrix(flow_matrix);
    });
    {
        auto f = [=](auto&& sst_algo, QLineEdit* iters) {
            auto [spanning_tree, cost, iterations] = sst_algo(matrixModel->matrix());
            iters->setText(QString::number(iterations));
            ui->cost_mst->setText(QString::number(cost));
            for (auto [from, to, _]: spanning_tree) {
                graphScene->highlightEdge(from, to);
            }
        };
        connect(ui->prim, &QPushButton::pressed, [=] { return f(graphs::prim_sst, ui->iter_prim); });
        connect(ui->kruskal, &QPushButton::pressed, [=] { return f(graphs::kruskal_sst, ui->iter_kruskal); });
    }

    ui->graphicsView->setViewport(new QOpenGLWidget);
    ui->graphicsView->setScene(graphScene);
    ui->graphicsView->installEventFilter(new ResizeFilter);
    ui->adjMatrix->setModel(matrixModel);
    ui->shimbellMatrix->setModel(shimbelModel);
    ui->costsMatrix->setModel(costMatrixModel);
    ui->flowMatrix->setModel(flowModel);
    for (auto m : {ui->adjMatrix, ui->shimbellMatrix, ui->distMatrix, ui->costsMatrix, ui->flowMatrix}) {
        m->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
    for (auto box : {ui->startBox,
                     ui->finishBox,
                     ui->startBox_2,
                     ui->finishBox_2}) {
        box->setModel(nodesModel);
    }
}

Lab1::~Lab1() {
    delete ui;
}
