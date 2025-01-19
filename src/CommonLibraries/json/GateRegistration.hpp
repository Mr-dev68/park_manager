/*
 * GateRegistration.hpp
 *
 *  Created on: Jul 1, 2022
 *      Author: rohan
 */

#ifndef SRC_PARKMANAGER_JSON_GATEREGISTRATION_HPP_
#define SRC_PARKMANAGER_JSON_GATEREGISTRATION_HPP_

#include "json.hpp"

#include "QuicktypeCommon.hpp"
#include <boost/optional.hpp>
#include <regex>
#include <stdexcept>

namespace quicktype {
using nlohmann::json;

class GateRegistration {
  public:
    GateRegistration() = default;
    virtual ~GateRegistration() = default;

  private:
    std::string gateName;
    std::string gateType;

  public:
    const std::string& getGateName() const { return gateName; }
    std::string& getMutableGateName() { return gateName; }
    void setGateName(const std::string& value) { this->gateName = value; }

    const std::string& getGateType() const { return gateType; }
    std::string& getMutableGateType() { return gateType; }
    void setGateType(const std::string& value) { this->gateType = value; }
};
} // namespace quicktype

namespace nlohmann {

void from_json(const json& j, quicktype::GateRegistration& x);
void to_json(json& j, const quicktype::GateRegistration& x);

inline void from_json(const json& j, quicktype::GateRegistration& x) {
    x.setGateName(j.at("GateName").get<std::string>());
    x.setGateType(j.at("GateType").get<std::string>());
}

inline void to_json(json& j, const quicktype::GateRegistration& x) {
    j = json::object();
    j["GateName"] = x.getGateName();
    j["GateType"] = x.getGateType();
}
} // namespace nlohmann

#endif /* SRC_PARKMANAGER_JSON_GATEREGISTRATION_HPP_ */
