#include "Editor.h"

#include <QtWidgets>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTableView>

#include "ComboHeaderView.h"
#include "CheckableComboBox.h"
#include "GraphScene.h"
#include "AmountDelegate.h"
#include "ResizeFilter.h"

#include <utility>

void populate(QStandardItemModel* model) {
    auto q = QSqlQuery{ "select distinct type from Nodes order by type" };
    while (q.next())
    {
        auto it = new QStandardItem{ q.value(0).toString() };
        it->setCheckState(Qt::Unchecked);
        model->appendRow(it);
    }
}

Editor::Editor(QWidget* parent)
    : QWidget(parent)
{
    static auto id_of = [](auto* model, const auto& index) {
        return model->data(model->index(index.row(), 0), 0).toInt();
    };

    ui.setupUi(this);
    ui.horizontalSpliiter->setStretchFactor(0, 0);
    ui.horizontalSpliiter->setStretchFactor(1, 1);
    ui.tableSplitter->setStretchFactor(0, 1);
    ui.tableSplitter->setStretchFactor(1, 0);

    auto groupModel = new QSqlQueryModel{ this };
    groupModel->setQuery(R"(select type, count(1) as 'count', '' as ''
            from Nodes
            group by type
            order by type)");
    ui.groupsTable->setItemDelegateForColumn(2, new AmountDelegate{});
    ui.groupsTable->setModel(groupModel);
    ui.groupsTable->setColumnWidth(2, 70);

    auto model = new QSqlTableModel{ this };
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("Nodes");
    model->select();
    ui.primitivesTable->setModel(model);
    
    auto header = new ComboHeaderView{ new CheckableComboBox, ui.primitivesTable };
    auto headerModel = new QStandardItemModel;
    populate(headerModel);
    header->setModelForColumn(1, headerModel);
    
    ui.primitivesTable->setHorizontalHeader(header);
    ui.primitivesTable->horizontalHeader()->setDefaultSectionSize(49);
    ui.primitivesTable->horizontalHeader()->setStretchLastSection(true);

    // selection of nodes
    connect(ui.primitivesTable->selectionModel(), &QItemSelectionModel::selectionChanged,
        [=](const QItemSelection& selected, const QItemSelection& deselected) {
            auto scene = static_cast<GraphScene*>(ui.graphicsView->scene());
            for (const auto& index : selected.indexes()) {
                scene->setSelectedNode(id_of(model, index), true);
            }
            for (const auto& index : deselected.indexes()) {
                scene->setSelectedNode(id_of(model, index), false);
            }
        });

    connect(model, &QSqlTableModel::modelReset, [=]() {
        static_cast<GraphScene*>(ui.graphicsView->scene())->reset(model->filter());
    });

    // скрыть выбранные
    connect(ui.hideSelected, &QPushButton::pressed, [=]() {    
        auto scene = static_cast<GraphScene*>(ui.graphicsView->scene());
        ui.primitivesTable->setUpdatesEnabled(false);
        for (const auto& index : ui.primitivesTable->selectionModel()->selectedIndexes())
        {
            scene->hideNode(id_of(model, index));
            ui.primitivesTable->hideRow(index.row());
        }
        ui.primitivesTable->setUpdatesEnabled(true);
    });

    // показать скрытые
    connect(ui.showHidden, &QPushButton::pressed, [=]() {
        auto scene = static_cast<GraphScene*>(ui.graphicsView->scene());
        for (int i = ui.primitivesTable->model()->rowCount() - 1; i >= 0; i--)
        {
            scene->showNode(id_of(model, model->index(i, 0)));
            ui.primitivesTable->showRow(i);
        }
    });

    // удаление строк
    connect(ui.removeSelected, &QPushButton::pressed, [=]() {
        auto m = ui.primitivesTable->selectionModel();
        if (!m->hasSelection()) {
            return;
        }
        QSqlQuery count{ "select count() from (select distinct type from Nodes)" };
        for (const auto& index : m->selectedRows())
        {
            model->removeRows(index.row(), 1);
            static_cast<GraphScene*>(ui.graphicsView->scene())->hideNode(id_of(model, index));
        }
        int before = count.value(0).toInt();
        model->submitAll();
        groupModel->query().exec();
        groupModel->setQuery(groupModel->query());
        count.exec();
        int after = count.value(0).toInt();
        if (after < before) {
            headerModel->clear();
            populate(headerModel);
        }
    });

    using p = std::pair<QPushButton*, const char*>;
    for (auto [button, t] : {
        p{ui.addEllipse, "ellipse"},
        p{ui.addRect, "rectangle"},
        p{ui.addTriangle, "triangle"},
        p{ui.addHex, "hexagon"}
    }) {
        auto type = t;
        // add node
        connect(button, &QPushButton::pressed, [=]() {
            const int row = model->rowCount();
            model->insertRows(row, 1);
            model->setData(model->index(row, 1), type);
            model->submitAll();
            static_cast<GraphScene*>(ui.graphicsView->scene())
                ->updateNode(model->data(model->index(row, 0)).toInt(), type);
            groupModel->query().exec();
            groupModel->setQuery(groupModel->query());
            if (headerModel->findItems(type).empty()) {
                auto it = new QStandardItem{ type };
                it->setCheckState(Qt::Unchecked);
                headerModel->appendRow(it);
                // check if that clears selection or not
                headerModel->sort(0);
            }
         });
    }
    ui.graphicsView->setViewport(new QOpenGLWidget);
    ui.graphicsView->setScene(new GraphScene({}, ui.graphicsView));
    
    ui.graphicsView->installEventFilter(new ResizeFilter);
}