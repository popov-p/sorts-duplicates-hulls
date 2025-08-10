#pragma once

#include <QObject>
#include <QPoint>
#include <QSet>
#include <cmath>

inline uint qHash(const QPointF &key, uint seed = 0)
{
    return qHash(qMakePair(key.x(), key.y()), seed);
}

class IHullAlgorithm : public QObject {
    Q_OBJECT
public:
    explicit IHullAlgorithm(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IHullAlgorithm() = default;

    virtual const QSet<QPointF> result() const = 0;

    static QVector<QPointF> sortPointsClockwise(const QSet<QPointF>& points) {
        if (points.isEmpty())
            return {};

        QPointF center(0, 0);
        for (const QPointF& p : points)
            center += p;

        center /= points.size();

        QVector<QPointF> sortedPoints(points.constBegin(), points.constEnd());

        std::sort(sortedPoints.begin(), sortedPoints.end(), [center](const QPointF& a, const QPointF& b) {
            qreal angleA = std::atan2(a.y() - center.y(), a.x() - center.x());
            qreal angleB = std::atan2(b.y() - center.y(), b.x() - center.x());

            return angleA > angleB;
        });

        return sortedPoints;
    }

signals:
    void finished(const QSet<QPointF>& hull_points);
protected:
    QVector<QPointF> _points;
};
