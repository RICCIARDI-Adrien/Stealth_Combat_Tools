/** @file Main.c
 * Propose a collection of tools to mod Stealth Combat - Utimate War.
 * @author Adrien RICCIARDI
 */
#include <direct.h>
#include <IDP_Archive.h>
#include <Map.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** The command string to build an IDP file. */
#define MAIN_COMMAND_STRING_IDP_BUILD "-idp-build"
/** The command string to extract an IDP file content. */
#define MAIN_COMMAND_STRING_IDP_EXTRACT "-idp-extract"
/** The command string to extract a map file content. */
#define MAIN_COMMAND_STRING_MAP_EXTRACT "-map-extract"

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Display an help message telling how to use the program.
 * @param Pointer_String_Program_Name The program executable current name.
 */
static void MainDisplayProgramUsage(char *Pointer_String_Program_Name)
{
	printf("Usage : %s Command [Arguments]\n"
		"Command :\n"
		"  " MAIN_COMMAND_STRING_IDP_BUILD " Input_Directory Output_IDP_File (not implemented) : generate an IDP file from a directory. Input_Directory is the path of the source directory. Output_IDP_File is the path of the IDP file to create.\n"
		"  " MAIN_COMMAND_STRING_IDP_EXTRACT " Input_IDP_File Output_Directory : extract the content from an existing IDP file (like SCom.idp). Input_IDP_File is the path of the IDP file to extract. Output_Directory is a directory path where the data will be extracted.\n"
		"  " MAIN_COMMAND_STRING_MAP_EXTRACT " Input_Map_File Output_Directory : extract as much content as possible from an existing map file. Input_Map_File is the path of the map file to extract. Output_Directory is a directory path where the data will be extracted.\n"
		"\n"
		"Notes :\n"
		"  * The map files are stored in the SCom.idp archive, so it needs to be extracted first.\n",
		Pointer_String_Program_Name);
}

/** Extract the content of an IDP archive.
 * @param Pointer_String_Input_File The IDP file to extract.
 * @param Pointer_File_Output_Directory The directory to put the extracted data to.
 * @return -1 if an error occurred,
 * @return 0 if the archive was successfully extracted. 
 */
static int MainIDPExtract(char *Pointer_String_Input_File, char *Pointer_File_Output_Directory)
{
	TIDPArchiveTag *Pointer_IDP_Tags;
	int Tags_Count, i, Return_Value = -1;
	char *Pointer_String_File_Name, String_System_Command[256], String_File_Path[256]; // 256 characters should be enough
	FILE *Pointer_File_Data;
	size_t Length;
	
	// Try to uncompress the IDP archive
	if (IDPArchiveRead(Pointer_String_Input_File, &Pointer_IDP_Tags, &Tags_Count) != 0)
	{
		printf("Error : failed to uncompress IDP archive.\n");
		return -1;
	}
	
	// Try to create the output directory
	if (_mkdir(Pointer_File_Output_Directory) != 0)
	{
		if (errno != EEXIST)
		{
			printf("Error : failed to create output directory (%s).\n", strerror(errno));
			goto Exit;
		}
	}
	
	// Go to output directory to avoid prefixing all paths with the output directory one
	if (_chdir(Pointer_File_Output_Directory) != 0)
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
		Length = strlen(Pointer_IDP_Tags[i].Pointer_String_Name) - strlen(Pointer_String_File_Name) - 1;
		strncpy(String_File_Path, Pointer_IDP_Tags[i].Pointer_String_Name, Length); // Copy up to the character before the '\' (-1 removes the last '\')
		String_File_Path[Length] = 0; // Append terminating zero
		
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

/** Extract as much content as possible from a map file.
 * @param Pointer_String_Input_File The map file to extract.
 * @param Pointer_File_Output_Directory The directory to put the extracted data to.
 * @return -1 if an error occurred,
 * @return 0 if the map was successfully extracted.
 */
static int MainMapExtract(char* Pointer_String_Input_File, char* Pointer_File_Output_Directory)
{
	// Try to create the output directory
	if (_mkdir(Pointer_File_Output_Directory) != 0)
	{
		if (errno != EEXIST)
		{
			printf("Error : failed to create the output directory (%s).\n", strerror(errno));
			return -1;
		}
	}

	// Retrieve the map content
	return MapExtract(Pointer_String_Input_File, Pointer_File_Output_Directory);
}

//-------------------------------------------------------------------------------------------------
// Entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int Return_Value = EXIT_FAILURE;

	// Check parameters
	if (argc < 2)
	{
		MainDisplayProgramUsage(argv[0]);
		return EXIT_FAILURE;
	}
	
	// Handle command
	if (strcmp(argv[1], MAIN_COMMAND_STRING_IDP_BUILD) == 0)
	{
		if (argc == 4) printf("the IDP build command is not yet available.\n");
		else MainDisplayProgramUsage(argv[0]);
	}
	else if (strcmp(argv[1], MAIN_COMMAND_STRING_IDP_EXTRACT) == 0)
	{
		if (argc == 4) Return_Value = MainIDPExtract(argv[2], argv[3]);
		else MainDisplayProgramUsage(argv[0]);
	}
	else if (strcmp(argv[1], MAIN_COMMAND_STRING_MAP_EXTRACT) == 0)
	{
		if (argc == 4) Return_Value = MainMapExtract(argv[2], argv[3]);
		else MainDisplayProgramUsage(argv[0]);
	}
	else
	{
		printf("Error : unknown command.\n");
		MainDisplayProgramUsage(argv[0]);
		return EXIT_FAILURE;
	}
	
	return Return_Value;
}
