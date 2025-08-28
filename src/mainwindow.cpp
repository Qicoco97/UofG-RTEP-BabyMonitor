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
    , alertSound_(new QSoundEffect(this))   // 🔔 初始化声音对象
{
    ui->setupUi(this);
    setupCharts();
    myCallback.window = this;
    camera.registerCallback(&myCallback);
    ui->motionStatusLabel->setText("No Motion");

    if (!alarmPub_.init()) {
        qWarning() << "AlarmPublisher initialization failed";
    }

    // 配置声音文件（请确保项目目录或资源中有 alarm.wav）
    alertSound_->setSource(QUrl::fromLocalFile("alarm.wav"));
    alertSound_->setLoopCount(QSoundEffect::Infinite);
    alertSound_->setVolume(0.8f);

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

    chart = new QtCharts::QChart();
    chart->addSeries(tempSeries);
    chart->addSeries(humSeries);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    axisX = new QtCharts::QValueAxis();
    axisX->setLabelFormat("%g");
    axisX->setTitleText("Index");
    chart->addAxis(axisX, Qt::AlignBottom);
    tempSeries->attachAxis(axisX);
    humSeries ->attachAxis(axisX);

    axisY = new QtCharts::QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText("Value");
    axisY->setLabelsVisible(true);

    chart->addAxis(axisY, Qt::AlignLeft);
    tempSeries->attachAxis(axisY);
    humSeries ->attachAxis(axisY);

    ui->thChartView->setChart(chart);
    ui->thChartView->setRenderHint(QPainter::Antialiasing);

    timeIndex = 0;
}

void MainWindow::updateImage(const cv::Mat &mat) {
    const QImage frame(mat.data, mat.cols, mat.rows, mat.step,
                       QImage::Format_RGB888);
    ui->cameraLabel->setPixmap(QPixmap::fromImage(frame));
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
            if (alertSound_ && !alertSound_->isPlaying()) {
                alertSound_->play();   // 🔔 播放报警音
            }
        } else {
            qDebug() << "No listener, alarm not sent.";
        }
    }
    else {
        AlarmMsg msg;
        msg.index(samplesSent_++);
        msg.message("On motion !!!");
        if (alarmPub_.publish(msg)) {
            qDebug() << "motion msg sent:" << QString::fromStdString(msg.message());
        } else {
            qDebug() << "No listener, alarm not sent.";
        }
        motionDetected_ = false;

        // 停止报警声
        if (alertSound_ && alertSound_->isPlaying()) {
            alertSound_->stop();
        }
    }
}

void MainWindow::onMotionStatusChanged(bool detected)
{
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

    ui->tempLabel->setText(
        QString::number(temperature, 'f', 2) + " ℃"
    );
    ui->humLabel->setText(
        QString::number(humidity,    'f', 2) + " %"
    );

    tempSeries->append(timeIndex, temperature);
    humSeries ->append(timeIndex, humidity);
    timeIndex++;

    int minX = std::max(0, timeIndex - 100);
    axisX->setRange(minX, timeIndex);
}

void MainWindow::onDHTError()
{
    ui->tempLabel->setText("Read Err");
    ui->humLabel->setText("Read Err");
}
