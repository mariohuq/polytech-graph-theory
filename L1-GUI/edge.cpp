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

#include <qmath.h>
#include <QPainter>

Edge::Edge(Node *sourceNode, Node *destNode)
    : arrowSize{ 10 }
    , m_source{ sourceNode }
    , m_destination{ destNode }
{
    setAcceptedMouseButtons(Qt::NoButton);
    m_source->m_edges.insert(this);
    m_destination->m_edges.insert(this);
    setZValue(-2);
    adjust();
}

int Edge::startNodeId() const {
    return m_source->id();
}

int Edge::destNodeId() const {
    return m_destination->id();
}

void Edge::adjust()
{
    if (!m_source || !m_destination)
        return;

    QLineF line(mapFromItem(m_source, 0, 0), mapFromItem(m_destination, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();
    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = line.p1();
    }
}

inline Edge::~Edge() {
    if (m_source) {
        m_source->m_edges.erase(this);
    }
    if (m_destination) {
        m_destination->m_edges.erase(this);
    }
}

QRectF Edge::boundingRect() const
{
    if (!m_source || !m_destination) {
        return QRectF{};
    }

    qreal penWidth = 1;
    qreal extra = (penWidth + arrowSize) / 2.0;
    return QRectF{ sourcePoint, destPoint }
        .normalized()
        .marginsAdded({ extra, extra, extra, extra });
}

QPainterPath Edge::shape() const
{
    QLineF line(sourcePoint, destPoint);
    auto base = line.pointAt(1 - arrowSize / line.length());
    auto d = destPoint - base;
    d = { d.y(), -d.x() };
    auto path = QPainterPath{};
    path.addPolygon(QVector<QPointF>{
        sourcePoint + d, sourcePoint - d, destPoint - d, destPoint + d
    });
    return path;
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!m_source || !m_destination) {
        return;
    }
    if (!m_source->isVisible() || !m_destination->isVisible()) {
        return;
    }

    QLineF line(sourcePoint, destPoint);
    auto segmentLength = line.length();
    if (qFuzzyCompare(segmentLength, qreal(0.)))
        return;

    // Draw the line itself
    painter->setPen({ Qt::black, 1.4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin });
    painter->drawLine(line);
    // arrow head (namely stealth arrow)
    auto base = line.pointAt(1 - arrowSize / segmentLength);
    auto d = (destPoint - base) / 2;
    painter->setBrush(Qt::black);
    painter->drawPolygon(QVector<QPointF> {
        destPoint,
        base + QPointF{ -d.y(), d.x() },
        line.pointAt(1 - 0.75 * arrowSize / segmentLength),
        base + QPointF{ d.y(), -d.x() }
    });
}
