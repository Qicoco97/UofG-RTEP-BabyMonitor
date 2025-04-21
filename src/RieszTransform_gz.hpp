#ifndef RIESZ_TRANSFORM_H_INCLUDED
#define RIESZ_TRANSFORM_H_INCLUDED

/**
 * This file is released under the Patented Algorithm with Software Released
 * Agreement. See LICENSE.md for more information, and view the original repo
 * at https://github.com/tbl3rd/Pyramids
 */

#include <opencv2/opencv.hpp>
#include <memory>

struct RieszTransformState;

class RieszTransform {
    RieszTransform &operator=(const RieszTransform &) = delete;

    cv::Mat itsFrame;
    std::unique_ptr<RieszTransformState> state;
    double itsAlpha;
    double itsThreshold;

public:
    RieszTransform();
    RieszTransform(const RieszTransform &);
    ~RieszTransform();

    // Initialize with the first input frame (converted to float)
    void initialize(const cv::Mat &frame);

    // Set the frames per second (filter sampling rate)
    void fps(double fps);

    // Set the low or high cutoff frequency for the bandpass filter
    void lowCutoff(double frequency);
    void highCutoff(double frequency);

    // Set the amplification factor (alpha)
    void alpha(int value) { itsAlpha = value; }

    // Set the maximum allowed phase difference (threshold)
    void threshold(int t) { itsThreshold = t; }

    // Return a motion-magnified frame based on current configuration
    cv::Mat transform(const cv::Mat &frame);
};

#endif // RIESZ_TRANSFORM_H_INCLUDED
