#include "concave_hull.h"
#include <QDebug>

ConcaveHullAlgorithm::ConcaveHullAlgorithm(QObject* parent) : IHullAlgorithm(parent) {

};

void ConcaveHullAlgorithm::compute(const QVector<QPointF>& points, const QVector<QPointF>& convex_hull, const qreal gamma) {
    if (convex_hull.isEmpty()) {
        qInfo() << "convex_hull is empty, exiting.";
        return;
    }

    QVector<QPointF> H = convex_hull;
    QVector<QPointF> G;

    for (const QPointF& pt : points) {
        if (!H.contains(pt))
            G.append(pt);
    }

    qInfo() << "Initial convex hull points H:" << H;
    qInfo() << "Points outside convex hull G:" << G;

    while (true) {
        int m = indexMaxSide(H);
        QPointF pb = H[m];
        QPointF pe = H[(m + 1) % H.size()];

        qInfo() << "\nProcessing edge between points at indices" << m << "и" << (m + 1) % H.size();
        qInfo() << "pb:" << pb << ", pe:" << pe;

        int jpt = -1;
        double qd0 = squaredDist(pb, pe);
        double Smin = 4 * qd0;

        qInfo() << "qd0 (sq distance between pb and pe):" << qd0;
        qInfo() << "Initial Smin:" << Smin;

        for (int i = 0; i < G.size(); ++i) {
            const QPointF& pt = G[i];
            double qd1 = squaredDist(pb, pt);
            double qd2 = squaredDist(pe, pt);

            if (qd1 + qd2 - qd0 > gamma * std::min(qd1, qd2)) {
                qInfo() << "Skipping point" << pt << "due to distance condition.";
                continue;
            }

            double St = triangleArea(pb, pt, pe);

            qInfo() << "Checking point" << pt << ": St =" << St;

            if (St < Smin) {
                if (isCrossHull(pb, pt, H)) {
                    qInfo() << "Skipping point" << pt << "because it crosses hull.";
                    continue;
                }
                jpt = i;
                Smin = St;
                qInfo() << "Point" << pt << "is a new candidate with Smin =" << Smin;
            }
        }

        if (jpt >= 0) {
            qInfo() << "Inserting point" << G[jpt] << "into hull at position" << m + 1;
            H.insert(m + 1, G[jpt]);
            G.removeAt(jpt);

            qInfo() << "Updated hull points H:" << H;
            qInfo() << "Remaining points G:" << G;
        } else {
            qInfo() << "No suitable point found for insertion, finishing.";
            break;
        }
    }

    _concave_hull = {H.begin(), H.end()};
    qInfo() << "Final concave hull points:" << _concave_hull;
    emit finished(QSet<QPointF>(H.begin(), H.end()));
}

