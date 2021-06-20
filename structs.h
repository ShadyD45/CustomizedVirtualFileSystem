//#pragma once

#ifndef CVFS_STRUCTS_H
#define CVFS_STRUCTS_H

#include "limits.h"

typedef struct SuperBlock
{
	int iTotalInodes;
	int iFreeInodes;
}SUPER_BLOCK, *PSUPER_BLOCK;

typedef struct Inode
{
	int iInodeNo;
	int iFileSize;
	int iFileType;
	int iActualFileSize;
	int iLinkCount;
	int iReferenceCount;
	int iPermission;
	char szFileName[50];
	char *buffer;		// char *Buffer[5]; Buffer is a one dimensional array which contains 5 elements each element will point to 1024 bytes 
						// allocate next 1024 using malloc after first 1024 are consumed
	struct Inode *next;
}INODE, *PINODE, **PPINODE;

typedef struct FileTable
{
	int iReadOffset;
	int iWriteOffset;
	int iCount;
	int iMode;
	PINODE ptrInode;
}FILE_TABLE, *PFILE_TABLE;

typedef struct UFDT
{
	PFILE_TABLE ptrFileTable;
}ufdt;

#endif
