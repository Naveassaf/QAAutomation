/*
Authors – Roi Elad 305625402, Nave Assaf 308044809
Project – IOS_ex2
Description – The header of the module which contains all the functions used to parse, write, and compare files and lines from files provided by the user.
It also contains a function which parses the cmd_file into a linked list to be used throughout the project (not global).
*/


//Includes  ------------------------------
#include <stdio.h>
#include <windows.h>
#pragma warning (disable : 4996)

//Constants ------------------------------
#define FAILURE_CODE -1
#define MAX_LINE_LEN 100
#define EXIT_FAILED 3000
#define EXIT_SUCCEEDED 4000
#define EXIT_TIMEOUT 1000
#define EXIT_CRASH 2000
#define MAX_STATUS_LEN 20
#define FUNC_SUCCESS 0
#define FUNC_FAIL -1

//Function declarations (description of functionality found in source file)
typedef struct _ProgNode {
	char cmd_line[MAX_LINE_LEN];
	char cmd_call[MAX_LINE_LEN];
	int exit_code;
	int crash_code;
	struct _ProgNode *next;
}ProgNode;

ProgNode* read_top_file(char* file_path, int* row_count);

int file_comperator(char* path_a, char* path_b);

void print_summary(ProgNode* head, char* path);

int get_actual_results(char* cmd_line, char* result_path);

int get_expected_results(char* cmd_line, char* result_path);

int get_cmd_call(char* cmd_line, char* cmd_call);

int get_last_space(char* cmd_line);

int free_list(ProgNode* head);
