// PerformanceMonitor.h - Lightweight real-time performance monitoring
#pragma once

#include <QDateTime>
#include <QString>
#include <QMap>
#include <QList>
#include <chrono>
#include <atomic>
#include <algorithm>
#include <cmath>
#include "../ErrorHandler.h"

namespace BabyMonitor {

/**
 * Real-time constraints and performance requirements
 * These define the acceptable performance thresholds for the baby monitoring system
 */
namespace RealTimeConstraints {
    // Critical timing constraints (in milliseconds)
    constexpr double MAX_MOTION_DETECTION_LATENCY_MS = 100.0;    // Motion must be detected within 100ms
    constexpr double MAX_ALARM_RESPONSE_LATENCY_MS = 200.0;      // Alarm must trigger within 200ms
    constexpr double MAX_FRAME_PROCESSING_LATENCY_MS = 33.0;     // Frame processing must complete within 33ms (30fps)
    constexpr double MAX_SENSOR_READ_LATENCY_MS = 500.0;         // Sensor reading should complete within 500ms
    constexpr double MAX_UI_UPDATE_LATENCY_MS = 50.0;            // UI updates should be smooth (20fps)
    
    // Throughput requirements
    constexpr double MIN_CAMERA_FPS = 25.0;                      // Minimum acceptable frame rate
    constexpr double MIN_MOTION_DETECTION_FPS = 20.0;            // Motion detection processing rate
    constexpr double MAX_ALARM_PUBLISH_INTERVAL_MS = 1000.0;     // Maximum time between alarm checks
    
    // Resource usage limits
    constexpr double MAX_CPU_USAGE_PERCENT = 80.0;               // Maximum CPU usage per component
    constexpr size_t MAX_MEMORY_USAGE_MB = 512;                  // Maximum memory usage
    
    // Error tolerance
    constexpr int MAX_CONSECUTIVE_FRAME_DROPS = 3;               // Maximum consecutive frame drops
    constexpr int MAX_CONSECUTIVE_SENSOR_ERRORS = 5;             // Maximum consecutive sensor errors
    
    // Performance adaptation thresholds
    constexpr double PERFORMANCE_ADAPTATION_THRESHOLD = 0.8;     // Adapt when reaching 80% of limit
    constexpr double PERFORMANCE_RECOVERY_THRESHOLD = 0.6;       // Recover when below 60% of limit
}

/**
 * High-precision timer for performance measurements
 */
class HighPrecisionTimer {
public:
    HighPrecisionTimer() : startTime_(std::chrono::high_resolution_clock::now()) {}
    
    void start() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    double elapsedMs() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime_);
        return duration.count() / 1000.0; // Convert to milliseconds
    }
    
    double elapsedUs() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime_);
        return static_cast<double>(duration.count());
    }

private:
    std::chrono::high_resolution_clock::time_point startTime_;
};

/**
 * Performance statistics aggregator
 */
class PerformanceStats {
public:
    void addSample(double value) {
        samples_.append(value);
        if (samples_.size() > maxSamples_) {
            samples_.removeFirst();
        }
        updateStats();
    }
    
    double getAverage() const { return average_; }
    double getMin() const { return min_; }
    double getMax() const { return max_; }
    double getStdDev() const { return stdDev_; }
    double getPercentile(double p) const {
        if (samples_.isEmpty()) return 0.0;
        QList<double> sorted = samples_;
        std::sort(sorted.begin(), sorted.end());
        int index = static_cast<int>(p * (sorted.size() - 1));
        return sorted[index];
    }
    
    int getSampleCount() const { return samples_.size(); }
    void clear() { samples_.clear(); updateStats(); }
    void setMaxSamples(int max) { maxSamples_ = max; }

private:
    QList<double> samples_;
    int maxSamples_ = 100;
    double average_ = 0.0;
    double min_ = 0.0;
    double max_ = 0.0;
    double stdDev_ = 0.0;
    
    void updateStats() {
        if (samples_.isEmpty()) {
            average_ = min_ = max_ = stdDev_ = 0.0;
            return;
        }
        
        // Calculate average
        double sum = 0.0;
        for (double sample : samples_) {
            sum += sample;
        }
        average_ = sum / samples_.size();
        
        // Calculate min/max
        min_ = *std::min_element(samples_.begin(), samples_.end());
        max_ = *std::max_element(samples_.begin(), samples_.end());
        
        // Calculate standard deviation
        double variance = 0.0;
        for (double sample : samples_) {
            variance += (sample - average_) * (sample - average_);
        }
        stdDev_ = std::sqrt(variance / samples_.size());
    }
};

/**
 * Real-time performance requirements for different operations
 */
struct RealTimeRequirements {
    QString operationName;
    double maxLatencyMs;
    double minThroughputHz;
    double maxCpuPercent;
    size_t maxMemoryMB;
    
    RealTimeRequirements(const QString& name, double latency, double throughput, 
                        double cpu = 50.0, size_t memory = 100)
        : operationName(name)
        , maxLatencyMs(latency)
        , minThroughputHz(throughput)
        , maxCpuPercent(cpu)
        , maxMemoryMB(memory)
    {}
    
    bool isWithinConstraints(double latencyMs) const {
        return latencyMs <= maxLatencyMs;
    }
    
