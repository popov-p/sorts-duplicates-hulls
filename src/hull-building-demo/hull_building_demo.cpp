#include "hull_building_demo.h"

#include "ui_main_window.h"

HullBuildingDemo::HullBuildingDemo(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

HullBuildingDemo::~HullBuildingDemo() {
    delete ui;
}
