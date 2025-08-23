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
    , led_(0, 27)
    , timeIndex(0)
{
    ui->setupUi(this);
    setupCharts();
 	  myCallback.window = this;
	  camera.registerCallback(&myCallback);
    ui->motionStatusLabel->setText("No Motion");

    if (!alarmPub_.init()) {
        qWarning() << "AlarmPublisher initialization failed";
    }

    // Start Qt timer: call timerEvent every 1000ms
    alarmTimerId_ = startTimer(1000);


    QThread *motionThread = new QThread(this);
    MotionWorker *worker = new MotionWorker(500, 25);
    worker->moveToThread(motionThread);

    connect(motionThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &MainWindow::frameReady, worker, &MotionWorker::processFrame);
    connect(worker, &MotionWorker::motionDetected,
            this, &MainWindow::onMotionStatusChanged);

    // Start thread
    motionThread->start();

    dhtWorker_ = new DHT11Worker("/dev/gpiochip0" /*gpiochip*/,
                                 17             /*BCM pin 17*/,
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
    camera.stop();
    delete ui;
}

void MainWindow::setupCharts()
{
    tempSeries = new QtCharts::QLineSeries();
    tempSeries->setName("temperature (℃)");
    humSeries  = new QtCharts::QLineSeries();
    humSeries ->setName("humidity (%)");

    // 2) Create a new Chart and add both series to it
    chart = new QtCharts::QChart();
    chart->addSeries(tempSeries);
    chart->addSeries(humSeries);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // 3) Create unified X/Y axes
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

    // 4) Bind this chart to the UI's ChartView
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
    // Update UI here
    motionDetected_ = detected;
    if (detected){
        ui->motionStatusLabel->setText(tr("On motion"));
        led_.blink(5, 200, 100);
        }
    else
        ui->motionStatusLabel->setText(tr("no motion"));
}

void MainWindow::onNewDHTReading(int t_int, int t_dec,
                                 int h_int, int h_dec)
{
    float temperature = t_int + t_dec / 100.0f;
    float humidity    = h_int + h_dec    / 100.0f;

    // Display on labels, keep two decimal places
     // This slot is definitely called in the GUI thread
    ui->tempLabel->setText(
        QString::number(temperature, 'f', 2) + " ℃"
    );
    //tempSeries->append(timeIndex, temperature);
    ui->humLabel->setText(
        QString::number(humidity,    'f', 2) + " %"
    );
    //humSeries->append(timeIndex, humidity);
    //timeIndex++;

    tempSeries->append(timeIndex, temperature);
    humSeries ->append(timeIndex, humidity);
    timeIndex++;

    // Make X-axis always show the latest 100 samples
    int minX = std::max(0, timeIndex - 100);
    axisX->setRange(minX, timeIndex);



   
}
void MainWindow::onDHTError()
{
    ui->tempLabel->setText("Read Err");
    ui->humLabel->setText("Read Err");
}
