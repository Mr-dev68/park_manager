/*
 * GateRegistration.hpp
 *
 *  Created on: Jul 1, 2022
 *      Author: rohan
 */

#ifndef SRC_PARKMANAGER_JSON_GATEREGISTRATION_HPP_
#define SRC_PARKMANAGER_JSON_GATEREGISTRATION_HPP_

#include "json.hpp"

#include <boost/optional.hpp>
#include <stdexcept>
#include <regex>
#include "QuicktypeCommon.hpp"

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
        const std::string & getGateName() const { return gateName; }
        std::string & getMutableGateName() { return gateName; }
        void setGateName(const std::string & value) { this->gateName = value; }

        const std::string & getGateType() const { return gateType; }
        std::string & getMutableGateType() { return gateType; }
        void setGateType(const std::string & value) { this->gateType = value; }
    };
}

namespace nlohmann {

    void from_json(const json & j, quicktype::GateRegistration & x);
    void to_json(json & j, const quicktype::GateRegistration & x);

    inline void from_json(const json & j, quicktype::GateRegistration& x) {
        x.setGateName(j.at("GateName").get<std::string>());
        x.setGateType(j.at("GateType").get<std::string>());
    }

    inline void to_json(json & j, const quicktype::GateRegistration & x) {
        j = json::object();
        j["GateName"] = x.getGateName();
        j["GateType"] = x.getGateType();
    }
}




#endif /* SRC_PARKMANAGER_JSON_GATEREGISTRATION_HPP_ */
