#pragma once
#include <QMainWindow>

class HullView;
class QuickConvexHullAlgorithm;
class PointsDataManager;
class ConcaveHullAlgorithm;

class QElapsedTimer;

namespace Ui {
class MainWindow;
}

typedef struct HullConfig {
    QString file_path;
} HullConfig;

class HullBuildingDemo : public QMainWindow {
    Q_OBJECT
public:
    explicit HullBuildingDemo(HullConfig config, QWidget* parent = nullptr);
    QuickConvexHullAlgorithm* algorithm();
    ~HullBuildingDemo();

private:
    Ui::MainWindow* _ui;
    HullView* _view;
    PointsDataManager* _data_manager;
    QuickConvexHullAlgorithm* _convex_algorithm;
    ConcaveHullAlgorithm* _concave_algorithm;
    QElapsedTimer* _timer;
};
