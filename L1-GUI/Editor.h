#pragma once

#include <QWidget>
#include "ui_Editor.h"
#include <QSqlQuery>

class Editor : public QWidget
{
	Q_OBJECT

public:
	Editor(QWidget *parent = nullptr);
	~Editor() = default;
private:
	Ui::editor ui;
};
