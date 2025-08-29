// MainWindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDateTime>
#include <QPixmap>
#include <QDebug>
#include <QString>
#include <QThread>
#include <QImage>
#include <QCoreApplication>
#include <QMediaPlayer>
#include <QUrl>
#include <QKeyEvent>
#include <motionworker.h>
#include "performance/PerformanceMonitor.h"
#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
    , led_(BabyMonitorConfig::LED_CHIP_NUMBER, BabyMonitorConfig::LED_PIN_NUMBER)
    , timeIndex(0)
    , errorHandler_(BabyMonitor::ErrorHandler::getInstance())
    , perfMonitor_(&BabyMonitor::PerformanceMonitor::getInstance())
    , frameTimer_(new BabyMonitor::HighPrecisionTimer())
    , alarmTimer_(new BabyMonitor::HighPrecisionTimer())
    , isFrameProcessingAdapted_(false)
    , frameSkipCounter_(0)
    , adaptiveFrameSkip_(1)
    , dht11ConsecutiveErrors_(0)
    , motionThread_(nullptr)
    , motionWorker_(nullptr)
    , injectedAlarmSystem_(nullptr)
    , audioPlayer_(nullptr)
    , noMotionCount_(0)
    , alarmPlaying_(false)
{
    ui->setupUi(this);
    setupCharts();
    initializeLED();
    initializeAudioPlayer();
    initializePerformanceMonitoring();

    // Initialize all sensors
    initializeSensors();

    // AlarmSystem will be initialized via dependency injection
    systemStatus_.alarmSystemActive = false; // Will be set to true when injected system is initialized

    // Start Qt timer: call timerEvent every 1000ms
    alarmTimerId_ = startTimer(BabyMonitorConfig::ALARM_TIMER_INTERVAL_MS);

    // Start all sensors
    startSensors();
}

MainWindow::~MainWindow()
{
    // Stop all sensors
    stopSensors();

    if (alarmTimerId_ != -1) killTimer(alarmTimerId_);

    // Clean up audio player
    if (audioPlayer_) {
        audioPlayer_->stop();
        delete audioPlayer_;
        audioPlayer_ = nullptr;
    }

    // Clean up performance timers
    delete frameTimer_;
    delete alarmTimer_;

    // ui is now managed by unique_ptr, no manual delete needed
}

void MainWindow::setupCharts()
{
    tempSeries = new QtCharts::QLineSeries();
    tempSeries->setName("temperature (℃)");
    humSeries  = new QtCharts::QLineSeries();
    humSeries ->setName("humidity (%)");

    // Create a new Chart and add both series to it
    chart = new QtCharts::QChart();
    chart->addSeries(tempSeries);
    chart->addSeries(humSeries);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Configure chart axes
    configureChartAxes();

    // Bind this chart to the UI's ChartView
    ui->thChartView->setChart(chart);
    ui->thChartView->setRenderHint(QPainter::Antialiasing);

    // Initialize timeIndex
    timeIndex = 0;
}



//void MainWindow::onPMExceeded(float pm25, float pm10)
//{
//    // Show popup or status notification on the interface
//    QMessageBox::warning(this, "PM2.5 Exceeded", QString("PM2.5: %1 μg/m³").arg(pm25));
//}
void MainWindow::updateImage(const cv::Mat &mat) {
	const QImage frame(mat.data, mat.cols, mat.rows, mat.step,
			   QImage::Format_RGB888);
	ui->cameraLabel->setPixmap(QPixmap::fromImage(frame));
  //bool motion = detectMotion(mat);
  // Note: frameReady emission moved to processNewFrame()
}

void MainWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId() != alarmTimerId_) {
        QMainWindow::timerEvent(event);
        return;
    }

    // Start alarm response timing
    alarmTimer_->start();

    // Use injected alarm system
    if (injectedAlarmSystem_) {
        if (!motionDetected_) {
            noMotionCount_++;
            QString message = QString("No motion detected !!!! Dangerous! (Sample #%1, Count: %2)").arg(samplesSent_++).arg(noMotionCount_);
            injectedAlarmSystem_->publishAlarm(message, 3); // High severity

            // Play alarm sound after reaching threshold
            if (noMotionCount_ >= BabyMonitorConfig::NO_MOTION_ALARM_THRESHOLD && !alarmPlaying_) {
                playAlarmSound();
            }
        } else {
            noMotionCount_ = 0; // Reset counter when motion is detected
            QString message = QString("On motion !!! (Sample #%1)").arg(samplesSent_++);
            injectedAlarmSystem_->publishAlarm(message, 1); // Low severity
            motionDetected_ = false;
        }
    } else {
        errorHandler_.reportWarning("MainWindow", "No AlarmSystem available for publishing");
    }

    // Record alarm response performance
    double alarmResponseTime = alarmTimer_->elapsedMs();
    if (perfMonitor_) {
        perfMonitor_->recordLatency("MainWindow", "AlarmResponse", alarmResponseTime);
    }
}


void MainWindow::onMotionStatusChanged(bool detected)
{
    // Update structured motion data
    lastMotionData_ = BabyMonitor::MotionData(detected, 0.8);

    // Update UI here
    motionDetected_ = detected;
    if (detected){
        ui->motionStatusLabel->setText(tr("On motion"));
        triggerMotionAlert();
        }
    else
        ui->motionStatusLabel->setText(tr("no motion"));
}

void MainWindow::onNewDHTReading(int t_int, int t_dec,
                                 int h_int, int h_dec)
{
    float temperature = t_int + t_dec / 100.0f;
    float humidity    = h_int + h_dec    / 100.0f;

    // Update structured sensor data
    lastTempHumData_ = BabyMonitor::TemperatureHumidityData(temperature, humidity, true);

    // Check if this is a recovery from error state
    bool wasInErrorState = (dht11ConsecutiveErrors_ > 0);

    // Reset consecutive error count on successful reading
    dht11ConsecutiveErrors_ = 0;
    systemStatus_.dht11Active = true;

    // Report successful reading
    static int readingCount = 0;
    readingCount++;

    if (wasInErrorState) {
        // Always report when recovering from error state
        errorHandler_.reportInfo("DHT11", QString("Sensor recovered - Reading successful (T:%1°C, H:%2%)")
                                .arg(temperature, 0, 'f', 1).arg(humidity, 0, 'f', 1));
    } else if (readingCount % 20 == 0) {  // Report every 20th reading when stable
        errorHandler_.reportInfo("DHT11", QString("Sensor stable - Reading #%1 (T:%2°C, H:%3%)")
                                .arg(readingCount).arg(temperature, 0, 'f', 1).arg(humidity, 0, 'f', 1));
    }

    // Display on labels, keep two decimal places
     // This slot is definitely called in the GUI thread
    ui->tempLabel->setText(
        QString::number(temperature, 'f', 2) + " ℃"
    );
    ui->humLabel->setText(
        QString::number(humidity,    'f', 2) + " %"
    );

    // Update chart with structured data
    updateTemperatureHumidityChart(lastTempHumData_);



   
}
void MainWindow::onDHTError()
{
    // Update structured sensor data with invalid reading
    lastTempHumData_ = BabyMonitor::TemperatureHumidityData(0.0f, 0.0f, false);

    // Increment consecutive error count
    dht11ConsecutiveErrors_++;

    // Only mark system as offline after multiple consecutive failures
    if (dht11ConsecutiveErrors_ >= DHT11_MAX_CONSECUTIVE_ERRORS) {
        if (systemStatus_.dht11Active) {  // Only report once when transitioning to offline
            handleSystemError("DHT11", QString("Sensor offline after %1 consecutive failures")
                            .arg(dht11ConsecutiveErrors_));
        }
    } else {
        // Report individual errors but don't mark system as offline yet
        errorHandler_.reportWarning("DHT11", QString("Reading failed (%1/%2)")
                                  .arg(dht11ConsecutiveErrors_).arg(DHT11_MAX_CONSECUTIVE_ERRORS));
    }

    ui->tempLabel->setText("Read Err");
    ui->humLabel->setText("Read Err");
}

// LED control methods implementation
void MainWindow::initializeLED()
{
    // LED is initialized in constructor member initializer list
    // This method can be extended for additional LED setup if needed
}

