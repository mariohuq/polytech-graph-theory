#include "ComboHeaderView.h"

#include <QPaintEvent>
#include <QSqlTableModel>
#include <QStandardItemModel>
#include <QStringBuilder>

ComboHeaderView::ComboHeaderView(CheckableComboBox* editor, QWidget* parent)
	: QHeaderView(Qt::Orientation::Horizontal, parent)
	, models{}
{
	setSectionsClickable(true);
	editor->setParent(this);
	editor->hide();
	
	QObject::connect(this, &QHeaderView::sectionPressed, [=](int logicalIndex) {
		if (logicalIndex == -1)
		{
			// pressed section without logical index (e.g. on the right of the header)
			return;
		}
		Q_ASSERT(logicalIndex < models.size());
		if (models[logicalIndex] == nullptr) {
			return;
		}
		if (editor->model() != models[logicalIndex])
		{
			editor->adjustSize();
			editor->setModel(models[logicalIndex]);
			editor->setMinimumWidth(sectionSize(logicalIndex));
			editor->move(sectionPosition(logicalIndex), geometry().bottom());
		}
		editor->showPopup();
	});
}

void ComboHeaderView::setModelForColumn(int column, QStandardItemModel* source)
{
	size_t s = std::max(static_cast<size_t>(column) + 1, models.size());
	models.resize(s);
	filters.resize(s);
	models[column] = source;
	QObject::connect(source, &QStandardItemModel::itemChanged, [=](QStandardItem* item) {
		if (item->checkState())
		{
			filters[column].insert(item->data(Qt::DisplayRole).toString());
		}
		else
		{
			bool removed = filters[column].remove(item->data(Qt::DisplayRole).toString());
			if (!removed)
			{
				return;
			}
		}
		setFilter();
	});

}

// 
int accumulatedSize(const QSet<QString>& list, int separatorLen, int quoteLen)
{
	int result = 0;
	if (!list.isEmpty()) {
        result += (list.size() - 1) * separatorLen;
	}
    for (const auto& e : list)
        result += e.size();
	return result + list.size() * quoteLen;
}

// QStringList_join
QString join(const QSet<QString>& list, const QString& sep, const QString& quote = "'")
{
	QString result;
	if (!list.isEmpty()) {
		result.reserve(accumulatedSize(list, sep.size(), quote.size()));
		const auto end = list.cend();
		auto it = list.cbegin();
		result += quote;
		result += *it;
		result += quote;
		while (++it != end) {
			result += sep;
			result += quote;
			result += *it;
			result += quote;
		}
	}
	return result;
}

void ComboHeaderView::setFilter()
{
	Q_ASSERT(model() != nullptr);
	auto m = qobject_cast<QSqlTableModel*>(model());
	Q_ASSERT(m != nullptr);

	QString filter;
	if (!filters[0].isEmpty())
	{
		filter += "id in (" % join(filters[0], ",") % ")";
	}
	if (!filters[0].isEmpty() && !filters[1].isEmpty())
	{
		filter += " and ";
	}
	if (!filters[1].isEmpty())
	{
		filter += "type in (" % join(filters[1], ",") % ")";
	}
	m->setFilter(filter);
}
