/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "edge.h"
#include "node.h"
#include "GraphScene.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <utility>

static QRectF SHAPE{ -10, -10, 20, 20 };

Node::Node(int id, QString type)
    : m_id{id}
    , m_type{std::move(type)} {
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}

inline Node::~Node() {
    for (auto e : m_edges)
    {
        if (e->m_source == this) {
            e->m_source = nullptr;
        }
        if (e->m_destination == this) {
            e->m_destination = nullptr;
        }
    }
}

void Node::calculateForces()
{
    if (!scene() || scene()->mouseGrabberItem() == this) {
        m_new_pos = pos();
        return;
    }
    // Sum up all forces pushing this item away
    QPointF velocity{0, 0};
    for (const auto& [_, node] : scene()->nodes) {
        if (!node->isVisible()) {
            continue;
        }
        QPointF delta = mapToItem(node, 0, 0);
        double l = 2.0 * QPointF::dotProduct(delta, delta);
        if (l > 0) {
            velocity += delta * 150.0 / l;
        }
    }

    // Now subtract all forces pulling items together
    double weight = static_cast<double>(m_edges.size() + 1) * 10;
    for (Edge *edge : m_edges) {
        if (!edge->m_source->isVisible() || !edge->m_source->isVisible()) {
            continue;
        }
        velocity -= mapToItem((edge->m_source == this ? edge->m_destination : edge->m_source), 0, 0) / weight;
    }

    if (qAbs(velocity.x()) < 0.5 && qAbs(velocity.y()) < 0.5) {
        velocity = { 0, 0 };
    }
    QRectF boundary = scene()->sceneRect().marginsRemoved({10, 10, 10, 10});
    m_new_pos = pos() + velocity;
    m_new_pos.setX(qMin(qMax(m_new_pos.x(), boundary.left()), boundary.right()));
    m_new_pos.setY(qMin(qMax(m_new_pos.y(), boundary.top()), boundary.bottom()));
}

bool Node::advancePosition()
{
    if (m_new_pos == pos())
        return false;

    setPos(m_new_pos);
    return true;
}

QRectF Node::boundingRect() const
{
    qreal adjust = 2;
    return SHAPE.marginsAdded({ adjust, adjust, adjust, adjust });
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(SHAPE);
    return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setBrush({ Qt::white });
    painter->drawEllipse(SHAPE.toAlignedRect());
    painter->setBrush(Qt::NoBrush);
    Q_ASSERT(((void)"Too many vertices, can't show label properly", 'a' + m_id <= 'z'));
    auto delta = option->fontMetrics.boundingRect('a' + m_id).center();
    painter->drawText(SHAPE.center() - delta, QString{ 'a' + m_id });
    if (isSelected()) {
        painter->setPen(QPen{{Qt::blue},1,Qt::DashLine});
        painter->drawRoundedRect(SHAPE.marginsAdded({ 2,2,2,2 }), 3, 3, Qt::RelativeSize);
    }
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        for (Edge *edge : m_edges) {
            edge->adjust();
        }
        scene()->nodeMoved();
        break;
    default:
        break;
    }
    return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

GraphScene *Node::scene() const {
    return dynamic_cast<GraphScene*>(QGraphicsItem::scene());
}

void Node::setType(QString type) {
    if (type != m_type) {
        show();
        prepareGeometryChange();
        m_type = std::move(type);
    }
}
