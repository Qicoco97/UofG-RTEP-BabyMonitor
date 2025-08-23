// MainWindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDateTime>
#include <QPixmap>
#include <motionworker.h>
#include <opencv2/opencv.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , timeIndex(0)
{
    ui->setupUi(this);
    setupCharts();
 	  myCallback.window = this;
	  camera.registerCallback(&myCallback);
    ui->motionStatusLabel->setText("无运动");
    
    
    QThread *motionThread = new QThread(this);
    MotionWorker *worker = new MotionWorker(500, 25);
    worker->moveToThread(motionThread);
    
    connect(motionThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &MainWindow::frameReady, worker, &MotionWorker::processFrame);
    connect(worker, &MotionWorker::motionDetected,
            this, &MainWindow::onMotionStatusChanged);
    
    // 启动线程
    motionThread->start();

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
      camera.start();
}

MainWindow::~MainWindow()
{
//    motionTimer->stop(); delete motionTimer;
//    tempSensor->stop(); delete tempSensor;
//    humSensor->stop(); delete humSensor;
//    pmSensor->stop(); delete pmSensor;
    camera.stop();
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
void MainWindow::updateImage(const cv::Mat &mat) {
	const QImage frame(mat.data, mat.cols, mat.rows, mat.step,
			   QImage::Format_RGB888);
	ui->cameraLabel->setPixmap(QPixmap::fromImage(frame));
  //bool motion = detectMotion(mat);
  emit frameReady(mat.clone()); 
	update();
}
/* bool MainWindow::detectMotion(const cv::Mat &currentFrame){
		int            thresh{25};    // 二值化阈值
		double         minArea{500};  // 最小运动面积
        // 1. 转灰度 + 高斯模糊
		if (previousFrame.empty()) {
        currentFrame.copyTo(previousFrame);
        ui->motionStatusLabel->setText("无运动");
        std:: cout<< "GGGGGG" << endl;
        return false;
			}
        cv::Mat gray, blur;
        cv::cvtColor(currentFrame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blur, cv::Size(21,21), 0);

        // 3. 计算差分并二值化
        cv::Mat delta, mask;
        cv::absdiff(previousFrame, blur, delta);
        cv::threshold(delta, mask, thresh, 255, cv::THRESH_BINARY);
        cv::dilate(mask, mask, cv::Mat(), cv::Point(-1,-1), 2);

        // 4. 查找轮廓
        std::vector<std::vector<cv::Point>> cons;
        cv::findContours(mask, cons, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        bool detected = false;
        for (auto &c : cons) {
            if (cv::contourArea(c) < minArea) continue;
            detected = true;
            break;
        }
        if(detected){
        ui->motionStatusLabel->setText("On motion");
        }
        else{
        ui->motionStatusLabel->setText("no motion");
        }
        previousFrame = blur;
		return detected;
} */

void MainWindow::onMotionStatusChanged(bool detected)
{
    // 这里更新 UI
    if (detected)
        ui->motionStatusLabel->setText(tr("On motion"));
    else
        ui->motionStatusLabel->setText(tr("no motion"));
}