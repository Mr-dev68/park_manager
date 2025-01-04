/*
 * TestUnit.cpp
 *
 *  Created on: Jul 4, 2022
 *      Author: rohan
 */


#include <cute/ide_listener.h>
#include <cute/xml_listener.h>
#include <cute/cute_runner.h>
#include "SCPM.hpp"

int main(int argc, char const *argv[])
{
	cute::xml_file_opener xmlfile(argc, argv);
	cute::xml_listener<cute::ide_listener<> > lis(xmlfile.out);

	cute::makeRunner(lis, argc, argv)(PMUTS::make_suite(),"PMUTS");
    return 0;
}
