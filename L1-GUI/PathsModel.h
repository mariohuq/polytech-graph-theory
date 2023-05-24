//
// Created by mhq on 22/05/23.
//
#pragma once

#include <QAbstractTableModel>
#include "../L1/graphs.h"

class PathsModel : public QAbstractTableModel {
public:
    PathsModel(const std::vector<graphs::costed_path_t> &underlying, QObject *parent);

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    int columnCount(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent) const override;

    void setUnderlying(std::vector<graphs::costed_path_t> that);

private:
    std::vector<graphs::costed_path_t> underlying;
};