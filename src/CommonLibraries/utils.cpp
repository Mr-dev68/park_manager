/*
 * utils.cpp
 *
 *  Created on: Jul 1, 2022
 *      Author: rohan
 */

#include "utils.hpp"

std::string utils::GetValueFromConfigFile(const std::string& key) {
    std::unique_lock<std::mutex> guard(m_mutex);
    std::string value;
    if (!m_ConfigValid) {
        printf("Configuration not valid\n");
        return "";
    }

    const char* result = m_Ini.GetValue("", key.c_str(), nullptr); // Get the value for the key
    if (result) {
        value = result;
    } else {
        printf("Key %s not found in config file\n", key.c_str());
    }
    return value;
}

void utils::SetValueFromConfigFile(const std::string& key, const std::string& value) {
    std::unique_lock<std::mutex> guard(m_mutex);
    if (!m_ConfigValid) {
        printf("Configuration not valid\n");
        return;
    }

    m_Ini.SetValue("", key.c_str(), value.c_str()); // Set the value for the key
    if (m_Ini.SaveFile(m_ConfigPath.c_str()) != SI_OK) {
        printf("Failed to save configuration file\n");
    }
}

utils::~utils() {
    // Destructor
}
