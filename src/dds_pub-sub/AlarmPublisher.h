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

//! Initialize DDS (participant/publisher/topic/writer)
    bool init();

//! Publish an AlarmMsg; return true if there is a subscriber
    bool publish(AlarmMsg& msg);

private:
    eprosima::fastdds::dds::DomainParticipant* participant_{nullptr};
    eprosima::fastdds::dds::Publisher* publisher_{nullptr};
    eprosima::fastdds::dds::Topic* topic_{nullptr};
    eprosima::fastdds::dds::DataWriter* writer_{nullptr};
    eprosima::fastdds::dds::TypeSupport type_;

// Internal listener to track the number of matches
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
