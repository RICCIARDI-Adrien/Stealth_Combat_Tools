/** @file IDP_Archive.h
 * Gather all discovered information about IDP files.
 * @author Adrien RICCIARDI
 */
#ifndef H_IDP_ARCHIVE_H
#define H_IDP_ARCHIVE_H

//-------------------------------------------------------------------------------------------------
// Types
//-------------------------------------------------------------------------------------------------
/** Useful data from a tag. All data present in the IDP but that can be easily computed are not present here. */
typedef struct
{
	char *Pointer_String_Name; //!< The tag name, this string is dynamically allocated.
	int Data_Offset; //!< Offset of the tag data, starting from the data area. This field is used internally, do not modify it.
	int Data_Size; //!< Data size in bytes.
	void *Pointer_Data; //!< Data buffer, dynamically allocated.
} TIDPArchiveTag;

//-------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------
/** Extract all data from an IDP file and store it in a dynamically allocated buffer.
 * @param Pointer_String_IDP_File The IDP file to read.
 * @param Pointer_Pointer_Output_Buffer On output, contain data in an usable form. Call IDPArchiveFreeBuffer() to release all allocated memory when the buffer is not used anymore.
 * @param Pointer_Tags_Count On output, tell how many tags are contained in the buffer.
 * @return 0 if archive data were successfully retrieved,
 * @return -1 if an error occurred.
 */
int IDPArchiveRead(char *Pointer_String_IDP_File, TIDPArchiveTag **Pointer_Pointer_Output_Buffer, int *Pointer_Tags_Count);

/** Free all allocated memory in buffer internal fields an buffer itself.
 * @param Pointer_Buffer The buffer to release.
 * @param Tags_Count How many tags the buffer contains.
 */
void IDPArchiveFreeBuffer(TIDPArchiveTag *Pointer_Buffer, int Tags_Count);

#endif
