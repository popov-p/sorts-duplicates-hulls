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
    void addPoint(const QPointF& point, qreal circleRadius = 5.0);
    void connectPoints();
    void clearAll();
protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private:
    QGraphicsScene* scene;
    QVector<QGraphicsEllipseItem*> points;
    QVector<QGraphicsLineItem*> lines;
    void clearLines()
    {
        for (auto line : lines)
            scene->removeItem(line);
        lines.clear();
    }
};
