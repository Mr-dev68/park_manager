
#pragma once
#include <bitset>
#include <iostream>
#include <sstream>
struct WSMessage {
	std::string uri;
	std::string type;
	std::string payload;
};


namespace ParkManagment
{
enum GateType
{
	NotSet	= -1,
	Enterence = 0,
	Exiting = 1
};

}
namespace ReturnValue
{
static const int SUCCESS = 0;
static const int FAILED = -1;
}

struct UpdateStatus
{
	std::string comment;
	std::string payable;
	std::string Status;
};
