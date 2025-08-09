#include "hull_view.h"

#include <QWheelEvent>
#include <QDebug>
#include <QMessageBox>

bool HullView::pointsAreVisible(QGraphicsScene* scene) {
    for (QGraphicsItem* item : scene->items())
        if (item->isVisible())
                return true;
    return false;
}

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
    : QGraphicsView(parent), _scene(new QGraphicsScene(this))
{
    setScene(_scene);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    resetTransform();
    scale(100.0, 100.0);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void HullView::addPoint(const QPointF& point, qreal circle_radius)
{
    qreal r = circle_radius;

    QGraphicsEllipseItem* ellipse = _scene->addEllipse(
        point.x() - r, point.y() - r,
        2*r, 2*r,
        Qt::NoPen, QBrush(Qt::blue));

    _points.append(ellipse);
}

void HullView::connectPoints(const QList<QPointF>& hull_points)
{
    clearLines();

    if (hull_points.size() < 2)
        return;

    if (_points.empty()) {
        QMessageBox::critical(
            this,
            tr("Ошибка"),
            tr("Нет точек для отображения!")
            );
        return;
    }

    for (int i = 0; i < hull_points.size() - 1; ++i)
    {
        QPointF p1 = _points[i]->rect().center() + _points[i]->pos();
        QPointF p2 = _points[i+1]->rect().center() + _points[i+1]->pos();
        QGraphicsLineItem* line = _scene->addLine(QLineF(p1, p2), QPen(Qt::red, 0.02));
        _lines.append(line);
    }
}

void HullView::clearAll()
{
    for (auto* item : _points)
        _scene->removeItem(item);
    _points.clear();

    clearLines();
}

void HullView::clearLines()
{
    for (auto line : _lines)
        _scene->removeItem(line);
    _lines.clear();
}
