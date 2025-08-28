// ApplicationBootstrap.cpp - Application startup implementation
#include "ApplicationBootstrap.h"
#include "../interfaces/IComponent.h"
#include "../ErrorHandler.h"
#include <QStringList>
#include <QString>
#include <memory>

// Forward declarations to avoid including Qt MOC headers
class MainWindow;
class QWidget;

namespace BabyMonitor {
    class AlarmSystem;

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

void* ApplicationBootstrap::createMainWindow(QWidget* parent) {
    errorHandler_.reportInfo("Bootstrap", "Creating main window with dependency injection");

    // This method will be implemented in main.cpp to avoid Qt MOC issues
    // For now, return nullptr and handle creation in main.cpp
    errorHandler_.reportWarning("Bootstrap", "MainWindow creation deferred to main.cpp");
    return nullptr;
}

void ApplicationBootstrap::registerAlarmSystem() {
    // AlarmSystem registration will be handled in main.cpp to avoid Qt MOC issues
    errorHandler_.reportInfo("Bootstrap", "AlarmSystem registration deferred to main.cpp");
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
