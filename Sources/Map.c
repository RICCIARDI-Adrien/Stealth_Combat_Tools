/** @file Map.c
 * See Map.h for description.
 * @author Adrien RICCIARDI
 */
#include <errno.h>
#include <Map.h>
#include <stdio.h>
#include <string.h>

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** The maximum supported record identifier. */
#define MAP_MAXIMUM_RECORD_IDENTIFIER 19

/** How many tiles per side of the map (i.e. the map width or the map height in tile units). Map is square. */
#define MAP_TERRAIN_GEOMETRY_MAXIMUM_TILES_PER_SIDE 100

/** How many vertices per side of a tile (i.e. a tile width or a tile height in vertex units). A tile is square. */
#define MAP_TERRAIN_GEOMETRY_VERTICES_PER_TILE_SIDE 16

/** The name of the file that stores the units information. */
#define MAP_FILE_NAME_UNITS "Units.ini"

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
// Private variables
//-------------------------------------------------------------------------------------------------
/** Hold the terrain heightmap. */
static float Map_Terrain_Heights[MAP_TERRAIN_GEOMETRY_MAXIMUM_TILES_PER_SIDE * MAP_TERRAIN_GEOMETRY_VERTICES_PER_TILE_SIDE][MAP_TERRAIN_GEOMETRY_MAXIMUM_TILES_PER_SIDE * MAP_TERRAIN_GEOMETRY_VERTICES_PER_TILE_SIDE];

/** How many tiles per side of the map (i.e. the map width or the map height in tile units). Map is always square. */
static int Map_Tiles_Per_Side = -1;
/** How many vertices per map side (i.e. the map width or the map height in vertex units). See Map_Tiles_Per_Side for more details. */
static int Map_Vertices_Per_Side = -1;

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Concatenate the prefix and the file name and try to open the file.
 * @param Pointer_String_Prefix_Path The base path.
 * @param Pointer_String_File_Name The file name to append to the base path.
 * @param Pointer_String_Opening_Mode The mode to provide to fopen().
 * @return NULL if an error occurred,
 * @return a non-NULL value on success.
 */
