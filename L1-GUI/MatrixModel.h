//
// Created by mhq on 27/03/23.
//

#pragma once

#include <QAbstractTableModel>
#include "../L1/graphs.h"

class MatrixModel : public QAbstractTableModel {
public:
    explicit MatrixModel(QObject *parent = nullptr, graphs::adjacency_matrix<> underlying = {});

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    int rowCount(const QModelIndex &parent) const override {
        return underlying.size();
    }

    int columnCount(const QModelIndex &parent) const override {
        if (underlying.empty()) {
            return 0;
        }
        return underlying[0].size();
    }

    void setMatrix(graphs::adjacency_matrix<> that);

    const graphs::adjacency_matrix<>& matrix() const {
        return underlying;
    }

private:
    graphs::adjacency_matrix<> underlying;
};
