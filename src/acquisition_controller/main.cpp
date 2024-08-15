#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>

#include "EchoDataPubSubTypes.h"

#include <random>

using namespace eprosima::fastdds::dds;

class AcquisitionController
{
private:
    DomainParticipant* participant_;
    Publisher* publisher_;
    Topic* topic_;
    DataWriter* writer_;
    TypeSupport type_;
    EchoSounder::EchoData echo_data_;

public:
    AcquisitionController() : participant_(nullptr), publisher_(nullptr), topic_(nullptr), writer_(nullptr),
                              type_(new EchoSounder::EchoDataPubSubType()) {}

    bool init()
    {
        DomainParticipantQos participantQos;
        participantQos.name("AcquisitionParticipant");
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr)
            return false;

        // Register the type
        type_.register_type(participant_);

        // Create the publisher
        publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT);
        if (publisher_ == nullptr)
            return false;

        // Create the topic
        topic_ = participant_->create_topic("RawEchoDataTopic", "EchoSounder::EchoData", TOPIC_QOS_DEFAULT);
        if (topic_ == nullptr)
            return false;

        // Create the data writer
        writer_ = publisher_->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT);
        if (writer_ == nullptr)
            return false;

        return true;
    }

    bool publish()
    {
        // Simulate data acquisition
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        echo_data_.timestamp(timestamp);
        
        // Generate some random data
        std::random_device rd;  // Used to seed the engine
        std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<> dis(0.0, 100.0);

        float random_value = dis(gen);  // Generate the random number
        
        echo_data_.rawData(random_value); // Example data

        writer_->write(&echo_data_);

        std::cout << "Timestamp: " << echo_data_.timestamp() << " Published data: " << echo_data_.rawData() << std::endl;

        return true;
    }
};

int main()
{
    AcquisitionController controller;
    if (controller.init())
    {
        while (true)
        {
            controller.publish();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return 0;
}