#include <fstream>
#include <inttypes.h>

#include "VideoSource.hpp"
#include "MotionDetection.hpp"

#include <time.h>

static inline void print_time(uint64_t& time, char c) {
	struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    uint64_t new_time = (uint64_t)((uint64_t)ts.tv_sec * 1000000 + (uint64_t)ts.tv_nsec / 1000);

    if (time == 0) {
        fprintf(stderr, "%c Start time: %" PRIu64 "\n", c, new_time);
        time = new_time;
    } else {
	fprintf(stderr, "%c Delta: %" PRIu64 "\n", c, new_time - time);
	time = new_time;
    }
}

// Transform video in command-line or "batch" mode according to cl.
// Return 0 on success or 1 on failure.
//
static int batch(const CommandLine &cl)
{
    // Buffer of 3 frames for use with the DifferentialCollins algorithm.
    MotionDetection detector(cl);
    
    // 设置回调函数
    detector.setMotionCallback([](int changes, double rate) {
        printf("[info] Pixel Movement: %d\t [info] Motion Estimate: %f Hz\n", changes, rate);
    });
    
    detector.setAlarmCallback([]() {
        printf("[ERROR] >>>>>  NO MOVEMENT DETECTED!!!!!!\n\n");
    });
    
    detector.setStateChangeCallback([](motionDetection_st newState) {
        switch(newState) {
            case init_st:
                printf("State changed to: init_st\n");
                break;
            case reset_st:
                printf("State changed to: reset_st\n");
                break;
            case idle_st:
                printf("State changed to: idle_st\n");
                break;
            case monitor_motion_st:
                printf("State changed to: monitor_motion_st\n");
                break;
            case compute_roi_st:
                printf("State changed to: compute_roi_st\n");
                break;
            case valid_roi_st:
                printf("State changed to: valid_roi_st\n");
                break;
        }
    });

    uint64_t frame_time = 0;
    VideoSource source(cl.cameraId, cl.inFile, cl.input_fps, cl.frameWidth, cl.frameHeight);
    
    if (cl.showTimes)
        print_time(frame_time, 'A');
        
    for (;;) {
        // for each frame
        cv::Mat frame;
        const bool more = source.read(frame);
        
        if (cl.showTimes)
            print_time(frame_time, 'A');
            
        if (frame.empty()) {
            if (cl.showTimes)
                print_time(frame_time, 'B');
            if (!more) {
                //time(&end);
                //double diff_t = difftime(end, start);
                //printf("[info] time: %f\n", diff_t);
                return 0;
            }
        } else {
            detector.update(frame);
            if (cl.showTimes)
                print_time(frame_time, 'B');
        }
    }
}

int main(int argc, char *argv[])
{
    try {
        const CommandLine cl(argc, argv);
        if (cl.help || cl.about) return 0;
        if (cl.ok) {
            printf("[info] starting batch processing.\n");
            if (cl.sourceCount) return batch(cl);
        }
    } catch (const std::runtime_error& e) {
        // die gracefully for uncaught runtime errors
        std::cerr << "System error: " << e.what() << std::endl;
    }
    return 1;
}
