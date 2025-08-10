#include "concave_hull.h"

#include <QDebug>
#include <QFuture>
#include <QtConcurrent>

ConcaveHullAlgorithm::ConcaveHullAlgorithm(QObject* parent) : IHullAlgorithm(parent) {};

void ConcaveHullAlgorithm::compute(const QVector<QPointF>& points, const QVector<QPointF>& convex_hull, const qreal gamma) {
    if (convex_hull.isEmpty()) {
        qWarning() << "convex_hull is empty, exiting.";
        return;
    }

    QVector<QPointF> H = convex_hull;
    QVector<QPointF> G;

    for (const QPointF& pt : points) {
        if (!H.contains(pt))
            G.append(pt);
    }

    while (true) {
        int m = indexMaxSide(H);
        QPointF pb = H[m];
        QPointF pe = H[(m + 1) % H.size()];

        double qd0 = squaredDist(pb, pe);
        double Smin = 4 * qd0;


        if (G.isEmpty())
            break;

        QVector<QFuture<Candidate>> futures;
        for (int i = 0; i < G.size(); ++i) {
            auto task = [=, &G, &H]() -> Candidate {
                return checkCandidate(i, G, pb, pe, gamma, H, Smin);
            };
            futures.append(QtConcurrent::run(task));
        }

        for (auto& f : futures)
            f.waitForFinished();

        Candidate bestCandidate{-1, Smin, false};
        for (int i = 0; i < futures.size(); ++i) {
            Candidate c = futures[i].result();
            if (c.valid && c.Smin < bestCandidate.Smin)
                bestCandidate = c;
        }

        if (bestCandidate.valid) {
            H.insert(m + 1, G[bestCandidate.index]);
            G.removeAt(bestCandidate.index);
        } else
            break;
    }

    _concave_hull = {H.begin(), H.end()};
    emit finished(QSet<QPointF>(H.begin(), H.end()));
}

