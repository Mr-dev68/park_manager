//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     CarEnterence data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#include "QuicktypeCommon.hpp"
#include <boost/optional.hpp>
#include <regex>
#include <stdexcept>
namespace quicktype {
using nlohmann::json;

class CarEnterence {
  public:
    CarEnterence() = default;
    virtual ~CarEnterence() = default;

  private:
    std::string carPlate;
    std::string handicap;
    std::string enteranceTime;
    std::string enterenceGate;

  public:
    const std::string& getCarPlate() const { return carPlate; }
    std::string& getMutableCarPlate() { return carPlate; }
    void setCarPlate(const std::string& value) { this->carPlate = value; }

    const std::string& getHandicap() const { return handicap; }
    std::string& getMutableHandicap() { return handicap; }
    void setHandicap(const std::string& value) { this->handicap = value; }

    const std::string& getEnteranceTime() const { return enteranceTime; }
    std::string& getMutableEnteranceTime() { return enteranceTime; }
    void setEnteranceTime(const std::string& value) { this->enteranceTime = value; }

    const std::string& getEnterenceGate() const { return enterenceGate; }
    std::string& getMutableEnterenceGate() { return enterenceGate; }
    void setEnterenceGate(const std::string& value) { this->enterenceGate = value; }
};
} // namespace quicktype

namespace nlohmann {
void from_json(const json& j, quicktype::CarEnterence& x);
void to_json(json& j, const quicktype::CarEnterence& x);

inline void from_json(const json& j, quicktype::CarEnterence& x) {
    x.setCarPlate(j.at("CarPlate").get<std::string>());
    x.setHandicap(j.at("handicap").get<std::string>());
    x.setEnteranceTime(j.at("enteranceTime").get<std::string>());
    x.setEnterenceGate(j.at("enterenceGate").get<std::string>());
}

inline void to_json(json& j, const quicktype::CarEnterence& x) {
    j = json::object();
    j["CarPlate"] = x.getCarPlate();
    j["handicap"] = x.getHandicap();
    j["enteranceTime"] = x.getEnteranceTime();
    j["enterenceGate"] = x.getEnterenceGate();
}
} // namespace nlohmann
