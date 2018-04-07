#include <iostream>
#include <cstdio>
#include <map>
#include "Parse.h"
#include "String.h"
#include "helpers.h"
#include "dataStructures.h"

using namespace std;

void runCommand()
{
	if (String(next_token()) == String("do")) {
		evalDoLoop();
	}
	else if (String(next_token()) == String("if")) {
		evalIfElse();
	}
	else if (String(next_token()) == String("od") ||
			 String(next_token()) == String("fi"))
	{
		// do nothing
	}
	else {
		evalStatement();
	}
}


/**
* Runs the BLIP input file. If successfully evaluated, clears the global map
* 'variabes' and sets a bool 'fileDone' to false for the next BLIP input file.
* Exits program if the BLIP file has invalid structure / input.
*
* param: N/A
* return: N/A
*/
void runBlip()
{
	if (fileDone) {
		fileDone = false;
		variables.clear();														// remove elements from global map container
		return;
	}

	read_next_token();

	if (next_token_type == SYMBOL) {											// reached a comment
		skip_line();
	}

	if (next_token_type == NAME) {
		runCommand();
	}

	else if (next_token_type == END) {
		fileDone = false;
		variables.clear();
		return;
	}

	runBlip();																	// recursively read the BLIP file.
}
