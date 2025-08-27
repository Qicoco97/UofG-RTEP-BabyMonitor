// ErrorHandler.h - Centralized error handling
#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QDebug>

namespace BabyMonitor {

/**
 * Error severity levels
 */
enum class ErrorLevel {
    INFO,       // Informational messages
    WARNING,    // Warning conditions
    ERROR,      // Error conditions
    CRITICAL    // Critical system errors
};

/**
 * Error information structure
 */
struct ErrorInfo {
    ErrorLevel level;
    QString component;      // Which component reported the error
    QString message;        // Error description
    QString details;        // Additional details
    QDateTime timestamp;    // When the error occurred
    
    ErrorInfo(ErrorLevel lvl, const QString& comp, const QString& msg, const QString& det = "")
        : level(lvl), component(comp), message(msg), details(det), timestamp(QDateTime::currentDateTime()) {}
    
    QString toString() const {
        QString levelStr;
        switch (level) {
            case ErrorLevel::INFO:     levelStr = "INFO"; break;
            case ErrorLevel::WARNING:  levelStr = "WARN"; break;
            case ErrorLevel::ERROR:    levelStr = "ERROR"; break;
            case ErrorLevel::CRITICAL: levelStr = "CRITICAL"; break;
        }
        
        return QString("[%1] %2 - %3: %4 %5")
               .arg(timestamp.toString("hh:mm:ss"))
               .arg(levelStr)
               .arg(component)
               .arg(message)
               .arg(details.isEmpty() ? "" : "(" + details + ")");
    }
};

/**
 * Centralized error handler - Single responsibility for error management
 */
class ErrorHandler : public QObject {
    Q_OBJECT

public:
    static ErrorHandler& getInstance();
    
    // Error reporting methods
    void reportError(ErrorLevel level, const QString& component, 
                    const QString& message, const QString& details = "");
    void reportInfo(const QString& component, const QString& message, const QString& details = "");
    void reportWarning(const QString& component, const QString& message, const QString& details = "");
    void reportError(const QString& component, const QString& message, const QString& details = "");
    void reportCritical(const QString& component, const QString& message, const QString& details = "");
    
    // Error retrieval
    QList<ErrorInfo> getRecentErrors(int maxCount = 50) const;
    QList<ErrorInfo> getErrorsByLevel(ErrorLevel level) const;
    bool hasErrors() const;
    bool hasCriticalErrors() const;
    
    // Error management
    void clearErrors();
    void setMaxErrorHistory(int maxCount);

signals:
    void errorReported(const ErrorInfo& error);
    void criticalErrorReported(const ErrorInfo& error);

private:
    ErrorHandler() = default;
    
    QList<ErrorInfo> errorHistory_;
    int maxErrorHistory_ = 100;
    
    void addToHistory(const ErrorInfo& error);
    void logToConsole(const ErrorInfo& error);
};

} // namespace BabyMonitor