static FILE *MapOpenFileWithPrefixPath(const char *Pointer_String_Prefix_Path, const char *Pointer_String_File_Name, const char *Pointer_String_Opening_Mode)
{
	static char String_File_Path[1024]; // Should be enough for all files; the variable is allocated statically because the function can't be called in a reentrant way
	FILE *Pointer_File;

	// Concatenate the path and the file name
	if (snprintf(String_File_Path, sizeof(String_File_Path), "%s/%s", Pointer_String_Prefix_Path, Pointer_String_File_Name) >= sizeof(String_File_Path))
	{
		printf("Error when trying to open the file \"%s\", the file path is too long.\n", Pointer_String_File_Name);
		return NULL;
	}

	// Try to open the file
	Pointer_File = fopen(String_File_Path, Pointer_String_Opening_Mode);

	return Pointer_File;
}

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
	int *Pointer_Double_Word = (int *) Pointer_Payload, Width, Height;

	printf("Found a matrix tile field (i.e. map size and texture coordinates) record. It is currently not supported.\n");

	// Retrieve terrain width and height in tile unit
	Width = *Pointer_Double_Word;
	Pointer_Double_Word++;
	Height = *Pointer_Double_Word;

	// Make sure the values are coherent
	// Is the width in the allowed limits ?
	if (Width < 0)
	{
		printf("Error : map width %d is invalid.\n", Width);
		return -1;
	}
	if (Width >= MAP_TERRAIN_GEOMETRY_MAXIMUM_TILES_PER_SIDE)
	{
		printf("Error : map width %d is too large.\n", Width);
		return -1;
	}
	// Is the height in the allowed limits ?
	if (Height < 0)
	{
		printf("Error : map height %d is invalid.\n", Height);
		return -1;
	}
	if (Height >= MAP_TERRAIN_GEOMETRY_MAXIMUM_TILES_PER_SIDE)
	{
		printf("Error : map height %d is too large.\n", Height);
		return -1;
	}

	// Only square maps are supported
	if (Width != Height)
	{
		printf("Error : map width (%d) and height (%d) are different, but only square maps are supported.\n", Width, Height);
		return -1;
	}

	// Make terrain size globally available
	Map_Tiles_Per_Side = Width; // Width and height are equal, so use any of them
	Map_Vertices_Per_Side = Map_Tiles_Per_Side * MAP_TERRAIN_GEOMETRY_VERTICES_PER_TILE_SIDE;
	printf("Extracted map size : %dx%d tiles.\n", Map_Tiles_Per_Side, Map_Tiles_Per_Side);

	// TODO extract texture coordinates

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
	int Tile_Starting_Offset, Vertex_X, Vertex_Y;
	short *Pointer_Word;

	printf("Found a tile def pool (i.e. terrain geometry) record.\n");

	// Make sure needed global variables are available
	if ((Map_Tiles_Per_Side == -1) || (Map_Vertices_Per_Side == -1))
	{
		printf("Error : map coordinates have not been found. The map file is malformed and is missing a record of type 1 at the file beginning.\n");
		return -1;
	}

	// Bypass the first 4 bytes (their value is currently unknown)
	Pointer_Payload += 4;
	Pointer_Word = (short *) Pointer_Payload;

	// Process all vertices
	for (Tile_Starting_Offset = 0; Tile_Starting_Offset < Map_Vertices_Per_Side; Tile_Starting_Offset += MAP_TERRAIN_GEOMETRY_VERTICES_PER_TILE_SIDE)
	{
		for (Vertex_Y = 0; Vertex_Y < Map_Vertices_Per_Side; Vertex_Y++)
		{
			for (Vertex_X = 0; Vertex_X < MAP_TERRAIN_GEOMETRY_VERTICES_PER_TILE_SIDE; Vertex_X++)
			{
				Map_Terrain_Heights[Vertex_Y][Tile_Starting_Offset + Vertex_X] = *Pointer_Word / 300.f;
				Pointer_Word += 4;
			}
		}
	}
	printf("Terrain geometry has been extracted.\n");
	
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
	FILE *Pointer_File;
	int Return_Value = -1;
	unsigned int *Pointer_Double_Word, Units_Count, i, Record_Type;

	printf("Found a units record. It is currently partially supported.\n");

	// Append the data to the dedicated file
	Pointer_File = MapOpenFileWithPrefixPath(Pointer_String_Output_Path, MAP_FILE_NAME_UNITS, "a");
	if (Pointer_File == NULL)
	{
		printf("Error : failed to open the units file (%s).", strerror(errno));
		return -1;
	}

	// The section name is the unit group name
	printf("Unit group name : \"%s\".\n", Pointer_Payload);
	fprintf(Pointer_File, "; The section name matches with a single name in the units section of the map script file\n[%s]\n", Pointer_Payload);
	Pointer_Payload += 32; // There seems to be a 32-byte fixed width for this string

	// The name is followed by what has been called "record type"
	Pointer_Double_Word = (unsigned int *) Pointer_Payload;
	Record_Type = *Pointer_Double_Word;
	Pointer_Payload += 8; // Also bypass the following 0x00000007, it does not seem to be used (setting it to 0 seems to have no effect)
	// Only some record types 0, 1 and 2 are supported for now
	if (Record_Type > 2)
	{
		printf("Error : unknown record type %u following the unit name in the map file, aborting.\n", Record_Type);
		goto Exit;
	}
	fprintf(Pointer_File, "RecordType=%u\n", Record_Type);
	// Extract additional information according to the record type
	if (Record_Type == 1) Pointer_Payload += 4; // TODO
	// The format is 0x00000002 0x00000007 0x<index in unit list>
	// The value 0x00000007 does not seem to be used, setting it to 0 seems to have no effect
	// The index in the unit list seems to be multiplied by 4
	else if (Record_Type == 2)
	{
		Pointer_Double_Word = (unsigned int *) Pointer_Payload;
		fprintf(Pointer_File, "UnitsListIndex=%u\n", *Pointer_Double_Word);
		Pointer_Payload += 4;
	}

	// Retrieve the amount of units in the group
	Pointer_Double_Word = (unsigned int *) Pointer_Payload;
	Units_Count = *Pointer_Double_Word;
	Pointer_Payload += 4;
	fprintf(Pointer_File, "UnitsCount=%u\n", Units_Count);

	// Extract each single unit from the group
	for (i = 0; i < Units_Count; i++)
	{
		// Extract the unit type
		fprintf(Pointer_File, "; This type is declared in the app/units file\nUnit%uType=\"%s\"\n", i, Pointer_Payload);
		Pointer_Payload += 24; // There seems to be a 24-byte fixed width for this string

		// Bypass unknown fields (flags ?)
		Pointer_Payload += 12;

		// Extract the unit coordinates in the world
		// X
		Pointer_Double_Word = (unsigned int*) Pointer_Payload;
		fprintf(Pointer_File, "Unit%uCoordinateX=%u\n", i, *Pointer_Double_Word);
		Pointer_Payload += 8;
		// Y
		Pointer_Double_Word = (unsigned int*) Pointer_Payload;
		fprintf(Pointer_File, "Unit%uCoordinateY=%u\n", i, *Pointer_Double_Word);
		Pointer_Payload += 8;
		// Z
		Pointer_Double_Word = (unsigned int*) Pointer_Payload;
		fprintf(Pointer_File, "Unit%uCoordinateZ=%u\n", i, *Pointer_Double_Word);
		Pointer_Payload += 8;
	}

	// TODO

	// Everything went well
	Return_Value = 0;

Exit:
	// Separate each section by an empty line
	fprintf(Pointer_File, "\n");
	fclose(Pointer_File);

	return Return_Value;
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

