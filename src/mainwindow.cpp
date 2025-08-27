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
{
    ui->setupUi(this);
    setupCharts();
    initializeLED();

    // Initialize camera callback
    cameraCallback.window = this;
    camera.registerCallback(&cameraCallback);
    ui->motionStatusLabel->setText("No Motion");

    // Initialize motion detection
    initializeMotionDetection();

    if (!alarmPub_.init()) {
        qWarning() << "AlarmPublisher initialization failed";
    }

    // Start Qt timer: call timerEvent every 1000ms
    alarmTimerId_ = startTimer(BabyMonitorConfig::ALARM_TIMER_INTERVAL_MS);

    dhtWorker_ = new DHT11Worker(BabyMonitorConfig::GPIO_CHIP_DEVICE /*gpiochip*/,
                                 BabyMonitorConfig::DHT11_PIN_NUMBER /*BCM pin*/,
                                 this);         // Make its lifecycle depend on MainWindow

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
  emit frameReady(mat.clone()); 
	update();
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

    // Emit frame for motion detection processing
    emit frameReady(frame.clone());
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
    }
}
