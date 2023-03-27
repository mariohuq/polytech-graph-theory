//
// Created by mhq on 27/03/23.
//

#pragma once

#include <QAbstractListModel>

class NodesModel : public QAbstractListModel {

public:
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    NodesModel(QObject *parent, size_t size = 0);

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void setSize(int newSize);
private:
    size_t size;
};
