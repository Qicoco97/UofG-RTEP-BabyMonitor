// ChartController.h - Chart management controller
#pragma once

#include "IUIController.h"
#include "../../utils/SensorData.h"
#include "../../utils/Config.h"
#include "../../utils/ErrorHandler.h"
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChartView>
#include <QDateTime>

QT_CHARTS_USE_NAMESPACE

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
 * Chart controller - handles data visualization and chart management
 * Single responsibility: Chart display and data visualization
 */
class ChartController : public IUIController {
    Q_OBJECT

public:
    explicit ChartController(QChartView* tempHumChartView, QChartView* motionChartView, 
                           QObject* parent = nullptr);
    ~ChartController();

    // IComponent interface
    bool initialize() override;
    void start() override;
    void stop() override;
    bool isRunning() const override { return isRunning_; }
    QString getName() const override { return "ChartController"; }
    bool isHealthy() const override;

    // IUIController interface
    void updateDisplay() override;
    void handleError(const QString& message) override;

    // Chart-specific methods
    void updateTemperatureHumidityChart(const TemperatureHumidityData& data);
    void updateMotionChart(const MotionData& data);
    void clearCharts();

public slots:
    void onTemperatureHumidityUpdate(const TemperatureHumidityData& data);
    void onMotionUpdate(const MotionData& data);

signals:
    void chartUpdated(const QString& chartType);

private:
    // Chart views
    QChartView* tempHumChartView_;
    QChartView* motionChartView_;
    
    // Temperature/Humidity chart components
    QChart* tempHumChart_;
    QLineSeries* tempSeries_;
    QLineSeries* humSeries_;
    QValueAxis* tempHumAxisX_;
    QValueAxis* tempHumAxisY_;
    
    // Motion chart components
    QChart* motionChart_;
    QLineSeries* motionSeries_;
    QValueAxis* motionAxisX_;
    QValueAxis* motionAxisY_;
    
    // Chart data management
    int timeIndex_;
    
    // Error handling
    BabyMonitor::ErrorHandler& errorHandler_;
    
    // Helper methods
    void setupTemperatureHumidityChart();
    void setupMotionChart();
    void configureChartAxes();
    void addTemperatureHumidityPoint(float temperature, float humidity);
    void addMotionPoint(bool detected, double confidence);
    void maintainChartDataLimits();
};

} // namespace BabyMonitor
