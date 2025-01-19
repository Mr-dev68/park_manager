//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     GateRegistrationResponse data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#include "QuicktypeCommon.hpp"
#include <boost/optional.hpp>
#include <regex>
#include <stdexcept>
namespace quicktype {
using nlohmann::json;

class GateRegistrationResponse {
  public:
    GateRegistrationResponse() = default;
    virtual ~GateRegistrationResponse() = default;

  private:
    std::string result;

  public:
    const std::string& getResult() const { return result; }
    std::string& getMutableResult() { return result; }
    void setResult(const std::string& value) { this->result = value; }
};
} // namespace quicktype

namespace nlohmann {
void from_json(const json& j, quicktype::GateRegistrationResponse& x);
void to_json(json& j, const quicktype::GateRegistrationResponse& x);

inline void from_json(const json& j, quicktype::GateRegistrationResponse& x) { x.setResult(j.at("Result").get<std::string>()); }

inline void to_json(json& j, const quicktype::GateRegistrationResponse& x) {
    j = json::object();
    j["Result"] = x.getResult();
}
} // namespace nlohmann