void MainWindow::triggerMotionAlert()
{
    led_.blink(BabyMonitorConfig::LED_BLINK_COUNT,
               BabyMonitorConfig::LED_ON_DURATION_MS,
               BabyMonitorConfig::LED_OFF_DURATION_MS);
}

// Chart management methods implementation
void MainWindow::configureChartAxes()
{
    // Create unified X/Y axes
    axisX = new QtCharts::QValueAxis();
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Index");
    chart->addAxis(axisX, Qt::AlignBottom);
    tempSeries->attachAxis(axisX);
    humSeries ->attachAxis(axisX);

    axisY = new QtCharts::QValueAxis();
    axisY->setRange(0, 100);  // Temperature and humidity both in 0-100 range
    axisY->setTitleText("Value");
    axisY->setLabelsVisible(true);

    chart->addAxis(axisY, Qt::AlignLeft);
    tempSeries->attachAxis(axisY);
    humSeries ->attachAxis(axisY);
}

void MainWindow::updateTemperatureHumidityChart(const BabyMonitor::TemperatureHumidityData& data)
{
    if (!data.isValid) {
        return; // Don't add invalid data to chart
    }

    tempSeries->append(timeIndex, data.temperature);
    humSeries->append(timeIndex, data.humidity);
    timeIndex++;

    // Make X-axis always show the latest configured max points
    int minX = std::max(0, timeIndex - BabyMonitorConfig::CHART_MAX_POINTS);
    axisX->setRange(minX, timeIndex);
}

void MainWindow::updateMotionChart(const BabyMonitor::MotionData& data)
{
    // Motion chart implementation can be added here in future
    // For now, we just store the data in lastMotionData_
    Q_UNUSED(data);
}

// Frame processing methods implementation
void MainWindow::processNewFrame(const cv::Mat& frame)
{
    // Start frame processing timing
    frameTimer_->start();

    // Adaptive frame processing - skip frames if performance is poor
    frameSkipCounter_++;
    if (isFrameProcessingAdapted_ && frameSkipCounter_ % adaptiveFrameSkip_ != 0) {
        return; // Skip this frame for performance
    }

    // Update camera display
    updateImage(frame);

    // Emit frame for motion detection processing (using original logic)
    emit frameReady(frame.clone());

    // Trigger UI update (this was missing!)
    update();

    // Record frame processing performance
    double frameProcessingTime = frameTimer_->elapsedMs();
    if (perfMonitor_) {
        perfMonitor_->recordLatency("MainWindow", "FrameProcessing", frameProcessingTime);

        // Check if frame processing adaptation is needed
        if (perfMonitor_->shouldAdaptPerformance("MainWindow", "FrameProcessing")) {
            adaptFrameProcessing();
        } else if (isFrameProcessingAdapted_ && perfMonitor_->canRecoverPerformance("MainWindow", "FrameProcessing")) {
            recoverFrameProcessing();
        }
    }
}

void MainWindow::initializeMotionDetection()
{
    // Create motion detection using factory
    auto motionSetup = BabyMonitor::SensorFactory::createMotionDetection(this);
    motionThread_ = motionSetup.thread;
    motionWorker_ = motionSetup.worker;

    // Setup connections using factory
    BabyMonitor::SensorFactory::connectMotionDetection(motionSetup, this, this);

    // Connect performance alert signal
    connect(motionWorker_, &MotionWorker::performanceAlert,
            this, &MainWindow::onMotionWorkerPerformanceAlert);

    // Start thread
    motionThread_->start();
}

void MainWindow::cleanupMotionDetection()
{
    if (motionThread_) {
        motionThread_->quit();
        motionThread_->wait();
        errorHandler_.reportInfo("MotionDetection", "Thread cleanup completed");

        // Thread and worker will be cleaned up automatically as children of this
        motionThread_ = nullptr;
        motionWorker_ = nullptr;
    }
}

