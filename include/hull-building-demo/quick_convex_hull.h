#pragma once

#include "hull_algorithm.h"

#include <QObject>
#include <QSet>
#include <QMutex>
#include <QPointF>
#include <QRunnable>
#include <QHash>

class QuickConvexHullAlgorithm : public HullAlgorithm {
    Q_OBJECT
public:
    explicit QuickConvexHullAlgorithm(QObject* parent = nullptr);
    ~QuickConvexHullAlgorithm() = default;
    void compute(const QVector<QPointF>& points);
private:
    static int findSide(const QPointF& p1, const QPointF& p2, const QPointF& p) {
        qreal val = (p.y() - p1.y()) * (p2.x() - p1.x()) - (p2.y() - p1.y()) * (p.x() - p1.x());
        if (val > 0)
            return 1;
        else if (val < 0)
            return -1;
        else
            return 0;
    }

    static qreal lineDist(const QPointF& p1, const QPointF& p2, const QPointF& p)
    {
        return std::abs((p.y() - p1.y()) * (p2.x() - p1.x()) - (p2.y() - p1.y()) * (p.x() - p1.x()));
    }

    void quickHullParallelImpl(const QVector<QPointF>& points, const QPointF& p1, const QPointF& p2, int side);

    class QuickConvexHullTask : public QRunnable
    {
    public:
        QuickConvexHullTask(QuickConvexHullAlgorithm* parent, const QVector<QPointF>& pts, const QPointF& p1, const QPointF& p2, int side);
        void run() override;

    private:
        QuickConvexHullAlgorithm* _parent;
        const QVector<QPointF>& _points;
        QPointF _p1, _p2;
        int _side;
    };

    QMutex _hull_mutex;
};
