#pragma once
#include <QObject>
#include <QSet>
#include <QMutex>
#include <QPointF>
#include <QRunnable>

#include <QHash>

inline uint qHash(const QPointF &key, uint seed = 0)
{
    return qHash(key.x(), seed) ^ qHash(key.y(), seed << 1);
}

class QuickConvexHullAlgorithm : public QObject {
    Q_OBJECT
signals:
    void finished(const QList<QPointF>& hull_points);
public:
    explicit QuickConvexHullAlgorithm(QObject* parent = nullptr);
    ~QuickConvexHullAlgorithm() = default;
    void compute(const QVector<QPointF>& points);

    QList<QPointF> result() const;
private:
    static int findSide(const QPointF& p1, const QPointF& p2, const QPointF& p)
    {
        int val = (p.y() - p1.y()) * (p2.x() - p1.x()) - (p2.y() - p1.y()) * (p.x() - p1.x());
        if (val > 0) return 1;
        if (val < 0) return -1;
        return 0;
    }

    static int lineDist(const QPointF& p1, const QPointF& p2, const QPointF& p)
    {
        return std::abs((p.y() - p1.y()) * (p2.x() - p1.x()) - (p2.y() - p1.y()) * (p.x() - p1.x()));
    }

    void quickHullParallel(const QVector<QPointF>& points, const QPointF& p1, const QPointF& p2, int side);

    class QuickConvexHullTask : public QRunnable
    {
    public:
        QuickConvexHullTask(QuickConvexHullAlgorithm* parent, const QVector<QPointF>& pts, const QPointF& p1, const QPointF& p2, int side);
        void run() override;

    private:
        QuickConvexHullAlgorithm* parent;
        QVector<QPointF> points;
        QPointF p1, p2;
        int side;
    };

    QVector<QPointF> _data;
    QSet<QPointF> _hull;
    QMutex _hull_mutex;
};
