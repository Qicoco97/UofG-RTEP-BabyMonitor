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

private slots:
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
