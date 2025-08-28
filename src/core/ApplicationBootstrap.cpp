// ApplicationBootstrap.cpp - Application startup implementation
#include "ApplicationBootstrap.h"
#include "../mainwindow.h"
#include "../managers/AlarmSystem.h"
#include "../SensorFactory.h"

namespace BabyMonitor {

ApplicationBootstrap::ApplicationBootstrap()
    : container_(ServiceContainer::getInstance())
    , errorHandler_(ErrorHandler::getInstance())
{
}

ApplicationBootstrap::~ApplicationBootstrap() = default;

bool ApplicationBootstrap::configureServices() {
    errorHandler_.reportInfo("Bootstrap", "Configuring application services");
    
    try {
        registerAlarmSystem();
        registerSensorFactory();
        
        logServiceRegistration();
        
        errorHandler_.reportInfo("Bootstrap", "All services configured successfully");
        return true;
        
    } catch (const std::exception& e) {
        errorHandler_.reportCritical("Bootstrap", 
            QString("Failed to configure services: %1").arg(e.what()));
        return false;
    }
}

std::unique_ptr<MainWindow> ApplicationBootstrap::createMainWindow(QWidget* parent) {
    errorHandler_.reportInfo("Bootstrap", "Creating main window with dependency injection");
    
    // Resolve dependencies
    auto alarmSystem = container_.resolve<IAlarmSystem>("AlarmSystem");
    if (!alarmSystem) {
        errorHandler_.reportCritical("Bootstrap", "Failed to resolve AlarmSystem dependency");
        return nullptr;
    }
    
    // Create MainWindow with injected dependencies
    auto mainWindow = std::make_unique<MainWindow>(parent);
    
    // For now, we'll inject dependencies after construction
    // In a full DI implementation, we'd pass them to constructor
    mainWindow->setAlarmSystem(alarmSystem);
    
    errorHandler_.reportInfo("Bootstrap", "Main window created with injected dependencies");
    return mainWindow;
}

void ApplicationBootstrap::registerAlarmSystem() {
    container_.registerService<IAlarmSystem>("AlarmSystem", []() {
        return std::make_shared<AlarmSystem>();
    });
    
    errorHandler_.reportInfo("Bootstrap", "Registered AlarmSystem service");
}

void ApplicationBootstrap::registerSensorFactory() {
    // For now, SensorFactory is static, so we don't need to register it
    // In a full implementation, we'd make it instance-based
    errorHandler_.reportInfo("Bootstrap", "SensorFactory available (static implementation)");
}

void ApplicationBootstrap::logServiceRegistration() {
    QStringList services = {"AlarmSystem"};
    
    for (const QString& service : services) {
        if (container_.isRegistered(service)) {
            errorHandler_.reportInfo("Bootstrap", QString("✓ Service registered: %1").arg(service));
        } else {
            errorHandler_.reportWarning("Bootstrap", QString("✗ Service missing: %1").arg(service));
        }
    }
}

} // namespace BabyMonitor
