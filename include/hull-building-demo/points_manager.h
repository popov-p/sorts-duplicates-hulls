#pragma once

#include <QObject>
#include <QSharedPointer>

class PointsDataManager : public QObject
{
    Q_OBJECT
public:
    explicit PointsDataManager(QObject* parent = nullptr);

    QSharedPointer<QVector<QPointF>> points() const;
    bool loadPoints(const QString& file_path);
    bool savePoints(const QString& file_path, const QVector<QPointF>& points);
private:
    QSharedPointer<QVector<QPointF>> _points;
};
