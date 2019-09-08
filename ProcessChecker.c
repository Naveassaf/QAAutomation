/*
Authors – Roi Elad 305625402, Nave Assaf 308044809
Project – IOS_ex2
Description – This module contains a single function which, given a node which represents a single line in the command file,
				creates a process which runs the executable provided in the line and checks its outcome.
*/


//Includes -------------------
#include "ProcessChecker.h"

/*
	Function: check_program

	Inputs:			 ProgNode* node = a strcut containing the line in the cmd_file that represents the program we wish to check here

	Outputs:		 return value = 0 if successful, FAILURE_CODE otherwise (returned as DWORD)

	Functionality:    Running simultaneously in several threads (as many as created in the main() function) this function creates a
					  process of the executable provided in the specific line of the cmd_file that *node represents. It then checks 
					  if the process has run and terminatd successfully in less than TIMEOUT_IN_MS and checks if the output is has
					  expected. It sets the exit_code and crash_code of the node according to the exitcode of the process.

*/
DWORD WINAPI check_program(ProgNode* node)
{
	int failed = 0;
	PROCESS_INFORMATION procinfo;
	DWORD				waitcode;
	DWORD				exitcode;
	BOOL				retVal;
	TCHAR				command[MAX_LINE_LEN];				/* <ISP> TCHAR is a win32		*/
															/* generic char which may be either a simple (ANSI) char or a unicode char, */
															/* depending on behind-the-scenes operating system definitions. Type LPTSTR */
															/* is a string of TCHARs. Type LPCTSTR is a const string of TCHARs. */
	STARTUPINFO	startinfo = { sizeof(STARTUPINFO), NULL, 0 };
	char actual_results[MAX_LINE_LEN], expected_results[MAX_LINE_LEN];
	_tcscpy(command, _T(node->cmd_call));

	//Creates a new process with the commandline (including arguments) provided in the cmd_file. Sets retVal equal to ret. val of the CreateProcess() function
	retVal = CreateProcess(NULL, /*  No module name (use command line). */
		command,				/*  Command line. */
		NULL,					/*  Process handle not inheritable. */
		NULL,					/*  Thread handle not inheritable. */
		FALSE,					/*  Set handle inheritance to FALSE. */
		NORMAL_PRIORITY_CLASS,	/*  creation/priority flags. */
		NULL,					/*  Use parent's environment block. */
		NULL,					/*  Use parent's starting directory. */
		&startinfo,				/*  Pointer to STARTUPINFO structure. */
		&procinfo				/*  Pointer to PROCESS_INFORMATION structure. */
	);
	
	//Create process returns zero for failure and non zero for success - checks for success here
	if (retVal == 0)
	{
		printf("Process Creation Failed For: %s", node->cmd_call);
		node->exit_code = FAILURE_CODE;
		return FAILURE_CODE;
	}

	//Checks the return value of the process and if it has returned within 10 [s]. Sets waitcode accordingly.
	waitcode = WaitForSingleObject(procinfo.hProcess, TIMEOUT_IN_MS);	

	//If process ran for more than 10 [s] sets node's exit_code accordingly and returns 0 for successful run of check_program()
	if (waitcode == WAIT_TIMEOUT)
	{
		node->exit_code = EXIT_TIMEOUT;
		return FUNC_SUCCESS;
	}

	//If reached here, the process did not time out. Checks result of process
	GetExitCodeProcess(procinfo.hProcess, &exitcode);

	//If exitcode!= 0, the process crashed. Acts accordigly
	if (0 != exitcode)
	{
		node->exit_code = EXIT_CRASH;
		node->crash_code = exitcode;
	}
	else
	{
		//If here, the process ran successfully without crashing. CHecks if the output is as expected
		get_actual_results(node->cmd_line, actual_results);
		get_expected_results(node->cmd_line, expected_results);
	
		node->exit_code = file_comperator(actual_results, expected_results);
	}
		
	return FUNC_SUCCESS;

}
