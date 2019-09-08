/*
Authors – Roi Elad 305625402, Nave Assaf 308044809
Project – IOS_ex2
Description – This module contains all the functions used to parse, write, and compare files and lines from files provided by the user. 
				It also contains a function which parses the cmd_file into a linked list to be used throughout the project (not global).
*/

//Includes ----------------
#include "IOHandler.h"
	
/*
	Function:	read_top_file

	Inputs : file_path - pointer to tests file.
	row_count - nubmer of rows in tests file.

	Outputs : tests' linked list, and updated row_count (through pointer)

	Functionality : the function recieves a file that has list of tests we should check and create a linked list of tests when each node in list is
	from type ProgNode. if the memory allocation fails, the function prints an error to the screen and return FAILURE_CODE.

*/
ProgNode* read_top_file(char* file_path, int* row_count)
{
	ProgNode *head_node = NULL, *cur_node = NULL, *prev_node = NULL;
	errno_t retval = NULL;
	FILE *fp = NULL;
	char* cur_line[MAX_LINE_LEN];
	char cmd_call[MAX_LINE_LEN];
	int first = 1;

	//checks succeful opening of file containg all command line calls. If unsuccesful, prints so and return FAILURE_CODE
	retval = fopen_s(&fp, file_path, "r");
	if (0 != retval)
	{
		printf("Failed to open file %s",file_path);
		return FAILURE_CODE;
	}
	
	//Iterate of file's lines, creating a ProgNode (as declared in the header file) for each line of the cmd_file
	while (!feof(fp))
	{	
		//if reached end of file, break from parsing
		if (NULL == fgets(cur_line, MAX_LINE_LEN, fp))
			break;

		//Allocated memory for current node in linked list
		if (NULL == (cur_node = (ProgNode*)malloc(sizeof(ProgNode))))
		{
			printf("Memory allocation failed");
			fclose(fp);
			return FAILURE_CODE;
		}

		//sets values of current node's fields based on the line of the cmd_file this node represents
		strncpy(cur_node->cmd_line, cur_line, MAX_LINE_LEN);
		get_cmd_call(cur_node->cmd_line, cmd_call);
		strncpy(cur_node->cmd_call, cmd_call, MAX_LINE_LEN);

		//sets the head of the linked list to be the first node created
		if (first == 1)
		{
			head_node = cur_node;
			first = 0;
		}
		else
			prev_node->next = cur_node;

		//sets next field of the node properly
		cur_node->next = NULL;
		prev_node = cur_node;

		//increments the number of lines in the file - to be used in main to allocated room for thread handles and ids
		(*row_count)++;
	}

	fclose(fp);
	return head_node;
}

/*
	Function:	file_comperator

	Inputs:		path_created - pointer to the file that the program being check was suppose to create.
				path_b - pointer to the file of expected results.

	Outputs:	int SUCCEEDED/FAILED result of comparison between these two files.

	Functionality: The function recieves two paths to files, opens them and compare their contents. If the created file does not 
					exist, the program being checked FAILED. Meanwhile, if the checking file does not exist, this is the user's 
					error and thus an error will be printed and no summary file created. 

*/
int file_comperator(char* path_created, char* path_expected)
{
	FILE *fp_a = NULL, *fp_b = NULL;				// file pointer.
	char c1 = NULL, c2 = NULL;						// stores each char from relevant file for text comparison.

	//No expected results file exists, thus crash of the checking program
	if (NULL == (fp_a = fopen(path_expected, "r")))
	{
		printf("Error opening file: %s", path_expected);					
		return FAILURE_CODE;													
	}

	//No file being created is equivalent to the program failing due to wrong output
	if (NULL == (fp_b = fopen(path_created, "r")))
	{
		return EXIT_FAILED;
	}



	//Iterated over all characters in files and checks one-by-one if they are equal. Once one is not, returns that the process failed (output isnt as expected)
	while (!feof(fp_a) && !feof(fp_b) && c1 != '\n' && c2 != '\n')
	{
		c1 = getc(fp_a);
		c2 = getc(fp_b);
		if (c1 != c2 && c1 != '\n' && c2 != '\n' && !feof(fp_a) && !feof(fp_b))
		{
			fclose(fp_a);
			fclose(fp_b);
			return EXIT_FAILED;
		}
	}
	if ((feof(fp_a) && !feof(fp_b)) || (!feof(fp_a) && feof(fp_b)))		// if files has different lengths, ofcourse they are different.
	{
		fclose(fp_a);
		fclose(fp_b);
		return EXIT_FAILED;
	}

	fclose(fp_a);
	fclose(fp_b);

	//If reached here, the files are identical, so the program ran successfully as expected
	return EXIT_SUCCEEDED;

}

