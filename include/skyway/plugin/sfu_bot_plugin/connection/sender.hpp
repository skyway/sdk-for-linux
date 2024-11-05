//
//  sender.hpp
//  skyway
//
//  Created by salmon on 2022/1/25.
//  Copyright © 2022 NTT Communications. All rights reserved.
//

#ifndef SKYWAY_PLUGIN_SFU_BOT_PLUGIN_CONNECTION_SENDER_HPP_
#define SKYWAY_PLUGIN_SFU_BOT_PLUGIN_CONNECTION_SENDER_HPP_

#include <mediasoupclient.hpp>
#include <unordered_map>

#include "skyway/analytics/interface/analytics_client.hpp"
#include "skyway/core/interface/local_media_stream.hpp"
#include "skyway/core/interface/publication.hpp"
#include "skyway/plugin/sfu_bot_plugin/forwarding.hpp"
#include "skyway/plugin/sfu_bot_plugin/interface/sfu_api_client.hpp"
#include "skyway/plugin/sfu_bot_plugin/interface/sfu_connection.hpp"
#include "skyway/plugin/sfu_bot_plugin/interface/transport_repository.hpp"

namespace skyway {
namespace plugin {
namespace sfu_bot {
namespace connection {

using PublicationInterface = core::interface::Publication;
using LocalMediaStream     = core::interface::LocalMediaStream;

class Sender : public interface::SendTransport::Listener,
               public mediasoupclient::Producer::Listener,
               public core::interface::Publication::InternalListener,
               public PublicationInterface::Callback {
public:
    Sender(const std::string& local_person_id,
           const std::string& bot_id,
           interface::SfuApiClient* client,
           interface::TransportRepository* transport_repo,
           PublicationInterface* publication,
           ForwardingConfigure configure,
           analytics::interface::AnalyticsClient* analytics_client);
    ~Sender();
    boost::optional<interface::StartForwardingResult> StartForwarding(
        const interface::Device::PeerConnectionOptions* pc_options);
    bool StopForwarding(bool with_api_request);

    // SendTransport::Listener;
    std::future<std::string> OnProduce(mediasoupclient::SendTransport* transport,
                                       const std::string& kind,
                                       nlohmann::json rtp_parameters,
                                       const nlohmann::json& app_data) override;

    // mediasoupclient::Producer::Listener
    void OnTransportClose(mediasoupclient::Producer* producer) override;

    // core::interface::Publication::InternalListener
    void OnEncodingsUpdated(core::interface::Publication* publication,
                            std::vector<model::Encoding> encodings) override;
    void OnStreamReplaced(core::interface::Publication* publication,
                          std::shared_ptr<core::interface::LocalMediaStream> stream) override;

    // core::interface::Publication::Callback
    const boost::optional<nlohmann::json> GetStatsReport(
        PublicationInterface* publication) override;

private:
    bool ApplyEncoding(std::vector<model::Encoding> encoding);
    void Produce(const std::string& transaction_id);
    void SetupTransportAccessForStream(PublicationInterface* publication);
    bool SupportedCodecs(std::vector<model::Codec> codecs);

    std::string local_person_id_;
    std::string bot_id_;
    interface::SfuApiClient* client_;
    interface::TransportRepository* transport_repo_;

    PublicationInterface* publication_;
    ForwardingConfigure configure_;

    analytics::interface::AnalyticsClient* analytics_client_;

    interface::SendTransport* transport_ = nullptr;
    ProducerId producer_id_;

    boost::optional<std::string> forwarding_id_;
    boost::optional<std::string> transaction_id_;

public:
    friend class SfuBotPluginSenderTest;
};

}  // namespace connection
}  // namespace sfu_bot
}  // namespace plugin
}  // namespace skyway

#endif /* SKYWAY_PLUGIN_SFU_BOT_PLUGIN_CONNECTION_SENDER_HPP_ */