// IUIController.h - Base interface for UI controllers
#pragma once

#include <QObject>
#include <memory>
#include "../../interfaces/IComponent.h"

// Ensure Qt macros are defined
#ifndef signals
#define signals Q_SIGNALS
#endif
#ifndef slots
#define slots Q_SLOTS
#endif
#ifndef emit
#define emit Q_EMIT
#endif

namespace BabyMonitor {

/**
 * Base interface for UI controllers
 * Extends IComponent with UI-specific functionality
 */
class IUIController : public QObject, public IComponent {
    Q_OBJECT

public:
    explicit IUIController(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IUIController() = default;

    // IComponent interface
    bool initialize() override = 0;
    void start() override = 0;
    void stop() override = 0;
    bool isRunning() const override = 0;
    QString getName() const override = 0;
    bool isHealthy() const override = 0;

    // UI-specific methods
    virtual void updateDisplay() = 0;
    virtual void handleError(const QString& message) = 0;

signals:
    void statusChanged(const QString& status);
    void errorOccurred(const QString& component, const QString& message);
    void performanceAlert(const QString& message);

protected:
    bool isInitialized_ = false;
    bool isRunning_ = false;
};

} // namespace BabyMonitor
