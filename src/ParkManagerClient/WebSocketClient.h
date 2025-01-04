/*
 * Gate.hpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */

#ifndef SOURCES_WEBSOCKET_WEBSOCKETCLIENT_H_
#define SOURCES_WEBSOCKET_WEBSOCKETCLIENT_H_

#include "BoostShmObj.hpp"
#include "MessageManager.hpp"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#ifdef __WEBSOCKET_SSL__
#include <websocketpp/config/asio_client.hpp>
typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<boost::asio::ssl::context> context_ptr;
#else
#include <websocketpp/config/asio_no_tls_client.hpp>
typedef websocketpp::client<websocketpp::config::asio_client> client;
#endif

class WebSocketClient {
  public:
    WebSocketClient(std::string url, std::string id, MessageManager* messageManager, bool LocalController = false);
    virtual ~WebSocketClient();

    void Connect();
    void Close();

    bool SendMessage(std::string message);

  private:
    std::string m_url;
    std::string m_id;
    int m_status;
    bool m_IsLC;

    client m_client;
    std::string m_message;
    websocketpp::connection_hdl m_hdl;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
    std::unique_ptr<C_BoostShmObj> m_shmObj;
    MessageManager* m_messageManager;

    // void onPong(client* client, websocketpp::connection_hdl hdl, std::string msg);
    void on_open(client* client, websocketpp::connection_hdl hdl);
    void on_fail(client* client, websocketpp::connection_hdl hdl);
    void on_close(client* client, websocketpp::connection_hdl hdl);
    void on_message(client* client, websocketpp::connection_hdl hdl, client::message_ptr msg);

#ifdef __WEBSOCKET_SSL__
    context_ptr on_tls_init(websocketpp::connection_hdl hdl);
#endif
};

#endif /* SOURCES_WEBSOCKET_WEBSOCKETCLIENT_H_ */
