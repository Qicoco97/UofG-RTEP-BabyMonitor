//
// Created by George Winsdor on 2025/6/18.
//

#ifndef MOTIONTIMER_H
#define MOTIONTIMER_H

#include "CppTimer.h"
#include <opencv2/opencv.hpp>
#include <functional>
#include <mutex>
#include <chrono>

/**
 * @brief MotionTimer
 * - Each time a new frame is created, setFrame() is called: grayscaled and then upsampled upsampleLevels times,
 * and then frame difference is used to detect minor motion and can also use the butterworth filter in our modules.
 * - CppTimer calls timerEvent() every intervalMs milliseconds in the background,
 * - Checks if there is no motion for timeoutMs milliseconds, and triggers the no-motion callback
 * - For example, if there is no motion in 5 seconds, will calls the onnomotion.
 */
class MotionTimer : public CppTimer {
public:
    using Callback = std::function<void()>;

    /**
   * @param threshold Grayscale difference average threshold
   * @param timeoutMs Alarm after how many milliseconds of no motion
   * @param intervalMs Timer callback interval in milliseconds
   * @param upsampleLevels Upsampling times (size per level × 2)
   */
    MotionTimer(int threshold,
                int timeoutMs,
                int intervalMs = 1000,
                int upsampleLevels = 1); // the upsample levels can be changed if the result is not good

    ~MotionTimer();

    /// Called when a new frame is received: grayscale → pyramid upsampling → differential detection
    void setFrame(const cv::Mat& frame);

    /// Register "no motion" callback
    void onNoMotion(Callback cb);
    /// Register the "motion resume" callback
    void onMotion(Callback cb);

protected:
    /// CppTimer Called when the timer expires
    void timerEvent() override;

private:
    int   _threshold;      ///< Grayscale difference threshold
    int   _timeout;        ///< Continuous inactivity timeout in milliseconds
    bool  _inMotion;       ///< Current status
    int   _upsampleLevels; ///< Pyramid upsample levels

    cv::Mat _prevGray;     ///< The sampled grayscale image of the previous frame
    std::chrono::steady_clock::time_point _lastMotionTime;
    std::mutex _mtx;

    Callback _cbNoMotion;
    Callback _cbMotion;
};

#endif // MOTION_TIMER_H
#endif //LIBCAMERA2OPENCV_MASTER_MOTIONTIMER_H
