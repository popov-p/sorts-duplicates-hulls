#include "quick_convex_hull.h"
#include <QDebug>
#include <QThreadPool>
#include <QString>

QuickConvexHullAlgorithm::QuickConvexHullAlgorithm(QObject* parent)
    : QObject(parent) {};

QList<QPointF> QuickConvexHullAlgorithm::result() const {
    return _convex_hull.values();
}

void QuickConvexHullAlgorithm::compute(const QVector<QPointF>& points) {
    _convex_hull.clear();
    _data = points;

    if (_data.size() < 3) {
        qWarning() << "Convex hull not possible (less than 3 points)";
        emit finished({},{});
        return;
    }

    int minX = 0, maxX = 0;
    for (int i = 1; i < _data.size(); ++i) {
        if (_data[i].x() < _data[minX].x()) minX = i;
        if (_data[i].x() > _data[maxX].x()) maxX = i;
    }

    qInfo() << "Leftmost point:" << _data[minX];
    qInfo() << "Rightmost point:" << _data[maxX];

    QThreadPool* pool = QThreadPool::globalInstance();
    int oldMaxThreads = pool->maxThreadCount();
    pool->setMaxThreadCount(QThread::idealThreadCount());

    pool->start(new QuickConvexHullTask(this, _data, _data[minX], _data[maxX], 1));
    pool->start(new QuickConvexHullTask(this, _data, _data[minX], _data[maxX], -1));

    pool->waitForDone();

    pool->setMaxThreadCount(oldMaxThreads);

    qInfo() << "_convex_hull:" << _convex_hull;
    qInfo() << "_points:" << _data;
    emit finished(points, _convex_hull);
}

void QuickConvexHullAlgorithm::quickHullParallelImpl(
    const QVector<QPointF>& points, const QPointF& p1, const QPointF& p2, int side)
{
    qInfo() << "quickHullParallelImpl called with side" << side
            << ", segment points:" << p1 << p2;

    int idx = -1;
    qreal max_dist = 0.0;

    for (int i = 0; i < points.size(); ++i) {
        qreal dist = lineDist(p1, p2, points[i]);
        int sideOfPoint = findSide(p1, p2, points[i]);
        if (sideOfPoint == side && dist > max_dist) {
            qInfo() << "Distance from point" << points[i] << "to line segment" << p1 << "-" << p2
                    << "is" << dist << ", side =" << sideOfPoint;
            idx = i;
            max_dist = dist;
        }
    }
    if (idx == -1) {
        QMutexLocker locker(&_hull_mutex);
        _convex_hull.insert(p1);
        _convex_hull.insert(p2);
        qInfo() << "No further points on side" << side << ". Added points to hull:"
                << p1 << p2;
        return;
    }

    qInfo() << "Found farthest point at index" << idx << ":" << points[idx]
            << "distance:" << max_dist;

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
