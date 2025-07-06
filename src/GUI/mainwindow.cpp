// MainWindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDateTime>
#include <QPixmap>
//#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , timeIndex(0)
{
    ui->setupUi(this);
    setupCharts();

//    // 摄像头回调 (使用 libcam2opencv 回调注册)
//    motionTimer = new MotionTimer(10, 5000, 1000, 1);
//    connect(motionTimer, &MotionTimer::frameReady, this, &MainWindow::onNewFrame);
//    connect(motionTimer, &MotionTimer::motion, this, &MainWindow::onMotionDetected);
//    connect(motionTimer, &MotionTimer::noMotion, this, &MainWindow::onNoMotion);
//    motionTimer->start();

//    // 温度传感器
//    tempSensor = new TemperatureSensor(5, 10000, 60.0f); // GPIO pin 5
//    connect(tempSensor, &TemperatureSensor::temperatureUpdated, this, &MainWindow::onTemperatureUpdated);
//    tempSensor->start();

//    // 湿度传感器
//    humSensor = new HumiditySensor(6, 10000, 40.0f); // GPIO pin 6
//    connect(humSensor, &HumiditySensor::humidityUpdated, this, &MainWindow::onHumidityUpdated);
//    humSensor->start();

//    // PM2.5 检测
//    pmSensor = new SDS011Detector("/dev/ttyUSB0", 9600, 75.0f);
//    connect(pmSensor, &SDS011Detector::highPM, this, &MainWindow::onPMExceeded);
//    pmSensor->start();
}

MainWindow::~MainWindow()
{
//    motionTimer->stop(); delete motionTimer;
//    tempSensor->stop(); delete tempSensor;
//    humSensor->stop(); delete humSensor;
//    pmSensor->stop(); delete pmSensor;
    delete ui;
}

void MainWindow::setupCharts()
{
//    tempSeries = new QtCharts::QLineSeries();
//    humSeries = new QtCharts::QLineSeries();
//    motionSeries = new QtCharts::QLineSeries();

//    tempChart = new QtCharts::QChart();
//    tempChart->addSeries(tempSeries);
//    tempChart->createDefaultAxes();
//    ui->thChartView->setChart(tempChart);

//    humChart = new QtCharts::QChart();
//    humChart->addSeries(humSeries);
//    humChart->createDefaultAxes();
//    ui->thChartView->setChart(humChart);

//    motionChart = new QtCharts::QChart();
//    motionChart->addSeries(motionSeries);
//    motionChart->createDefaultAxes();
//    ui->motionChartView->setChart(motionChart);
}

//void MainWindow::onNewFrame(const QImage &img)
//{
//    ui->cameraLabel->setPixmap(QPixmap::fromImage(img));
//}

//void MainWindow::onMotionDetected()
//{
//    ui->motionStatusLabel->setText("运动中");
//    motionSeries->append(timeIndex, 1);
//    timeIndex++;
//}

//void MainWindow::onNoMotion()
//{
//    ui->motionStatusLabel->setText("无运动");
//    motionSeries->append(timeIndex, 0);
//    timeIndex++;
//}

//void MainWindow::onTemperatureUpdated(float temp)
//{
//    ui->tempLabel->setText(QString("温度: %1 °C").arg(temp));
//    tempSeries->append(timeIndex, temp);
//    timeIndex++;
//}

//void MainWindow::onHumidityUpdated(float hum)
//{
//    ui->humLabel->setText(QString("湿度: %1 %").arg(hum));
//    humSeries->append(timeIndex, hum);
//    timeIndex++;
//}

//void MainWindow::onPMExceeded(float pm25, float pm10)
//{
//    // 在界面上可弹框或状态提示
//    QMessageBox::warning(this, "PM2.5 超标", QString("PM2.5: %1 μg/m³").arg(pm25));
//}
