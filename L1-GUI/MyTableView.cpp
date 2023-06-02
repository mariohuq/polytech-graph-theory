//
// Created by mhq on 02/06/23.
//

#include "MyTableView.h"
#include <QApplication>
#include <QKeyEvent>
#include <QClipboard>

void MyTableView::keyPressEvent(QKeyEvent *event) {
    // If Ctrl-C typed
    if (event->matches(QKeySequence::Copy))
    {
        QModelIndexList cells = selectedIndexes();
        qSort(cells); // Necessary, otherwise they are in column order

        QString text;
        int currentRow = 0; // To determine when to insert newlines
        foreach (const QModelIndex& cell, cells) {
            if (text.length() == 0) {
                // First item
            } else if (cell.row() != currentRow) {
                // New row
                text += '\n';
            } else {
                // Next cell
                text += '\t';
            }
            currentRow = cell.row();
            text += cell.data().toString();
        }
        QApplication::clipboard()->setText(text);
        return;
    }
    if(event->matches(QKeySequence::Paste)) {
         QString text = QApplication::clipboard()->text();
         QStringList rowContents = text.split("\n", QString::SkipEmptyParts);

         QModelIndex initIndex = selectedIndexes().at(0);
         auto initRow = initIndex.row();
         auto initCol = initIndex.column();

         for (auto i = 0; i < rowContents.size(); ++i) {
             QStringList columnContents = rowContents.at(i).split("\t");
             for (auto j = 0; j < columnContents.size(); ++j) {
                 model()->setData(model()->index(
                                      initRow + i, initCol + j), columnContents.at(j));
             }
         }
         return;
    }
    QTableView::keyPressEvent(event);
}
