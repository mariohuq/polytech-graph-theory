#pragma once

#include <QGraphicsScene>
#include <QSqlQuery>
#include <set>

class Node;
class Edge;

class GraphScene : public QGraphicsScene
{
	Q_OBJECT
public:
	explicit GraphScene(const QRectF& sceneRect, QObject* parent = nullptr);

	void updateNode(int id, const QString& type);
    void addEdge(int startId, int finishId, int label = 0);
    void updateEdge(int startId, int finishId, int label);
	void hideNode(int id);
	void showNode(int id);
	void setSelectedNode(int id, bool selected);
	void reset(const QString& filter = "");

	~GraphScene() override = default;
protected:
	void keyPressEvent(QKeyEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
	void timerEvent(QTimerEvent*) override;
private:
	void nodeMoved();
	void placeRandom();
	friend Node;
	std::map<int, Node*> nodes;
    std::map<std::pair<int, int>, Edge*> edges;
	int timerId;
	enum NodeId { NoNode = -1 };
	int startNodeId;
	QSqlQuery prepared_add_edge;
	QSqlQuery prepared_remove_edge;
};
