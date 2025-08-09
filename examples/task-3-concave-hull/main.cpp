#include "hull_building_demo.h"

#include <QApplication>
#include <QDebug>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QTimer>
int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QCommandLineParser parser;
    parser.setApplicationDescription("Convex Hull Builder");
    parser.addHelpOption();

    QCommandLineOption file_option(QStringList() << "f" << "file",
                                  "Path to points file.",
                                  "file");
    parser.addOption(file_option);

    parser.process(app);

    QString file_path;
    if (parser.isSet(file_option)) {
        file_path = parser.value(file_option);
    }

    HullBuildingDemo d({file_path});
    d.show();

    // QTimer::singleShot(1000, &app, &QCoreApplication::quit);
    app.exec();
    return 0;
}
