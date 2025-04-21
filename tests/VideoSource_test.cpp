#include <gtest/gtest.h>
#include "VideoSource.hpp"
#include <opencv2/opencv.hpp>

const std::string testVideoPath = "test_video.mp4";  
TEST(VideoSourceTest, ConstructFromFileSuccess) {
    VideoSource source(-1, testVideoPath, 30, 640, 480);
    cv::Size size = source.frameSize();
    EXPECT_GT(size.width, 0);
    EXPECT_GT(size.height, 0);
}

TEST(VideoSourceTest, ConstructFromInvalidFileThrows) {
    EXPECT_THROW({
        VideoSource source(-1, "nonexistent.mp4", 30, 640, 480);
    }, std::runtime_error);
}

TEST(VideoSourceTest, ReadSingleFrame) {
    VideoSource source(-1, testVideoPath, 30, 640, 480);
    cv::Mat frame;
    bool success = source.read(frame);
    EXPECT_TRUE(success);
    EXPECT_FALSE(frame.empty());
}

TEST(VideoSourceTest, FrameSizeMatchesMat) {
    VideoSource source(-1, testVideoPath, 30, 640, 480);
    cv::Mat frame;
    ASSERT_TRUE(source.read(frame));
    cv::Size size = source.frameSize();
    EXPECT_EQ(frame.cols, size.width);
    EXPECT_EQ(frame.rows, size.height);  // 因为只提取Y通道，高度应一致
}
