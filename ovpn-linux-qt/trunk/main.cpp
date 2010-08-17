#include <QtGui/QApplication>
#include <QDir>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    // generate client conf directory
    QDir dir;
    dir.mkpath(QDir::homePath()+"/.ovpn-linux/conf");

    MainWindow mw;

    return app.exec();
}
