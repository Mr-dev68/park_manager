
#include "MessageManager.hpp"
#include "TimeMeasuring.h"

#include <boost/algorithm/string.hpp>
#include <chrono>
#include <thread>

#include "logging.hpp"
#define GET_RESULT_TIMEOUT 5 // unit second

#define CALL "0"
#define CALLRESULT "1"
MessageManager::MessageManager() {}

MessageManager::~MessageManager() {}

unsigned int MessageManager::GetCALLMessageSize() { return ocppCALLMessageQueue.size(); }

bool MessageManager::AddMessage(std::string rawMessage) {
    //	client: "Client#" + m_id + "#" + m_message
    //	server: "Server#" strUri + "#" + m_message

    // LOG_FATAL("[%ld] >> %s:%d raw message: %s\n", time(0), __PRETTY_FUNCTION__, __LINE__, rawMessage.c_str());

    bool rc = false;
    std::vector<std::string> rawMessageItems;
    boost::split(rawMessageItems, rawMessage, boost::is_any_of("#"));
    std::string type = rawMessageItems[0];
    std::string uri = rawMessageItems[1];
    std::string payload = rawMessageItems[2];

    if (payload.size() > 0) {
        std::string incoming = payload.substr(1, payload.length() - 2);
        std::vector<std::string> requestItems;
        boost::split(requestItems, incoming, boost::is_any_of(","));
        std::string messageType = requestItems[0];

        WSMessage message;
        message.type = type;
        message.uri = uri;
        message.payload = payload;
        // LOG_DEBUG("messageType %s message.type %s ,message.uri %s ,message.payload %s
        // \n",messageType.c_str(),message.type.c_str(),message.uri.c_str() ,message.payload.c_str() );
        if (messageType.compare(CALL) == 0) {
            std::lock_guard<std::mutex> guard(m_CALL_Lock);
            ocppCALLMessageQueue.push_back(message);
            // LOG_TRACE("%s:%d Put message in ocppCALLMessageQueue Size:%ld\n", __PRETTY_FUNCTION__, __LINE__, ocppCALLMessageQueue.size());
            rc = true;
        } else if (messageType.compare(CALLRESULT) == 0) {
            std::lock_guard<std::mutex> guard(m_CALLRESULT_Lock);
            ocppCALLRESULTMessageQueue.push_back(message);
            // LOG_TRACE("%s:%d Put message in ocppCALLRESULTMessageQueue Size:%ld\n", __PRETTY_FUNCTION__, __LINE__,
            // ocppCALLRESULTMessageQueue.size());
            rc = true;
        }
    }

    return rc;
}

WSMessage MessageManager::GetCALLMessage() {
    std::lock_guard<std::mutex> guard(m_CALL_Lock);

    std::vector<WSMessage>::iterator it = ocppCALLMessageQueue.begin();
    WSMessage ocppMessage = *it;
    ocppCALLMessageQueue.erase(it);

    return ocppMessage;
}

std::string MessageManager::GetResponseMessage(std::string messageId, bool& isCallError) {
    LOG_INFO("getting response \n");
    isCallError = false;

    std::vector<WSMessage>::iterator it;

    long startTime = GetCurrentSecond();

    do {
        for (it = ocppCALLRESULTMessageQueue.begin(); it != ocppCALLRESULTMessageQueue.end(); it++) {

            std::string message = (*it).payload;
            std::vector<std::string> requestItems;
            boost::split(requestItems, message, boost::is_any_of(","));
            std::string tmp = requestItems[1];

            if (tmp.substr(1, tmp.length() - 2).compare(messageId) == 0) {
                std::lock_guard<std::mutex> guard(m_CALLRESULT_Lock);
                ocppCALLRESULTMessageQueue.erase(it);

                return message;
            }
        }

        std::this_thread::sleep_for(std::chrono::microseconds(500));
    } while (GetCurrentSecond() - startTime < GET_RESULT_TIMEOUT);

    LOG_FATAL("%s %d. Not find response Message with messageId:%s\n", __PRETTY_FUNCTION__, __LINE__, messageId.c_str());
    return std::string("");
}

long MessageManager::GetCurrentSecond() {
    struct timeval aTime;
    gettimeofday(&aTime, 0);
    return aTime.tv_sec;
}
