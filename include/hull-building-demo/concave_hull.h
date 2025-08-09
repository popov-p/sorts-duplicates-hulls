#pragma once
#include <i_algorithm.h>

#include <QObject>
#include <QDebug>

class ConcaveHullAlgorithm : public IHullAlgorithm {
    Q_OBJECT
public:
    ConcaveHullAlgorithm(QObject* parent = nullptr);

    void compute(const QVector<QPointF>& points, const QVector<QPointF>& convex_hull, const qreal gamma = 1.5);
    virtual const QSet<QPointF> result() const { return _concave_hull; }
private:
    QSet<QPointF> _concave_hull;

    static double squaredDist(const QPointF& a, const QPointF& b) {
        double dx = a.x() - b.x();
        double dy = a.y() - b.y();
        return dx*dx + dy*dy;
    }

    static double triangleArea(const QPointF& a, const QPointF& b, const QPointF& c) {
        return std::abs((b.x() - a.x())*(c.y() - a.y()) - (b.y() - a.y())*(c.x() - a.x())) / 2.0;
    }

    static int indexMaxSide(const QVector<QPointF>& hull) {
        int idx = 0;
        double maxDist = 0;
        for (int i = 0; i < hull.size(); ++i) {
            QPointF p1 = hull[i];
            QPointF p2 = hull[(i + 1) % hull.size()];
            double d = squaredDist(p1, p2);
            if (d > maxDist) {
                maxDist = d;
                idx = i;
            }
        }
        return idx;
    }

#include <QDebug>

    static bool pointsEqual(const QPointF& a, const QPointF& b) {
        const double EPS = 1e-12;
        return std::abs(a.x() - b.x()) < EPS && std::abs(a.y() - b.y()) < EPS;
    }

    static bool segmentsIntersect(const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4) {
        auto orientation = [](const QPointF& a, const QPointF& b, const QPointF& c) {
            double val = (b.y() - a.y()) * (c.x() - b.x()) - (b.x() - a.x()) * (c.y() - b.y());
            const double EPS = 1e-12;
            if (std::abs(val) < EPS) {
                qDebug() << "orientation: points are collinear:" << a << b << c;
                return 0; // коллинеарны
            }
            int result = (val > 0) ? 1 : 2;
            qDebug() << "orientation:" << a << b << c << "->" << (result == 1 ? "clockwise" : "counterclockwise");
            return result;
        };

        auto onSegment = [](const QPointF& p, const QPointF& q, const QPointF& r) {
            bool res = q.x() <= std::max(p.x(), r.x()) && q.x() >= std::min(p.x(), r.x()) &&
                       q.y() <= std::max(p.y(), r.y()) && q.y() >= std::min(p.y(), r.y());
            if (res) {
                qDebug() << "onSegment: point" << q << "lies on segment" << p << "->" << r;
            }
            return res;
        };

        qDebug() << "segmentsIntersect: проверяем пересечение сегментов:";
        qDebug() << "Сегмент 1:" << p1 << "->" << p2;
        qDebug() << "Сегмент 2:" << p3 << "->" << p4;

        int o1 = orientation(p1, p2, p3);
        int o2 = orientation(p1, p2, p4);
        int o3 = orientation(p3, p4, p1);
        int o4 = orientation(p3, p4, p2);

        // Общий случай
        if (o1 != o2 && o3 != o4) {
            qDebug() << "Общий случай ориентаций: возможное пересечение.";
            // Проверяем касание в концах — не считать пересечением
            if (pointsEqual(p1, p3)) {
                qDebug() << "Касание в вершине: p1 == p3:" << p1;
                return false;
            }
            if (pointsEqual(p1, p4)) {
                qDebug() << "Касание в вершине: p1 == p4:" << p1;
                return false;
            }
            if (pointsEqual(p2, p3)) {
                qDebug() << "Касание в вершине: p2 == p3:" << p2;
                return false;
            }
            if (pointsEqual(p2, p4)) {
                qDebug() << "Касание в вершине: p2 == p4:" << p2;
                return false;
            }

            qDebug() << "Пересечение найдено!";
            return true;
        }

        // Специальные случаи (коллинеарность)
        if (o1 == 0 && onSegment(p1, p3, p2)) {
            qDebug() << "Коллинеарный случай: точка p3 лежит на сегменте p1->p2";
            return true;
        }
        if (o2 == 0 && onSegment(p1, p4, p2)) {
            qDebug() << "Коллинеарный случай: точка p4 лежит на сегменте p1->p2";
            return true;
        }
        if (o3 == 0 && onSegment(p3, p1, p4)) {
            qDebug() << "Коллинеарный случай: точка p1 лежит на сегменте p3->p4";
            return true;
        }
        if (o4 == 0 && onSegment(p3, p2, p4)) {
            qDebug() << "Коллинеарный случай: точка p2 лежит на сегменте p3->p4";
            return true;
        }

        qDebug() << "Пересечений не найдено.";
        return false;
    }


    static bool isCrossHull(const QPointF& pb, const QPointF& pt, const QVector<QPointF>& hull) {
        for (int i = 0; i < hull.size(); ++i) {
            QPointF h1 = hull[i];
            QPointF h2 = hull[(i + 1) % hull.size()];

            if ((h1 == pb && h2 == pt) || (h1 == pt && h2 == pb))
                continue;

            bool cross = segmentsIntersect(pb, pt, h1, h2);
            if (cross) {
                qInfo() << "isCrossHull: пересечение с ребром hull:" << h1 << "->" << h2;
                qInfo() << "Проверяется сегмент:" << pb << "->" << pt;
                return true;
            }
        }
        return false;
    }
};
