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
#include <motionworker.h>
#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , led_(BabyMonitorConfig::LED_CHIP_NUMBER, BabyMonitorConfig::LED_PIN_NUMBER)
    , timeIndex(0)
    , errorHandler_(BabyMonitor::ErrorHandler::getInstance())
    , dht11ConsecutiveErrors_(0)
{
    ui->setupUi(this);
    setupCharts();
    initializeLED();

    // Initialize camera callback
    cameraCallback.window = this;
    camera.registerCallback(&cameraCallback);
    ui->motionStatusLabel->setText("No Motion");
    errorHandler_.reportInfo("Camera", "Callback registered successfully");
    systemStatus_.cameraActive = true;  // Mark camera as active

    // Initialize motion detection
    initializeMotionDetection();
    errorHandler_.reportInfo("MotionDetection", "Initialization completed");
    systemStatus_.motionDetectionActive = true;  // Mark motion detection as active

    if (!alarmPub_.init()) {
        handleSystemError("AlarmPublisher", "Initialization failed");
    } else {
        errorHandler_.reportInfo("AlarmPublisher", "Initialized successfully");
        systemStatus_.alarmSystemActive = true;  // Mark alarm system as active
    }

    // Start Qt timer: call timerEvent every 1000ms
    alarmTimerId_ = startTimer(BabyMonitorConfig::ALARM_TIMER_INTERVAL_MS);

    dhtWorker_ = new DHT11Worker(BabyMonitorConfig::GPIO_CHIP_DEVICE /*gpiochip*/,
                                 BabyMonitorConfig::DHT11_PIN_NUMBER /*BCM pin*/,
                                 this,         // Make its lifecycle depend on MainWindow
                                 BabyMonitorConfig::DHT11_READ_INTERVAL_S); // Use configured interval

    // 2) Connect signals to UI slots
    connect(dhtWorker_, &DHT11Worker::newReading,
            this,       &MainWindow::onNewDHTReading);
    connect(dhtWorker_, &DHT11Worker::errorReading,
            this,       &MainWindow::onDHTError);

    // 3) Start background thread loop reading
    dhtWorker_->start();


//    // PM2.5 detection
//    pmSensor = new SDS011Detector("/dev/ttyUSB0", 9600, 75.0f);
//    connect(pmSensor, &SDS011Detector::highPM, this, &MainWindow::onPMExceeded);
//    pmSensor->start();
    camera.start();
}

MainWindow::~MainWindow()
{

    if (alarmTimerId_ != -1) killTimer(alarmTimerId_);
    if (dhtWorker_) {
        dhtWorker_->stop();
        // dhtWorker_ is a child of this, no need to delete
    }
    cleanupMotionDetection();
    camera.stop();
    delete ui;
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
    if (!motionDetected_) {
        AlarmMsg msg;
        msg.index(samplesSent_++);
        msg.message("No motion detected !!!! Dangerous!");
        if (alarmPub_.publish(msg)) {
            qDebug() << "No-motion alarm sent:" << QString::fromStdString(msg.message());
        } else {
            qDebug() << "No listener, alarm not sent.";
        }
    }
    else{
        AlarmMsg msg;
        msg.index(samplesSent_++);
        msg.message("On motion !!!");
        if (alarmPub_.publish(msg)) {
            qDebug() << "motion msg sent:" << QString::fromStdString(msg.message());
        } else {
            qDebug() << "No listener, alarm not sent.";
    }
    motionDetected_ = false;
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
    // Update camera display
    updateImage(frame);

    // Emit frame for motion detection processing (using original logic)
    emit frameReady(frame.clone());

    // Trigger UI update (this was missing!)
    update();
}

void MainWindow::initializeMotionDetection()
{
    QThread *motionThread = new QThread(this);
    MotionWorker *worker = new MotionWorker(BabyMonitorConfig::MOTION_MIN_AREA,
                                           BabyMonitorConfig::MOTION_THRESHOLD);
    worker->moveToThread(motionThread);

    connect(motionThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &MainWindow::frameReady, worker, &MotionWorker::processFrame);
    connect(worker, &MotionWorker::motionDetected,
            this, &MainWindow::onMotionStatusChanged);

    // Start thread
    motionThread->start();

    // Store references for cleanup (could be improved with member variables)
    setProperty("motionThread", QVariant::fromValue(motionThread));
    setProperty("motionWorker", QVariant::fromValue(worker));
}

void MainWindow::cleanupMotionDetection()
{
    // Get stored references
    QThread* motionThread = property("motionThread").value<QThread*>();
    if (motionThread) {
        motionThread->quit();
        motionThread->wait();
        errorHandler_.reportInfo("MotionDetection", "Thread cleanup completed");
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

    // Log system status
    if (systemStatus_.isAllSystemsActive()) {
        errorHandler_.reportInfo("System", "All systems operational");
    } else {
        errorHandler_.reportWarning("System", "Some systems offline: " + statusText);
    }
}
