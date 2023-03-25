#pragma once

#include <QWidget>
#include "ui_Main.h"
#include <QSqlQuery>

class Main : public QWidget
{
	Q_OBJECT

public:
	Main(QWidget *parent = nullptr);
	~Main() = default;
private:
	Ui::main ui;
};
