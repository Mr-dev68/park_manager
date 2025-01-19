
#ifndef SOURCES_MAIN_MESSSGEMANAGER_H_
#define SOURCES_MAIN_MESSSGEMANAGER_H_

#include <mutex>
#include <vector>

#include "common.h"

class MessageManager {
  public:
    MessageManager();
    virtual ~MessageManager();

    unsigned int GetCALLMessageSize();
    bool AddMessage(std::string rawMessage);
    std::string GetResponseMessage(std::string messageId, bool& isCallError);
    WSMessage GetCALLMessage();

  private:
    std::mutex m_CALLRESULT_Lock;
    std::mutex m_CALLERROR_Lock;
    std::mutex m_CALL_Lock;

    std::vector<WSMessage> ocppCALLMessageQueue;
    std::vector<WSMessage> ocppCALLRESULTMessageQueue;
    long GetCurrentSecond();
};

#endif /* SOURCES_MAIN_MESSSGEMANAGER_H_ */
