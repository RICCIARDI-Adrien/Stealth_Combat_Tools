/** @file Main.c
 * Extract as many data as possible from a Stealth Combat map file.
 * @author Adrien RICCIARDI
 */
#include <stdio.h>
#include <string.h>
#include <Windows.h>

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
/** Entry point. */
int main(int argc, char *argv[])
{
	FILE *Pointer_File_Map = NULL;
	unsigned char Buffer[4];
	int Return_Value = -1, Temporary_Integer;
	char String_Temporary[5];

	// Check parameters
	if (argc != 3)
	{
		printf("Usage : %s Map_File Output_Files_Prefix", argv[0]);
		return -1;
	}

	// Try to open the map file
	Pointer_File_Map = fopen(argv[1], "rb");
	if (Pointer_File_Map == NULL)
	{
		printf("Error : failed to open map file \"%s\" (%s).\n", argv[1], strerror(errno));
		return -1;
	}

	// Check file signature
	if (fread(String_Temporary, 1, 4, Pointer_File_Map) != 4)
	{
		printf("Error : failed to read map file signature (%s).\n", strerror(errno));
		goto Exit;
	}
	if (strncmp(String_Temporary, "IDWD", 4) != 0)
	{
		printf("Error : invalid map file signature. File must start with \"IDWD\" header identifier.\n");
		goto Exit;
	}

	// Check file version
	if (fread(&Temporary_Integer, 1, 4, Pointer_File_Map) != 4)
	{
		printf("Error : failed to read map file version (%s).\n", strerror(errno));
		goto Exit;
	}
	if (Temporary_Integer != 0x66)
	{
		printf("Error : invalid map file version. Supported version is 0x66.\n");
		goto Exit;
	}
	
	// Everything went fine
	Return_Value = 0;

Exit:
	if (Pointer_File_Map != NULL) fclose(Pointer_File_Map);

	return Return_Value;
}
