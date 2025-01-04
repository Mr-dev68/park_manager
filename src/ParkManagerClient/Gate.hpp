/*
 * Gate.hpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */

#ifndef SRC_PARKMANAGERCLIENT_GATE_HPP_
#define SRC_PARKMANAGERCLIENT_GATE_HPP_

#include <vector>
#include <map>
#include <algorithm>
#include <thread>
#include <future>
#include <chrono>
#include <functional>
#include <atomic>
#include <mutex>
#include "WebScoket.h"
#include "BoostShmObj.hpp"
#include "WebSocketClient.h"
#include "MessageManager.hpp"
#include "utils.hpp"
#include "common.h"
class Gate {
public:
	Gate();
	Gate(std::string ConfigPath);
	int GateRegistration();
	int CarRegistration(std::string CarPlate,bool Handicap,std::string &msg);
	int CarStatusUpdate(std::string CarPlate,UpdateStatus &UpdateStatusMsg);
	std::string GetGateName()
	{
		return m_GateName;
	}
	int GetGateType()
	{
		return m_GateType ;
	}
	virtual ~Gate();

private:


	std::unique_ptr<utils> m_untils;
	std::unique_ptr<C_BoostShmObj> 			 m_shmObj;
	std::unique_ptr<MessageManager> 		 m_messageManager;
	std::unique_ptr<WebSocketClient> 		 m_GateWebSocketClient;

	std::future<int> m_GateNetworkThread;
	std::mutex m_WebSocketAccess_LOCK;
	std::atomic<bool> m_running;
	std::string m_WebsocketURL;
	std::string m_ClientURI;
	std::string m_GateName;
	int m_GateType;
	bool m_NetworkProblem;


	int ConnectionHolder(std::atomic<bool>&);
};

#endif /* SRC_PARKMANAGERCLIENT_GATE_HPP_ */
