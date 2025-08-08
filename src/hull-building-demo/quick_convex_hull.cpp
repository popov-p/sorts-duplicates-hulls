#include "quick_convex_hull.h"
#include <QDebug>
#include <QThreadPool>

QuickConvexHullAlgorithm::QuickConvexHullAlgorithm(QObject* parent)
    : QObject(parent) {};

QList<QPointF> QuickConvexHullAlgorithm::result() const {
    return _hull.values();
}

void QuickConvexHullAlgorithm::compute(const QVector<QPointF>& points) {
    _hull.clear();
    _data = points;

    if (_data.size() < 3) {
        qWarning() << "Convex hull not possible (less than 3 points)";
        emit finished(QList<QPointF>{});
        return;
    }

    int minX = 0, maxX = 0;
    for (int i = 1; i < _data.size(); ++i) {
        if (_data[i].x() < _data[minX].x()) minX = i;
        if (_data[i].x() > _data[maxX].x()) maxX = i;
    }

    QThreadPool* pool = QThreadPool::globalInstance();
    pool->setMaxThreadCount(QThread::idealThreadCount());

    pool->start(new QuickConvexHullTask(this, _data, _data[minX], _data[maxX], 1));
    pool->start(new QuickConvexHullTask(this, _data, _data[minX], _data[maxX], -1));

    pool->waitForDone();

    emit finished(_hull.values());
}

void QuickConvexHullAlgorithm::quickHullParallel(const QVector<QPointF>& points, const QPointF& p1, const QPointF& p2, int side)
{
    int idx = -1;
    int max_dist = 0;

    for (int i = 0; i < points.size(); ++i) {
        int dist = lineDist(p1, p2, points[i]);
        if (findSide(p1, p2, points[i]) == side && dist > max_dist) {
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
    : parent(parent), points(pts), p1(p1), p2(p2), side(side)
{
    setAutoDelete(true);
}

void QuickConvexHullAlgorithm::QuickConvexHullTask::run()
{
    parent->quickHullParallel(points, p1, p2, side);
}
