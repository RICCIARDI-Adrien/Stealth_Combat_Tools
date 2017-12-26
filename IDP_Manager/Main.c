/** @file Main.c
 * Allow to uncompress or to create an IDP archive file (like SCom.idp).
 * @author Adrien RICCIARDI
 * TODO : add IDP file structure
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Display an help message telling how to use the program.
 * @param Pointer_String_Program_Name The program executable current name.
 */
static void MainDisplayProgramUsage(char *Pointer_String_Program_Name)
{
	printf("Usage : %s Command Input_Path Output_Path.\n"
		"Command :\n"
		"  -c : compress a directory into an IDP file. Input_Path is the path to the directory to compress. Output_Path is the path of the IDP file to create.\n"
		"  -u : uncompress an existing IDP file. Input_Path is the path of the IDP file to uncompress. Output_Path is a directory path where data will be extracted.\n", Pointer_String_Program_Name);
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	// Check parameters
	if (argc != 4)
	{
		MainDisplayProgramUsage(argv[0]);
		return EXIT_FAILURE;
	}
	
	// Handle command
	if (strcmp(argv[1], "-c") == 0) printf("Compress command is not available yet.\n");
	//else if (strcmp(argv[1], "-u") == 0) return MainUncompress(argv[2], argv[3]);
	else
	{
		printf("Error : unknown command.\n");
		MainDisplayProgramUsage(argv[0]);
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
