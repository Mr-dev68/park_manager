/*
 * ParkManagerServer.cpp
 *
 *  Created on: Jul 1, 2022
 *      Author: rohan
 */

#include "ParkManagerServer.h"
#include "common.h"
#include "models.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <stdlib.h>

#include <math.h>

#include "logging.hpp"

ParkManagerServer::ParkManagerServer()
    : m_untils(new utils(PMSeverConfig::ConfigPath)), m_websocketPort(stoi(m_untils->GetValueFromConfigFile(PMSeverConfig::WebSocketPort))),
      m_shmObj(new C_BoostShmObj("Rasekh::PMS")), m_messageManager(new MessageManager()),
      m_HandicapSlotCount(stoi(m_untils->GetValueFromConfigFile(PMSeverConfig::HandicapSlotCount))),
      m_NormalSlotCount(stoi(m_untils->GetValueFromConfigFile(PMSeverConfig::NormalSlotCount))), m_HandicapSlotMax(m_HandicapSlotCount),
      m_NormalSlotMax(m_NormalSlotCount) {

    m_server.reset(new WebSocketServer(m_websocketPort, m_messageManager.get()));
    m_server->StartWebsocketServer();
    m_running.store(true, std::memory_order_relaxed);

    m_GateHanlder = std::async(std::launch::async, std::bind(&ParkManagerServer::GateHanlder, this, std::ref(m_running)));
}

ParkManagerServer::~ParkManagerServer() {
    m_running.store(false, std::memory_order_relaxed);
    // ManualStop();
    if (m_RunHanlder.valid())
        m_RunHanlder.get();
    if (m_GateHanlder.valid())
        m_GateHanlder.get();
}

void ParkManagerServer::start() {
    m_RunHanlder = std::async(std::launch::async, std::bind(&ParkManagerServer::RunServer, this, std::ref(m_running)));
}

