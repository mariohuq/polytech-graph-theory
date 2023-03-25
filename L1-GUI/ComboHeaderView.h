#pragma once

#include <QHeaderView>
#include "CheckableComboBox.h"

class QStandardItemModel;

class ComboHeaderView : public QHeaderView
{
public:
	ComboHeaderView(CheckableComboBox* editor, QWidget *parent = nullptr);
	~ComboHeaderView() = default;
	void setModelForColumn(int column, QStandardItemModel* source);
private:
	void setFilter();
private:
	std::vector<QStandardItemModel*> models;
	std::vector<QSet<QString>> filters;
	CheckableComboBox* editor;
};
