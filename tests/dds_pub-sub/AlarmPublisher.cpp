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
#include "MotionTimer.h"


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

int main(
    int,
    char**)
{
    AlarmPublisher publisher;
    if (!publisher.init()) {
        std::cerr << "DDS 初始化失败\n";
        return -1;
    }

    // ======= 2. 初始化运动检测 =======
    // 参数：阈值 avg diff、超时 ms、检测间隔 ms、上采样层数

    // MotionTimer 在后台线程里定期调用 timerEvent()
    // onMotion 回调在检测到运动时触发
    motionTimer.onMotion([&](){
        static uint32_t idx = 0;
        AlarmMsg msg;
        msg.index(++idx);
        msg.message("Motion Detected!");
        publisher.publish(msg);
        std::cout << "[DDS] 运动报警消息 #" << msg.message() << " 已发送\n";
    });

    // 只要 start()，MotionTimer 会启动自己的后台线程
    motionTimer.startms(1000,PERIODC);

    // ======= 3. 启动摄像头，并把帧传给 MotionTimer =======
    Libcam2OpenCVSettings camset;
    camset.width     = 640;
    camset.height    = 480;
    camset.framerate = 30;
    Libcam2OpenCV camera;
    // 注册一个轻量级的 Callback 来把每帧给 MotionTimer

    camera.registerCallback(&bridge);
    camera.start(camset);

    std::cout << "摄像头已启动，开始运动检测并通过 DDS 报警。\n"
              << "按 Ctrl+C 退出。\n";
}
