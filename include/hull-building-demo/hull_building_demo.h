#pragma once
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class HullBuildingDemo : public QMainWindow {
    Q_OBJECT
public:
    explicit HullBuildingDemo(QWidget* parent = nullptr);
    ~HullBuildingDemo();

private:
    Ui::MainWindow* ui;
};
