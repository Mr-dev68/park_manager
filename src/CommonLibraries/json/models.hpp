/*
 * models.hpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */

#ifndef SRC_PARKMANAGER_JSON_MODELS_HPP_
#define SRC_PARKMANAGER_JSON_MODELS_HPP_

#include "Base.hpp"
#include "CarEnterence.hpp"
#include "CarEnterenceResponse.hpp"
#include "CarStatusUpdate.hpp"
#include "CarStatusUpdateResponse.hpp"
#include "GateRegistration.hpp"
#include "GateRegistrationResponse.hpp"
namespace models {
static const std::string ActionCarEnterence = "CarEnterence";
static const std::string CarEnterenceResponse = "CarEnterenceResponse";
static const std::string ActionCarStatusUpdate = "CarStatusUpdate";
static const std::string CarStatusUpdateResponse = "CarStatusUpdateResponse";
static const std::string ActionGateRegistration = "GateRegistration";
static const std::string GateRegistrationResponse = "GateRegistrationResponse";
} // namespace models

namespace RegistrationStatus {
static const char Accept[] = "Accept";
static const char Full[] = "Full";
static const char Exist[] = "Exist";
static const char Reject[] = "Reject";
static const char InvalidIdTag[] = "-1";
} // namespace RegistrationStatus
namespace HandicapStatus {
static const char Yes[] = "Yes";
static const char No[] = "No";
} // namespace HandicapStatus

#endif /* SRC_PARKMANAGER_JSON_MODELS_HPP_ */
