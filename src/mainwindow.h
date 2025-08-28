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
#include <memory>
#include "DHT11Worker.h"
#include "AlarmPublisher.h"
#include "LedController.h"
#include "Config.h"
#include "SensorData.h"
#include "SensorFactory.h"
#include "interfaces/IComponent.h"
#include "managers/AlarmSystem.h"

QT_CHARTS_USE_NAMESPACE

#include "ErrorHandler.h"

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
    
    struct CameraCallback : Libcam2OpenCV::Callback {
        MainWindow* window = nullptr;

        virtual void hasFrame(const cv::Mat &frame, const libcamera::ControlList &) override {
            if (window != nullptr) {
                window->processNewFrame(frame);
            }
        }
    };
    Libcam2OpenCV camera;
    CameraCallback cameraCallback;
    
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

    // Interface-based components (dependency injection)
    std::shared_ptr<BabyMonitor::IAlarmSystem> injectedAlarmSystem_;
    std::unique_ptr<BabyMonitor::AlarmSystem> alarmSystem_; // Fallback for backward compatibility

    DHT11Worker   *dhtWorker_;

    // Motion detection components
    QThread* motionThread_;
    MotionWorker* motionWorker_;

    // Structured sensor data
    BabyMonitor::TemperatureHumidityData lastTempHumData_;
    BabyMonitor::MotionData lastMotionData_;
    BabyMonitor::SystemStatus systemStatus_;

    // Error handling
    BabyMonitor::ErrorHandler& errorHandler_;

    // DHT11 error tracking
    int dht11ConsecutiveErrors_;
    static constexpr int DHT11_MAX_CONSECUTIVE_ERRORS = 5;

    void setupCharts();

    // Chart management methods
    void updateTemperatureHumidityChart(const BabyMonitor::TemperatureHumidityData& data);
    void updateMotionChart(const BabyMonitor::MotionData& data);
    void configureChartAxes();

    // Frame processing methods
    void processNewFrame(const cv::Mat& frame);

    // Sensor management methods
    void initializeSensors();
    void startSensors();
    void stopSensors();
    void initializeMotionDetection();
    void cleanupMotionDetection();
    void initializeDHT11Sensor();
    void cleanupDHT11Sensor();

    // LED control methods
    void initializeLED();
    void triggerMotionAlert();

    // Error handling methods
    void handleSystemError(const QString& component, const QString& message);
    void handleCriticalError(const QString& component, const QString& message);
    void updateSystemStatus();

public:
    // Dependency injection methods (public for bootstrap access)
    void setAlarmSystem(std::shared_ptr<BabyMonitor::IAlarmSystem> alarmSystem);

private:
};

#endif // MAINWINDOW_H
