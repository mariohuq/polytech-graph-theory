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
        : QWidget(parent), ui{new Ui::Lab1}, gen{std::random_device{}()} {
    ui->setupUi(this);

    auto matrixModel = new MatrixModel{this};
    auto shimbelModel = new MatrixModel{this};
    auto distances2dModel = new MatrixModel{this};
    auto distances1dModel = new VectorModel{this};

    auto graphScene = new GraphScene{{}, ui->graphicsView};
    auto nodesModel = new NodesModel{this, 10};

    connect(ui->nVertices, qOverload<int>(&QSpinBox::valueChanged), nodesModel, &NodesModel::setSize);

    connect(ui->generate, &QPushButton::pressed, [=]() {
        auto adjacency = graphs::generate(ui->nVertices->value(), gen);
        matrixModel->setMatrix(adjacency);
        shimbelModel->setMatrix({});
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
                ui->pathOut->setText(list.join("→"));
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
        ui->pathOut->setText(list.join("→"));
        ui->iterationsOut->setText(QString::number(iterations));
    });

    ui->graphicsView->setViewport(new QOpenGLWidget);
    ui->graphicsView->setScene(graphScene);
    ui->graphicsView->installEventFilter(new ResizeFilter);
    ui->adjMatrix->setModel(matrixModel);
    ui->shimbellMatrix->setModel(shimbelModel);
    for (auto m : {ui->adjMatrix, ui->shimbellMatrix, ui->distMatrix}) {
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
