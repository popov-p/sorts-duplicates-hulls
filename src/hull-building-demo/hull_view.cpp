#include "hull_view.h"
#include <QWheelEvent>


void HullView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        viewport()->setCursor(Qt::ArrowCursor);
        QGraphicsView::mousePressEvent(event);
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void HullView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        setDragMode(QGraphicsView::NoDrag);
        viewport()->setCursor(Qt::ArrowCursor);
        QGraphicsView::mouseReleaseEvent(event);
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void HullView::wheelEvent(QWheelEvent* event){
    constexpr double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

HullView::HullView(QWidget* parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this))
{
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(100.0, 100.0);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void HullView::addPoint(const QPointF& point, qreal circle_radius)
{   
    QGraphicsEllipseItem* ellipse = scene->addEllipse(
        point.x() - circle_radius, point.y() - circle_radius,
        circle_radius*0.008, circle_radius*0.008,
        Qt::NoPen, QBrush(Qt::blue));
    points.append(ellipse);
}

void HullView::connectPoints()
{
    clearLines();

    if (points.size() < 2)
        return;

    for (int i = 0; i < points.size() - 1; ++i)
    {
        QPointF p1 = points[i]->rect().center() + points[i]->pos();
        QPointF p2 = points[i+1]->rect().center() + points[i+1]->pos();
        QGraphicsLineItem* line = scene->addLine(QLineF(p1, p2), QPen(Qt::red, 0.005));
        lines.append(line);
    }
}

void HullView::clearAll()
{
    for (auto item : points)
        scene->removeItem(item);
    points.clear();

    clearLines();
}