// Error handling methods implementation
void MainWindow::handleSystemError(const QString& component, const QString& message)
{
    errorHandler_.reportError(component, message);

    // Update system status
    if (component == "DHT11") {
        systemStatus_.dht11Active = false;
    } else if (component == "Camera") {
        systemStatus_.cameraActive = false;
    } else if (component == "MotionDetection") {
        systemStatus_.motionDetectionActive = false;
    } else if (component == "AlarmPublisher") {
        systemStatus_.alarmSystemActive = false;
    }

    updateSystemStatus();
}

void MainWindow::handleCriticalError(const QString& component, const QString& message)
{
    errorHandler_.reportCritical(component, message);

    // For critical errors, we might want to disable the entire system
    systemStatus_.cameraActive = false;
    systemStatus_.dht11Active = false;
    systemStatus_.motionDetectionActive = false;
    systemStatus_.alarmSystemActive = false;

    updateSystemStatus();

    // Could show a critical error dialog to user
    ui->motionStatusLabel->setText("SYSTEM ERROR");
}

void MainWindow::updateSystemStatus()
{
    systemStatus_.lastUpdate = QDateTime::currentDateTime();

    // Update UI based on system status
    QString statusText = systemStatus_.toString();

    // Add performance information to status
    if (BabyMonitorConfig::ENABLE_PERFORMANCE_MONITORING && perfMonitor_) {
        auto motionStats = perfMonitor_->getStats("MotionWorker", "MotionDetection");
        auto frameStats = perfMonitor_->getStats("MainWindow", "FrameProcessing");
        auto alarmStats = perfMonitor_->getStats("AlarmSystem", "AlarmResponse");

        if (motionStats && frameStats) {
            // Show performance levels as percentages
            double motionLevel = perfMonitor_->getPerformanceLevel("MotionWorker", "MotionDetection") * 100;
            double frameLevel = perfMonitor_->getPerformanceLevel("MainWindow", "FrameProcessing") * 100;

            statusText += QString(" | PERF: M:%1% F:%2%")
                         .arg(motionLevel, 0, 'f', 0)
                         .arg(frameLevel, 0, 'f', 0);

            if (alarmStats) {
                double alarmLevel = perfMonitor_->getPerformanceLevel("AlarmSystem", "AlarmResponse") * 100;
                statusText += QString(" A:%1%").arg(alarmLevel, 0, 'f', 0);
            }
        }

        if (isFrameProcessingAdapted_) {
            statusText += " | ⚠️ADAPTED";
        }
    }

    // Update motion status label with performance info
    ui->motionStatusLabel->setText(statusText);

    // Log system status
    if (systemStatus_.isAllSystemsActive()) {
        errorHandler_.reportInfo("System", "All systems operational");
    } else {
        errorHandler_.reportWarning("System", "Some systems offline: " + statusText);
    }
}

// Sensor management methods implementation
void MainWindow::initializeSensors()
{
    // Initialize camera
    cameraCallback.window = this;
    camera.registerCallback(&cameraCallback);
    ui->motionStatusLabel->setText("No Motion");
    errorHandler_.reportInfo("Camera", "Callback registered successfully");
    systemStatus_.cameraActive = true;

    // Initialize DHT11 sensor
    initializeDHT11Sensor();

    // Initialize motion detection
    initializeMotionDetection();
    errorHandler_.reportInfo("MotionDetection", "Initialization completed");
    systemStatus_.motionDetectionActive = true;
}

void MainWindow::startSensors()
{
    // Start camera
    camera.start();
    errorHandler_.reportInfo("Camera", "Started successfully");

    // Start DHT11 sensor
    if (dhtWorker_) {
        dhtWorker_->start();
        errorHandler_.reportInfo("DHT11", "Started successfully");
    }

    // Motion detection starts automatically when thread starts
}

void MainWindow::stopSensors()
{
    // Stop camera
    camera.stop();
    errorHandler_.reportInfo("Camera", "Stopped");

    // Stop and cleanup sensors
    cleanupDHT11Sensor();
    cleanupMotionDetection();
}

void MainWindow::initializeDHT11Sensor()
{
    // Create DHT11 sensor using factory
    dhtWorker_ = BabyMonitor::SensorFactory::createDHT11Worker(this);

    // Connect signals to UI slots
    connect(dhtWorker_, &DHT11Worker::newReading,
            this,       &MainWindow::onNewDHTReading);
    connect(dhtWorker_, &DHT11Worker::errorReading,
            this,       &MainWindow::onDHTError);

    errorHandler_.reportInfo("DHT11", "Sensor initialized");
}

