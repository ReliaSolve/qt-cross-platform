#include "mainwindow.h"
#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef __APPLE__
    // Set the OpenGL profile before constructing the MainWindow.
    // This must be done on the mac so that it does not use the
    // legacy profile.
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setMajorVersion(3);
    surfaceFormat.setMinorVersion(3);
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);
#endif

    MainWindow w;
    w.show();

    return a.exec();
}
