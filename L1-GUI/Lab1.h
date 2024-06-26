//
// Created by mhq on 25/03/23.
//

#pragma once

#include <QWidget>
#include <random>
#include "../L1/graphs.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Lab1; }
QT_END_NAMESPACE

class Lab1 : public QWidget {
Q_OBJECT

public:
    explicit Lab1(QWidget *parent = nullptr);

    ~Lab1() override;

private:
    Ui::Lab1 *ui;
    std::mt19937 gen;
    graphs::Vertex m_flow_source, m_flow_sink;
};

