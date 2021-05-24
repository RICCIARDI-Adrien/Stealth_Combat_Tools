/** @file Main.c
 * Allow to uncompress or to create an IDP archive file (like SCom.idp).
 * @author Adrien RICCIARDI
 */
#include <direct.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include "IDP_Archive.h"

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

/** Uncompress an IDP archive.
 * @param Pointer_String_Input_File The IDP file to uncompress.
 * @param Pointer_File_Output_Directory The directory to put uncompressed data to.
 * @return 0 if archive was successfully uncompressed,
 * @return -1 if an error occurred.
 */
static int MainUncompress(char *Pointer_String_Input_File, char *Pointer_File_Output_Directory)
{
	TIDPArchiveTag *Pointer_IDP_Tags;
	int Tags_Count, i, j, Return_Value = -1;
	char *Pointer_String_File_Name, String_System_Command[256], String_File_Path[256]; // 256 characters should be enough
	FILE *Pointer_File_Data;
	
	// Try to uncompress the IDP archive
	if (IDPArchiveRead(Pointer_String_Input_File, &Pointer_IDP_Tags, &Tags_Count) != 0)
	{
		printf("Error : failed to uncompress IDP archive.\n");
		return -1;
	}
	
	// Try to create the output directory
	if (mkdir(Pointer_File_Output_Directory) != 0)
	{
		if (errno != EEXIST)
		{
			printf("Error : failed to create output directory (%s).\n", strerror(errno));
			goto Exit;
		}
	}
	
	// Go to output directory to avoid prefixing all paths with the output directory one
	if (chdir(Pointer_File_Output_Directory) != 0)
	{
		printf("Error : failed to change to output directory (%s).\n", strerror(errno));
		goto Exit;
	}
	
	// Create all tag-related files
	for (i = 0; i < Tags_Count; i++)
	{
		printf("Creating tag %d data file (name : '%s', size : %d bytes).\n", i, Pointer_IDP_Tags[i].Pointer_String_Name, Pointer_IDP_Tags[i].Data_Size);
		
		// Extract the file name and the directories path from the tag name
		// Find the file name beginning
		Pointer_String_File_Name = strrchr(Pointer_IDP_Tags[i].Pointer_String_Name, '\\');
		if (Pointer_String_File_Name == NULL)
		{
			printf("Error : could not retrieve the last '\\' in the name string.\n");
			goto Exit;
		}
		Pointer_String_File_Name++; // Bypass the last '\'
		// Extract file path
		j = strlen(Pointer_IDP_Tags[i].Pointer_String_Name) - strlen(Pointer_String_File_Name) - 1; // Recycle j variable
		strncpy(String_File_Path, Pointer_IDP_Tags[i].Pointer_String_Name, j); // Copy up to the character before the '\' (-1 removes the last '\')
		String_File_Path[j] = 0; // Append terminating zero
		
		// Create target directories with no safety but no effort
		sprintf(String_System_Command, "mkdir %s > NUL", String_File_Path);
		system(String_System_Command);
		
		// Create data file
		Pointer_File_Data = fopen(Pointer_IDP_Tags[i].Pointer_String_Name, "wb");
		if (Pointer_File_Data == NULL)
		{
			printf("Error : failed to open tag %d data file (%s).\n", i, strerror(errno));
			goto Exit;
		}
		// Fill data file
		if (fwrite(Pointer_IDP_Tags[i].Pointer_Data, 1, Pointer_IDP_Tags[i].Data_Size, Pointer_File_Data) != Pointer_IDP_Tags[i].Data_Size)
		{
			printf("Error : failed to write tag %d data file (%s).\n", i, strerror(errno));
			goto Exit;
		}
		fclose(Pointer_File_Data);
	}
	
	printf("All files were successfully created.\n");
	Return_Value = 0;
	
Exit:
	IDPArchiveFreeBuffer(Pointer_IDP_Tags, Tags_Count);
	return Return_Value;
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
	else if (strcmp(argv[1], "-u") == 0) return MainUncompress(argv[2], argv[3]);
	else
	{
		printf("Error : unknown command.\n");
		MainDisplayProgramUsage(argv[0]);
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
