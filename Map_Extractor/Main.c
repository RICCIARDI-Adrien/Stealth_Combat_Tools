/** @file Main.c
 * Extract as many data as possible from a Stealth Combat map file.
 * @author Adrien RICCIARDI
 */
#include <stdio.h>
#include <string.h>
#include "Map.h"

//-------------------------------------------------------------------------------------------------
// Public functions
//-------------------------------------------------------------------------------------------------
/** Entry point. */
int main(int argc, char *argv[])
{
	// Check parameters
	if (argc != 3)
	{
		printf("Usage : %s Map_File Output_Files_Prefix\n", argv[0]);
		return -1;
	}

	// Try to extract as many data as possible
	if (MapExtract(argv[1], argv[2]) != 0) return -1;

	return 0;
}
