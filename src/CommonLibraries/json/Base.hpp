/*
 * Base.hpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */
#pragma once
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <time.h>

#include "date.h"
#include "logging.hpp"

enum messageType { CALL = 0, RESPOND = 1 };

inline void handleIncomingMsg(const std::string& JM, std::string& messageId, std::string& JsonString, int mT) {
    std::string incoming = JM;
    std::vector<std::string> requestItems;
    boost::split(requestItems, incoming, boost::is_any_of(","));
    messageId = requestItems[1];
    JsonString.clear();
    int offset(0);
    if (mT == messageType::CALL) {
        offset = 3;
    } else {
        offset = 2;
    }
    for (std::vector<std::string>::iterator itr = requestItems.begin() + offset; itr != requestItems.end(); ++itr) {
        // printf("%s\n",itr->c_str());
        JsonString = JsonString + *itr + ",";
    }
    JsonString = JsonString.substr(0, JsonString.length() - 2);
    //  printf("IncomingMsg %s\n",JsonString.c_str());
}
inline std::string AddBase(const std::string& JM, int mT, const std::string Action, std::string messageId = "") {
    std::ostringstream os;
    if (mT == messageType::CALL) {
        os << "[" << mT << ",\"" << random() << "\",\"" << Action << "\"," << JM << "]";
    } else if (mT == messageType::RESPOND) {

        os << "[" << mT << ",\"" << messageId << "\"," << JM << "]";
    }
    return os.str();
}

inline std::string ConvertSecondsToTime(time_t timeCounter) {
    char buffer[100];

    strftime(buffer, 100, "%FT%TZ", gmtime(&timeCounter));
    std::string ss(buffer);
    return ss;
}
inline time_t GetTimeByString(std::string timeStr) {
    struct tm tm;
    time_t ts = 0;
    // timeStr.append("\n");
    if (0 != strptime(timeStr.c_str(), "%Y-%m-%dT%H:%M:%S%Z", &tm)) {
        std::string offset;
        std::size_t found = timeStr.find("T");
        if (found == std::string::npos) {
            printf("timestamp format error.\n");
        }
        std::string time = timeStr.substr(found + 1);
        found = time.find("+");
        if (found != std::string::npos) {
            offset = time.substr(found);
        } else {
            found = time.find("-");
            if (found != std::string::npos) {
                offset = time.substr(found);
            }
        }

        if (!offset.empty()) {
            std::string ss;
            if (offset.size() == 6) //[-/+]hh:mm
            {
                found = offset.find_last_of(":");
                ss = offset.substr(0, found) + offset.substr(found + 1);
            } else {
                ss = offset;
            }

            int diffHour = atoi(ss.c_str()) / 100;
            //			int diffMinuten = atoi(ss.c_str()) % 100;

            tm.tm_hour = tm.tm_hour + (-1) * diffHour;

            if (tm.tm_hour > 23) {
                tm.tm_hour = tm.tm_hour % 23;
                tm.tm_yday += 1;
            } else if (tm.tm_hour < 0) {
                tm.tm_hour = tm.tm_hour + 24;
                tm.tm_yday -= 1;
            }
        }

        // LOG_TRACE(" GetTimeByStringTest year: %d month:%d day:%d hour:%d minuten:%d second:%d\n",
        //	tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        tm.tm_hour = tm.tm_hour + 2;
        ts = mktime(&tm); // t is now your desired time_t
    }
    return ts;
}

inline std::string GetUTCTime(std::string timeStr) {
    try {
        std::string save(timeStr);
        std::istringstream in{timeStr};
        date::sys_time<std::chrono::milliseconds> tp;
        in >> date::parse("%FT%TZ", tp);
        if (in.fail()) {
            in.clear();
            in.exceptions(std::ios::failbit);
            in.str(save);
            in >> date::parse("%FT%T%Ez", tp);
        }
        return (date::format("%FT%TZ", date::floor<std::chrono::seconds>(tp)));
    } catch (std::exception& e) {
        // LOG_ERROR("GetUTCTime what: %s\n", e.what());
        return "";
    }
}
void inline GetTimeinSeconds(std::string timeUTC, time_t& TT) {

    try {

        time_t Time(0);
        TT = 0;

        if (!timeUTC.empty()) {
            timeUTC = GetUTCTime(timeUTC);
            // printf("GetTime timeUTC : %s \n",timeUTC.c_str());
            Time = GetTimeByString(timeUTC);
            struct tm tm;

            strptime(timeUTC.c_str(), "%Y-%m-%dT%H:%M:%S%Z", &tm);
            // printf("%ld\n ",mktime(&tm));
        }
        TT = Time;
    } catch (std::exception& e) {
        printf(" time format is not correct %s \n", timeUTC.c_str());
        return;
    }
}

void inline GetCurrentUTCTime(std::string& timeUTC) {
    time_t now1 = time(0);
    time_t now;
    char bufNow[42];
    std::strftime(bufNow, 42, "%FT%TZ", std::gmtime(&now1));
    std::string TimeNowStr(bufNow);
    timeUTC = GetUTCTime(TimeNowStr);
}
