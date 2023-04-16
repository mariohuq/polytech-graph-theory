//
// Created by mhq on 16/04/23.
//

#pragma once

#include <QAbstractTableModel>
#include "../L1/graphs.h"

class VectorModel : public QAbstractTableModel {
public:
    explicit VectorModel(QObject *parent = nullptr, std::vector<int> underlying = {}, graphs::Vertex from = {});

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    int rowCount(const QModelIndex &parent) const override {
        return 1;
    }

    int columnCount(const QModelIndex &parent) const override {
        return m_underlying.size();
    }

    void setVector(std::vector<int> that);

    void setFrom(graphs::Vertex from);

    const std::vector<int>& vector() const {
        return m_underlying;
    }


private:
    std::vector<int> m_underlying;
    graphs::Vertex m_from;
};
