/*
 * Gate.hpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */
#include "WebSocketServer.h"
#include "WebScoket.h"
#include <stdlib.h>

#include "logging.hpp"

#define SEND_RETRY 5

using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::lock_guard;
using websocketpp::lib::mutex;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

WebSocketServer::WebSocketServer(int port, MessageManager* messageManager)
    : m_port(port), m_status(INIT), m_shmObj(new C_BoostShmObj("Rasekh::PMS")), m_messageManager(messageManager) {
    m_server.init_asio();
    m_server.set_reuse_addr(true);

    m_server.set_access_channels(websocketpp::log::alevel::none);
    m_server.set_error_channels(websocketpp::log::alevel::none);

    m_server.set_open_handler(bind(&WebSocketServer::on_open, this, &m_server, ::_1));
    m_server.set_close_handler(bind(&WebSocketServer::on_close, this, &m_server, ::_1));
    m_server.set_validate_handler(bind(&WebSocketServer::on_validate, this, &m_server, ::_1));
    m_server.set_message_handler(bind(&WebSocketServer::on_message, this, &m_server, ::_1, ::_2));

#ifdef __WEBSOCKET_TSL__
    m_server.set_tls_init_handler(bind(&WebSocketServer::on_tls_init, this, ::_1));
#endif

    std::string value = "#" + std::to_string(m_status);
    m_shmObj->SetValue("WebsocketServerStatus", value.c_str());
    //	std::string message = "#" + std::string("");
    //	m_shmObj->SetValue("WebsocketServerMessage", message.c_str());
}

WebSocketServer::~WebSocketServer() {

    websocketpp::lib::error_code ec;
    m_server.stop_listening(ec);

    std::map<std::string, connection_hdl>::iterator iter;
    for (iter = m_connections.begin(); iter != m_connections.end(); ++iter) {
        m_server.close(iter->second, websocketpp::close::status::normal, "Terminating connection...", ec);
    }
    m_server.stop();
    if (m_WebsocketServerThread.valid())
        m_WebsocketServerThread.get();
    sleep(4);
}

void WebSocketServer::Close(std::string strUri) {
    std::map<std::string, connection_hdl>::iterator it;
    it = m_connections.find(strUri);
    if (it != m_connections.end()) {
        websocketpp::lib::error_code ec;
        m_server.close(it->second, websocketpp::close::status::normal, "Terminating connection...", ec);

        lock_guard<mutex> guard(m_connection_lock);
        m_connections.erase(it);
    }
}

void WebSocketServer::StartWebsocketServer() { m_WebsocketServerThread = std::async(std::launch::async, std::bind(&WebSocketServer::Run, this)); }
int WebSocketServer::Run() {
    m_server.listen(boost::asio::ip::tcp::v4(), m_port);
    m_server.start_accept();

    LOG_FATAL("%s %d: websocketServer start listening port: %d\n", __PRETTY_FUNCTION__, __LINE__, m_port);

    try {
        m_server.run();
    } catch (const std::exception& e) {
        LOG_FATAL("%s %d: %s\n", __PRETTY_FUNCTION__, __LINE__, e.what());
    }

    return 0;
}

bool WebSocketServer::on_validate(server* s, connection_hdl hdl) {

    std::string subprotocol = "PMS";
    server::connection_ptr con = s->get_con_from_hdl(hdl);

    std::vector<std::string> requested_subprotocols = con->get_requested_subprotocols();
    for (size_t i = 0; i < requested_subprotocols.size(); ++i) {
        if (requested_subprotocols[i] == subprotocol) {
            websocketpp::lib::error_code ec;
            con->select_subprotocol(subprotocol, ec);
            if (ec) {
                LOG_FATAL("%s %d: select_subprotocol error: %s\n", __PRETTY_FUNCTION__, __LINE__, ec.message());
                return false;
            } else {
                LOG_FATAL("%s %d: select_subprotocol: %s\n", __PRETTY_FUNCTION__, __LINE__, subprotocol.c_str());
                return true;
            }
        }
    }

    return false;
}

void WebSocketServer::on_open(server* s, connection_hdl hdl) {
    LOG_TRACE("%s: %d\n", __PRETTY_FUNCTION__, __LINE__);
    m_status = OPEN;
    server::connection_ptr con = s->get_con_from_hdl(hdl);
    websocketpp::config::core::request_type requestClient = con->get_request();
    std::string strUri = requestClient.get_uri();
    LOG_INFO("connection opened \n");
    LOG_INFO("%s: %d strUri:%s\n", __PRETTY_FUNCTION__, __LINE__, strUri.c_str());
    lock_guard<mutex> guard(m_connection_lock);
    m_connections.insert(std::pair<std::string, connection_hdl>(strUri, hdl));

    std::string value = strUri + "#" + std::to_string(m_status);
    m_shmObj->SetValue("WebsocketServerStatus", value.c_str());
}

void WebSocketServer::on_close(server* s, connection_hdl hdl) {
    LOG_TRACE("%s: %d\n", __PRETTY_FUNCTION__, __LINE__);
    m_status = CLOSE;
    server::connection_ptr con = s->get_con_from_hdl(hdl);
    websocketpp::config::core::request_type requestClient = con->get_request();
    std::string strUri = requestClient.get_uri();

    std::map<std::string, connection_hdl>::iterator it;
    it = m_connections.find(strUri);
    if (it != m_connections.end()) {
        lock_guard<mutex> guard(m_connection_lock);
        m_connections.erase(it);
    }

    std::string value = strUri + "#" + std::to_string(m_status);
    m_shmObj->SetValue("WebsocketServerStatus", value.c_str());
}

void WebSocketServer::on_message(server* s, connection_hdl hdl, server::message_ptr msg) {
    LOG_DEBUG("[%ld] >> %s: %d\n", time(0), __PRETTY_FUNCTION__, __LINE__);
    m_websocketMessage = "";
    server::connection_ptr con = s->get_con_from_hdl(hdl);
    websocketpp::config::core::request_type requestClient = con->get_request();
    std::string strUri = requestClient.get_uri();
    m_websocketMessage = msg->get_payload();
    LOG_TRACE("message: %s\n", m_websocketMessage.c_str());

    std::string value = "Server#" + strUri + "#" + m_websocketMessage;
    //	m_shmObj->SetValue("WebsocketServerMessage", value.c_str());

    m_messageManager->AddMessage(value);
}

bool WebSocketServer::SendMessage(std::string uri, std::string message) {
    LOG_DEBUG("%s %d send message:%s\n", __FUNCTION__, __LINE__, message.c_str());
    std::map<std::string, connection_hdl>::iterator it;
    it = m_connections.find(uri);
    if (it != m_connections.end()) {
        websocketpp::lib::error_code ec;

        int i = 0;
        do {
            if (m_status == OPEN) {
                m_server.send(it->second, message, websocketpp::frame::opcode::text, ec);
                if (!ec) {
                    LOG_DEBUG("%s %d: Sending message successful.\n", __PRETTY_FUNCTION__, __LINE__);
                    return true;
                }
            }
            usleep(1000000);
        } while (i++ < 5);

        if (ec) {
            std::string error = ec.message();
            LOG_FATAL("%s %d: Error Sending message (%s)\n", __PRETTY_FUNCTION__, __LINE__, error.c_str());
        }
    } else {
        LOG_FATAL("%s %d: unknown uri:%s\n", __PRETTY_FUNCTION__, __LINE__, uri.c_str());
    }
    return false;
}
