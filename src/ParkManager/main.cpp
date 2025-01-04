/*
 * main.cpp
 *
 *  Created on: Jul 2, 2022
 *      Author: rohan
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __cplusplus
}
#endif

#include "ParkManagerServer.h"

void PrintHelp(char* prg_name) {
    printf("\nUsage: %s [OPTIONS]\n"
           "\nOptions:\n"
           "\t-u			OCPP1.6 CSMS URL\n"
           "\t-s			indicate if OCPP 1.6 security whitepaper edition 2 supported\n"
           "\t-v			print version informations\n"
           "\t-h			prints this help\n\n",
           prg_name);
}

bool bTerminate = false;

void signalHandler(int sig) {
    printf("get signalHandler...\n");

    bTerminate = true;
}

int main(int argc, char** argv) {
    int c = 0;

    while ((c = getopt(argc, argv, "h")) != -1) {
        switch (c) {
        case 'h':
            PrintHelp(basename(argv[0]));
            return EXIT_SUCCESS;
        case '?':
        default:
            if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return EXIT_FAILURE;
        }
    }
    printf("starting the ParkManagerServer\n");

    std::unique_ptr<ParkManagerServer> PMServer = std::make_unique<ParkManagerServer>();
    PMServer->start();
    // sleep(2);

    while (!bTerminate) {
        sleep(1);
    }
    return 0;
}
