/*
 * WebSocketClient.cpp
 *
 *  Created on: Apr 13, 2021
 *      Author: hhong
 */

#include "WebSocketClient.h"
#include "WebScoket.h"
#include <stdlib.h>

#include "logging.hpp"
#define SEND_RETRY 5

using websocketpp::lib::bind;
using websocketpp::lib::thread;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

WebSocketClient::WebSocketClient(std::string url, std::string id, MessageManager* messageManager, bool LocalController)
    : m_url(url), m_id(id), m_status(INIT), m_shmObj(new C_BoostShmObj(m_id.c_str())), m_messageManager(messageManager), m_IsLC(LocalController) {
    LOG_INFO("initializing new client %s \n", std::string(url + "/" + id).c_str());
    std::string value = m_id + "#" + std::to_string(m_status);
    m_shmObj->SetValue("WebsocketClientStatus", value.c_str());
}

WebSocketClient::~WebSocketClient() {
    LOG_ERROR("\033[0;33m WebSocketClient for id %s destroyed \033[0m \n", m_id.c_str());
    m_client.stop_perpetual();

    if (m_status == OPEN) {
        websocketpp::lib::error_code ec;
        m_client.close(m_hdl, websocketpp::close::status::going_away, "", ec);
        if (ec) {
            LOG_FATAL("%s %d: Error closing connection(%s)\n", __PRETTY_FUNCTION__, __LINE__, ec.message());
        }
    }
    m_thread->join();
}

void WebSocketClient::Connect() {

    m_client.set_access_channels(websocketpp::log::elevel::none);
    m_client.set_error_channels(websocketpp::log::alevel::none);
    m_client.init_asio();

    m_client.start_perpetual();

    m_thread = websocketpp::lib::make_shared<thread>(&client::run, &m_client);

    websocketpp::lib::error_code ec;

    std::string csms = m_url + "/" + m_id;
    LOG_INFO("WebSocketClient::Connect:: connecting to %s \n", csms.c_str());
    client::connection_ptr pConnection = m_client.get_connection(csms, ec);
    if (ec) {
        LOG_FATAL("%s %d: Connect initialization error (%s)\n", __PRETTY_FUNCTION__, __LINE__, ec.message().c_str());
        return;
    }

    std::string request("PMS");
    pConnection->add_subprotocol(request, ec);
    if (ec) {
        std::cerr << "> Connect add_subprotocol error: " << ec.message() << std::endl;
    }

    std::vector<std::string> requested_subprotocols = pConnection->get_requested_subprotocols();
    for (size_t i = 0; i < requested_subprotocols.size(); ++i) {
        std::cout << "Connection subprotocols: " << requested_subprotocols[i] << std::endl;
    }

    m_hdl = pConnection->get_handle();
    pConnection->set_open_handler(bind(&WebSocketClient::on_open, this, &m_client, ::_1));
    pConnection->set_fail_handler(bind(&WebSocketClient::on_fail, this, &m_client, ::_1));
    pConnection->set_close_handler(bind(&WebSocketClient::on_close, this, &m_client, ::_1));
    //	pConnection->set_pong_handler(bind(&WebSocketClient::onPong, this, &m_client, ::_1,::_2));
    pConnection->set_message_handler(bind(&WebSocketClient::on_message, this, &m_client, ::_1, ::_2));

    m_client.connect(pConnection);

    m_status = CONNECTING;
}

void WebSocketClient::on_open(client* client, websocketpp::connection_hdl hdl) {
    LOG_TRACE("%s: %d\n", __PRETTY_FUNCTION__, __LINE__);
    m_status = OPEN;
    if (!m_IsLC) {
        std::string value = m_id + "#" + std::to_string(m_status);
        m_shmObj->SetValue("WebsocketClientStatus", value.c_str());
    } else {
        std::string value = std::to_string(m_status);
        m_shmObj->SetValue("LCWebsocketStatus", value.c_str());
    }
}

void WebSocketClient::on_fail(client* client, websocketpp::connection_hdl hdl) {
    LOG_TRACE("%s: %d, failed client %s\n", __PRETTY_FUNCTION__, __LINE__, m_id.c_str());
    m_status = FAILED;
    std::string value = m_id + "#" + std::to_string(m_status);
    m_shmObj->SetValue("WebsocketClientStatus", value.c_str());
}

void WebSocketClient::on_close(client* client, websocketpp::connection_hdl hdl) {
    LOG_TRACE("%s: %d closing client %s\n", __PRETTY_FUNCTION__, __LINE__, m_id.c_str());
    m_status = CLOSE;
    std::string value = m_id + "#" + std::to_string(m_status);
    m_shmObj->SetValue("WebsocketClientStatus", value.c_str());
}

void WebSocketClient::on_message(client* client, websocketpp::connection_hdl hdl, client::message_ptr msg) {
    LOG_FATAL("[%ld] >> %s: %d\n", time(0), __PRETTY_FUNCTION__, __LINE__);
    m_message = "";
    if (msg->get_opcode() == websocketpp::frame::opcode::text) {
        m_message = msg->get_payload();
        LOG_TRACE("message:%s\n", m_message.c_str());
    }
    if (!m_IsLC) {
        std::string message = "Client#" + m_id + "#" + m_message;
        //	m_shmObj->SetValue("WebsocketClientMessage", message.c_str());
        m_messageManager->AddMessage(message);
    } else {
        std::string message = "LC#" + m_id + "#" + m_message;
        //	m_shmObj->SetValue("WebsocketClientMessage", message.c_str());
        m_messageManager->AddMessage(message);
    }
}

void WebSocketClient::Close() {
    websocketpp::lib::error_code ec;

    m_client.close(m_hdl, websocketpp::close::status::normal, "Soft close", ec);

    if (ec) {
        LOG_FATAL("%s %d: Error initiating close (%s)\n", __PRETTY_FUNCTION__, __LINE__, ec.message().c_str());
    }
}

bool WebSocketClient::SendMessage(std::string message) {
    LOG_TRACE("%s: %d message:%s\n", __PRETTY_FUNCTION__, __LINE__, m_message.c_str());

    websocketpp::lib::error_code ec;

    int i = 0;
    do {
        if (m_status == OPEN) {
            m_client.send(m_hdl, message, websocketpp::frame::opcode::text, ec);
            if (!ec) {
                LOG_FATAL("%s %d: Sending message successful.\n", __PRETTY_FUNCTION__, __LINE__);
                return true;
            }
        }
        usleep(1000000);
    } while (i++ < SEND_RETRY);

    if (ec) {
        LOG_FATAL("%s %d: Error Sending message (%s)\n", __PRETTY_FUNCTION__, __LINE__, ec.message());
    }
    return false;
}
