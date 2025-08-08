#include "hull_building_demo.h"

#include <QApplication>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    HullBuildingDemo d;
    d.show();
    app.exec();
    return 0;
}
