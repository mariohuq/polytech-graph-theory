#include "CheckableComboBox.h"

#include <QAbstractItemView>
#include <QStandardItemModel>

CheckableComboBox::CheckableComboBox(QWidget *parent)
	: QComboBox(parent)
{
	QObject::connect(view(), &QAbstractItemView::pressed, [=](const QModelIndex& index) {
		auto m = qobject_cast<QStandardItemModel*>(model());
		Q_ASSERT(m != nullptr);
		QStandardItem* item = m->itemFromIndex(index);
		item->setCheckState(item->checkState() ? Qt::Unchecked : Qt::Checked);
	});
}

void CheckableComboBox::setModel(QStandardItemModel* model)
{
	QComboBox::setModel(model);
}
