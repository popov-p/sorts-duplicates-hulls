#include "hull_building_demo.h"
#include "ui_main_window.h"
#include "hull_view.h"
#include "quick_convex_hull.h"
#include "points_manager.h"
#include "concave_hull.h"
#include <QPushButton>
#include <QDebug>
#include <QElapsedTimer>

QuickConvexHullAlgorithm* HullBuildingDemo::algorithm() { return _convex_algorithm; }

HullBuildingDemo::HullBuildingDemo(HullConfig config, QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow) {
    _ui->setupUi(this);
    _view = new HullView(this);
    _ui->verticalLayout->addWidget(_view);

    _data_manager = new PointsDataManager(this);
    if(!_data_manager->loadPoints(config.file_path))
        qFatal("Unable to load points from file.");

    _convex_algorithm = new QuickConvexHullAlgorithm(this);
    _concave_algorithm =  new ConcaveHullAlgorithm(this);

    auto add_points = [this]() {
        for (const QPointF& p : *_data_manager->points())
            _view->addPoint(QPointF(p), 0.001);
    };

    connect(_convex_algorithm, &QuickConvexHullAlgorithm::finished, this, [this, add_points](const QSet<QPointF>& convex_hull_points) {
        qInfo() << "&QuickConvexHullAlgorithm::finished";
        if(!_data_manager->points()->empty() || !_view->points_presented())
            add_points();

        _view->connectPoints(convex_hull_points, Qt::red, 0.04);
    });
    connect(_concave_algorithm, &ConcaveHullAlgorithm::finished, this, [this, add_points](const QSet<QPointF>& concave_hull_points) {
        qInfo() << "&ConcaveHullAlgorithm::finished";
        if(!_data_manager->points()->empty() || !_view->points_presented())
            add_points();

        _view->connectPoints(concave_hull_points, Qt::green, 0.04);
    });
    connect(_ui->clear_area_button, &QPushButton::clicked, _view, &HullView::clearAll);
    connect(_ui->emplace_points_button, &QPushButton::clicked, _view, [this]() {
        const auto& points = *_data_manager->points();
        _view->clearAll();
        for (const auto& point : points) {
            _view->addPoint(point);
        }
    });

    _timer = new QElapsedTimer();
    connect(_ui->build_button, &QPushButton::clicked, _view, [this]() {
        const auto& points = *_data_manager->points();

        _timer->start();
        _convex_algorithm->compute(points);
        qint64 convex_time = _timer->elapsed();
        qDebug() << "Convex hull compute time (ms):" << convex_time;

        _timer->restart();
        _concave_algorithm->compute(points,
                                    IHullAlgorithm::sortPointsClockwise(_convex_algorithm->result()));
        qint64 concave_time = _timer->elapsed();
        qDebug() << "Concave hull compute time (ms):" << concave_time;
    });

    _ui->emplace_points_button->click();
    _ui->build_button->click();
}

HullBuildingDemo::~HullBuildingDemo() {
    delete _ui;
    delete _timer;
}

