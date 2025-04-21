#include <gtest/gtest.h>
#include "VideoSource.hpp"
#include <opencv2/opencv.hpp>

// Path to your test video file (make sure this file exists)
const std::string testVideoPath = "test_data/test_video.mp4";

// Utility function to check if the file exists
bool fileExists(const std::string &path) {
    return access(path.c_str(), F_OK) == 0;
}

// --- Test successful construction from a valid video file ---
TEST(VideoSourceTest, ConstructFromValidFile) {
    if (!fileExists(testVideoPath)) {
        // Skip this test if the file doesn't exist
        GTEST_SKIP() << "Test video not found: " << testVideoPath;
    }

    // Should not throw if the file is valid
    EXPECT_NO_THROW({
        VideoSource source(-1, testVideoPath, 30, 640, 480);
    });
}

// --- Test that construction from an invalid file path throws an exception ---
TEST(VideoSourceTest, ConstructFromInvalidFileThrows) {
    // This should throw a runtime_error since the file doesn't exist
    EXPECT_THROW({
        VideoSource source(-1, "invalid_file.mp4", 30, 640, 480);
    }, std::runtime_error);
}

// --- Test that frameSize() returns a valid non-zero size ---
TEST(VideoSourceTest, FrameSizeIsPositive) {
    if (!fileExists(testVideoPath)) {
        GTEST_SKIP() << "Test video not found: " << testVideoPath;
    }

    VideoSource source(-1, testVideoPath, 30, 640, 480);
    cv::Size size = source.frameSize();
    EXPECT_GT(size.width, 0);
    EXPECT_GT(size.height, 0);
}

// --- Test that a frame can be successfully read ---
TEST(VideoSourceTest, CanReadSingleFrame) {
    if (!fileExists(testVideoPath)) {
        GTEST_SKIP() << "Test video not found: " << testVideoPath;
    }

    VideoSource source(-1, testVideoPath, 30, 640, 480);
    cv::Mat frame;

    // Attempt to read one frame from the file
    bool result = source.read(frame);

    // It should succeed and return a non-empty grayscale image (Y channel)
    EXPECT_TRUE(result);
    EXPECT_FALSE(frame.empty());
    EXPECT_EQ(frame.type(), CV_8UC1);  // Expect a single-channel 8-bit image
}
