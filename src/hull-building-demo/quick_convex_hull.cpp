#include "quick_convex_hull.h"

#include <QDebug>
#include <QThreadPool>
#include <QString>

QuickConvexHullAlgorithm::QuickConvexHullAlgorithm(QObject* parent)
    : HullAlgorithm(parent) {};

void QuickConvexHullAlgorithm::compute(const QVector<QPointF>& points) {
    _hull.clear();

    if (points.size() < 3) {
        qWarning() << "Convex hull not possible (less than 3 points)";
        emit finished({});
        return;
    }

    int min_x = 0, max_x = 0;
    for (int i = 1; i < points.size(); ++i) {
        if (points[i].x() < points[min_x].x()) min_x = i;
        if (points[i].x() > points[max_x].x()) max_x = i;
    }

    QThreadPool* pool = QThreadPool::globalInstance();
    int old_max_threads = pool->maxThreadCount();
    pool->setMaxThreadCount(QThread::idealThreadCount());

    pool->start(new QuickConvexHullTask(this, points, points[min_x], points[max_x], 1));
    pool->start(new QuickConvexHullTask(this, points, points[min_x], points[max_x], -1));

    pool->waitForDone();

    pool->setMaxThreadCount(old_max_threads);

    emit finished(_hull);
}

void QuickConvexHullAlgorithm::quickHullParallelImpl(
    const QVector<QPointF>& points, const QPointF& p1, const QPointF& p2, int side)
{
    int idx = -1;
    qreal max_dist = 0.0;

    for (int i = 0; i < points.size(); ++i) {
        qreal dist = lineDist(p1, p2, points[i]);
        int side_of_point = findSide(p1, p2, points[i]);
        if (side_of_point == side && dist > max_dist) {
            idx = i;
            max_dist = dist;
        }
    }
    if (idx == -1) {
        QMutexLocker locker(&_hull_mutex);
        _hull.insert(p1);
        _hull.insert(p2);
        return;
    }

    QThreadPool* pool = QThreadPool::globalInstance();
    pool->start(new QuickConvexHullTask(this, points, points[idx], p1, -findSide(points[idx], p1, p2)));
    pool->start(new QuickConvexHullTask(this, points, points[idx], p2, -findSide(points[idx], p2, p1)));
}

QuickConvexHullAlgorithm::QuickConvexHullTask::QuickConvexHullTask(QuickConvexHullAlgorithm* parent, const QVector<QPointF>& pts, const QPointF& p1, const QPointF& p2, int side)
    : _parent(parent), _points(pts), _p1(p1), _p2(p2), _side(side)
{
    setAutoDelete(true);
}

void QuickConvexHullAlgorithm::QuickConvexHullTask::run()
{
    _parent->quickHullParallelImpl(_points, _p1, _p2, _side);
}
