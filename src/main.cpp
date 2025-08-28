#include "mainwindow.h"
#include "core/ApplicationBootstrap.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<cv::Mat>("cv::Mat");

    // Use dependency injection bootstrap
    BabyMonitor::ApplicationBootstrap bootstrap;

    if (!bootstrap.configureServices()) {
        QMessageBox::critical(nullptr, "Startup Error",
            "Failed to configure application services. Check console for details.");
        return -1;
    }

    auto mainWindow = bootstrap.createMainWindow();
    if (!mainWindow) {
        QMessageBox::critical(nullptr, "Startup Error",
            "Failed to create main window. Check console for details.");
        return -1;
    }

    mainWindow->show();
    return a.exec();
}
