#include "points_manager.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QPointF>

QSharedPointer<QVector<QPointF>> PointsDataManager::points() const { return _points; }
PointsDataManager::PointsDataManager(QObject* parent) : QObject(parent) {}

bool PointsDataManager::loadPoints(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for reading:" << filePath;
        return false;
    }

    auto loadedPoints = QSharedPointer<QVector<QPointF>>::create();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() != 2)
            continue;

        bool okX = false, okY = false;
        double x = parts[0].toDouble(&okX);
        double y = parts[1].toDouble(&okY);
        if (okX && okY)
            loadedPoints->append(QPointF(x, y));
    }

    file.close();

    if (!loadedPoints->isEmpty()) {
        _points = loadedPoints;
        return true;
    }

    return false;
}
