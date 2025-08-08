#include "hull_building_demo.h"
#include "ui_main_window.h"
#include "hull_view.h"
#include "quick_convex_hull.h"
#include "points_manager.h"

#include <QPushButton>
QuickConvexHullAlgorithm* HullBuildingDemo::algorithm() { return _algorithm; }

HullBuildingDemo::HullBuildingDemo(HullConfig config, QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow) {
    _ui->setupUi(this);
    _view = new HullView(this);
    _ui->verticalLayout->addWidget(_view);

    _data_manager = new PointsDataManager(this);
    if(!_data_manager->loadPoints(config.file_path))
        qFatal("Unable to load points from file.");

    _algorithm = new QuickConvexHullAlgorithm(this);

    connect(_algorithm, &QuickConvexHullAlgorithm::finished, this, [this](const QList<QPointF>& hull_points) {
        for (const QPointF& p : hull_points)
            _view->addPoint(QPointF(p));

        _view->connectPoints();
    });

    connect(_ui->clear_area_button, &QPushButton::clicked, _view, &HullView::clearAll);
    connect(_ui->emplace_points_button, &QPushButton::clicked, _view, [this]() {
        const auto& points = *_data_manager->points();
        for (const auto& point : points) {
            _view->addPoint(point);
        }
    });

    connect(_ui->build_button, &QPushButton::clicked, _view, [this]() {
        const auto& points = *_data_manager->points();
        _algorithm->compute(points);
    });
}

HullBuildingDemo::~HullBuildingDemo() {
    delete _ui;
}

