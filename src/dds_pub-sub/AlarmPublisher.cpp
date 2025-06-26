// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
// Copyright 2025 Bernd Porr
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file AlarmPublisher.cpp
 *
 */

#include "AlarmMsgPubSubTypes.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>
#include <csignal>

#include "libcam2opencv.h"



#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include "CppTimer.h"

using namespace eprosima::fastdds::dds;

class AlarmPublisher
{
private:

    DomainParticipant* participant_ = nullptr;

    Publisher* publisher_ = nullptr;

    Topic* topic_ = nullptr;

    DataWriter* writer_ = nullptr;

    TypeSupport type_;

    class PubListener : public DataWriterListener
    {
    public:

        PubListener()
            : matched_(0)
        {
        }

        ~PubListener() override
        {
        }

        void on_publication_matched(
                DataWriter*,
                const PublicationMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                matched_ = info.total_count;
                std::cout << "Publisher matched." << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                matched_ = info.total_count;
                std::cout << "Publisher unmatched." << std::endl;
            }
            else
            {
                std::cout << info.current_count_change
                        << " is not a valid value for PublicationMatchedStatus current count change." << std::endl;
            }
        }

        std::atomic_int matched_;

    } listener_;

public:

    AlarmPublisher() : type_(new AlarmMsgPubSubType()) {}

    virtual ~AlarmPublisher()
    {
        if (writer_ != nullptr)
        {
            publisher_->delete_datawriter(writer_);
        }
        if (publisher_ != nullptr)
        {
            participant_->delete_publisher(publisher_);
        }
        if (topic_ != nullptr)
        {
            participant_->delete_topic(topic_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    //!Initialize the publisher
    bool init()
    {
        DomainParticipantQos participantQos;
        participantQos.name("Participant_publisher");
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr)
        {
            return false;
        }

        // Register the Type
        type_.register_type(participant_);

        // Create the publications Topic
	// !! Important that this matches with the name of message defined in AlarmMsg.idl !!
        topic_ = participant_->create_topic("AlarmTopic", "AlarmMsg", TOPIC_QOS_DEFAULT);

        if (topic_ == nullptr)
        {
            return false;
        }

        // Create the Publisher
        publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);

        if (publisher_ == nullptr)
        {
            return false;
        }

        // Create the DataWriter
        writer_ = publisher_->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT, &listener_);

        if (writer_ == nullptr)
        {
            return false;
        }
        return true;
    }

    //!Send a publication
    bool publish(AlarmMsg& hello)
    {
        if (listener_.matched_ > 0)
        {
            writer_->write(&hello);
            return true;
        }
        return false;
    }

};


class EventEmitter : public CppTimer {

public:
    AlarmPublisher mypub;
    uint32_t samples_sent = 1;
    
    void timerEvent() {
	AlarmMsg hello;
	hello.message("No motion detected !!!! Dangerous!");
	hello.index(samples_sent);
	if (mypub.publish(hello))
	{
	    std::cout << "Message: " << hello.message()
		      << " SENT" << std::endl;
	    samples_sent++;
	} else {
	    std::cout << "No messages sent as there is no listener." << std::endl;
	}
    }

    void start() {
	std::cout << "Starting publisher." << std::endl;

	if(!mypub.init())
	{
	    std::cerr << "Pub not init'd." << std::endl;
	    return;
	}
        startms(1000);
    }

};



// 全局退出标志
std::atomic<bool> running{true};
void onSigint(int){ running = false; }

// 定义并注册回调结构体：负责帧差运动检测 + 发布报警
struct FrameBridge : Libcam2OpenCV::Callback {
    AlarmPublisher &pub;
    cv::Mat        prevBlur;      // 上一帧模糊灰度图
    bool           firstFrame{true};
    int            thresh{25};    // 二值化阈值
    double         minArea{500};  // 最小运动面积

    FrameBridge(AlarmPublisher &p) : pub(p) {}

    void hasFrame(const cv::Mat &frame, const libcamera::ControlList&) override {
        // 1. 转灰度 + 高斯模糊
        cv::Mat gray, blur;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blur, cv::Size(21,21), 0);

        // 2. 第一帧初始化
        if (firstFrame) {
            prevBlur   = blur;
            firstFrame = false;
            return;
        }

        // 3. 计算差分并二值化
        cv::Mat delta, mask;
        cv::absdiff(prevBlur, blur, delta);
        cv::threshold(delta, mask, thresh, 255, cv::THRESH_BINARY);
        cv::dilate(mask, mask, cv::Mat(), cv::Point(-1,-1), 2);

        // 4. 查找轮廓
        std::vector<std::vector<cv::Point>> cons;
        cv::findContours(mask, cons, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        bool detected = false;
        for (auto &c : cons) {
            if (cv::contourArea(c) < minArea) continue;
            detected = true;
            break;
        }

        // 5. 如果检测到运动，发布 DDS 报警
        if (detected) {
            static uint32_t idx = 0;
            AlarmMsg msg;
            msg.index(++idx);
            msg.message("Motion Detected");
            pub.publish(msg);
            std::cout << "[DDS] 运动报警 #" << idx << " 已发送\n";
        }

        // 6. 更新上一帧
        prevBlur = blur;
    }
};
int main()
{
    std::signal(SIGINT, onSigint);

    // 初始化 DDS 发布器
    AlarmPublisher publisher;
    if (!publisher.init()) {
        std::cerr << "DDS 初始化失败\n";
        return -1;
    }

   

    // 构造桥梁并启动摄像头
    FrameBridge bridge(publisher);
    Libcam2OpenCV camera;
    camera.registerCallback(&bridge);

    Libcam2OpenCVSettings s;
    s.width     = 640;
    s.height    = 480;
    s.framerate = 30;
    camera.start(s);

    std::cout << "摄像头已启动，开始运动检测并通过 DDS 报警。\n"
              << "按 Ctrl+C 退出。\n";

    // 主线程保持运行
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    camera.stop();
    return 0;
}

