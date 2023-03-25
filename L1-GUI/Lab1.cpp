//
// Created by mhq on 25/03/23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Lab1.h" resolved

#include <QOpenGLWidget>
#include "Lab1.h"
#include "ui_Lab1.h"
#include "../L1/graphs.h"
#include "GraphScene.h"
#include "ResizeFilter.h"


Lab1::Lab1(QWidget *parent)
    : QWidget(parent)
    , ui{new Ui::Lab1}
    , gen{std::random_device{}()} {
    ui->setupUi(this);

    connect(ui->generate, &QPushButton::pressed, [&]() {
        graphs::adjacency_matrix<> g = graphs::generate(ui->nVertices->value(), gen);
        auto view = static_cast<GraphScene*>(ui->graphicsView->scene());
        view->reset();
        for (int i = 0; i < g.size(); ++i) {
            view->updateNode(i, "ellipse");
        }
        for (int i = 0; i < g.size(); ++i) {
            for (int j = 0; j < g.size(); ++j) {
                if (!g[i][j]) {
                    continue;
                }
                view->addEdge(i,j,g[i][j]);
            }
        }
    });

    ui->graphicsView->setViewport(new QOpenGLWidget);
    ui->graphicsView->setScene(new GraphScene({}, ui->graphicsView));

    ui->graphicsView->installEventFilter(new ResizeFilter);
}

Lab1::~Lab1() {
    delete ui;
}
