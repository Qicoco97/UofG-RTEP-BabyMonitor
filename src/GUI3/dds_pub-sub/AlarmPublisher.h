// AlarmPublisher.h
#pragma once

#include "AlarmMsgPubSubTypes.h"
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <atomic>

class AlarmPublisher
{
public:
    AlarmPublisher();
    ~AlarmPublisher();

    //! 初始化 DDS（participant/publisher/topic/writer）
    bool init();

    //! 发布一条 AlarmMsg；如果有订阅者则返回 true
    bool publish(AlarmMsg& msg);

private:
    eprosima::fastdds::dds::DomainParticipant* participant_{nullptr};
    eprosima::fastdds::dds::Publisher* publisher_{nullptr};
    eprosima::fastdds::dds::Topic* topic_{nullptr};
    eprosima::fastdds::dds::DataWriter* writer_{nullptr};
    eprosima::fastdds::dds::TypeSupport type_;

    // 内部 listener，用来跟踪匹配数量
    class PubListener : public eprosima::fastdds::dds::DataWriterListener {
    public:
        std::atomic_int matched_{0};
        void on_publication_matched(
            eprosima::fastdds::dds::DataWriter*,
            const eprosima::fastdds::dds::PublicationMatchedStatus& info) override
        {
            matched_ += info.current_count_change;
        }
    } listener_;
};
