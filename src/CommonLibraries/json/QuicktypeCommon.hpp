/*
 * QuicktypeCommon.hpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */

#ifndef SRC_PARKMANAGER_JSON_QUICKTYPECOMMON_HPP_
#define SRC_PARKMANAGER_JSON_QUICKTYPECOMMON_HPP_
#include "json.hpp"

#include <boost/optional.hpp>
#include <stdexcept>
#include <regex>

namespace quicktype {
    using nlohmann::json;

    inline json get_untyped(const json & j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }
}


#endif /* SRC_PARKMANAGER_JSON_QUICKTYPECOMMON_HPP_ */
