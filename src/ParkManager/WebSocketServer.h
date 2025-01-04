/*
 * Gate.hpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */
#ifndef SOURCES_WEBSOCKET_WEBSOCKETSERVER_H_
#define SOURCES_WEBSOCKET_WEBSOCKETSERVER_H_

#include "BoostShmObj.hpp"
#include "MessageManager.hpp"
#include <future>
#include <websocketpp/server.hpp>

#include <websocketpp/config/asio_no_tls.hpp>
typedef websocketpp::server<websocketpp::config::asio> server;


class WebSocketServer
{
public:
	WebSocketServer(int port, MessageManager* messageManager);
	virtual ~WebSocketServer();

	void StartWebsocketServer();
	bool SendMessage(std::string uri, std::string message);
	void Close(std::string strUri);

private:
	int m_port;
	server m_server;
	int m_status;
	std::string m_websocketMessage;
    std::map<std::string, websocketpp::connection_hdl> m_connections;
    websocketpp::lib::mutex m_connection_lock;
	std::unique_ptr<C_BoostShmObj> m_shmObj;
	std::future<int>	m_WebsocketServerThread;
	MessageManager* m_messageManager;

    virtual int Run();
	bool on_validate(server* s, websocketpp::connection_hdl hdl);
	void on_open(server* s, websocketpp::connection_hdl hdl);
	void on_close(server* s, websocketpp::connection_hdl hdl);
	void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg);
};

#endif /* SOURCES_WEBSOCKET_WEBSOCKETSERVER_H_ */