int ParkManagerServer::RunServer(std::atomic<bool>& m_running) {
    while (m_running) {
        ////////////////////////////////check websocket server status////////////////////
        std::string serverStatusRaw = std::string("");
        m_shmObj->GetValue("WebsocketServerStatus", serverStatusRaw);

        if (serverStatusRaw.compare(m_currentServerStatus) != 0) {
            std::vector<std::string> serverStatusRawItems;
            boost::split(serverStatusRawItems, serverStatusRaw, boost::is_any_of("#"));
            if (serverStatusRawItems.size() != 2) {
                m_currentServerStatus = serverStatusRaw;
                std::this_thread::sleep_for(std::chrono::microseconds(1000));
                continue;
            }
            std::string uri = serverStatusRawItems[0];
            int serverStatus = std::stoi(serverStatusRawItems[1].c_str());
            LOG_TRACE("%s %d uri: %s    ServerStatus: %d\n", __PRETTY_FUNCTION__, __LINE__, uri.c_str(), serverStatus);
            if (!uri.empty()) {
                if (serverStatus == OPEN) {

                    LOG_INFO("server has a new connection \n");
                }

                if (serverStatus == CLOSE || serverStatus == FAILED) {
                    LOG_INFO("closing %s connection \n", uri.c_str());
                    m_server->Close(uri);
                }
            }
            m_currentServerStatus = serverStatusRaw;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
    LOG_INFO("terminating Run\n");
    return 0;
}

int ParkManagerServer::GateHanlder(std::atomic<bool>& m_running) {

    LOG_INFO("activating GateHanlder\n");
    while (m_running) {

        if (m_messageManager->GetCALLMessageSize() > 0) {
            WSMessage ocppMessage = m_messageManager->GetCALLMessage();

            std::string type = ocppMessage.type;
            std::string uri = ocppMessage.uri;
            std::string input = ocppMessage.payload;

            std::vector<std::string> requestItems;
            boost::split(requestItems, input, boost::is_any_of(","));
            std::string messageId = requestItems[1];
            std::string action = requestItems[2];
            LOG_INFO("###################Incoming Message PM <-> Gate #############\n");
            LOG_DEBUG("timestamp:%ld\n", time(0));
            LOG_DEBUG("type:%s\n", type.c_str());
            LOG_DEBUG("uri:%s\n", uri.c_str());
            LOG_DEBUG("input:%s\n", input.c_str());
            LOG_DEBUG("###############################################\n");
            if (type.compare("Server") == 0) {
                if (action.find("\"" + models::ActionGateRegistration + "\"") != std::string::npos) {
                    OnGateRegistration(uri, input);
                } else if (action.find("\"" + models::ActionCarStatusUpdate + "\"") != std::string::npos) {
                    OnCarStatusUpdate(uri, input);
                } else if (action.find("\"" + models::ActionCarEnterence + "\"") != std::string::npos) {
                    OnCarEnterence(uri, input);
                } else {
                    LOG_FATAL("unkown or unsupport action: %s\n", action.c_str());
                }
            }
        }
    }
    LOG_INFO("terminating GateHanlder\n");
    return 0;
}

void ParkManagerServer::OnGateRegistration(std::string uri, std::string input) {
    LOG_INFO("%s %d Gate GateRegistration request:%s\n", __FUNCTION__, __LINE__, input.c_str());
    std::string messageId, JsonString;
    handleIncomingMsg(input, messageId, JsonString, messageType::CALL);

    quicktype::GateRegistration GateRegist;
    quicktype::GateRegistrationResponse GateRegistResp;

    LOG_INFO("messageId %s JsonString %s \n", messageId.c_str(), JsonString.c_str());

    nlohmann::from_json(nlohmann::json::parse(JsonString), GateRegist);
    std::string GateName(GateRegist.getGateName());
    int GateType(-1);
    try {
        GateType = stoi(GateRegist.getGateType());
    } catch (std::exception& e) {
        LOG_ERROR(" error on registration of gate with URI %s \n ", e.what());
    }

    if (GateType == ParkManagment::GateType::Enterence) {
        if (m_EnternceGates.find(GateName) == m_EnternceGates.end()) {
            LOG_INFO("Registering Entrance gate %s with uri %s \n", GateName.c_str(), uri.c_str());
            m_EnternceGates.insert(GateName);
            GateRegistResp.setResult(RegistrationStatus::Accept);

        } else {
            LOG_INFO("gate %s is already registered\n");
            GateRegistResp.setResult(RegistrationStatus::Exist);
        }
    } else if (GateType == ParkManagment::GateType::Exiting) {
        if (m_exitingGates.find(GateName) == m_exitingGates.end()) {
            LOG_INFO("Registering exitingGates  %s with uri %s \n", GateName.c_str(), uri.c_str());
            m_exitingGates.insert(GateName);
            GateRegistResp.setResult(RegistrationStatus::Accept);

        } else {
            LOG_INFO("gate %s is already registered\n");
            GateRegistResp.setResult(RegistrationStatus::Exist);
        }
    } else {
        LOG_ERROR("unknown Gate type %d \n", GateType);
        GateRegistResp.setResult(RegistrationStatus::Reject);
    }
    nlohmann::json j;
    nlohmann::to_json(j, GateRegistResp);
    std::string Response(AddBase(j.dump(), messageType::RESPOND, "", messageId).c_str());
    LOG_INFO("%s %d  GateRegistrationResponse : \n %s \n", __FUNCTION__, __LINE__, Response.c_str());
    std::unique_lock<std::mutex> Sendguard(m_WebSocketAccess_LOCK);
    m_server->SendMessage(uri, Response);
}

void ParkManagerServer::OnCarEnterence(std::string uri, std::string input) {
    LOG_INFO("%s %d Gate CarEnterence request:%s\n", __FUNCTION__, __LINE__, input.c_str());

    std::string messageId, JsonString;
    handleIncomingMsg(input, messageId, JsonString, messageType::CALL);

    quicktype::CarEnterence CarEnter;
    quicktype::CarEnterenceResponse CarEnterResp;

    nlohmann::from_json(nlohmann::json::parse(JsonString), CarEnter);

    if (m_EnternceGates.find(CarEnter.getEnterenceGate()) == m_EnternceGates.end()) {
        LOG_INFO(" gate %s with uri %s is not registered \n", CarEnter.getEnterenceGate().c_str(), uri.c_str());
        CarEnterResp.setResult(RegistrationStatus::Reject);
        std::string comment("Can not access, gate not registered");
        CarEnterResp.setComment(comment);
    } else {

        char buffer[250];
        std::string comment;
        std::string Handicap(CarEnter.getHandicap());
        int slot(0);
        if ((Handicap == HandicapStatus::Yes) && m_HandicapSlotCount) {
            LOG_INFO("allocating Handicap slot , remaining slots %d \n", m_HandicapSlotCount);
            slot = m_NormalSlotMax + m_HandicapSlotCount--;
        } else if (m_NormalSlotCount) {
            slot = m_NormalSlotCount--;
            Handicap = HandicapStatus::No;
        }

        if (slot) {
            std::unique_ptr<Car> NewEntryCar =
                std::make_unique<Car>(random(), CarEnter.getCarPlate(), CarEnter.getEnteranceTime(), CarEnter.getEnterenceGate(), Handicap);

            snprintf(buffer, 250,
                     "Welcome %s ,Gate %s will be opened for you, first %s minutes of your stay is free of charge then its only %s%s per min proceed "
                     "to slot number %d entering time %s",
                     CarEnter.getCarPlate().c_str(), CarEnter.getEnterenceGate().c_str(),
                     m_untils->GetValueFromConfigFile(PMSeverConfig::FreeDurationinMin).c_str(),
                     m_untils->GetValueFromConfigFile(PMSeverConfig::PricePerMin).c_str(),
                     m_untils->GetValueFromConfigFile(PMSeverConfig::Currency).c_str(), slot, NewEntryCar->GetEnterenceTime().c_str());

            m_RegisteredCars[NewEntryCar->GatPlate()] = std::move(NewEntryCar);
            CarEnterResp.setResult(RegistrationStatus::Accept);
            comment = buffer;

        } else {
            CarEnterResp.setResult(RegistrationStatus::Full);
            comment = "sorry the Parking is full";
        }

        CarEnterResp.setComment(comment);
    }

    nlohmann::json j;
    nlohmann::to_json(j, CarEnterResp);
    std::string Response(AddBase(j.dump(), messageType::RESPOND, "", messageId).c_str());
    LOG_INFO("%s %d  CarEnterence : \n %s \n", __FUNCTION__, __LINE__, Response.c_str());
    std::unique_lock<std::mutex> Sendguard(m_WebSocketAccess_LOCK);
    m_server->SendMessage(uri, Response);
}

void ParkManagerServer::OnCarStatusUpdate(std::string uri, std::string input) {
    LOG_INFO("%s %d Gate CarStatusUpdate request:%s\n", __FUNCTION__, __LINE__, input.c_str());

    std::string messageId, JsonString;
    handleIncomingMsg(input, messageId, JsonString, messageType::CALL);

    quicktype::CarStatusUpdate CarStatusUp;
    quicktype::CarStatusUpdateResponse CarStatusUpResp;
    char buffer[100];
    std::string comment;
    std::string payable("0");
    std::string curreny(m_untils->GetValueFromConfigFile(PMSeverConfig::Currency));
    std::string Status(RegistrationStatus::Exist);
    nlohmann::from_json(nlohmann::json::parse(JsonString), CarStatusUp);
    if (m_exitingGates.find(CarStatusUp.getExitingGate()) == m_exitingGates.end()) {
        LOG_INFO("gate not register ,register the gate\n");
    }

    if (m_RegisteredCars.find(CarStatusUp.getCarPlate()) == m_RegisteredCars.end()) {
        comment = "How did you get in ?? you are not registered , calling the police ....just kidding :D bye bye";
    } else {
        std::string timeUTC;
        GetCurrentUTCTime(timeUTC);
        LOG_DEBUG("current time UTC %s \n ", timeUTC.c_str());
        time_t CT(0);
        GetTimeinSeconds(timeUTC, CT);
        LOG_DEBUG("current time %d \n ", CT);
        std::string EnterenceTimeUTC = m_RegisteredCars[CarStatusUp.getCarPlate()]->GetEnterenceTime();
        time_t ET(0);
        GetTimeinSeconds(EnterenceTimeUTC, ET);
        time_t Duration = CT - ET;
        LOG_DEBUG("EnterenceTimeUTC  %d EnterenceTimeUTC  %s durationSec %d , free time s:%d\n ", ET, EnterenceTimeUTC.c_str(), Duration,
                  stoi(m_untils->GetValueFromConfigFile(PMSeverConfig::FreeDurationinMin)) * 60);
        comment = " bye bye come again :D";

        if (Duration > stoi(m_untils->GetValueFromConfigFile(PMSeverConfig::FreeDurationinMin)) * 60) {
            double PricePerMin = std::atof(m_untils->GetValueFromConfigFile(PMSeverConfig::PricePerMin).c_str());
            int ToPay = std::round(PricePerMin * ((Duration / 60) - stoi(m_untils->GetValueFromConfigFile(PMSeverConfig::FreeDurationinMin))));
            payable = std::to_string(ToPay);
        }

        if (m_RegisteredCars[CarStatusUp.getCarPlate()]->GateHandicap() == HandicapStatus::Yes) {
            m_HandicapSlotCount++;
        } else {
            m_NormalSlotCount++;
            m_UnRegisteredCars[CarStatusUp.getCarPlate()] = std::move(m_RegisteredCars[CarStatusUp.getCarPlate()]);
        }
    }
    CarStatusUpResp.setCurreny(curreny);
    CarStatusUpResp.setPayable(payable);
    CarStatusUpResp.setComment(comment);
    CarStatusUpResp.setStatus(Status);
    nlohmann::json j;
    nlohmann::to_json(j, CarStatusUpResp);
    std::string Response(AddBase(j.dump(), messageType::RESPOND, "", messageId).c_str());
    LOG_INFO("%s %d  CarStatusUpdate : \n %s \n", __FUNCTION__, __LINE__, Response.c_str());
    std::unique_lock<std::mutex> Sendguard(m_WebSocketAccess_LOCK);
    m_server->SendMessage(uri, Response);
}
