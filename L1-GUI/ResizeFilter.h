//
// Created by mhq on 25/03/23.
//

#pragma once


#include <QObject>

class ResizeFilter : public QObject
{
public:
    using QObject::QObject;
protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};