    QString getViolationMessage(double latencyMs) const {
        if (latencyMs > maxLatencyMs) {
            return QString("Latency: %1ms > %2ms").arg(latencyMs, 0, 'f', 2).arg(maxLatencyMs);
        }
        return QString();
    }
};

/**
 * System-wide performance requirements registry
 */
class PerformanceRequirements {
public:
    static PerformanceRequirements& getInstance() {
        static PerformanceRequirements instance;
        return instance;
    }
    
    void registerRequirement(const RealTimeRequirements& req) {
        requirements_[req.operationName] = req;
    }
    
    const RealTimeRequirements* getRequirement(const QString& operation) const {
        auto it = requirements_.find(operation);
        return (it != requirements_.end()) ? &it.value() : nullptr;
    }
    
    QStringList getAllOperations() const {
        return requirements_.keys();
    }

private:
    PerformanceRequirements() {
        // Register default requirements
        registerRequirement(RealTimeRequirements("FrameProcessing", 
            RealTimeConstraints::MAX_FRAME_PROCESSING_LATENCY_MS, 
            RealTimeConstraints::MIN_CAMERA_FPS, 30.0, 50));
            
        registerRequirement(RealTimeRequirements("MotionDetection", 
            RealTimeConstraints::MAX_MOTION_DETECTION_LATENCY_MS, 
            RealTimeConstraints::MIN_MOTION_DETECTION_FPS, 25.0, 30));
            
        registerRequirement(RealTimeRequirements("AlarmResponse", 
            RealTimeConstraints::MAX_ALARM_RESPONSE_LATENCY_MS, 
            1.0, 10.0, 10));
            
        registerRequirement(RealTimeRequirements("SensorReading", 
            RealTimeConstraints::MAX_SENSOR_READ_LATENCY_MS, 
            0.5, 5.0, 5));
            
        registerRequirement(RealTimeRequirements("UIUpdate", 
            RealTimeConstraints::MAX_UI_UPDATE_LATENCY_MS, 
            20.0, 15.0, 20));
    }
    
    QMap<QString, RealTimeRequirements> requirements_;
};

/**
 * Lightweight performance monitor for real-time constraint checking
 * Singleton pattern for easy access across components
 */
class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    /**
     * Record latency for a specific operation and check constraints
     */
    void recordLatency(const QString& component, const QString& operation, double latencyMs) {
        QString key = component + "::" + operation;
        
        // Update statistics
        if (!stats_.contains(key)) {
            stats_[key] = PerformanceStats();
        }
        stats_[key].addSample(latencyMs);
        
        // Check constraints and report violations
        checkConstraints(component, operation, latencyMs);
    }
    
    /**
     * Check if operation should adapt performance based on recent metrics
     */
    bool shouldAdaptPerformance(const QString& component, const QString& operation) {
        QString key = component + "::" + operation;
        if (!stats_.contains(key)) return false;
        
        auto& requirements = PerformanceRequirements::getInstance();
        auto* req = requirements.getRequirement(operation);
        if (!req) return false;
        
        double avgLatency = stats_[key].getAverage();
        return avgLatency > (req->maxLatencyMs * RealTimeConstraints::PERFORMANCE_ADAPTATION_THRESHOLD);
    }
    
    /**
     * Check if operation can recover to higher quality
     */
    bool canRecoverPerformance(const QString& component, const QString& operation) {
        QString key = component + "::" + operation;
        if (!stats_.contains(key)) return true;
        
        auto& requirements = PerformanceRequirements::getInstance();
        auto* req = requirements.getRequirement(operation);
        if (!req) return true;
        
        double avgLatency = stats_[key].getAverage();
        return avgLatency < (req->maxLatencyMs * RealTimeConstraints::PERFORMANCE_RECOVERY_THRESHOLD);
    }
    
    /**
     * Get performance statistics for an operation
     */
    const PerformanceStats* getStats(const QString& component, const QString& operation) const {
        QString key = component + "::" + operation;
        auto it = stats_.find(key);
        return (it != stats_.end()) ? &it.value() : nullptr;
    }
    
    /**
     * Get current performance level (0.0 = excellent, 1.0 = at limit)
     */
    double getPerformanceLevel(const QString& component, const QString& operation) const {
        QString key = component + "::" + operation;
        if (!stats_.contains(key)) return 0.0;
        
        auto& requirements = PerformanceRequirements::getInstance();
        auto* req = requirements.getRequirement(operation);
        if (!req) return 0.0;
        
        double avgLatency = stats_[key].getAverage();
        return std::min(1.0, avgLatency / req->maxLatencyMs);
    }
    
    /**
     * Clear statistics for all operations
     */
    void clearStats() {
        stats_.clear();
    }

private:
    PerformanceMonitor() = default;
    
    QMap<QString, PerformanceStats> stats_;
    
    void checkConstraints(const QString& component, const QString& operation, double latencyMs) {
        auto& requirements = PerformanceRequirements::getInstance();
        auto* req = requirements.getRequirement(operation);
        if (!req) return;
        
        if (latencyMs > req->maxLatencyMs) {
            // Report constraint violation
            auto& errorHandler = ErrorHandler::getInstance();
            errorHandler.reportWarning(component, 
                QString("Performance constraint violated: %1 took %2ms (limit: %3ms)")
                .arg(operation).arg(latencyMs, 0, 'f', 2).arg(req->maxLatencyMs));
        }
    }
};

} // namespace BabyMonitor

#endif // PERFORMANCEMONITOR_H
