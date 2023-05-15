#include "GraphScene.h"
#include "node.h"
#include "edge.h"

#include <QRandomGenerator>
#include <QGraphicsSceneMouseEvent>
#include <QSqlQuery>
#include <QKeyEvent>

static auto randint = [](auto limit) {
    return QRandomGenerator::global()->bounded(limit);
};
static auto randPoint = [](QSizeF size) {
    return QPointF{ randint(size.width()), randint(size.height()) };
};

GraphScene::GraphScene(const QRectF& sceneRect, QObject* parent)
    : QGraphicsScene{ sceneRect, parent }
    , timerId{ 0 }
    , startNodeId{ NoNode } {
    prepared_add_edge.prepare("insert into Edges (start_id, end_id) values (:start_id, :end_id)");
    prepared_remove_edge.prepare("delete from Edges where start_id = :start_id and end_id = :end_id");
    setItemIndexMethod(QGraphicsScene::NoIndex);
    reset();
}

void GraphScene::updateNode(int id, const QString& type)
{
    auto&& node = nodes[id];
    if (node == nullptr) {
        node = new Node(id, type);
        addItem(node);
        node->setPos(sceneRect().topLeft() + randPoint(sceneRect().size()));
    }
    else {
        node->setType(type);
        node->show();
    }
} 

void GraphScene::hideNode(int id)
{
    nodes.at(id)->hide();
}

void GraphScene::showNode(int id)
{
    nodes.at(id)->show();
}

void GraphScene::setSelectedNode(int id, bool selected)
{
    nodes.at(id)->setSelected(selected);
}

void GraphScene::reset(const QString& filter)
{
    for (auto x : items()) {
        auto e = qgraphicsitem_cast<Edge*>(x);
        if (e) {
            removeItem(e);
        }
    }

    decltype(nodes) prev_nodes;
    prev_nodes.swap(nodes);
    for (auto q = QSqlQuery{ QString{"select id, type from Nodes"} + (filter.isEmpty() ? "" : " WHERE " + filter)}; q.next(); )
    {
        auto id = q.value(0).toInt();
        auto type = q.value(1).toString();
        
        auto&& node = prev_nodes[id];
        if (node == nullptr) {
            node = new Node(id, type);
            addItem(node);
            node->setPos(sceneRect().topLeft() + randPoint(sceneRect().size()));
        }
        else {
            node->setType(type);
        }
        node->show();
        nodes.insert(prev_nodes.extract(id));
    }
    for (const auto& [_, node] : prev_nodes) {
        node->hide();
    }
    for (auto q = QSqlQuery{ "select start_id, end_id from Edges" }; q.next(); )
    {
        int start_id = q.value(0).toInt();
        int end_id = q.value(1).toInt();
        if (nodes.find(start_id) == nodes.end()) {
            continue;
        }
        if (nodes.find(end_id) == nodes.end()) {
            continue;
        }
        addItem(new Edge(nodes.at(start_id), nodes.at(end_id)));
    }
    placeRandom();
}

void GraphScene::placeRandom() {
    auto topLeft = sceneRect().topLeft();
    auto size = sceneRect().size();
    for (const auto& [id, node] : nodes) {

        node->setPos(topLeft + randPoint(size));
	}
}

void GraphScene::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Space:
    case Qt::Key_Enter:
        placeRandom();
        break;
    default:
        QGraphicsScene::keyPressEvent(event);
    }
}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    static auto handle = [=](QGraphicsItem* item) {
        if (item == nullptr) {
            return;
        }
        auto node = qgraphicsitem_cast<Node*>(item);
        if (node != nullptr) {
            if (startNodeId == NoNode) {
                startNodeId = node->id();
                node->setSelected(true);
            }
            else if (startNodeId == node->id()) {
                node->setSelected(false);
                startNodeId = NoNode;
            }
            else {
                nodes[startNodeId]->setSelected(false);
                addItem(new Edge(nodes[startNodeId], node));
                prepared_add_edge.bindValue(":start_id", startNodeId);
                prepared_add_edge.bindValue(":end_id", node->id());
                prepared_add_edge.exec();
                startNodeId = NoNode;
            }
            return;
        }
        auto edge = qgraphicsitem_cast<Edge*>(item);
        if (edge != nullptr) {
            prepared_remove_edge.bindValue(":start_id", edge->startNodeId());
            prepared_remove_edge.bindValue(":end_id", edge->destNodeId());
            prepared_remove_edge.exec();
            removeItem(edge);
            delete edge;
        }
    };
    if ((mouseEvent->buttons() & Qt::MouseButton::RightButton)) {
        handle(itemAt(mouseEvent->scenePos(), {}));
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphScene::timerEvent(QTimerEvent*)
{
    for (const auto& [_, node] : nodes) {
        node->calculateForces();
    }
    bool itemsMoved = false;
    for (const auto& [_, node] : nodes) {
        if (node->advancePosition()) {
            itemsMoved = true;
        }
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}

void GraphScene::nodeMoved()
{
    if (!timerId) {
        timerId = startTimer(1000 / 25);
    }
}

void GraphScene::addEdge(int startId, int finishId, int label) {
    auto e = new Edge{nodes[startId], nodes[finishId], label};
    addItem(e);
    edges[{startId, finishId}] = e;
}

void GraphScene::updateEdge(int startId, int finishId, int label) {
    Q_ASSERT(nodes.find(startId) != nodes.end() && nodes.find(finishId) != nodes.end());
    auto it = edges.find({startId, finishId});
    // add?
    if (it == edges.end()) {
        addEdge(startId, finishId, label);
        return;
    }
    // update?
    if (label != 0) {
        it->second->setLabel(label);
        return;
    }
    // remove
    removeItem(it->second);
    delete it->second;
    edges.erase(it);
}

void GraphScene::highlightEdge(int startId, int finishId) {
    auto it = edges.find({startId, finishId});
    if (it == edges.end()) {
        it = edges.find({finishId, startId});
    }
    Q_ASSERT(it != edges.end());
    (*it).second->highlight();
}
