#pragma once

#include <QComboBox>

class QStandardItemModel;

/*
* Требует использования модели QStandardItemModel.
*/
class CheckableComboBox : public QComboBox
{
public:
	explicit CheckableComboBox(QWidget *parent = nullptr);
	// скрывает метод QComboBox! QStandardItemModel - более специфичный класс
	void setModel(QStandardItemModel* model);
};
