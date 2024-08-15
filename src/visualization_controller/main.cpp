#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

#include "EchoDataPubSubTypes.h"

using namespace eprosima::fastdds::dds;

class EchoDataListener : public DataReaderListener
{
public:
    EchoDataListener() = default;

    void on_data_available(DataReader* reader) override
    {
        EchoSounder::EchoData echo_data;
        SampleInfo info;
        if (reader->take_next_sample(&echo_data, &info) == ReturnCode_t::RETCODE_OK)
        {
            if (info.valid_data)
            {
                visualize_data(echo_data);
            }
        }
    }

private:
    float maxDepth = 100; // Maximum depth in meters
    int resolution = 20; // Resolution of the visualization
    void visualize_data(const EchoSounder::EchoData& data)
    {
        float depth = data.processedData();
        float normalizedDepth = std::min((depth / maxDepth), 1.0f);
        int level = static_cast<int>(normalizedDepth * resolution);

        // Create ASCII art representation
        std::string visualization = "|" + std::string(level, '=') + std::string(resolution - level, ' ') + "|";

        std::cout << "Depth: " << std::fixed << std::setprecision(2) << depth << "m" << std::endl;
        std::cout << visualization << std::endl;
        std::cout << std::string(resolution + 2, '-') << std::endl;
    }
};

class VisualizationController
{
private:
    DomainParticipant* participant_;
    Subscriber* subscriber_;
    Topic* topic_;
    DataReader* reader_;
    TypeSupport type_;
    EchoDataListener listener_;

public:
    VisualizationController() : participant_(nullptr), subscriber_(nullptr), topic_(nullptr), reader_(nullptr),
                                type_(new EchoSounder::EchoDataPubSubType()) {}

    bool init()
    {
        DomainParticipantQos participantQos;
        participantQos.name("VisualizationParticipant");
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr)
            return false;

        // Register the type
        type_.register_type(participant_);

        // Create the subscriber
        subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
        if (subscriber_ == nullptr)
            return false;

        // Create the topic
        topic_ = participant_->create_topic("ProcessedEchoDataTopic", "EchoSounder::EchoData", TOPIC_QOS_DEFAULT);
        if (topic_ == nullptr)
            return false;

        // Create the data reader
        reader_ = subscriber_->create_datareader(topic_, DATAREADER_QOS_DEFAULT, &listener_);
        if (reader_ == nullptr)
            return false;

        return true;
    }

    void run()
    {
        std::cout << "VisualizationController running. Waiting for processed data..." << std::endl;
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // In a real application, you might update a GUI event loop here
        }
    }
};

int main()
{
    VisualizationController controller;
    if (controller.init())
    {
        controller.run();
    }
    else
    {
        std::cerr << "Failed to initialize VisualizationController" << std::endl;
        return 1;
    }
    return 0;
}