/** Use the data extracted from various records to create a Wavefront OBJ file containing the terrain geometry.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this location.
 * @return -1 if an error occurred,
 * @return 0 on success.
 */
static int MapGenerateTerrain(char *Pointer_String_Output_Path)
{
	FILE *Pointer_File;
	char String_Output_File_Name[2048];
	int Vertex_X, Vertex_Y, Face_Vertices_Offset, Tile_Row, Tiles_Count;
	
	// Make sure needed global variables are available
	if ((Map_Tiles_Per_Side == -1) || (Map_Vertices_Per_Side == -1))
	{
		printf("Error : map coordinates have not been found. The map file is malformed and is missing a record of type 1 at the file beginning.\n");
		return -1;
	}
	
	// Generate the output file name
	snprintf(String_Output_File_Name, sizeof(String_Output_File_Name), "%s/Terrain_Geometry.obj", Pointer_String_Output_Path);
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
	
	// Append vertices to file
	printf("Adding vertices...\n");
	for (Vertex_Y = 0; Vertex_Y < Map_Vertices_Per_Side; Vertex_Y++)
	{
		for (Vertex_X = 0; Vertex_X < Map_Vertices_Per_Side; Vertex_X++) fprintf(Pointer_File, "v %d %d %f\n", Vertex_X, Vertex_Y, Map_Terrain_Heights[Vertex_Y][Vertex_X]);
	}
	
	// Generate quad faces from the vertices
	printf("Adding faces...\n");
	Tiles_Count = Map_Vertices_Per_Side * (Map_Vertices_Per_Side - 1); // Do not take last row into account because it is the bottom part of the last quads
	for (Tile_Row = 0; Tile_Row < Tiles_Count; Tile_Row += Map_Vertices_Per_Side)
	{
		for (Vertex_X = 1; Vertex_X < Map_Vertices_Per_Side; Vertex_X++)
		{
			Face_Vertices_Offset = Vertex_X + Tile_Row;
			fprintf(Pointer_File, "f %d %d %d %d\n", Face_Vertices_Offset, Face_Vertices_Offset + 1, Face_Vertices_Offset + Map_Vertices_Per_Side + 1, Face_Vertices_Offset + Map_Vertices_Per_Side);
		}
	}
	
	printf("Terrain was successfully generated.\n");
	fclose(Pointer_File);
	
	return 0;
}

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
int MapExtract(char *Pointer_String_Map_File_Name, char *Pointer_String_Output_Path)
{
	static unsigned char Payload_Buffer[20 * 1024 * 1024]; // 20 MB is enough for all existing maps
	FILE *Pointer_File_Map = NULL, *Pointer_File;
	int Return_Value = -1, Temporary_Integer, Record_Identifier, Records_Count = 1, Record_Payload_Size, Record_Offset;
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
	
	// Take the two above tags into account
	Record_Offset = 8;

	// Reset or remove some files from the output directory (if any), so a new map extraction data can't mix with an old one
	// Remove the units file previous content
	Pointer_File = MapOpenFileWithPrefixPath(Pointer_String_Output_Path, MAP_FILE_NAME_UNITS, "w");
	if (Pointer_File != NULL) fclose(Pointer_File);
	
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
		if (fread(Payload_Buffer, 1, Record_Payload_Size, Pointer_File_Map) != (size_t) Record_Payload_Size)
		{
			printf("Error : failed to read record %d payload (%s).\n", Records_Count, strerror(errno));
			break;
		}

		// Call the corresponding record handler if the record is valid
		printf("Found record %d at offset 0x%08X. ID : %d, payload size : %d.\n", Records_Count, Record_Offset, Record_Identifier, Record_Payload_Size);
		// Exit when the last record is detected
		if (Record_Identifier == 4097)
		{
			printf("End-of-file record has been found, exiting.\n\n");
			Return_Value = 0;
			break;
		}
		// Make sure the record identifier is valid
		if ((Record_Identifier < 0) || (Record_Identifier >= MAP_MAXIMUM_RECORD_IDENTIFIER)) printf("This record is not supported, bypassing it.\n");
		else
		{
			// Try to extract the record content
			if (Record_Handler_Functions[Record_Identifier](Payload_Buffer, Record_Payload_Size, Pointer_String_Output_Path) != 0)
			{
				printf("Error : failed to handle a record payload, aborting program.\n");
				break;
			}
		}
		
		// Adjust offset to next record beginning
		Record_Offset += 4 + 4 + Record_Payload_Size; // Take into account record ID field, record size field and record payload field

		putchar('\n');
		Records_Count++;
	}
	
	// All relevant data have been extracted to be able to generate the terrain
	if (MapGenerateTerrain(Pointer_String_Output_Path) != 0)
	{
		printf("Error : could not generate terrain.\n");
		goto Exit;
	}

Exit:
	if (Pointer_File_Map != NULL) fclose(Pointer_File_Map);
	return Return_Value;
}
