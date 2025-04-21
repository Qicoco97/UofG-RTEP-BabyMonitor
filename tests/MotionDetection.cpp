#include "MotionDetection.hpp"
#include "CommandLine.hpp" 
#include <opencv2/opencv.hpp>

int main() {
    CommandLine cl;
    cl.frameWidth = 640;
    cl.frameHeight = 480;
    cl.diffThreshold = 15;
    cl.showDiff = true;
    cl.showMagnification = false;
    cl.pixelThreshold = 10;
    cl.motionDuration = 2;
    cl.framesToSettle = 3;
    cl.roiUpdateInterval = 10;
    cl.roiWindow = 3;
    cl.crop = false;
    cl.full_fps = 30.0;
    cl.crop_fps = 30.0;
    cl.input_fps = 30.0;
    cl.erodeDimension = 2;
    cl.dilateDimension = 2;
    cl.timeToAlarm = 5;
    cl.cameraId = 0;

    MotionDetection detector(cl);

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open camera." << std::endl;
        return -1;
    }

    while (true) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        detector.update(frame);

        if (cv::waitKey(1) == 27) break; // ESC退出
    }

    return 0;
}
