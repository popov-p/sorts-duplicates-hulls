#include "points_manager.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QPointF>

QSharedPointer<QVector<QPointF>> PointsDataManager::points() const { return _points; }
PointsDataManager::PointsDataManager(QObject* parent) : QObject(parent) {}

bool PointsDataManager::loadPoints(const QString& file_path)
{
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for reading:" << file_path;
        return false;
    }

    auto loaded_points = QSharedPointer<QVector<QPointF>>::create();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() != 2)
            continue;

        bool ok_x = false, ok_y = false;
        double x = parts[0].toDouble(&ok_x);
        double y = parts[1].toDouble(&ok_y);
        if (ok_x && ok_y)
            loaded_points->append(QPointF(x, y));
    }

    file.close();

    if (!loaded_points->isEmpty()) {
        _points = loaded_points;
        return true;
    }

    return false;
}
