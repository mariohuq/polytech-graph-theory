//
// Created by mhq on 16/04/23.
//

#include "VectorModel.h"

VectorModel::VectorModel(QObject *parent, std::vector<int> underlying, graphs::Vertex from)
    : QAbstractTableModel(parent)
    , m_underlying(std::move(underlying))
    , m_from{from} {}

QVariant VectorModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole && index.row() == 0) {
        auto data = m_underlying[index.column()];
        if (data == graphs::INF) {
            return "∞";
        } else if (data == -graphs::INF) {
            return "-∞";
        }
        return data;
    }
    return {};
}

QVariant VectorModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return {};
    }
    switch (orientation) {
        case Qt::Vertical:
            return QString {static_cast<char>('a' + m_from)};
        default:
            return QString {'a' + section};
    }
}

void VectorModel::setVector(std::vector<int> that) {
    emit layoutAboutToBeChanged();
    m_underlying = std::move(that);
    emit layoutChanged();
}

void VectorModel::setFrom(graphs::Vertex from) {
    emit layoutAboutToBeChanged();
    m_from = std::move(from);
    emit layoutChanged();
}
