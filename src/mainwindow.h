#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "libcam2opencv.h"
#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QTimerEvent>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include "DHT11Worker.h"
#include "AlarmPublisher.h"
#include "LedController.h"
#include "Config.h"

QT_CHARTS_USE_NAMESPACE

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
    
protected:
    void timerEvent(QTimerEvent *event) override;
    
signals:
    // Emit this signal every time there is a new frame.
    void frameReady(const cv::Mat &mat);

private slots:
    void onMotionStatusChanged(bool detected);
    void onNewDHTReading(int t_int, int t_dec,
                         int h_int, int h_dec);
    void onDHTError();

//    void onPMExceeded(float pm25, float pm10);

private:
    Ui::MainWindow *ui;

    cv::Mat previousFrame;
    

    QtCharts::QLineSeries *motionSeries;
    QtCharts::QChart *motionChart;
    
    QtCharts::QChart      *chart;
    QtCharts::QLineSeries *tempSeries;
    QtCharts::QLineSeries *humSeries;
    QtCharts::QValueAxis  *axisX;
    QtCharts::QValueAxis  *axisY;       ///< Value axis Y
     ///< Current data index
    
    int timeIndex;
    
    AlarmPublisher alarmPub_; 
    int            alarmTimerId_{-1};
    uint32_t       samplesSent_{1};
    bool           motionDetected_{false};
    LEDController     led_;  
    
    DHT11Worker   *dhtWorker_; 

    void setupCharts();
};

#endif // MAINWINDOW_H