void MainWindow::cleanupDHT11Sensor()
{
    if (dhtWorker_) {
        dhtWorker_->stop();
        errorHandler_.reportInfo("DHT11", "Sensor stopped");
        // dhtWorker_ is a child of this, no need to delete
    }
}

// Dependency injection methods implementation
void MainWindow::setAlarmSystem(std::shared_ptr<BabyMonitor::IAlarmSystem> alarmSystem)
{
    injectedAlarmSystem_ = alarmSystem;

    // Initialize and start the injected alarm system
    if (injectedAlarmSystem_) {
        if (injectedAlarmSystem_->initialize()) {
            injectedAlarmSystem_->start();
            systemStatus_.alarmSystemActive = true;
            errorHandler_.reportInfo("DependencyInjection", "Injected AlarmSystem initialized and started");
        } else {
            systemStatus_.alarmSystemActive = false;
            errorHandler_.reportError("DependencyInjection", "Failed to initialize injected AlarmSystem");
        }
    }

    errorHandler_.reportInfo("DependencyInjection", "AlarmSystem dependency injected successfully");
}

// Performance monitoring methods implementation
void MainWindow::initializePerformanceMonitoring()
{
    // Initialize performance monitoring state
    isFrameProcessingAdapted_ = false;
    frameSkipCounter_ = 0;
    adaptiveFrameSkip_ = 1;

    // Setup performance reporting timer
    performanceReportTimer_ = new QTimer(this);
    connect(performanceReportTimer_, &QTimer::timeout, this, &MainWindow::logPerformanceReport);
    performanceReportTimer_->start(BabyMonitorConfig::PERFORMANCE_CHECK_INTERVAL_MS); // Every 5 seconds

    errorHandler_.reportInfo("PerformanceMonitor", "Performance monitoring initialized with periodic reporting");
    errorHandler_.reportInfo("PerformanceTest", "HOTKEYS: P=Report, T=Progressive Test, S=Severe Test, A=Force Adapt, R=Reset&Recover");
}

void MainWindow::adaptFrameProcessing()
{
    if (isFrameProcessingAdapted_) return; // Already adapted

    // Start skipping every other frame to improve performance
    adaptiveFrameSkip_ = 2;
    isFrameProcessingAdapted_ = true;

    errorHandler_.reportInfo("PerformanceMonitor",
        "Frame processing adapted: skipping frames for performance");
}

void MainWindow::recoverFrameProcessing()
{
    if (!isFrameProcessingAdapted_) return; // Not in adapted mode

    // Restore normal frame processing
    adaptiveFrameSkip_ = 1;
    isFrameProcessingAdapted_ = false;

    errorHandler_.reportInfo("PerformanceMonitor",
        "Frame processing recovered: restored full frame rate");
}

void MainWindow::onMotionWorkerPerformanceAlert(const QString& message)
{
    errorHandler_.reportInfo("MotionWorker", message);

    // Update system status when motion worker adapts
    updateSystemStatus();
}

void MainWindow::logPerformanceReport()
{
    if (perfMonitor_) {
        perfMonitor_->logPerformanceReport();
    }
}

