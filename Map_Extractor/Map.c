/** @file Map.c
 * See Map.h for description.
 * @author Adrien RICCIARDI
 */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "Map.h"

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** The maximum supported record identifier. */
#define MAP_MAXIMUM_RECORD_IDENTIFIER 19

//-------------------------------------------------------------------------------------------------
// Private types
//-------------------------------------------------------------------------------------------------
/** A record handler function.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
typedef int (*MapRecordHandler)(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path);

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Handle type 0 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier0(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a tile clone record. It is currently not supported.\n");

	return 0;
}

/** Handle type 1 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier1(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a matrix tile field record. It is currently not supported.\n");

	return 0;
}

/** Handle type 2 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier2(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	FILE *Pointer_File;
	int t, x, y;
	char String_Output_File_Name[2048];
	short *Pointer_Word;

	printf("Found a tile def pool (i.e. terrain geometry) record.\n");

	// Bypass the first 4 bytes (their value is currently unknown)
	Pointer_Payload += 4;
	Pointer_Word = (short *) Pointer_Payload;

	// Generate the output file name
	snprintf(String_Output_File_Name, sizeof(String_Output_File_Name), "%s\\Terrain_Geometry.obj", Pointer_String_Output_Path);
	printf("Saving terrain geometry to \"%s\" file.\n", String_Output_File_Name);

	// Try to open output file
	Pointer_File = fopen(String_Output_File_Name, "w");
	if (Pointer_File == NULL)
	{
		printf("Error : could not open output file (%s).\n", strerror(errno));
		return -1;
	}

	// Create OBJ file header
	fprintf(Pointer_File, "o terrain_geometry\n\n");

	// Process all items
	for (t = 0; t < 40; t++)
	{
		for (x = 0; x < 640; x++)
		{
			for (y = 0; y < 16; y++)
			{
				fprintf(Pointer_File, "v %d %d %f\n", x, y + (16 * t), *Pointer_Word / 300.f);
				Pointer_Word += 4;
			}
		}
	}
	fclose(Pointer_File);
	
	return 0;
}

/** Handle type 3 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier3(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a type 3 record. It is currently not supported.\n");

	return 0;
}

/** Handle type 4 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier4(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a texture 2 record. It is currently not supported.\n");

	return 0;
}

/** Handle type 5 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier5(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a sky record. It is currently not supported.\n");

	return 0;
}

/** Handle type 6 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier6(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a type 6 record. It is currently not supported.\n");

	return 0;
}

/** Handle type 7 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier7(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a units record. It is currently not supported.\n");

	return 0;
}

/** Handle type 8 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier8(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a units list record. It is currently not supported.\n");

	return 0;
}

/** Handle type 9 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier9(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a type 9 record. It is currently not supported.\n");

	return 0;
}

/** Handle type 10 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier10(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a type 10 record. It is currently not supported.\n");

	return 0;
}

/** Handle type 11 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier11(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a type 11 record. It is currently not supported.\n");

	return 0;
}

/** Handle type 12 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier12(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a saved AU record. It is currently not supported.\n");

	return 0;
}

/** Handle type 13 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier13(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a saved shared pool record. It is currently not supported.\n");

	return 0;
}

/** Handle type 14 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier14(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a saved dead body record. It is currently not supported.\n");

	return 0;
}

/** Handle type 15 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier15(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a type 15 record. It is currently not supported.\n");

	return 0;
}

/** Handle type 16 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier16(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a saved clan record. It is currently not supported.\n");

	return 0;
}

/** Handle type 17 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier17(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a saved tile field record. It is currently not supported.\n");

	return 0;
}

/** Handle type 18 records.
 * @param Pointer_Payload The record payload.
 * @param Payload_Size The payload size in bytes.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapRecordHandlerIdentifier18(unsigned char *Pointer_Payload, int Payload_Size, char *Pointer_String_Output_Path)
{
	printf("Found a saved HL clan record. It is currently not supported.\n");

	return 0;
}

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
int MapExtract(char *Pointer_String_Map_File_Name, char *Pointer_String_Output_Path)
{
	static unsigned char Payload_Buffer[20 * 1024 * 1024]; // 20 MB is enough for all existing maps
	FILE *Pointer_File_Map = NULL;
	int Return_Value = -1, Temporary_Integer, Record_Identifier, Records_Count = 1, Record_Payload_Size;
	char String_Temporary[5];
	MapRecordHandler Record_Handler_Functions[] =
	{
		MapRecordHandlerIdentifier0,
		MapRecordHandlerIdentifier1,
		MapRecordHandlerIdentifier2,
		MapRecordHandlerIdentifier3,
		MapRecordHandlerIdentifier4,
		MapRecordHandlerIdentifier5,
		MapRecordHandlerIdentifier6,
		MapRecordHandlerIdentifier7,
		MapRecordHandlerIdentifier8,
		MapRecordHandlerIdentifier9,
		MapRecordHandlerIdentifier10,
		MapRecordHandlerIdentifier11,
		MapRecordHandlerIdentifier12,
		MapRecordHandlerIdentifier13,
		MapRecordHandlerIdentifier14,
		MapRecordHandlerIdentifier15,
		MapRecordHandlerIdentifier16,
		MapRecordHandlerIdentifier17,
		MapRecordHandlerIdentifier18
	};

	// Try to open the map file
	Pointer_File_Map = fopen(Pointer_String_Map_File_Name, "rb");
	if (Pointer_File_Map == NULL)
	{
		printf("Error : failed to open map file \"%s\" (%s).\n", Pointer_String_Map_File_Name, strerror(errno));
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
	
	// Parse all file records
	while (1)
	{
		// Read record identifer
		if (fread(&Record_Identifier, 1, 4, Pointer_File_Map) != 4)
		{
			printf("Error : failed to read record %d identifier (%s).\n", Records_Count, strerror(errno));
			break;
		}

		// Read record size
		if (fread(&Record_Payload_Size, 1, 4, Pointer_File_Map) != 4)
		{
			printf("Error : failed to read record %d size (%s).\n", Records_Count, strerror(errno));
			break;
		}
		// Adjust size to take only payload into account
		Record_Payload_Size -= 8; // Record identifier and size tags are included into the record size field value

		// Read record payload
		if (fread(Payload_Buffer, 1, Record_Payload_Size, Pointer_File_Map) != Record_Payload_Size)
		{
			printf("Error : failed to read record %d payload (%s).\n", Records_Count, strerror(errno));
			break;
		}

		// Call the corresponding record handler if the record is valid
		printf("Found record %d. ID : %d, payload size : %d.\n", Records_Count, Record_Identifier, Record_Payload_Size);
		// Exit when the last record is detected
		if (Record_Identifier == 4097)
		{
			printf("End-of-file record has been found, exiting.\n");
			Return_Value = 0;
			break;
		}
		// Make sure the record identifier is valid
		if ((Record_Identifier < 0) || (Record_Identifier >= MAP_MAXIMUM_RECORD_IDENTIFIER))
		{
			printf("This record is not supported, bypassing it.\n");
			continue;
		}
		// Try to extract the record content
		if (Record_Handler_Functions[Record_Identifier](Payload_Buffer, Record_Payload_Size, Pointer_String_Output_Path) != 0)
		{
			printf("Error : failed to handle a record payload, aborting program.\n");
			break;
		}

		Records_Count++;
	}

Exit:
	if (Pointer_File_Map != NULL) fclose(Pointer_File_Map);
	return Return_Value;
}
