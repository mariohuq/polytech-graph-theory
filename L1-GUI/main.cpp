//
// Created by mhq on 25/03/23.
//

#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QLabel label{ "Hello Qt!\nЗдравствуй, Qt!" };
    label.setMinimumSize({ 200, 200 });
    label.setAlignment(Qt::AlignCenter);
    label.show();
    return a.exec();
}