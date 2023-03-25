#pragma once

#include <QStyledItemDelegate>

class AmountDelegate  : public QStyledItemDelegate
{
public:
	AmountDelegate(QObject *parent = nullptr);
	~AmountDelegate() = default;
public:
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
