/*
 * CarStatusUpdateResponse.hpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */

#ifndef SRC_PARKMANAGER_JSON_CARSTATUSUPDATERESPONSE_HPP_
#define SRC_PARKMANAGER_JSON_CARSTATUSUPDATERESPONSE_HPP_

#include "json.hpp"

#include "QuicktypeCommon.hpp"
#include <boost/optional.hpp>
#include <regex>
#include <stdexcept>
namespace quicktype {
using nlohmann::json;

class CarStatusUpdateResponse {
  public:
    CarStatusUpdateResponse() = default;
    virtual ~CarStatusUpdateResponse() = default;

  private:
    std::string status;
    std::string payable;
    std::string curreny;
    std::string comment;

  public:
    const std::string& getStatus() const { return status; }
    std::string& getMutableStatus() { return status; }
    void setStatus(const std::string& value) { this->status = value; }

    const std::string& getPayable() const { return payable; }
    std::string& getMutablePayable() { return payable; }
    void setPayable(const std::string& value) { this->payable = value; }

    const std::string& getCurreny() const { return curreny; }
    std::string& getMutableCurreny() { return curreny; }
    void setCurreny(const std::string& value) { this->curreny = value; }

    const std::string& getComment() const { return comment; }
    std::string& getMutableComment() { return comment; }
    void setComment(const std::string& value) { this->comment = value; }
};
} // namespace quicktype

namespace nlohmann {
void from_json(const json& j, quicktype::CarStatusUpdateResponse& x);
void to_json(json& j, const quicktype::CarStatusUpdateResponse& x);

inline void from_json(const json& j, quicktype::CarStatusUpdateResponse& x) {
    x.setStatus(j.at("status").get<std::string>());
    x.setPayable(j.at("payable").get<std::string>());
    x.setCurreny(j.at("curreny").get<std::string>());
    x.setComment(j.at("comment").get<std::string>());
}

inline void to_json(json& j, const quicktype::CarStatusUpdateResponse& x) {
    j = json::object();
    j["status"] = x.getStatus();
    j["payable"] = x.getPayable();
    j["curreny"] = x.getCurreny();
    j["comment"] = x.getComment();
}
} // namespace nlohmann

#endif /* SRC_PARKMANAGER_JSON_CARSTATUSUPDATERESPONSE_HPP_ */
