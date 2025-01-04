#include "SCPM.hpp"
#include "Gate.hpp"
#include "ParkManagerServer.h"
#include "utils.hpp"
#include "version.h"
#include <chrono>
#include <condition_variable>
#include <cute/cute.h>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
using namespace std;

#define LOG_TRACE printf
#define LOG_INFO printf
#define LOG_FATAL printf
#define LOG_ERROR printf
/**
 * Prints out help message.
 **/
namespace PMUTS {
std::mutex ConfigLockEnt;
std::mutex ConfigLockExi;
std::vector<std::string> CarsPlateList = {"car1", "car2", "car3", "car4", "car5", "car6"};
std::vector<std::string> CarsPlateList1 = {"car11", "car12", "car13", "car14", "car15", "car16"};
std::vector<std::string> CarsPlateList2 = {"car21", "car22", "car23", "car24", "car25", "car26"};
int ClientConfigEnterenceCount = 0;
int ClientConfigExitCount = 0;

int EnterenceGate(std::vector<std::string> CarsPlateListIn) {
    std::string ConfigName;

    {
        std::unique_lock<std::mutex> lock(ConfigLockEnt);
        ConfigName = "ClientConfigEnterence" + std::to_string(ClientConfigEnterenceCount) + ".ini";
        printf("\033[0;32m  ClientConfigEnterence ConfigName %s \033[0m\n", ConfigName.c_str());
        ClientConfigEnterenceCount++;
    }

    std::unique_ptr<Gate> ParkManagerGateEnterence = std::make_unique<Gate>(ConfigName);
    ParkManagerGateEnterence->GateRegistration();
    sleep(2);
    printf("\033[0;32m registering new cars\033[0m\n");
    for (unsigned int i = 0; i < 5; i++) {

        bool Handicap = false;
        if (i % 4 == 0)
            Handicap = true;

        std::string CarPlate = CarsPlateListIn[i];
        std::string PMComment("");
        int ret = ParkManagerGateEnterence->CarRegistration(CarPlate, Handicap, PMComment);

        printf("\033[0;32mCarRegistration ret %d comment %s \033[0m\n", ret, PMComment.c_str());
        sleep(1);
    }

    return 0;
}

int ExitingGate(std::vector<std::string> CarsPlateListOut) {
    std::string ConfigName;
    printf("\033[0;32m  ExitingGate ConfigName %s \033[0m\n", ConfigName.c_str());
    {
        std::unique_lock<std::mutex> lock(ConfigLockExi);
        ConfigName = "ClientConfigExitCount" + std::to_string(ClientConfigExitCount) + ".ini";
        ClientConfigExitCount++;
    }

    std::unique_ptr<Gate> ParkManagerGateExit = std::make_unique<Gate>(ConfigName);
    ParkManagerGateExit->GateRegistration();
    for (unsigned int i = 0; i < 5; i++) {

        bool Handicap = false;
        if (i % 4 == 0)
            Handicap = true;

        std::string CarPlate = CarsPlateListOut[i];
        UpdateStatus UpdateStatusExit;
        int ret = ParkManagerGateExit->CarStatusUpdate(CarPlate, UpdateStatusExit);

        printf("\033[0;32mCarCarStatusUpdate ret %d  car %s comment %s \033[0m\n", ret, CarPlate.c_str(), UpdateStatusExit.comment.c_str());
        printf("\033[0;32mCarCarStatusUpdate  payable %s \033[0m\n", UpdateStatusExit.payable.c_str());
        printf("\033[0;32mCarCarStatusUpdate Status %s \033[0m\n", UpdateStatusExit.Status.c_str());
        sleep(1);
    }
    return 0;
}

void cleanup() {
    printf("Cleanup::testing Utils\n");
    printf("Reseting Config files\n");
    ClientConfigEnterenceCount = 0;
    ClientConfigExitCount = 0;
    for (unsigned int i = 0; i < 3; i++) {
        printf("\033[0;32m cleaning  %s \033[0m\n", std::string("ClientConfigEnterence" + std::to_string(i) + ".ini").c_str());
        utils util(std::string("ClientConfigEnterence" + std::to_string(i) + ".ini"));
        util.SetValueFromConfigFile(PMClientConfig::RegistratinResult, "-1");
    }
    for (unsigned int i = 0; i < 3; i++) {
        printf("\033[0;32m cleaning  %s \033[0m\n", std::string("ClientConfigExitCount" + std::to_string(i) + ".ini").c_str());
        utils util(std::string("ClientConfigExitCount" + std::to_string(i) + ".ini"));
        util.SetValueFromConfigFile(PMClientConfig::RegistratinResult, "-1");
    }
}

void SingleEnterenceSingleExit() {
    printf("SingleEnterenceSingleExit::starting the ParkManagerServer\n");

    std::unique_ptr<ParkManagerServer> PMServer = std::make_unique<ParkManagerServer>();
    PMServer->start();
    std::future<int> EnterenceGateThread = std::async(std::launch::async, EnterenceGate, CarsPlateList);

    sleep(15);
    printf("activating exit gate \n");
    std::future<int> ExitGateThread = std::async(std::launch::async, ExitingGate, CarsPlateList);

    EnterenceGateThread.get();
    ExitGateThread.get();
    printf("Reseting Config files\n");
    ClientConfigEnterenceCount = 0;
    ClientConfigExitCount = 0;
    utils util0("ClientConfigEnterence0.ini");
    util0.SetValueFromConfigFile(PMClientConfig::RegistratinResult, "-1");
    utils util1("ClientConfigExitCount0.ini");
    util1.SetValueFromConfigFile(PMClientConfig::RegistratinResult, "-1");
    // printf("\033[0;31m 10s break \033[0m\n");
    // sleep(10);
}

void MultiEnterenceMultiExit() {
    printf("MultiEnterenceMultiExit::starting the ParkManagerServer\n");

    std::unique_ptr<ParkManagerServer> PMServer = std::make_unique<ParkManagerServer>();

    PMServer->start();

    std::future<int> EnterenceGateThread0 = std::async(std::launch::async, EnterenceGate, CarsPlateList);
    std::future<int> EnterenceGateThread1 = std::async(std::launch::async, EnterenceGate, CarsPlateList1);
    std::future<int> EnterenceGateThread2 = std::async(std::launch::async, EnterenceGate, CarsPlateList2);

    sleep(30);
    printf("activating exit gate \n");
    std::future<int> ExitGateThread0 = std::async(std::launch::async, ExitingGate, CarsPlateList);
    std::future<int> ExitGateThread1 = std::async(std::launch::async, ExitingGate, CarsPlateList1);
    std::future<int> ExitGateThread2 = std::async(std::launch::async, ExitingGate, CarsPlateList2);

    sleep(50);
    EnterenceGateThread0.get();
    EnterenceGateThread1.get();
    EnterenceGateThread2.get();
    ExitGateThread0.get();
    ExitGateThread1.get();
    ExitGateThread2.get();
    printf("Terminate\n");
}

cute::suite make_suite() {
    cute::suite s;

    s.push_back(CUTE(cleanup));
    s.push_back(CUTE(SingleEnterenceSingleExit));
    s.push_back(CUTE(MultiEnterenceMultiExit));
    return s;
}
} // namespace PMUTS
