#include "mainwindow.h"
#include "core/ApplicationBootstrap.h"
#include "managers/AlarmSystem.h"

#include <QApplication>
#include <QMessageBox>
#include <memory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<cv::Mat>("cv::Mat");

    // Use dependency injection bootstrap
    BabyMonitor::ApplicationBootstrap bootstrap;

    // Setup dependency injection manually before configuring services
    auto& container = bootstrap.getServiceContainer();

    // Register AlarmSystem service here (before configureServices)
    container.registerService<BabyMonitor::IAlarmSystem>("AlarmSystem", []() {
        return std::make_shared<BabyMonitor::AlarmSystem>();
    });

    if (!bootstrap.configureServices()) {
        QMessageBox::critical(nullptr, "Startup Error",
            "Failed to configure application services. Check console for details.");
        return -1;
    }

    // Create MainWindow directly to avoid Qt MOC issues in ApplicationBootstrap
    MainWindow mainWindow;

    // Resolve and inject dependencies
    auto alarmSystem = container.resolve<BabyMonitor::IAlarmSystem>("AlarmSystem");
    if (alarmSystem) {
        mainWindow.setAlarmSystem(alarmSystem);
    }

    mainWindow.show();
    return a.exec();
}
