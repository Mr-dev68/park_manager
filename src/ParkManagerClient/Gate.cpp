/*
 * Gate.cpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */

#include "Gate.hpp"
#include "MessageManager.hpp"
#include "TimeMeasuring.h"
#include "WebScoket.h"
#include "models.hpp"
#include <boost/algorithm/string.hpp>

#include "logging.hpp"

Gate::Gate()
    : m_untils(new utils(PMClientConfig::ConfigPath)), m_WebsocketURL(m_untils->GetValueFromConfigFile(PMClientConfig::WebsocketURL)),
      m_ClientURI(m_untils->GetValueFromConfigFile(PMClientConfig::ClientURI)),
      m_GateName(m_untils->GetValueFromConfigFile(PMClientConfig::GateName)),
      m_GateType(stoi(m_untils->GetValueFromConfigFile(PMClientConfig::GateType))), m_NetworkProblem(false)

{
    printf("ClientURI  %s \n", m_ClientURI.c_str());
    m_shmObj.reset(new C_BoostShmObj(m_ClientURI.c_str()));
    m_running.store(true, std::memory_order_relaxed);
    // TODO Auto-generated constructor stub
    m_messageManager.reset(new MessageManager());
    m_GateWebSocketClient.reset(new WebSocketClient(m_WebsocketURL, m_ClientURI, m_messageManager.get(), true));
    {
        std::lock_guard<std::mutex> guard(m_WebSocketAccess_LOCK);
        m_GateWebSocketClient->Connect();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    m_GateNetworkThread = std::async(std::launch::async, std::bind(&Gate::ConnectionHolder, this, std::ref(m_running)));

    std::string value = "#" + std::to_string(INIT);
    m_shmObj->SetValue("WebsocketClientStatus", value.c_str());
}
Gate::Gate(std::string ConfigPath)
    : m_untils(new utils(ConfigPath)), m_WebsocketURL(m_untils->GetValueFromConfigFile(PMClientConfig::WebsocketURL)),
      m_ClientURI(m_untils->GetValueFromConfigFile(PMClientConfig::ClientURI)),
      m_GateName(m_untils->GetValueFromConfigFile(PMClientConfig::GateName)),
      m_GateType(stoi(m_untils->GetValueFromConfigFile(PMClientConfig::GateType))), m_NetworkProblem(false)

{
    printf("ClientURI  %s \n", m_ClientURI.c_str());
    m_shmObj.reset(new C_BoostShmObj(m_ClientURI.c_str()));
    m_running.store(true, std::memory_order_relaxed);
    // TODO Auto-generated constructor stub
    m_messageManager.reset(new MessageManager());
    m_GateWebSocketClient.reset(new WebSocketClient(m_WebsocketURL, m_ClientURI, m_messageManager.get(), true));
    {
        std::lock_guard<std::mutex> guard(m_WebSocketAccess_LOCK);
        m_GateWebSocketClient->Connect();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    m_GateNetworkThread = std::async(std::launch::async, std::bind(&Gate::ConnectionHolder, this, std::ref(m_running)));

    std::string value = "#" + std::to_string(INIT);
    m_shmObj->SetValue("WebsocketClientStatus", value.c_str());
}
Gate::~Gate() {
    m_running.store(false, std::memory_order_relaxed);
    if (m_GateNetworkThread.valid())
        m_GateNetworkThread.get();
}

int Gate::ConnectionHolder(std::atomic<bool>& m_running) {
    while (m_running) {
        if (true == m_NetworkProblem) {
            LOG_FATAL("%s:%d restarting GateWebSocketClient Connection to PM Lost\n", __PRETTY_FUNCTION__, __LINE__);

            std::lock_guard<std::mutex> guard(m_WebSocketAccess_LOCK);
            m_GateWebSocketClient.reset(new WebSocketClient(m_WebsocketURL, m_ClientURI, m_messageManager.get(), true));
            m_GateWebSocketClient->Connect();
            m_NetworkProblem = false;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
    LOG_INFO("exiting ConnectionHolder thread\n");
    return ReturnValue::SUCCESS;
}

int Gate::GateRegistration() {

    if (m_GateType == ParkManagment::GateType::NotSet) {
        LOG_INFO("this gate type is not set! \n");
        return ReturnValue::FAILED;
    }
    if (m_untils->GetValueFromConfigFile(PMClientConfig::RegistratinResult).compare(std::to_string(ReturnValue::SUCCESS)) == 0) {
        LOG_INFO("Gate is registered \n");
        return ReturnValue::SUCCESS;
    }
    quicktype::GateRegistration GateRegist;
    GateRegist.setGateName(m_GateName);
    GateRegist.setGateType(std::to_string(m_GateType));
    nlohmann::json j;
    nlohmann::to_json(j, GateRegist);
    std::string Request(AddBase(j.dump(), messageType::CALL, models::ActionGateRegistration).c_str());
    LOG_INFO("%s %d Gate GateRegistration request:%s\n", __FUNCTION__, __LINE__, Request.c_str());

    std::unique_lock<std::mutex> guard(m_WebSocketAccess_LOCK);
    if (m_GateWebSocketClient->SendMessage(Request)) {
        std::vector<std::string> requestItems;
        boost::split(requestItems, Request, boost::is_any_of(","));
        std::string messageId = requestItems[1];
        bool isCallError;
        std::string Respond = m_messageManager->GetResponseMessage(messageId, isCallError);
        LOG_INFO("PM->Gate Respond\n %s \n", Respond.c_str());
        if (!Respond.empty()) {
            m_NetworkProblem = false;
            std::string messageId, JsonString;
            handleIncomingMsg(Respond, messageId, JsonString, messageType::RESPOND);
            quicktype::GateRegistrationResponse GateRegistResp;
            nlohmann::from_json(nlohmann::json::parse(JsonString), GateRegistResp);
            if (GateRegistResp.getResult() != RegistrationStatus::Accept) {
                LOG_INFO("registration not successful result : %s \n", GateRegistResp.getResult().c_str());
                return ReturnValue::FAILED;
            } else {
                LOG_INFO("registration  successful result : %s \n", GateRegistResp.getResult().c_str());
                m_untils->SetValueFromConfigFile(PMClientConfig::RegistratinResult, std::to_string(ReturnValue::SUCCESS));
                return ReturnValue::SUCCESS;
            }

        } else {
            m_NetworkProblem = true;
            LOG_ERROR("unable to send request Gate->PM \n", m_NetworkProblem);
            return ReturnValue::FAILED;
        }
    } else {
        LOG_ERROR("unable to send response Gate->PM");
        m_NetworkProblem = true;
        return ReturnValue::FAILED;
    }

    return ReturnValue::FAILED;
}

int Gate::CarRegistration(std::string CarPlate, bool Handicap, std::string& msg) {
    msg.clear();

    if (m_GateType != ParkManagment::GateType::Enterence) {
        msg = "this gate is not entrance gate! \n";
        return ReturnValue::FAILED;
    }
    quicktype::CarEnterence CarRegistr;

    CarRegistr.setCarPlate(CarPlate);
    if (Handicap)
        CarRegistr.setHandicap(HandicapStatus::Yes);
    else
        CarRegistr.setHandicap(HandicapStatus::No);

    std::string timeUTC;
    GetCurrentUTCTime(timeUTC);
    LOG_INFO("CarRegistration timeUTC %s \n ", timeUTC.c_str());
    CarRegistr.setEnteranceTime(timeUTC);
    CarRegistr.setEnterenceGate(m_GateName);

    nlohmann::json j;
    nlohmann::to_json(j, CarRegistr);
    std::string Request(AddBase(j.dump(), messageType::CALL, models::ActionCarEnterence).c_str());
    LOG_INFO("%s %d Gate CarRegistration request:%s\n", __FUNCTION__, __LINE__, Request.c_str());

    std::unique_lock<std::mutex> guard(m_WebSocketAccess_LOCK);
    if (m_GateWebSocketClient->SendMessage(Request)) {
        std::vector<std::string> requestItems;
        boost::split(requestItems, Request, boost::is_any_of(","));
        std::string messageId = requestItems[1];
        bool isCallError;
        std::string Respond = m_messageManager->GetResponseMessage(messageId, isCallError);
        LOG_INFO("PM->Gate Respond\n %s \n", Respond.c_str());
        if (!Respond.empty()) {
            m_NetworkProblem = false;
            std::string messageId, JsonString;
            handleIncomingMsg(Respond, messageId, JsonString, messageType::RESPOND);
            quicktype::CarEnterenceResponse CarEnterResp;
            nlohmann::from_json(nlohmann::json::parse(JsonString), CarEnterResp);
            if (CarEnterResp.getResult() != RegistrationStatus::Accept) {
                LOG_INFO("CarEnterenceResponse not successful result : %s \n", CarEnterResp.getResult().c_str());
                return ReturnValue::FAILED;
            }
            msg = CarEnterResp.getComment();
            return ReturnValue::SUCCESS;
        } else {
            m_NetworkProblem = true;
            LOG_ERROR("unable to send request Gate->PM \n", m_NetworkProblem);
            return ReturnValue::FAILED;
        }
    } else {
        LOG_ERROR("unable to send response Gate->PM");
        m_NetworkProblem = true;
        return ReturnValue::FAILED;
    }
}
int Gate::CarStatusUpdate(std::string CarPlate, UpdateStatus& UpdateStatusMsg) {
    UpdateStatusMsg = {"", "", ""};
    if (m_GateType != ParkManagment::GateType::Exiting) {
        UpdateStatusMsg.comment = "this gate is not entrance gate! \n";
        UpdateStatusMsg.Status = RegistrationStatus::Reject;
        return ReturnValue::FAILED;
    }

    quicktype::CarStatusUpdate CarStatus;
    CarStatus.setCarPlate(CarPlate);
    CarStatus.setExitingGate(m_GateName);

    nlohmann::json j;
    nlohmann::to_json(j, CarStatus);
    std::string Request(AddBase(j.dump(), messageType::CALL, models::ActionCarStatusUpdate).c_str());
    LOG_INFO("%s %d Gate CarStatusUpdate request:%s\n", __FUNCTION__, __LINE__, Request.c_str());

    std::unique_lock<std::mutex> guard(m_WebSocketAccess_LOCK);
    if (m_GateWebSocketClient->SendMessage(Request)) {
        std::vector<std::string> requestItems;
        boost::split(requestItems, Request, boost::is_any_of(","));
        std::string messageId = requestItems[1];
        bool isCallError;
        std::string Respond = m_messageManager->GetResponseMessage(messageId, isCallError);
        LOG_INFO("PM->Gate Respond\n %s \n", Respond.c_str());
        if (!Respond.empty()) {
            m_NetworkProblem = false;
            std::string messageId, JsonString;
            handleIncomingMsg(Respond, messageId, JsonString, messageType::RESPOND);
            quicktype::CarStatusUpdateResponse CarStatusUpdateResp;
            nlohmann::from_json(nlohmann::json::parse(JsonString), CarStatusUpdateResp);
            if (CarStatusUpdateResp.getStatus() != RegistrationStatus::Exist) {
                LOG_INFO("CarStatusUpdate not successful result : %s \n", CarStatusUpdateResp.getStatus());
                return ReturnValue::FAILED;
            }
            UpdateStatusMsg.comment = CarStatusUpdateResp.getComment();
            UpdateStatusMsg.Status = CarStatusUpdateResp.getStatus();
            UpdateStatusMsg.payable = CarStatusUpdateResp.getPayable();
            return ReturnValue::SUCCESS;
        } else {
            m_NetworkProblem = true;
            LOG_ERROR("unable to send request Gate->PM \n", m_NetworkProblem);
            return ReturnValue::FAILED;
        }
    } else {
        LOG_ERROR("unable to send response Gate->PM");
        m_NetworkProblem = true;
        return ReturnValue::FAILED;
    }
}
