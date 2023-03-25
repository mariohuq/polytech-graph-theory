#pragma once

#include <QGraphicsScene>
#include <QSqlQuery>

class Node;

class GraphScene : public QGraphicsScene
{
	Q_OBJECT
public:
	GraphScene(const QRectF& sceneRect, QObject* parent = nullptr);

	void updateNode(int id, QString type);
	void hideNode(int id);
	void showNode(int id);
	void setSelectedNode(int id, bool selected);
	void reset(QString filter = "");

	~GraphScene() = default;
protected:
	void keyPressEvent(QKeyEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
	void timerEvent(QTimerEvent*) override;
private:
	void nodeMoved();
	void placeRandom();
	friend Node;
	std::map<int, Node*> nodes;
	int timerId;
	enum NodeId { NoNode = -1 };
	int startNodeId;
	QSqlQuery prepared_add_edge;
	QSqlQuery prepared_remove_edge;
};
