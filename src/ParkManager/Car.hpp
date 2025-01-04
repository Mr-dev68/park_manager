/*
 * Car.hpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */

#ifndef SRC_PARKMANAGER_CAR_HPP_
#define SRC_PARKMANAGER_CAR_HPP_
#include <iostream>
#include "common.h"

class Car {
public:

	Car() = delete;
	Car(unsigned int IdTag,std::string Plate,std::string EnterenceTime,std::string  EnterenceGate,std::string  HandicapStatus):m_IdTag(IdTag),m_Plate(Plate)
	,m_EnterenceTime(EnterenceTime),m_ExitingGate(""),m_EnterenceGate(EnterenceGate),m_payment(0),m_Handicap(HandicapStatus),m_slot(-1)
	{

	}
	unsigned int GetIdTag(){ return m_IdTag;};
	std::string	 GatPlate(){return m_Plate;};
	std::string	 GetEnterenceTime(){return m_EnterenceTime;};
	unsigned int GetPaymet(){ return m_payment;};
	std::string	 GateEnterenceGate(){return m_EnterenceGate;};
	std::string	 GateExitingGate(){return m_ExitingGate;};
	std::string	 GateHandicap(){return m_Handicap;};
	int Getslot(){ return m_slot;};

	void 	     SetIdTag(unsigned int IdTag ){m_IdTag = IdTag;};
	void 	     SetPlate(std::string  Plate ){m_Plate = Plate;};
	void 	     SetEnterenceTime(std::string  EnterenceTime ){m_EnterenceTime = EnterenceTime;};
	void		 SetPayment(unsigned int payment )	{m_payment = payment;};
	void 	     SetEnterenceGate(std::string  EnterenceGate ){m_EnterenceGate = EnterenceGate;};
	void 	     SetExitingGate(std::string  ExitingGate ){m_ExitingGate = ExitingGate;};
	void 	     SetHandicap(std::string  Handicap ){m_Handicap = Handicap;};
	void 	     Setslot(int  slot ){m_slot = slot;};
	virtual ~Car()
	{
		;
	}
private:
	unsigned int m_IdTag;
	std::string m_Plate;
	std::string m_EnterenceTime;
	unsigned int m_payment;
	std::string  m_EnterenceGate;
	std::string  m_ExitingGate;
	std::string m_Handicap;
	int 	m_slot;
};

#endif /* SRC_PARKMANAGER_CAR_HPP_ */
