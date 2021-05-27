/** @file Map.h
 * A Stealth Combat map definition.
 * @author Adrien RICCIARDI
 */
#ifndef H_MAP_H
#define H_MAP_H

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Extract map assets into usable files.
 * @param Pointer_String_Map_File_Name The map file to process.
 * @param Pointer_String_Output_Path On output, generated files will be stored to this path.
 */
int MapExtract(char *Pointer_String_Map_File_Name, char *Pointer_String_Output_Path);

#endif
