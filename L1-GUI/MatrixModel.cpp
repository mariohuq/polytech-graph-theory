//
// Created by mhq on 27/03/23.
//

#include "MatrixModel.h"

#include <utility>

MatrixModel::MatrixModel(QObject *parent, graphs::adjacency_matrix<> underlying)
        : QAbstractTableModel(parent), underlying(std::move(underlying)) {}

QVariant MatrixModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        return (uint)underlying[index.row()][index.column()];
    }
    return {};
}

bool MatrixModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    auto& x = underlying[index.row()][index.column()];
    bool emitting = x != value.toUInt();
    x = value.toUInt();
    if (emitting) {
        emit dataChanged(index, index);
    }
    return true;
}

void MatrixModel::setMatrix(graphs::adjacency_matrix<> that) {
    emit layoutAboutToBeChanged();
    underlying = std::move(that);
    emit layoutChanged();
}

QVariant MatrixModel::headerData(int section, Qt::Orientation orientation, int role) const {
    switch (role) {
        case Qt::DisplayRole:
            return QString {'a' + section};
        default:
            return {};
    }
}