/*
	Function:	print_summary

	Inputs:		head - pointer to tests linked list's head.
				path - pointer to the output file we are going to write to.

	Outputs:	No return value. The function creates a summary file that continas inforamtion about tests status according to given format.

	Functionality: the function recieves tests linked list, and creates a file with tests status according to given foramt (result of each test:
				   SUCCEEDED/FAILED/CRASHED/TIMED_OUT and return value when CRASHED only.
				   if there's an error opening the file, we print a message to the screen and return FAILURE_CODE.
*/
void print_summary(ProgNode* head, char* path)
{
	FILE *file_ptr = NULL;
	ProgNode* curr = NULL;
	int unsigned i = 1;
	char status[MAX_STATUS_LEN];

	//Checks that file was opened successfully
	if ((file_ptr = fopen(path, "w")) == NULL)
	{
		printf("Error! opening summary file");						
		return FAILURE_CODE;												
	}

	//Iterated over linked list (knowing all processes have stopped running) and prints output based on error_code and crash_code of every node.
	curr = head;
	while (curr != NULL)
	{
		switch (curr->exit_code)
		{
			case EXIT_CRASH:
				strncpy(status, "Crashed", MAX_STATUS_LEN); break;
			case EXIT_TIMEOUT:
				strncpy(status, "Timed Out", MAX_STATUS_LEN); break;
			case EXIT_SUCCEEDED:
				strncpy(status, "Succeeded", MAX_STATUS_LEN); break;
			case EXIT_FAILED:
				strncpy(status, "Failed", MAX_STATUS_LEN); break;
		}

		//Diffrent printing format for crash vs other 3 exit_codes
		if(curr->exit_code == EXIT_CRASH)
			fprintf(file_ptr, "test #%d: %s %d\n", i, status, curr->crash_code);
		else
			fprintf(file_ptr, "test #%d: %s\n", i,status);
		
		//Go to next node
		curr = curr->next;
		i++;
	}

	//close summary file
	fclose(file_ptr);
	return;
}

/*
	Function:	get_actual_results

	Inputs:		cmd_line - string of the command line that has been taken from the input file.
				result_path - char pointer (path to file).

	Outputs:	return 0 if successful. The function changes the result_path pointer.

	Functionality: the function takes the command line and helps us to get the path to the file where the actual result will be written to
	(file with same name but with .txt extension instead of .exe)
*/
int get_actual_results(char* cmd_line, char* result_path)
{
	char letter = 0;
	int i = 0;

	while (1)
	{
		letter = cmd_line[i];

		if (letter == ' ' || letter == '\0')
		{
			result_path[i] = '\0';
			result_path[i - 3] = 't';
			result_path[i - 2] = 'x';
			result_path[i - 1] = 't';
			break;
		}
		else
		{
			result_path[i] = letter;
			i++;
		}
	}


	return FUNC_SUCCESS;
}

/*
	Function:	get_expected_results

	Inputs:		cmd_line - string of the command line that has been taken from the input file.
	result_path - pointer (path to file).

	Outputs:	return 0. The function changes the result_path pointer.

	Functionality: the function takes the command line and helps us to get the path to the file where the expected result will be written to.
*/
int get_expected_results(char* cmd_line, char* result_path)
{
	char letter = 0;
	int i =0, j = 0, last_space = 0;


	i = get_last_space(cmd_line) + 1;

	while (1)
	{
		letter = cmd_line[i];
		result_path[j] = letter;


		if (letter == '\0'|| letter == '\n')
		{
			result_path[j] = '\0';
			break;
		}
		else
		{
			result_path[i] = letter;
			i++;
			j++;
		}
	}

	return FUNC_SUCCESS;
}

/*
	Function:	get_cmd_call

	Inputs:		cmd_line - string of the command line that has been taken from the input file.
	cmd_call - pointer that we'll change during the function to be the command line without the expected file (only exe file+arguments).

	Outputs:	return 0 if successful.

	Functionality:  The function changes the cmd_call to be the line from tests file without the expected file (i.e test1.exe <arg1> ).
*/
int get_cmd_call(char* cmd_line, char* cmd_call)
{
	char letter = 0;
	int i = 0, j = 0, last_space = 0;


	i = get_last_space(cmd_line);

	for (j = 0; j < i; j++)
	{
		cmd_call[j] = cmd_line[j];
	}
	cmd_call[j] = '\0';

	return FUNC_SUCCESS;
}

/*
	Function:	get_last_space

	Input:		cmd_line - string of the command line that has been taken from the input file.

	Outputs:	last space index in order to seperate the string to input+arguments and output.

	Functionality:  the function return the index of the lask 'space' in recieved string in order to help us seperate the string.
*/
int get_last_space(char* cmd_line)
{
	char letter = 0;
	int i = 0, last_space = 0;

	i = strlen(cmd_line) - 1;

	while (1)
	{
		letter = cmd_line[i];

		if (letter == ' ' || i == 0)
		{
			last_space = i;
			break;
		}
		else
			i--;
	}

	return last_space;

}

/*
Function:	free_list

Input:		head = head of linked list to be freed

Outputs:	0 if successful

Functionality:  Function releases all dynamic mempry allocated for linked list of ProgNodes.
*/
int free_list(ProgNode* head)
{
	ProgNode *cur = NULL;
	
	while (NULL != (cur = head))
	{
		head = head->next;
		free(cur);
	}
	return FUNC_SUCCESS;
}