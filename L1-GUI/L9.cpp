#include "Editor.h"
#include "Lab1.h"

#include <QtWidgets>
#include <QSqlDatabase>
#include <QSqlQuery>

// adds default connection and opens db
bool connect(const char* const filename) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filename);
    return db.open();
}

void initDatabase() {
    QSqlQuery(R"(
        create table if not exists Nodes (
          id integer primary key,
          type text check( type in ('ellipse', 'rectangle', 'triangle', 'hexagon') ) not null
        )
    )");
    QSqlQuery(R"(
        create table if not exists Edges (
          start_id integer,
          end_id integer,
          weight integer DEFAULT 1,
          primary key (start_id, end_id),
          foreign key (start_id) references Nodes(id) ON DELETE CASCADE,
          foreign key (end_id) references Nodes(id) ON DELETE CASCADE
        ) without rowid
    )");
    QSqlQuery("PRAGMA foreign_keys = ON");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (!connect("shapes.sqlite"))
    {
        qDebug("Error opening db");
        return EXIT_FAILURE;
    }
    initDatabase();
    Lab1 window;
    window.show();
    return a.exec();
}
