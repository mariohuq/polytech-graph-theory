//
// Created by mhq on 02/06/23.
//

#pragma once
#include <QTableView>

class MyTableView : public QTableView {
public:
    using QTableView::QTableView;
protected:
    void keyPressEvent(QKeyEvent *event) override;
};
