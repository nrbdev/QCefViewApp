#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>
#include <QStandardPaths>
#include <QCefConfig.h>
#include <QCefContext.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

    QCefConfig config;
    config.setCachePath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));

    QCefContext cefContext(&a, argc, argv, &config);

    MainWindow w;
    w.show();
    return QApplication::exec();
}