void MainWindow::triggerPerformanceTest()
{
    if (!perfMonitor_) return;

    static int testLevel = 1;

    errorHandler_.reportInfo("PerformanceTest",
        QString("=== PERFORMANCE STRESS TEST LEVEL %1 STARTED ===").arg(testLevel));

    // Progressive stress testing - each press increases stress level
    double motionStress = 25.0 * testLevel;    // Will exceed 50ms limit at level 3
    double frameStress = 10.0 * testLevel;     // Will exceed 20ms limit at level 3
    double alarmStress = 50.0 * testLevel;     // Will exceed 100ms limit at level 3

    // Simulate stress with increasing intensity
    perfMonitor_->simulatePerformanceStress("MotionWorker", "MotionDetection", motionStress);
    perfMonitor_->simulatePerformanceStress("MainWindow", "FrameProcessing", frameStress);
    perfMonitor_->simulatePerformanceStress("AlarmSystem", "AlarmResponse", alarmStress);

    errorHandler_.reportInfo("PerformanceTest",
        QString("Applied stress: Motion +%1ms, Frame +%2ms, Alarm +%3ms")
        .arg(motionStress).arg(frameStress).arg(alarmStress));

    testLevel++;
    if (testLevel > 5) testLevel = 1; // Reset after 5 levels

    errorHandler_.reportInfo("PerformanceTest",
        "Performance stress simulation completed - watch for adaptation in next few frames");
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_P:
        // Press 'P' to trigger performance report
        logPerformanceReport();
        break;
    case Qt::Key_T:
        // Press 'T' to trigger progressive performance stress test
        triggerPerformanceTest();
        break;
    case Qt::Key_S:
        // Press 'S' to trigger SEVERE stress test (guaranteed to trigger adaptation)
        if (perfMonitor_) {
            errorHandler_.reportInfo("PerformanceTest", "=== SEVERE STRESS TEST - FORCING ADAPTATION ===");

            // Add stress that will definitely exceed 80% threshold
            perfMonitor_->simulatePerformanceStress("MotionWorker", "MotionDetection", 45.0);  // Will push to ~90%
            perfMonitor_->simulatePerformanceStress("MainWindow", "FrameProcessing", 18.0);    // Will push to ~90%
            perfMonitor_->simulatePerformanceStress("AlarmSystem", "AlarmResponse", 90.0);     // Will push to ~90%

            // Force immediate adaptation check
            perfMonitor_->forceAdaptationCheck();

            errorHandler_.reportInfo("PerformanceTest", "Severe stress applied - checking for immediate adaptation");
        }
        break;
    case Qt::Key_A:
        // Press 'A' to manually trigger adaptation (for demonstration)
        errorHandler_.reportInfo("PerformanceTest", "=== MANUAL ADAPTATION TRIGGER ===");
        adaptFrameProcessing();

        // Trigger motion worker adaptation
        if (motionWorker_) {
            motionWorker_->forceAdaptation();
            errorHandler_.reportInfo("PerformanceTest", "Motion detection adaptation triggered manually");
        }

        errorHandler_.reportInfo("PerformanceTest", "All adaptations triggered - check UI for ADAPTED status");
        break;
    case Qt::Key_R:
        // Press 'R' to reset performance statistics and recover
        if (perfMonitor_) {
            perfMonitor_->clearStats();
            errorHandler_.reportInfo("PerformanceTest", "Performance statistics cleared");

            // Force recovery for all components
            recoverFrameProcessing();
            if (motionWorker_) {
                motionWorker_->forceRecovery();
            }

            errorHandler_.reportInfo("PerformanceTest", "All components recovered to normal performance mode");
        }
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

// Audio alarm methods implementation
void MainWindow::initializeAudioPlayer()
{
    audioPlayer_ = new QMediaPlayer(this);

    // Connect state change signal to handle playback completion
    connect(audioPlayer_, &QMediaPlayer::stateChanged,
            this, &MainWindow::onAudioPlayerStateChanged);

    // Set the alarm sound file path
    QString alarmPath = QCoreApplication::applicationDirPath() + "/" + BabyMonitorConfig::ALARM_SOUND_FILE;
    audioPlayer_->setMedia(QUrl::fromLocalFile(alarmPath));

    errorHandler_.reportInfo("AudioPlayer", "Audio player initialized with alarm file: " + alarmPath);
}

void MainWindow::playAlarmSound()
{
    if (!audioPlayer_ || alarmPlaying_) {
        return; // Don't play if already playing or player not initialized
    }

    alarmPlaying_ = true;
    audioPlayer_->play();
    errorHandler_.reportInfo("AudioPlayer", "Playing alarm sound due to no motion detected");
}

void MainWindow::onAudioPlayerStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState) {
        alarmPlaying_ = false;
        // Reset the media position for next playback
        audioPlayer_->setPosition(0);
        errorHandler_.reportInfo("AudioPlayer", "Alarm sound playback completed");
    }
}
