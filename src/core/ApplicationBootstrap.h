// ApplicationBootstrap.h - Application startup and dependency configuration
#pragma once

#include <QObject>
#include <memory>
#include "ServiceContainer.h"
#include "../mainwindow.h"

namespace BabyMonitor {

/**
 * Application bootstrap class
 * Responsible for configuring dependency injection and starting the application
 * Follows Single Responsibility Principle - only handles application startup
 */
class ApplicationBootstrap {
public:
    ApplicationBootstrap();
    ~ApplicationBootstrap();
    
    /**
     * Configure all services and dependencies
     */
    bool configureServices();
    
    /**
     * Create and configure the main window with injected dependencies
     */
    std::unique_ptr<MainWindow> createMainWindow(QWidget* parent = nullptr);
    
    /**
     * Get service container for manual service access
     */
    ServiceContainer& getServiceContainer() { return container_; }

private:
    ServiceContainer& container_;
    ErrorHandler& errorHandler_;
    
    void registerAlarmSystem();
    void registerSensorFactory();
    void logServiceRegistration();
};

} // namespace BabyMonitor
