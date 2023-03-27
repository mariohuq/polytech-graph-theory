//
// Created by mhq on 27/03/23.
//

#include "NodesModel.h"

NodesModel::NodesModel(QObject *parent, size_t size) : QAbstractListModel(parent), size(size) {}

int NodesModel::rowCount(const QModelIndex &parent) const {
    return size;
}

QVariant NodesModel::data(const QModelIndex &index, int role) const {
    switch (role) {
        case Qt::DisplayRole:
            return QString{ 'a' + index.row()};
        case Qt::UserRole:
            return (uint)index.row();
        default:
            return {};
    }
}

void NodesModel::setSize(int newSize) {
    if (size == newSize) {
        return;
    }
    emit layoutAboutToBeChanged();
    size = newSize;
    emit layoutChanged();
}

Qt::ItemFlags NodesModel::flags(const QModelIndex &index) const {
    return QAbstractListModel::flags(index) | Qt::ItemIsEnabled;
}
