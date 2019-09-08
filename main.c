/*
Authors – Roi Elad 305625402, Nave Assaf 308044809
Project – IOS_ex2
Description – This is the main source file in which the threads are created and the input arguments used to 
				call functions from the other two modules.
*/

//Includes --------------------
#include "ProcessChecker.h"

/*
	Function:		  main

	Inputs:			  int argc = number of command line arguments provided in the call to the IOS_ex2.exe file
					  char *argv[] = pointer to array of char* which are the arguments provided to the executable in the call

	Outputs:		  return value = 0 for successful run and FAILURE_CODE (=-1 and defined in IOHandler.h) for failed execution

	Functionality:    The main function is the main thread of the executable. It ensures the correct number of arguments were provided,
					  calls read_top_file() to parse the command file, then allocates space for arrays of thread handles and thread ids.
					  It then creates thread which run the check_program() function from the ProcessChecker module. Finaly, calling print_summary() 
					  it ensures the output file summarizing all the process checks is printed to the correct location (argv[2]).			  
*/
int main(int argc, char *argv[])
{
	int num_of_rows = 0, i = 0;
	ProgNode *head = NULL, *cur_node = NULL;
	HANDLE* p_thread_handles = NULL;
	DWORD* p_thread_ids = NULL;
	DWORD exitcode;
	unsigned int thread_ind = 0;
	char test[100];
	int failed = 0;


	//Checks that the correct number of arguments was provided. If not, prints so and returns FAILURE_CODE
	if (argc != 3)
	{
		printf("ERROR: Not enough input arguments");
		return FAILURE_CODE;
	}

	//Sets head = head of linked list of ProgNode structs declared in IOHandler.h 
	head = read_top_file(argv[1], &num_of_rows);

	//Ensures read_top_file() created linked list successfully
	if (FAILURE_CODE == head)
		return FAILURE_CODE;

	cur_node = head;
	
	//Allocates mem in heap for an array of handles of threads 
	if (NULL == (p_thread_handles = (HANDLE*)calloc(num_of_rows , sizeof(HANDLE))))
	{
		printf("Memory allocation failed");
		return FAILURE_CODE;
	}

	//Allocates mem in heap for an array of ids of threads 
	if (NULL == (p_thread_ids = (DWORD*)calloc(num_of_rows , sizeof(DWORD))))
	{
		printf("Memory allocation failed");
		return FAILURE_CODE;
	}

	//Begins iterating over linked list, creating a check_program thread for each node in the list (for each line in the cmd_file)
	while (cur_node != NULL)
	{
		p_thread_handles[thread_ind] = CreateThread(NULL, 0, check_program, cur_node, 0, p_thread_ids[thread_ind]);
		cur_node = cur_node->next;
		thread_ind++;
	}

	//Waits until all threads have finished running (non can take more than 10 [s]) and prints summary file once all have terminated
	WaitForMultipleObjects((DWORD)thread_ind, p_thread_handles, TRUE, INFINITE);
	
	//Close all handles
	cur_node = head;
	for (i = 0; i < num_of_rows; i++)
	{
		if (FAILURE_CODE == cur_node->exit_code)
			failed = FAILURE_CODE;
		CloseHandle(p_thread_handles[i]);
		cur_node = cur_node->next;
	}
	
	if (FAILURE_CODE != failed)
		print_summary(head, argv[2]);

	free_list(head);
	free(p_thread_handles);
	free(p_thread_ids);

	return failed;
}