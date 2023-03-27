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


Lab1::Lab1(QWidget *parent)
        : QWidget(parent), ui{new Ui::Lab1}, gen{std::random_device{}()} {
    ui->setupUi(this);

    auto matrixModel = new MatrixModel{ this };
    auto shimbelModel = new MatrixModel{ this };
    auto graphScene = new GraphScene{{}, ui->graphicsView};
    auto nodesModel = new NodesModel{ this, 10 };

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
    connect(ui->edgesCountInPath, qOverload<int>(&QSpinBox::valueChanged), [=](int value){
        if (value == 0) {
            return;
        }
        shimbelModel->setMatrix(graphs::max_path_lengths(matrixModel->matrix(), value));
    });
    connect(ui->countPaths, &QPushButton::pressed, [=]() {
        if (matrixModel->rowCount({}) == 0){
            return;
        }
        size_t start = ui->startBox->currentData(Qt::UserRole).toUInt();
        size_t end = ui->finishBox->currentData(Qt::UserRole).toUInt();
        auto res = graphs::count_paths{matrixModel->matrix(), start}(end);
        ui->pathsCountOut->setText(QString::number(res));
    });

    ui->graphicsView->setViewport(new QOpenGLWidget);
    ui->graphicsView->setScene(graphScene);
    ui->graphicsView->installEventFilter(new ResizeFilter);
    ui->adjMatrix->setModel(matrixModel);
    ui->adjMatrix->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->shimbellMatrix->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->shimbellMatrix->setModel(shimbelModel);
    ui->startBox->setModel(nodesModel);
    ui->finishBox->setModel(nodesModel);
}

Lab1::~Lab1() {
    delete ui;
}
