//#ifndef MAINWINDOW_H
//#define MAINWINDOW_H

//#include <QMainWindow>

//QT_BEGIN_NAMESPACE
//namespace Ui { class MainWindow; }
//QT_END_NAMESPACE

//class MainWindow : public QMainWindow
//{
//    Q_OBJECT

//public:
//    MainWindow(QWidget *parent = nullptr);
//    ~MainWindow();

//private:
//    Ui::MainWindow *ui;
//};
//#endif // MAINWINDOW_H
// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "libcam2opencv.h"
#include <QMainWindow>
#include <QLabel>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
//#include "MotionTimer.h"
//#include "TemperatureSensor.h"
//#include "HumiditySensor.h"
//#include "SDS011Detector.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateImage(const cv::Mat &mat);
    bool detectMotion(const cv::Mat &currentFrame);
    
    struct MyCallback : Libcam2OpenCV::Callback {
	    MainWindow* window = nullptr;
	      virtual void hasFrame(const cv::Mat &frame, const libcamera::ControlList &) {
	    if (nullptr != window) {
		window->updateImage(frame);
	    }
	}
    };
    Libcam2OpenCV camera;
    MyCallback myCallback;
    
signals:
    // 每当有新帧时就 emit 这个信号
    void frameReady(const cv::Mat &mat);

private slots:
    void onMotionStatusChanged(bool detected);

//private slots:
//    void onNewFrame(const QImage &img);
//    void onMotionDetected();
//    void onNoMotion();
//    void onTemperatureUpdated(float temp);
//    void onHumidityUpdated(float hum);
//    void onPMExceeded(float pm25, float pm10);

private:
    Ui::MainWindow *ui;
//    MotionTimer *motionTimer;
//    TemperatureSensor *tempSensor;
//    HumiditySensor *humSensor;
//    SDS011Detector *pmSensor;
    cv::Mat previousFrame;
    QtCharts::QLineSeries *tempSeries;
    QtCharts::QLineSeries *humSeries;
    QtCharts::QLineSeries *motionSeries;
    QtCharts::QChart *tempChart;
    QtCharts::QChart *humChart;
    QtCharts::QChart *motionChart;
    int timeIndex;

    void setupCharts();
};

#endif // MAINWINDOW_H
