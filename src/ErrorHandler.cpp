// ErrorHandler.cpp - Centralized error handling implementation
#include "ErrorHandler.h"

namespace BabyMonitor {

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

void ErrorHandler::reportError(ErrorLevel level, const QString& component,
                              const QString& message, const QString& details) {
    ErrorInfo error(level, component, message, details);
    addToHistory(error);
    logToConsole(error);
}

void ErrorHandler::reportInfo(const QString& component, const QString& message, const QString& details) {
    reportError(ErrorLevel::INFO, component, message, details);
}

void ErrorHandler::reportWarning(const QString& component, const QString& message, const QString& details) {
    reportError(ErrorLevel::WARNING, component, message, details);
}

void ErrorHandler::reportError(const QString& component, const QString& message, const QString& details) {
    reportError(ErrorLevel::ERROR, component, message, details);
}

void ErrorHandler::reportCritical(const QString& component, const QString& message, const QString& details) {
    reportError(ErrorLevel::CRITICAL, component, message, details);
}

QList<ErrorInfo> ErrorHandler::getRecentErrors(int maxCount) const {
    if (maxCount <= 0 || maxCount >= errorHistory_.size()) {
        return errorHistory_;
    }
    
    return errorHistory_.mid(errorHistory_.size() - maxCount);
}

QList<ErrorInfo> ErrorHandler::getErrorsByLevel(ErrorLevel level) const {
    QList<ErrorInfo> filtered;
    for (const auto& error : errorHistory_) {
        if (error.level == level) {
            filtered.append(error);
        }
    }
    return filtered;
}

bool ErrorHandler::hasErrors() const {
    for (const auto& error : errorHistory_) {
        if (error.level == ErrorLevel::ERROR || error.level == ErrorLevel::CRITICAL) {
            return true;
        }
    }
    return false;
}

bool ErrorHandler::hasCriticalErrors() const {
    for (const auto& error : errorHistory_) {
        if (error.level == ErrorLevel::CRITICAL) {
            return true;
        }
    }
    return false;
}

void ErrorHandler::clearErrors() {
    errorHistory_.clear();
}

void ErrorHandler::setMaxErrorHistory(int maxCount) {
    maxErrorHistory_ = maxCount;
    
    // Trim existing history if needed
    while (errorHistory_.size() > maxErrorHistory_) {
        errorHistory_.removeFirst();
    }
}

void ErrorHandler::addToHistory(const ErrorInfo& error) {
    errorHistory_.append(error);
    
    // Keep history within limits
    while (errorHistory_.size() > maxErrorHistory_) {
        errorHistory_.removeFirst();
    }
}

void ErrorHandler::logToConsole(const ErrorInfo& error) {
    QString logMessage = error.toString();
    
    switch (error.level) {
        case ErrorLevel::INFO:
            qInfo() << logMessage;
            break;
        case ErrorLevel::WARNING:
            qWarning() << logMessage;
            break;
        case ErrorLevel::ERROR:
            qCritical() << logMessage;
            break;
        case ErrorLevel::CRITICAL:
            qCritical() << "CRITICAL:" << logMessage;
            break;
    }
}

} // namespace BabyMonitor
