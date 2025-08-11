#include "hull_view.h"
#include <hull_algorithm.h>

#include <QWheelEvent>
#include <QMessageBox>
#include <cmath>

const int HullView::points_presented() const {
    return _points.length();
}

bool HullView::pointsAreVisible(QGraphicsScene* scene) {
    for (QGraphicsItem* item : scene->items())
        if (item->isVisible())
            return true;
    return false;
}

void HullView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setDragMode(QGraphicsView::ScrollHandDrag);
        viewport()->setCursor(Qt::ArrowCursor);
        QGraphicsView::mousePressEvent(event);
    } else
        QGraphicsView::mousePressEvent(event);
}

void HullView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setDragMode(QGraphicsView::NoDrag);
        viewport()->setCursor(Qt::ArrowCursor);
        QGraphicsView::mouseReleaseEvent(event);
    } else
        QGraphicsView::mouseReleaseEvent(event);
}

void HullView::wheelEvent(QWheelEvent* event) {
    const double scale_factor = 1.15;
    if (event->angleDelta().y() > 0)
        scale(scale_factor, scale_factor);
    else
        scale(1.0 / scale_factor, 1.0 / scale_factor);
}

HullView::HullView(QWidget* parent)
    : QGraphicsView(parent), _scene(new QGraphicsScene(this)) {
    setScene(_scene);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    resetTransform();
    scale(70.0, 70.0);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void HullView::addPoint(const QPointF& point, qreal circle_radius) {
    qreal r = circle_radius;

    QGraphicsEllipseItem* ellipse = _scene->addEllipse(
        point.x() - r, point.y() - r,
        2*r, 2*r,
        Qt::NoPen, QBrush(Qt::blue));

    _points.append(ellipse);
}

void HullView::connectPoints(const QSet<QPointF>& hull_points, const QColor& color, qreal width) {
    if (hull_points.size() < 2)
        return;

    QPointF center(0, 0);
    for (const QPointF& p : hull_points) {
        center += p;
    }
    center /= hull_points.size();

    QVector<QPointF> sorted_points = HullAlgorithm::sortPointsClockwise(hull_points);

    for (int i = 0; i < sorted_points.size(); ++i)
    {
        QPointF p1 = sorted_points[i];
        QPointF p2 = sorted_points[(i + 1) % sorted_points.size()];
        QGraphicsLineItem* line = _scene->addLine(QLineF(p1, p2), QPen(color, width));
        _lines.append(line);
    }
}

void HullView::clearAll() {
    clearLines();
    clearPoints();
}

void HullView::clearLines() {
    for (auto* line : _lines) {
        _scene->removeItem(line);
        delete line;
    }
    _lines.clear();
}

void HullView::clearPoints()
{
    for (auto* ellipse : _points) {
        _scene->removeItem(ellipse);
        delete ellipse;
    }
    _points.clear();
}

