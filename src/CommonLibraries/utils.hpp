/*
 * utils.hpp
 *
 *  Created on: Jul 1, 2022
 *      Author: rohan
 */

#ifndef SRC_PARKMANAGER_UTILS_HPP_
#define SRC_PARKMANAGER_UTILS_HPP_

#include <SimpleIni.h> // Include SimpleIni
#include <exception>
#include <fstream>
#include <iostream>
#include <mutex>
#include <set>
#include <sstream>
#include <string>

namespace PMClientConfig {
static const char WebsocketURL[] = "WebsocketURL";
static const char ClientURI[] = "ClientURI";
static const char GateName[] = "GateName";
static const char GateType[] = "GateType";
static const char RegistratinResult[] = "RegistratinResult";
static const char ConfigPath[] = "ClientConfig.ini";
} // namespace PMClientConfig

namespace PMSeverConfig {
static const char WebSocketPort[] = "WebSocketPort";
static const char HandicapSlotCount[] = "HandicapSlotCount";
static const char NormalSlotCount[] = "NormalSlotCount";
static const char Currency[] = "Currency";
static const char PricePerMin[] = "PricePerMin";
static const char FreeDurationinMin[] = "FreeDurationinMin";
static const char ConfigPath[] = "ServerConfig.ini";
} // namespace PMSeverConfig

class utils {
  public:
    utils() = delete;
    utils(const std::string& ConfigPath) : m_ConfigPath(ConfigPath), m_ConfigValid(false) {
        if (m_Ini.LoadFile(m_ConfigPath.c_str()) == SI_OK) {
            m_ConfigValid = true;
            printf("utils initialized\n");
        } else {
            printf("cannot read %s file \n", m_ConfigPath.c_str());
        }
    }

    std::string GetValueFromConfigFile(const std::string& key);
    void SetValueFromConfigFile(const std::string& key, const std::string& value);
    virtual ~utils();

  private:
    std::mutex m_mutex;
    std::string m_ConfigPath;
    bool m_ConfigValid;
    CSimpleIniA m_Ini; // SimpleIni object for INI operations
};

#endif /* SRC_PARKMANAGER_UTILS_HPP_ */
