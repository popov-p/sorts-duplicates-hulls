#include "concave_hull.h"
#include <QDebug>
#include <QFuture>
#include <QtConcurrent>

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

    // qInfo() << "Initial convex hull points H:" << H;
    // qInfo() << "Points outside convex hull G:" << G;

    while (true) {
        int m = indexMaxSide(H);
        QPointF pb = H[m];
        QPointF pe = H[(m + 1) % H.size()];

        // qInfo() << "\nProcessing edge between points at indices" << m << "и" << (m + 1) % H.size();
        // qInfo() << "pb:" << pb << ", pe:" << pe;

        double qd0 = squaredDist(pb, pe);
        double Smin = 4 * qd0;

        // qInfo() << "qd0 (sq distance between pb and pe):" << qd0;
        // qInfo() << "Initial Smin:" << Smin;

        if (G.isEmpty()) {
            // qInfo() << "No points outside hull remain, finishing.";
            break;
        }

        QVector<QFuture<Candidate>> futures;
        for (int i = 0; i < G.size(); ++i) {
            auto task = [=, &G, &H]() -> Candidate {
                return checkCandidate(i, G, pb, pe, gamma, H, Smin);
            };
            futures.append(QtConcurrent::run(task));
        }

        for (auto& f : futures) {
            f.waitForFinished();
        }

        Candidate bestCandidate{-1, Smin, false};
        for (int i = 0; i < futures.size(); ++i) {
            Candidate c = futures[i].result();
            if (c.valid && c.Smin < bestCandidate.Smin) {
                bestCandidate = c;
            }
        }

        if (bestCandidate.valid) {
            // qInfo() << "Inserting point" << G[bestCandidate.index] << "into hull at position" << m + 1;
            H.insert(m + 1, G[bestCandidate.index]);
            G.removeAt(bestCandidate.index);

            // qInfo() << "Updated hull points H:" << H;
            // qInfo() << "Remaining points G:" << G;
        } else {
            // qInfo() << "No suitable point found for insertion, finishing.";
            break;
        }
    }

    _concave_hull = {H.begin(), H.end()};
    // qInfo() << "Final concave hull points:" << _concave_hull;
    emit finished(QSet<QPointF>(H.begin(), H.end()));
}

