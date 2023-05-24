//
// Created by mhq on 22/05/23.
//

#include "PathsModel.h"
#include "MyString.h"

int PathsModel::rowCount(const QModelIndex &parent) const {
    return underlying.size();
}

int PathsModel::columnCount(const QModelIndex &parent) const {
    return 2;
}

PathsModel::PathsModel(const std::vector<graphs::costed_path_t> &underlying, QObject *parent = nullptr) : QAbstractTableModel(
    parent), underlying(underlying) {}

QVariant PathsModel::data(const QModelIndex &index, int role) const {
    auto& [path, cost] = underlying[index.row()];
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
            {
                return showUnoriented(path);
            }
            case 1:
                return static_cast<uint>(cost);
        }
    }
    return {};
}

QVariant PathsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return "Цикл";
            case 1:
                return "$";
            default:
                break;
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

void PathsModel::setUnderlying(std::vector<graphs::costed_path_t> that) {
    emit layoutAboutToBeChanged();
    underlying = std::move(that);
    emit layoutChanged();
}
