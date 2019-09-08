/*
Authors – Roi Elad 305625402, Nave Assaf 308044809
Project – IOS_ex2
Description – THe header of the module which contains a single function which, given a node which represents a single line in the command file,
creates a process which runs the executable provided in the line and checks its outcome.
*/


//Includes -------------------
#include "IOHandler.h"
#include <tchar.h> /* for TCHAR, _T() */

//Constants
#define TIMEOUT_IN_MS 10000

//Function declarations (description of functionality found in source file)
DWORD WINAPI check_program(ProgNode* node);

