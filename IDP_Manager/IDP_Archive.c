/** @file IDP_Archive.c
 * @see IDP_Archive.h for description.
 * @author Adrien RICCIARDI
 */
#include <IDP_Archive.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
int IDPArchiveRead(char *Pointer_String_IDP_File, TIDPArchiveTag **Pointer_Pointer_Output_Buffer, int *Pointer_Output_Buffer_Size)
{
	int Return_Value = -1, Tags_Count, i, Temporary_Double_Word;
	FILE *Pointer_File_Archive;
	char String_Temporary[16];
	TIDPArchiveTag *Pointer_Output_Buffer;
	
	printf("Starting uncompressing '%s' archive.\n", Pointer_String_IDP_File);
	
	// Try to open the IDP file
	Pointer_File_Archive = fopen(Pointer_String_IDP_File, "rb");
	if (Pointer_File_Archive == NULL)
	{
		printf("Error : failed to open IDP file '%s' (%s).\n", Pointer_String_IDP_File, strerror(errno));
		goto Exit;
	}
	
	// Check IDP header
	if (fread(String_Temporary, 1, 4, Pointer_File_Archive) != 4)
	{
		printf("Error : failed to read IDP header (%s).\n", strerror(errno));
		goto Exit;
	}
	if (strncmp(String_Temporary, "IDPK", 4) != 0)
	{
		printf("Error : invalid IDP header. IDP file must start with \"IDPK\" header identifier.\n");
		goto Exit;
	}
	printf("Found valid IDP header.\n");
	
	// Check version
	if (fread(&Tags_Count, 1, 4, Pointer_File_Archive) != 4) // Recycle Tags_Count variable
	{
		printf("Error : failed to read IDP version (%s).\n", strerror(errno));
		goto Exit;
	}
	if (Tags_Count != IDP_ARCHIVE_HEADER_VERSION)
	{
		printf("Error : bad archive version (read 0x%X, must be 0x%X).\n", Tags_Count, IDP_ARCHIVE_HEADER_VERSION);
		goto Exit;
	}
	printf("Found valid version.\n");
	
	// Read tags count
	if (fread(&Tags_Count, 1, 4, Pointer_File_Archive) != 4)
	{
		printf("Error : failed to read tags count (%s).\n", strerror(errno));
		goto Exit;
	}
	printf("Found %d tags.\n", Tags_Count);
	
	// Allocate the output buffer
	Pointer_Output_Buffer = malloc(sizeof(TIDPArchiveTag) * Tags_Count);
	if (Pointer_Output_Buffer == NULL)
	{
		printf("Error : failed to allocate the output buffer (%s).\n", strerror(errno));
		goto Exit;
	}
	
	// Read all tags
	for (i = 0; i < Tags_Count; i++)
	{
		// Get tag name size
		if (fread(&Temporary_Double_Word, 1, 4, Pointer_File_Archive) != 4)
		{
			printf("Error : failed to read tag %d name size (%s).\n", i, strerror(errno));
			goto Exit;
		}
		
		// Allocate tag name
		Pointer_Output_Buffer[i].Pointer_String_Name = malloc(Temporary_Double_Word);
		if (Pointer_Output_Buffer[i].Pointer_String_Name == NULL)
		{
			printf("Error : failed to allocate tag %d name buffer (%s).\n", i, strerror(errno));
			goto Exit;
		}
		
		// Read tag name
		if (fread(Pointer_Output_Buffer[i].Pointer_String_Name, 1, Temporary_Double_Word, Pointer_File_Archive) != Temporary_Double_Word)
		{
			printf("Error : failed to read tag %d name string (%s).\n", i, strerror(errno));
			goto Exit;
		}
		
		// Read data offset
		if (fread(&Pointer_Output_Buffer[i].Data_Offset, 1, 4, Pointer_File_Archive) != 4)
		{
			printf("Error : failed to read tag %d data offset (%s).\n", i, strerror(errno));
			goto Exit;
		}
		
		// Read data size
		if (fread(&Pointer_Output_Buffer[i].Data_Size, 1, 4, Pointer_File_Archive) != 4)
		{
			printf("Error : failed to read tag %d data size (%s).\n", i, strerror(errno));
			goto Exit;
		}
		
		// Bypass following 8 bytes that are unknown for now (maybe flags ?)
		if (fread(String_Temporary, 1, 8, Pointer_File_Archive) != 8)
		{
			printf("Error : failed to read tag %d unknown bytes (%s).\n", i, strerror(errno));
			goto Exit;
		}
		
		printf("Tag %d name : '%s', data offset : 0x%08X, data size : %d bytes.\n", i, Pointer_Output_Buffer[i].Pointer_String_Name, Pointer_Output_Buffer[i].Data_Offset, Pointer_Output_Buffer[i].Data_Size);
	}
	
	Return_Value = 0;
	
Exit:
	if (Pointer_File_Archive != NULL) fclose(Pointer_File_Archive);
	return Return_Value;
}
