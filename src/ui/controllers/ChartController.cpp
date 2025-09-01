// ChartController.cpp - Chart management controller implementation
#include "ChartController.h"
#include <QDebug>

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

ChartController::ChartController(QChartView* tempHumChartView, QChartView* motionChartView, 
                               QObject* parent)
    : IUIController(parent)
    , tempHumChartView_(tempHumChartView)
    , motionChartView_(motionChartView)
    , tempHumChart_(nullptr)
    , tempSeries_(nullptr)
    , humSeries_(nullptr)
    , tempHumAxisX_(nullptr)
    , tempHumAxisY_(nullptr)
    , motionChart_(nullptr)
    , motionSeries_(nullptr)
    , motionAxisX_(nullptr)
    , motionAxisY_(nullptr)
    , timeIndex_(0)
    , errorHandler_(BabyMonitor::ErrorHandler::getInstance())
{
}

ChartController::~ChartController()
{
    stop();
}

bool ChartController::initialize()
{
    if (isInitialized_) return true;
    
    errorHandler_.reportInfo("ChartController", "Initializing chart controller");
    
    try {
        setupTemperatureHumidityChart();
        setupMotionChart();
        configureChartAxes();
        
        isInitialized_ = true;
        errorHandler_.reportInfo("ChartController", "Initialized successfully");
        return true;
    } catch (const std::exception& e) {
        errorHandler_.reportError("ChartController", "Initialization failed", e.what());
        return false;
    }
}

void ChartController::start()
{
    if (!isInitialized_) {
        errorHandler_.reportError("ChartController", "Cannot start - not initialized");
        return;
    }
    
    isRunning_ = true;
    errorHandler_.reportInfo("ChartController", "Started successfully");
    emit statusChanged("Charts: Active");
}

void ChartController::stop()
{
    if (!isRunning_) return;
    
    isRunning_ = false;
    errorHandler_.reportInfo("ChartController", "Stopped successfully");
    emit statusChanged("Charts: Stopped");
}

bool ChartController::isHealthy() const
{
    return isInitialized_ && isRunning_ && tempHumChart_ && motionChart_;
}

void ChartController::updateDisplay()
{
    // Charts update automatically when data is added
    // This method can be used for manual refresh if needed
    if (tempHumChartView_) {
        tempHumChartView_->update();
    }
    if (motionChartView_) {
        motionChartView_->update();
    }
}

void ChartController::handleError(const QString& message)
{
    errorHandler_.reportError("ChartController", message);
    emit errorOccurred("ChartController", message);
}

void ChartController::updateTemperatureHumidityChart(const TemperatureHumidityData& data)
{
    if (!isRunning_ || !data.isValid) return;
    
    try {
        addTemperatureHumidityPoint(data.temperature, data.humidity);
        maintainChartDataLimits();
        emit chartUpdated("TemperatureHumidity");
    } catch (const std::exception& e) {
        handleError(QString("Failed to update temperature/humidity chart: %1").arg(e.what()));
    }
}

void ChartController::updateMotionChart(const MotionData& data)
{
    if (!isRunning_) return;
    
    try {
        addMotionPoint(data.detected, data.confidence);
        maintainChartDataLimits();
        emit chartUpdated("Motion");
    } catch (const std::exception& e) {
        handleError(QString("Failed to update motion chart: %1").arg(e.what()));
    }
}

void ChartController::clearCharts()
{
    if (tempSeries_) tempSeries_->clear();
    if (humSeries_) humSeries_->clear();
    if (motionSeries_) motionSeries_->clear();
    timeIndex_ = 0;
    
    errorHandler_.reportInfo("ChartController", "Charts cleared");
}

void ChartController::onTemperatureHumidityUpdate(const TemperatureHumidityData& data)
{
    updateTemperatureHumidityChart(data);
}

void ChartController::onMotionUpdate(const MotionData& data)
{
    updateMotionChart(data);
}

void ChartController::setupTemperatureHumidityChart()
{
    if (!tempHumChartView_) {
        throw std::runtime_error("Temperature/Humidity chart view is null");
    }
    
    // Create chart and series
    tempHumChart_ = new QChart();
    tempSeries_ = new QLineSeries();
    humSeries_ = new QLineSeries();
    
    tempSeries_->setName("Temperature (°C)");
    humSeries_->setName("Humidity (%)");
    
    tempHumChart_->addSeries(tempSeries_);
    tempHumChart_->addSeries(humSeries_);
    tempHumChart_->setTitle("Temperature & Humidity");
    
    tempHumChartView_->setChart(tempHumChart_);
}

void ChartController::setupMotionChart()
{
    if (!motionChartView_) {
        throw std::runtime_error("Motion chart view is null");
    }
    
    // Create motion chart and series
    motionChart_ = new QChart();
    motionSeries_ = new QLineSeries();
    
    motionSeries_->setName("Motion Detection");
    motionChart_->addSeries(motionSeries_);
    motionChart_->setTitle("Motion Detection Status");
    
    motionChartView_->setChart(motionChart_);
}

void ChartController::configureChartAxes()
{
    // Configure temperature/humidity chart axes
    if (tempHumChart_) {
        tempHumAxisX_ = new QValueAxis();
        tempHumAxisY_ = new QValueAxis();
        
        tempHumAxisX_->setTitleText("Time");
        tempHumAxisY_->setTitleText("Value");
        tempHumAxisY_->setRange(0, 100);
        
        tempHumChart_->addAxis(tempHumAxisX_, Qt::AlignBottom);
        tempHumChart_->addAxis(tempHumAxisY_, Qt::AlignLeft);
        
        tempSeries_->attachAxis(tempHumAxisX_);
        tempSeries_->attachAxis(tempHumAxisY_);
        humSeries_->attachAxis(tempHumAxisX_);
        humSeries_->attachAxis(tempHumAxisY_);
    }
    
    // Configure motion chart axes
    if (motionChart_) {
        motionAxisX_ = new QValueAxis();
        motionAxisY_ = new QValueAxis();
        
        motionAxisX_->setTitleText("Time");
        motionAxisY_->setTitleText("Motion");
        motionAxisY_->setRange(0, 1);
        
        motionChart_->addAxis(motionAxisX_, Qt::AlignBottom);
        motionChart_->addAxis(motionAxisY_, Qt::AlignLeft);
        
        motionSeries_->attachAxis(motionAxisX_);
        motionSeries_->attachAxis(motionAxisY_);
    }
}

void ChartController::addTemperatureHumidityPoint(float temperature, float humidity)
{
    if (!tempSeries_ || !humSeries_) return;
    
    tempSeries_->append(timeIndex_, temperature);
    humSeries_->append(timeIndex_, humidity);
    timeIndex_++;
}

void ChartController::addMotionPoint(bool detected, double confidence)
{
    if (!motionSeries_) return;
    
    motionSeries_->append(timeIndex_, detected ? confidence : 0.0);
}

void ChartController::maintainChartDataLimits()
{
    // Limit chart data points to prevent memory issues
    const int maxPoints = BabyMonitorConfig::CHART_MAX_POINTS;
    
    if (tempSeries_ && tempSeries_->count() > maxPoints) {
        tempSeries_->removePoints(0, tempSeries_->count() - maxPoints);
    }
    if (humSeries_ && humSeries_->count() > maxPoints) {
        humSeries_->removePoints(0, humSeries_->count() - maxPoints);
    }
    if (motionSeries_ && motionSeries_->count() > maxPoints) {
        motionSeries_->removePoints(0, motionSeries_->count() - maxPoints);
    }
}

} // namespace BabyMonitor
