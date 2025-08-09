#pragma once


#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QVector>
#include <QPointF>

class HullView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit HullView(QWidget* parent = nullptr);
public slots:
    bool pointsAreVisible(QGraphicsScene* scene);
    void addPoint(const QPointF& point, qreal circleRadius = 0.02);
    void connectPoints(const QList<QPointF>&);
    void clearAll();
protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private:
    QGraphicsScene* _scene;
    QVector<QGraphicsEllipseItem*> _points;
    QVector<QGraphicsLineItem*> _lines;
    void clearLines();
};
