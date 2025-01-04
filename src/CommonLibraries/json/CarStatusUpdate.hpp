//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     CarStatusUpdate data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#include <boost/optional.hpp>
#include <stdexcept>
#include <regex>
#include "QuicktypeCommon.hpp"
namespace quicktype {
    using nlohmann::json;


    class CarStatusUpdate {
        public:
        CarStatusUpdate() = default;
        virtual ~CarStatusUpdate() = default;

        private:
        std::string exitingGate;
        std::string carPlate;

        public:


        const std::string & getExitingGate() const { return exitingGate; }
        std::string & getMutableExitingGate() { return exitingGate; }
        void setExitingGate(const std::string & value) { this->exitingGate = value; }

        const std::string & getCarPlate() const { return carPlate; }
        std::string & getMutableCarPlate() { return carPlate; }
        void setCarPlate(const std::string & value) { this->carPlate = value; }
    };
}

namespace nlohmann {
    void from_json(const json & j, quicktype::CarStatusUpdate & x);
    void to_json(json & j, const quicktype::CarStatusUpdate & x);

    inline void from_json(const json & j, quicktype::CarStatusUpdate& x) {
        x.setExitingGate(j.at("exitingGate").get<std::string>());
        x.setCarPlate(j.at("CarPlate").get<std::string>());
    }

    inline void to_json(json & j, const quicktype::CarStatusUpdate & x) {
        j = json::object();

        j["exitingGate"] = x.getExitingGate();
        j["CarPlate"] = x.getCarPlate();
    }
}
