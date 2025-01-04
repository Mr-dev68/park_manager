/*
 * ParkManagerServer.h
 *
 *  Created on: Jul 1, 2022
 *      Author: rohan
 */

#pragma once
#include <map>
#include <set>
#include <thread>
#include <future>
#include <atomic>
#include <functional>
#include <atomic>
#include <mutex>


#include "utils.hpp"
#include "WebScoket.h"
#include "BoostShmObj.hpp"
#include "MessageManager.hpp"
#include "WebSocketServer.h"
#include "Car.hpp"


class ParkManagerServer {
public:

	ParkManagerServer();
	void start();
	virtual ~ParkManagerServer();
private:
	std::unique_ptr<utils> m_untils;
	unsigned int m_HandicapSlotCount;
	unsigned int m_NormalSlotCount;
	unsigned int m_HandicapSlotMax;
	unsigned int m_NormalSlotMax;

	unsigned int m_websocketPort ;
	std::string m_currentServerStatus;




	std::unique_ptr<C_BoostShmObj> 			 m_shmObj;
	std::unique_ptr<MessageManager> 		 m_messageManager;
	std::unique_ptr<WebSocketServer> 		 m_server;

	int RunServer(std::atomic<bool>& m_running);
	int GateHanlder(std::atomic<bool>& m_running);

	std::set<std::string> m_EnternceGates;
	std::set<std::string> m_exitingGates;
	std::map<std::string,std::unique_ptr<Car>> m_RegisteredCars; // idtag -> car class
	std::map<std::string,std::unique_ptr<Car>> m_UnRegisteredCars;// idtag -> car class

	std::atomic<bool> m_running;
	std::future<int> m_RunHanlder;
	std::future<int> m_GateHanlder;
	std::mutex m_WebSocketAccess_LOCK;



	void OnGateRegistration(std::string uri, std::string input);
	void OnCarEnterence(std::string uri, std::string input);
	void OnCarStatusUpdate(std::string uri, std::string input);
};

