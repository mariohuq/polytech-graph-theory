#include "AmountDelegate.h"

AmountDelegate::AmountDelegate(QObject *parent)
	:  QStyledItemDelegate(parent)
{}

int grade(int count) {
	if (count <= 3) {
		return 1;
	}
	if (count <= 10) {
		return 2;
	}
	return 3;
}

void AmountDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	static std::map<QString, QIcon> icons{
		{"ellipse", QIcon{":/ellipse"}},
		{"rectangle", QIcon{":/rectangle"}},
		{"triangle", QIcon{":/triangle"}},
		{"hexagon", QIcon{":/hexagon"}}
	};

	switch (index.column())
	{
	case 2: // Icons
	{
		int n = grade(index.model()->data(index.siblingAtColumn(1)).toInt());
		for (int i = 0; i < n; i++)
		{
			icons[index.model()->data(index.siblingAtColumn(0)).toString()].paint(painter, QRect{ option.rect.topLeft() + QPoint{20 * i + 6, 6}, QSize{20, 20} });
		}
		return;
	}
	default:
		QStyledItemDelegate::paint(painter, option, index);
	}
}


