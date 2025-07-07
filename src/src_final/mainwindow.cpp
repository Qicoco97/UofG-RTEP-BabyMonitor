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
    , led_(0, 27)
    , timeIndex(0)
{
    ui->setupUi(this);
    setupCharts();
 	  myCallback.window = this;
	  camera.registerCallback(&myCallback);
    ui->motionStatusLabel->setText("无运动");
    
    if (!alarmPub_.init()) {
        qWarning() << "AlarmPublisher 初始化失败";
    }

    // 启动 Qt 定时器：1000ms 调一次 timerEvent
    alarmTimerId_ = startTimer(1000);
    
    
    QThread *motionThread = new QThread(this);
    MotionWorker *worker = new MotionWorker(500, 25);
    worker->moveToThread(motionThread);
    
    connect(motionThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &MainWindow::frameReady, worker, &MotionWorker::processFrame);
    connect(worker, &MotionWorker::motionDetected,
            this, &MainWindow::onMotionStatusChanged);
    
    // 启动线程
    motionThread->start();
    
    dhtWorker_ = new DHT11Worker("/dev/gpiochip0" /*gpiochip*/, 
                                 17             /*BCM4引脚*/, 
                                 this);         // 让它的生命周期依赖于 MainWindow

    // 2) 连接信号到 UI 槽
    connect(dhtWorker_, &DHT11Worker::newReading,
            this,       &MainWindow::onNewDHTReading);
    connect(dhtWorker_, &DHT11Worker::errorReading,
            this,       &MainWindow::onDHTError);

    // 3) 启动后台线程循环读取
    dhtWorker_->start();


//    // PM2.5 检测
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
        // dhtWorker_ 是 this 的 child，不必 delete
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

    // 2) 新建一个 Chart，把两个系列都加进去
    chart = new QtCharts::QChart();
    chart->addSeries(tempSeries);
    chart->addSeries(humSeries);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // 3) 建立统一的 X/Y 轴
    axisX = new QtCharts::QValueAxis();
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Index");
    chart->addAxis(axisX, Qt::AlignBottom);
    tempSeries->attachAxis(axisX);
    humSeries ->attachAxis(axisX);

    axisY = new QtCharts::QValueAxis();
    axisY->setRange(0, 100);  // 温湿度都在 0–100
    axisY->setTitleText("Value");
    axisY->setLabelsVisible(true);

    chart->addAxis(axisY, Qt::AlignLeft);
    tempSeries->attachAxis(axisY);
    humSeries ->attachAxis(axisY);

    // 4) 只把这个 chart 绑定到 UI 的 ChartView
    ui->thChartView->setChart(chart);
    ui->thChartView->setRenderHint(QPainter::Antialiasing);

    // 初始化 timeIndex
    timeIndex = 0;
}



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
    // 这里更新 UI
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

    // 显示到标签，保留两位小数
     // 这里的槽一定在 GUI 线程被调用
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

    // 让 X 轴始终展示最近 100 个样本
    int minX = std::max(0, timeIndex - 100);
    axisX->setRange(minX, timeIndex);



   
}
void MainWindow::onDHTError()
{
    ui->tempLabel->setText("Read Err");
    ui->humLabel->setText("Read Err");
}
