/*
 * main.cpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __cplusplus
}
#endif
#include "Gate.hpp"


void PrintHelp(char *prg_name)
{
   printf("\nUsage: %s [OPTIONS]\n"\
          "\nOptions:\n"\
   		  "\t-u			OCPP1.6 CSMS URL\n"\
		  "\t-s			indicate if OCPP 1.6 security whitepaper edition 2 supported\n"\
          "\t-v			print version informations\n"\
          "\t-h			prints this help\n\n", prg_name);
}

bool bTerminate = false;

void signalHandler(int sig)
{
	printf("get signalHandler...\n");

	bTerminate = true;
}

int main(int argc , char** argv )
{
	  int c = 0;

    while ((c = getopt (argc, argv, "h")) != -1)
    {
	switch (c)
	{
	case 'h':
	    PrintHelp(basename(argv[0]));
	    return EXIT_SUCCESS;
	case '?':
	default:
	    if(isprint (optopt))
		fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	    else
		fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
	    return EXIT_FAILURE;
	}
    }


    std::unique_ptr<Gate> ParkManagerGateEnterence = std::make_unique<Gate>("ClientConfig.ini");
    std::unique_ptr<Gate> ParkManagerGateExit= std::make_unique<Gate>("ClientConfig1.ini");

    ParkManagerGateEnterence->GateRegistration();
    sleep(2);
    ParkManagerGateExit->GateRegistration();


    printf("\033[0;32m registering new cars\033[0m\n");
    for(unsigned int i= 0 ; i<5 ; i++)
    {

    	bool Handicap = false;
    	if(i%4 == 0)
    		Handicap = true;

    	std::string  CarPlate = ParkManagerGateEnterence->GetGateName() +std::to_string(i);
    	std::string  PMComment("");
    	  int ret = ParkManagerGateEnterence->CarRegistration(CarPlate,Handicap,PMComment);

    	  printf("\033[0;32mCarRegistration ret %d comment %s \033[0m\n",ret,PMComment.c_str());
    	  sleep(2);
    }






    for(unsigned int i= 0 ; i<5 ; i++)
    {

    	bool Handicap = false;
    	if(i%4 == 0)
    		Handicap = true;

    	std::string  CarPlate = ParkManagerGateEnterence->GetGateName()+std::to_string(i);
    	UpdateStatus  UpdateStatusExit;
    	int ret = ParkManagerGateExit->CarStatusUpdate(CarPlate, UpdateStatusExit );


    	printf("\033[0;32mCarCarStatusUpdate ret %d comment %s \033[0m\n",ret,UpdateStatusExit.comment.c_str());
    	printf("\033[0;32mCarCarStatusUpdate  payable %s \033[0m\n",UpdateStatusExit.payable.c_str());
    	printf("\033[0;32mCarCarStatusUpdate Status %s \033[0m\n",UpdateStatusExit.Status.c_str());
    	sleep(2);
    }










}
