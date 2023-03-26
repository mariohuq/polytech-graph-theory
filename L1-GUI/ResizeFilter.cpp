//
// Created by mhq on 25/03/23.
//

#include <QGraphicsView>
#include <QEvent>
#include <QResizeEvent>
#include "ResizeFilter.h"

bool ResizeFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Resize) {
        auto e = dynamic_cast<QResizeEvent*>(event);
        auto w = dynamic_cast<QGraphicsView*>(obj);
        w->scene()->setSceneRect(0, 0, e->size().width(), e->size().height());
    }
    return QObject::eventFilter(obj, event);
}
