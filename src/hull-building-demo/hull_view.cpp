#include "hull_view.h"

#include <QWheelEvent>
#include <QDebug>
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
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void HullView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setDragMode(QGraphicsView::NoDrag);
        viewport()->setCursor(Qt::ArrowCursor);
        QGraphicsView::mouseReleaseEvent(event);
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void HullView::wheelEvent(QWheelEvent* event) {
    constexpr double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
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

void HullView::connectPoints(const QSet<QPointF>& hull_points) {
    clearLines();

    if (hull_points.size() < 2)
        return;

    QPointF center(0, 0);
    for (const QPointF& p : hull_points) {
        center += p;
    }
    center /= hull_points.size();

    QVector<QPointF> sortedPoints(hull_points.constBegin(), hull_points.constEnd());
    std::sort(sortedPoints.begin(), sortedPoints.end(), [center](const QPointF& a, const QPointF& b) {
        qreal angleA = std::atan2(a.y() - center.y(), a.x() - center.x());
        qreal angleB = std::atan2(b.y() - center.y(), b.x() - center.x());
        return angleA < angleB;
    });

    for (int i = 0; i < sortedPoints.size(); ++i)
    {
        QPointF p1 = sortedPoints[i];
        QPointF p2 = sortedPoints[(i + 1) % sortedPoints.size()];
        QGraphicsLineItem* line = _scene->addLine(QLineF(p1, p2), QPen(Qt::red, 0.04));
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

