// ProcessingController (Subscriber and Publisher)
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>

#include "EchoDataPubSubTypes.h"

using namespace eprosima::fastdds::dds;

class EchoDataListener : public DataReaderListener
{
public:
    EchoDataListener(DataWriter* writer) : writer_(writer) {}

    void on_data_available(DataReader* reader) override
    {
        EchoSounder::EchoData raw_data;
        SampleInfo info;
        if (reader->take_next_sample(&raw_data, &info) == ReturnCode_t::RETCODE_OK)
        {
            if (info.valid_data)
            {
                process_and_publish_data(raw_data);
            }
        }
    }

private:
    DataWriter* writer_;

    void process_and_publish_data(const EchoSounder::EchoData& raw_data)
    {
        // Create a new EchoData instance for processed data
        EchoSounder::EchoData processed_data;
        processed_data.timestamp(raw_data.timestamp());
        processed_data.rawData(raw_data.rawData());
        processed_data.processedData(raw_data.rawData() * 0.9); // Simple processing: 10% error correction

        // Publish the processed data
        writer_->write(&processed_data);

        std::cout << "Published processed data with timestamp: " << processed_data.timestamp() << std::endl;
    }
};

class ProcessingController
{
private:
    DomainParticipant* participant_;
    Subscriber* subscriber_;
    Publisher* publisher_;
    Topic* raw_topic_;
    Topic* processed_topic_;
    DataReader* reader_;
    DataWriter* writer_;
    TypeSupport type_;
    EchoDataListener* listener_;

public:
    ProcessingController() : participant_(nullptr), subscriber_(nullptr), publisher_(nullptr),
                             raw_topic_(nullptr), processed_topic_(nullptr),
                             reader_(nullptr), writer_(nullptr),
                             type_(new EchoSounder::EchoDataPubSubType()),
                             listener_(nullptr) {}

    ~ProcessingController() {
        if (listener_) delete listener_;
    }

    bool init()
    {
        DomainParticipantQos participantQos;
        participantQos.name("ProcessingParticipant");
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr)
            return false;

        // Register the type
        type_.register_type(participant_);

        // Create the subscriber
        subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
        if (subscriber_ == nullptr)
            return false;

        // Create the publisher
        publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT);
        if (publisher_ == nullptr)
            return false;

        // Create the raw data topic
        raw_topic_ = participant_->create_topic("RawEchoDataTopic", "EchoSounder::EchoData", TOPIC_QOS_DEFAULT);
        if (raw_topic_ == nullptr)
            return false;

        // Create the processed data topic
        processed_topic_ = participant_->create_topic("ProcessedEchoDataTopic", "EchoSounder::EchoData", TOPIC_QOS_DEFAULT);
        if (processed_topic_ == nullptr)
            return false;

        // Create the data writer
        writer_ = publisher_->create_datawriter(processed_topic_, DATAWRITER_QOS_DEFAULT);
        if (writer_ == nullptr)
            return false;

        // Create the listener
        listener_ = new EchoDataListener(writer_);

        // Create the data reader
        reader_ = subscriber_->create_datareader(raw_topic_, DATAREADER_QOS_DEFAULT, listener_);
        if (reader_ == nullptr)
            return false;

        return true;
    }

    void run()
    {
        std::cout << "ProcessingController running. Waiting for raw data..." << std::endl;
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};

int main()
{
    ProcessingController controller;
    if (controller.init())
    {
        controller.run();
    }
    else
    {
        std::cerr << "Failed to initialize ProcessingController" << std::endl;
        return 1;
    }
    return 0;
}