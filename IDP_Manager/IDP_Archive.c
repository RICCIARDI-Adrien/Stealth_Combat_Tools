/** @file IDP_Archive.c
 * @see IDP_Archive.h for description.
 * @author Adrien RICCIARDI
 */
#include <errno.h>
#include <IDP_Archive.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** IDP header byte offset 4, called "version" in the Stealth Combat executable. */
#define IDP_ARCHIVE_HEADER_VERSION 0x64

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
int IDPArchiveRead(char *Pointer_String_IDP_File, TIDPArchiveTag **Pointer_Pointer_Output_Buffer, int *Pointer_Tags_Count)
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
	// Reset buffer to make all pointers NULL
	memset(Pointer_Output_Buffer, 0, sizeof(TIDPArchiveTag) * Tags_Count);
	
	// Read all tags
	for (i = 0; i < Tags_Count; i++)
	{
		// Get tag name size
		if (fread(&Temporary_Double_Word, 1, 4, Pointer_File_Archive) != 4)
		{
			printf("Error : failed to read tag %d name size (%s).\n", i, strerror(errno));
			goto Exit_Free_Buffer;
		}
		
		// Allocate tag name
		Pointer_Output_Buffer[i].Pointer_String_Name = malloc(Temporary_Double_Word);
		if (Pointer_Output_Buffer[i].Pointer_String_Name == NULL)
		{
			printf("Error : failed to allocate tag %d name buffer (%s).\n", i, strerror(errno));
			goto Exit_Free_Buffer;
		}
		
		// Read tag name
		if (fread(Pointer_Output_Buffer[i].Pointer_String_Name, 1, Temporary_Double_Word, Pointer_File_Archive) != Temporary_Double_Word)
		{
			printf("Error : failed to read tag %d name string (%s).\n", i, strerror(errno));
			goto Exit_Free_Buffer;
		}
		
		// Read data offset
		if (fread(&Pointer_Output_Buffer[i].Data_Offset, 1, 4, Pointer_File_Archive) != 4)
		{
			printf("Error : failed to read tag %d data offset (%s).\n", i, strerror(errno));
			goto Exit_Free_Buffer;
		}
		
		// Read data size
		if (fread(&Pointer_Output_Buffer[i].Data_Size, 1, 4, Pointer_File_Archive) != 4)
		{
			printf("Error : failed to read tag %d data size (%s).\n", i, strerror(errno));
			goto Exit_Free_Buffer;
		}
		
		// Bypass following 8 bytes that are unknown for now (maybe flags ?)
		if (fread(String_Temporary, 1, 8, Pointer_File_Archive) != 8)
		{
			printf("Error : failed to read tag %d unknown bytes (%s).\n", i, strerror(errno));
			goto Exit_Free_Buffer;
		}
		
		printf("Tag %d name : '%s', data offset : 0x%08X, data size : %d bytes.\n", i, Pointer_Output_Buffer[i].Pointer_String_Name, Pointer_Output_Buffer[i].Data_Offset, Pointer_Output_Buffer[i].Data_Size);
	}
	
	// Read tags data
	for (i = 0; i < Tags_Count; i++)
	{
		// Allocate tag data
		Pointer_Output_Buffer[i].Pointer_Data = malloc(Pointer_Output_Buffer[i].Data_Size);
		if (Pointer_Output_Buffer[i].Pointer_Data == NULL)
		{
			printf("Error : failed to allocate %d tag data buffer (%s).\n", i, strerror(errno));
			goto Exit_Free_Buffer;
		}
		
		// Read tag data
		if (fread(Pointer_Output_Buffer[i].Pointer_Data, 1, Pointer_Output_Buffer[i].Data_Size, Pointer_File_Archive) != Pointer_Output_Buffer[i].Data_Size)
		{
			printf("Error : failed to read tag %d data (%s).\n", i, strerror(errno));
			goto Exit_Free_Buffer;
		}
		
		printf("Read tag %d data.\n", i);
	}
	
	*Pointer_Pointer_Output_Buffer = Pointer_Output_Buffer;
	*Pointer_Tags_Count = Tags_Count;
	
	printf("IDP archive successfully read.\n");
	Return_Value = 0;
	goto Exit; // Do not free the buffer now that it is successfully filled
	
Exit_Free_Buffer:
	IDPArchiveFreeBuffer(Pointer_Output_Buffer, Tags_Count);
	
Exit:
	if (Pointer_File_Archive != NULL) fclose(Pointer_File_Archive);
	return Return_Value;
}

void IDPArchiveFreeBuffer(TIDPArchiveTag *Pointer_Buffer, int Tags_Count)
{
	int i;
	
	// Release all allocated data in buffer internal fields
	for (i = 0; i < Tags_Count; i++)
	{
		// Release tag name
		if (Pointer_Buffer[i].Pointer_String_Name != NULL)
		{
			free(Pointer_Buffer[i].Pointer_String_Name);
			Pointer_Buffer[i].Pointer_String_Name = NULL;
		}
		
		// Release tag data
		if (Pointer_Buffer[i].Pointer_Data != NULL)
		{
			free(Pointer_Buffer[i].Pointer_Data);
			Pointer_Buffer[i].Pointer_Data = NULL;
		}
	}
	
	// Release buffer itself
	free(Pointer_Buffer);
}
