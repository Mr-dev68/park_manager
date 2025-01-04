//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     GateRegistrationResponse data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#include <boost/optional.hpp>
#include <stdexcept>
#include <regex>
#include "QuicktypeCommon.hpp"
namespace quicktype {
    using nlohmann::json;

    class GateRegistrationResponse {
        public:
        GateRegistrationResponse() = default;
        virtual ~GateRegistrationResponse() = default;

        private:
        std::string result;

        public:
        const std::string & getResult() const { return result; }
        std::string & getMutableResult() { return result; }
        void setResult(const std::string & value) { this->result = value; }
    };
}

namespace nlohmann {
    void from_json(const json & j, quicktype::GateRegistrationResponse & x);
    void to_json(json & j, const quicktype::GateRegistrationResponse & x);

    inline void from_json(const json & j, quicktype::GateRegistrationResponse& x) {
        x.setResult(j.at("Result").get<std::string>());
    }

    inline void to_json(json & j, const quicktype::GateRegistrationResponse & x) {
        j = json::object();
        j["Result"] = x.getResult();
    }
}
