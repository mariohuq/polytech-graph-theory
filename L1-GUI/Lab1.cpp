//
// Created by mhq on 25/03/23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Lab1.h" resolved

#include <QOpenGLWidget>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include "Lab1.h"
#include "ui_Lab1.h"
#include "GraphScene.h"
#include "ResizeFilter.h"
#include "MatrixModel.h"
#include "NodesModel.h"
#include "VectorModel.h"
#include "PathsModel.h"
#include "MyString.h"

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
    auto hamiltonModel = new PathsModel{{}, this};

    auto graphScene = new GraphScene{{}, ui->graphicsView};
    auto nodesModel = new NodesModel{this, 10};

    auto reset_outputs = [=]() {
        shimbelModel->setMatrix({});
        costMatrixModel->setMatrix({});
        m_flow_sink = 0;
        m_flow_source = 0;
    };

    connect(ui->nVertices, qOverload<int>(&QSpinBox::valueChanged), nodesModel, &NodesModel::setSize);

    auto set_graph_image = [=](const auto& adjacency) {
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
    };

    connect(ui->generate, &QPushButton::pressed, [=]() {
        auto adjacency = graphs::generate(ui->nVertices->value(), gen);
        matrixModel->setMatrix(adjacency);
        reset_outputs();
        graphScene->reset();
        set_graph_image(adjacency);
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
    connect(matrixModel, &MatrixModel::dataChanged, [=](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
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
                auto path = graphs::reconstruct_path(precedents, start, end);
                ui->pathOut->setText(path.empty()
                                     ? "Такого пути нет"
                                     : showOriented(path)
                );
                ui->iterationsOut->setText(QString::number(iterations));
            };
        };
        connect(ui->dijkstra, &QPushButton::pressed, f(graphs::min_path_distances_dijkstra));
        connect(ui->bellman, &QPushButton::pressed, f(graphs::min_path_distances_bellman_ford));
    }
    connect(ui->floyd, &QPushButton::pressed, [=] {
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
        auto path = graphs::reconstruct_path(precedents[start], start, end);
        ui->pathOut->setText(path.empty()
                             ? "Такого пути нет"
                             : showOriented(path)
        );
        ui->iterationsOut->setText(QString::number(iterations));
    });
    connect(ui->addMinuses, &QPushButton::pressed, [=]() {
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
    connect(ui->generate_costs, &QPushButton::pressed, [=]() {
        costMatrixModel->setMatrix(graphs::generate_costs(matrixModel->matrix(), gen));
    });
    connect(ui->identify_source_sink, &QPushButton::pressed, [=]() {
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
    connect(ui->fulkerson, &QPushButton::pressed, [=]() {
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
    connect(ui->minFlow, &QPushButton::pressed, [=]() {
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
    // Lab 4
    {
        auto f = [=](auto &&sst_algo, QLineEdit *iters) {
            if (matrixModel->matrix().empty()) {
                return;
            }
            auto [spanning_tree, cost, iterations] = sst_algo(matrixModel->matrix());
            iters->setText(QString::number(iterations));
            ui->cost_mst->setText(QString::number(cost));
            for (auto [from, to, _]: spanning_tree) {
                graphScene->highlightEdge(from, to);
            }
        };
        connect(ui->prim, &QPushButton::pressed,
                [=] { return f(graphs::prim_mst, ui->iter_prim); });
        connect(ui->kruskal, &QPushButton::pressed,
                [=] { return f(graphs::kruskal_mst, ui->iter_kruskal); });
    }
    connect(ui->kirchhoff, &QPushButton::pressed, [=]() {
        if (matrixModel->matrix().empty()) {
            return;
        }
        ui->count_sts->setText(
            QString::number(graphs::spanning_trees_count(
                matrixModel->matrix())));
    });
    connect(ui->prufer_encode, &QPushButton::pressed, [=]() {
        if (matrixModel->matrix().size() <= 1) {
            return;
        }
        auto [code, weights] = graphs::pruefer::encode(matrixModel->matrix());
        QStringList list;
        for (auto x : code) {
            list.append({static_cast<char>('a' + x)});
        }
        for (auto w : weights) {
            list.append(QString::number(w));
        }
        ui->prufer_code->setPlainText(list.join(' '));
    });
    connect(ui->prufer_decode, &QPushButton::pressed, [=]() {
        auto list = ui->prufer_code->toPlainText().trimmed().split(' ', Qt::SkipEmptyParts);
        auto f = [=]() { QMessageBox::warning(this, "Поправьте ввод", "Введите n-2 вершины и n-1 ребро", QMessageBox::Close); };
        if (list.empty()) {
            f();
            return;
        }
        if (list.size() % 2 == 0) {
            f();
            return;
        }
        size_t n = (list.size() + 3) / 2;
        if (n <= 1) {
            f();
            return;
        }
        std::vector<graphs::Vertex> code(n - 2);
        for (int i = 0; i < code.size(); ++i) {
            auto ch = list[i].at(0);
            if (!(list[i].size() == 1 && 'a' <= ch && ch <= 'z')) {
                QMessageBox::warning(this, "Поправьте ввод",
                                     "Вершина на позиции " + QString::number(i+1) + " не a..z.", QMessageBox::Close);
                return;
            }
            code[i] = ch.toLatin1() - 'a';
        }
        std::vector<int> weights(n - 1);
        for (size_t i = 0; i < weights.size(); ++i) {
            bool ok = false;
            weights[i] = list[i + (n - 2)].toInt(&ok);
            if (!ok) {
                QMessageBox::warning(this, "Поправьте ввод",
                                     "Вес на позиции " + QString::number(i+1) + " не число.", QMessageBox::Close);
                return;
            }
        }
        auto g = graphs::pruefer::decode(code, weights);
        matrixModel->setMatrix(g);
        graphScene->reset();
        reset_outputs();
        for (int i = 0; i < g.size(); ++i) {
            graphScene->updateNode(i, "ellipse");
        }
        for (int i = 0; i < g.size(); ++i) {
            for (int j = 0; j < g.size(); ++j) {
                if (!g[i][j]) {
                    continue;
                }
                graphScene->addEdge(i, j, g[i][j]);
            }
        }
    });
    connect(graphScene, &GraphScene::edgeRemoved, [=](int from, int to) {
        matrixModel->setData(matrixModel->index(from, to), 0, Qt::EditRole);
    });
    connect(graphScene, &GraphScene::edgeAdded, [=](int from, int to) {
        matrixModel->setData(matrixModel->index(from, to), 1, Qt::EditRole);
    });
    // Lab 5
    {
        constexpr auto out = [](const std::vector<graphs::edge_t> &edges, QLineEdit *edit) {
            if (edges.empty()) {
                edit->setText("—");
                return;
            }
            QStringList list;
            for (auto [from, to, _]: edges) {
                std::tie(from, to) = std::minmax(from, to);
                list.append(QString(static_cast<char>('a' + from))
                                .append(static_cast<char>('a' + to)));
            }
            edit->setText(list.join(", "));
        };
        const auto check_print_cycle = [=]() {
            bool is = matrixModel->matrix().size() > 2 && graphs::is_eulerian(matrixModel->matrix());
            ui->isEulerianAns->setText(is ? "Да!" : "Нет!");
            if (is) {
                ui->eulerCycle->setText(showUnoriented(graphs::euler_cycle(matrixModel->matrix())));
            }
        };

        connect(ui->eulerize, &QPushButton::pressed, [=]() {
            const auto &m = matrixModel->matrix();
            if (m.size() <= 2) {
                return;
            }
            auto res = graphs::eulerize(m);
            reset_outputs();
            out(res.removed, ui->removedEdges);
            out(res.added, ui->addedEdges);
            if (!res.has_changed()) {
                return;
            }
            matrixModel->setMatrix(res.changed);
            for (auto [from, to, _] : res.removed) {
                graphScene->updateEdge(from, to, 0);
                graphScene->updateEdge(to, from, 0);
            }
            for (auto [from, to, _] : res.added) {
                graphScene->updateEdge(from, to, 1);
                graphScene->updateEdge(to, from, 0);
            }
            check_print_cycle();
        });
        connect(ui->isEulerian, &QPushButton::pressed, check_print_cycle);

        auto check_print_cycle_hamilton = [=]() {
            bool is = graphs::is_hamiltonian(matrixModel->matrix());
            ui->isHamiltonianAns->setText(is ? "Да!" : "Нет!");
            if (!is) return;
            graphs::hamilton_cycles hc{ matrixModel->matrix() };
            std::vector<graphs::costed_path_t> paths;

            auto it = hc.begin();
            for (int i = 0; i < 50 && it != hc.end(); ++i, ++it) {
                paths.push_back(*it);
            }

            std::sort(paths.begin(), paths.end());
            hamiltonModel->setUnderlying(paths);
            if (it == hc.end()) {
                return;
            }
            QFile file{QFileDialog::getSaveFileName(this, "Гамильтоновы циклы куда сохранить?", "hamilton.tsv", "Tab-Delimited Files (*.tsv)")};
            if (!file.open(QIODevice::WriteOnly)) {
                return;
            }
            QTextStream os{&file};
            os << "Cycle\tCost\n";
            auto write_line = [&](const auto& x){
                os << showUnoriented(x.path) << "\t" << x.cost << "\n";
            };
            for (const auto& x: paths) {
                write_line(x);
            }
            std::for_each(it, hc.end(), write_line);
        };

        connect(ui->hamiltonize, &QPushButton::pressed, [=]() {
            auto res = graphs::hamiltonize(matrixModel->matrix());
            reset_outputs();
            out(res.removed, ui->removedEdges);
            out(res.added, ui->addedEdges);
            if (!res.has_changed()) {
                return;
            }
            matrixModel->setMatrix(res.changed);
            for (auto [from, to, _] : res.added) {
                graphScene->updateEdge(from, to, 1);
            }
            check_print_cycle_hamilton();
        });
        connect(ui->isHamiltonian, &QPushButton::pressed, check_print_cycle_hamilton);
    }

    ui->graphicsView->setViewport(new QOpenGLWidget);
    ui->graphicsView->setScene(graphScene);
    ui->graphicsView->installEventFilter(new ResizeFilter);
    ui->adjMatrix->setModel(matrixModel);
    ui->shimbellMatrix->setModel(shimbelModel);
    ui->costsMatrix->setModel(costMatrixModel);
    ui->flowMatrix->setModel(flowModel);
    ui->hamiltonCycles->setModel(hamiltonModel);
    for (auto m: {static_cast<QTableView*>(ui->adjMatrix), ui->shimbellMatrix, ui->distMatrix, ui->costsMatrix, ui->flowMatrix, ui->hamiltonCycles}) {
        m->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
    for (auto box: {ui->startBox,
                    ui->finishBox,
                    ui->startBox_2,
                    ui->finishBox_2}) {
        box->setModel(nodesModel);
    }
}

Lab1::~Lab1() {
    delete ui;
}
