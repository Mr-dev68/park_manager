//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     CarEnterenceResponse data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#include <boost/optional.hpp>
#include <stdexcept>
#include <regex>
#include "QuicktypeCommon.hpp"
namespace quicktype {
    using nlohmann::json;

    class CarEnterenceResponse {
        public:
    	CarEnterenceResponse() = default;
        virtual ~CarEnterenceResponse() = default;

        private:
        std::string result;
        std::string comment;

        public:


        const std::string & getResult() const { return result; }
        std::string & getMutableResult() { return result; }
        void setResult(const std::string & value) { this->result = value; }

        const std::string & getComment() const { return comment; }
        std::string & getMutableComment() { return comment; }
        void setComment(const std::string & value) { this->comment = value; }
    };
}

namespace nlohmann {
    void from_json(const json & j, quicktype::CarEnterenceResponse & x);
    void to_json(json & j, const quicktype::CarEnterenceResponse & x);

    inline void from_json(const json & j, quicktype::CarEnterenceResponse& x) {
        x.setResult(j.at("result").get<std::string>());
        x.setComment(j.at("comment").get<std::string>());
    }

    inline void to_json(json & j, const quicktype::CarEnterenceResponse & x) {
        j = json::object();
        j["result"] = x.getResult();
        j["comment"] = x.getComment();
    }
}
