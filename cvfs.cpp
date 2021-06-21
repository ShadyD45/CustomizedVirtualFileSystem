#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "cvfs.h"

FileSystem :: FileSystem()
{
	Head = NULL;
	// Intialize the file system
	CreateDILB();
	InitialiseSuperBlock();
}

FileSystem :: ~FileSystem()
{
	// Free up allocated memory
	while(NULL != Head)
	{
		PINODE Target = Head;
		Head = Head->next;
		free(Target);
	}
}
// Check if file already exists or not
bool FileSystem :: FileExists(char *fname, PPINODE node)
{
	PINODE temp = Head;

	if(fname == NULL)
	{
		return false;
	}

	while(temp != NULL)
	{
		if((strcmp(temp->szFileName, fname) == 0) && (temp->iFileType != 0))
		{
			break;
		}
		temp = temp->next;
	}
	
	if(temp != NULL)
	{
		// If a bucket is passed then fill it with address of inode of file for the callers use
		if(node != NULL)
		{
			*node = temp;
		}
		return true;
	}
	else
	{
		return false;
	}
}

// Get a Inode which does not represent a file
PINODE FileSystem :: GetFreeInode()
{
	PINODE temp = Head;

	while(temp != NULL)
	{
		// If iFileType is 0 then the inode is unused
		if(temp->iFileType == 0)
		{
			break;
		}
		temp = temp->next;
	}
	return temp;
}

// Get File descriptor of given file
int FileSystem :: GetFDByName(char *fname)
{
	int i = 0;
	while(i < UFDT_ARR_SIZE)
	{
		if(UFDTArr[i].ptrFileTable != NULL)
		{
			if((strcmp(UFDTArr[i].ptrFileTable->ptrInode->szFileName, fname) == 0))
			{
				break;
			}
		}
		i++;
	}
	if(i == UFDT_ARR_SIZE)
	{
		// File not found
		return -1;
	}
	else
	{
		// return the file descriptor of given file
		return i;
	}
}

// Returns available File descriptor from UFDT
int FileSystem :: GetFreeFD()
{
	int i = 0;
	
	// The 'i' we get here will be the File Descriptor of the file
	for(i = 0; i < UFDT_ARR_SIZE; ++i)
	{
		if(UFDTArr[i].ptrFileTable == NULL)
		{
			break;
		}
	}

	if(i == UFDT_ARR_SIZE)
	{
		// No FD available right now
		return -1;
	}
	else
	{
		// Return FD
		return i;
	}
}

// Check READ permission
bool FileSystem :: HasReadPermission(int iFD)
{
	if((UFDTArr[iFD].ptrFileTable->iMode != READ) && (UFDTArr[iFD].ptrFileTable->iMode != READ+WRITE))
	{
		// Permission denied
		return false;
	}

	if((UFDTArr[iFD].ptrFileTable->ptrInode->iPermission != READ) && (UFDTArr[iFD].ptrFileTable->ptrInode->iPermission != READ+WRITE))
	{
		// Permission denied
		return false;
	}

	return true;
}

// Check WRITE permission
bool FileSystem :: HasWritePermission(int iFD)
{
	if((UFDTArr[iFD].ptrFileTable->iMode != WRITE) && (UFDTArr[iFD].ptrFileTable->iMode != READ+WRITE))
	{
		// Permission denied
		return false;
	}

	if((UFDTArr[iFD].ptrFileTable->ptrInode->iPermission != WRITE) && (UFDTArr[iFD].ptrFileTable->ptrInode->iPermission != READ+WRITE))
	{
		// Permission denied
		return false;
	}

	return true;
}

void FileSystem :: man(char *fname)
{

	if(fname == NULL) 
	{
		return;
	}

	if(strcmp(fname,"create") == 0)
	{
		printf("Description : Used to create new regular file\n");
		printf("Usage : create File_name Permission\n");
	}
	else if(strcmp(fname,"read") == 0)
	{
		printf("Description : Used to read data from regular file\n");
		printf("Usage : read File_name No_Of_Bytes_To_Read\n");
	}
	else if(strcmp(fname,"write") == 0)
	{
		printf("Description : Used to write into regular file\n");
		printf("Usage : write File_name\n After this enter the data that we want to write\n");
	}
	else if(strcmp(fname,"ls") == 0)
	{
		printf("Description : Used to list all information of files\n");
		printf("Usage : ls\n");
	}
	else if(strcmp(fname,"stat") == 0)
	{
		printf("Description : Used to display information of file\n");
		printf("Usage : stat File_name\n");
	}
	else if(strcmp(fname,"fstat") == 0)
	{
		printf("Description : Used to display information of file\n");
		printf("Usage : stat File_Descriptor\n");
	}
	else if(strcmp(fname,"truncate") == 0)
	{
		printf("Description : Used to remove data from file\n");
		printf("Usage : truncate File_name\n");
	}
	else if(strcmp(fname,"open") == 0)
	{
		printf("Description : Used to open existing file\n");
		printf("Usage : open File_name mode\n");
	}
	else if(strcmp(fname,"close") == 0)
	{
		printf("Description : Used to close opened file\n");
		printf("Usage : close File_name\n");
	}
	else if(strcmp(fname,"closeall") == 0)
	{
		printf("Description : Used to close all opened file\n");
		printf("Usage : closeall\n");
	}
	else if(strcmp(fname,"lseek") == 0)
	{
		printf("Description : Used to change file offset\n");
		printf("Usage : lseek File_Name ChangeInOffset StartPoint\n");
	}
	else if(strcmp(fname,"rm") == 0)
	{
		printf("Description : Used to delete the file\n");
		printf("Usage : rm File_Name\n");
	}
	else
	{
		printf("ERROR : No manual entry available.\n");
	}
}

void FileSystem :: DisplayHelp()
{
	printf("\nls     : To List out all files\n");
 	printf("clear 	 : To clear console\n");
 	printf("open 	 : To open the file\n");
 	printf("read 	 : To Read the contents from file\n");
	printf("write 	 : To Write contents into file\n");
	printf("rm       : To Delete the file\n");
 	printf("close 	 : To close the file\n");
	printf("closeall : To close all opened files\n");
	printf("stat 	 : To Display information of file using name\n");
	printf("fstat 	 : To Display information of file using file descriptor\n");
	printf("truncate : To Remove all data from file\n");
	printf("exit 	 : To Terminate file system\n");
}

int FileSystem :: stat_file(char *fname)
{
	PINODE temp = NULL;
	int i = 0;
	
	if(fname == NULL)
	{
		// Incorrect parameters
		return -1;
	}

	if(!FileExists(fname, &temp))
	{
		// File not found
		return -2;
	}
	
	printf("\n---------Statistical Information about file----------\n");
	printf("File name : %s\n",temp->szFileName);
	printf("Inode Number %d\n",temp->iInodeNo);
	printf("File size : %d\n",temp->iFileSize);
	printf("Actual File size : %d\n",temp->iActualFileSize);
	printf("Link count : %d\n",temp->iLinkCount);
	printf("Reference count : %d\n",temp->iReferenceCount);
	if(temp->iPermission == READ){
		printf("File Permission : Read only\n");
	}
	else if(temp->iPermission == WRITE){
		printf("File Permission : Write\n");
	}
	else if(temp->iPermission == (READ+WRITE)){
		printf("File Permission : Read & Write\n");
	}
	printf("------------------------------------------------------\n\n");
	
	return 0;
}

// Close all files
void FileSystem :: CloseAllFiles()
{
	int i = 0;
	for(i = 0; i < UFDT_ARR_SIZE; ++i)
	{
		if(UFDTArr[i].ptrFileTable != NULL)
		{
			UFDTArr[i].ptrFileTable->iReadOffset = 0;
			UFDTArr[i].ptrFileTable->iWriteOffset = 0;
			//UFDTArr[i].ptrFileTable->ptrInode = NULL;
			(UFDTArr[i].ptrFileTable->ptrInode->iReferenceCount)--;
			// Free the FD for others use
			UFDTArr[i].ptrFileTable = NULL;
		}
	}
}

// Close specified file
int FileSystem :: CloseFile(char *fname)
{
	int iFD = GetFDByName(fname);
	if(iFD == -1)
	{
		// File not found
		return -1;
	}
	
	UFDTArr[iFD].ptrFileTable->iReadOffset = 0;
	UFDTArr[iFD].ptrFileTable->iWriteOffset = 0;
	(UFDTArr[iFD].ptrFileTable->ptrInode->iReferenceCount)--;
	// Free the FD for others use
	UFDTArr[iFD].ptrFileTable = NULL;
	return 0;
}

// Remove specified file
int FileSystem :: rm_file(char *fname)
{
	if(fname == NULL)
	{
		// Invalid parameters
		return -1;
	}

	if(!FileExists(fname))
	{
		// File doesn't exists
		return -2;
	}

	int iFD = GetFDByName(fname);

	if(iFD == -1)
	{
		// File not found
		return -2;
	}

	// Decrement the reference count from inode table
	(UFDTArr[iFD].ptrFileTable->ptrInode->iLinkCount)--;
	
	// If reference count is 0 after decrementing then remove file from inode table
	if((UFDTArr[iFD].ptrFileTable->ptrInode->iLinkCount) == 0)
	{	
		UFDTArr[iFD].ptrFileTable->ptrInode->iFileType = 0;
		free(UFDTArr[iFD].ptrFileTable);
	}
	
	UFDTArr[iFD].ptrFileTable = NULL;
	
	// Increase count of free nodes by 1 after deleting
	(SuperBlockObj.iFreeInodes)++;

	// File deleted Successfully
	return 0;
	/*
	// Decrement the reference count from inode table
	(UFDTArr[iFD].ptrFileTable->ptrInode->iReferenceCount)--;
	
	// If reference count is 0 after decrementing then remove file from inode table
	if((UFDTArr[iFD].ptrFileTable->ptrInode->iReferenceCount) == 0)
	{	
		UFDTArr[iFD].ptrFileTable->ptrInode->iFileType = 0;
	}
	*/
}

// List all files
void FileSystem :: ls_files()
{
	int j = 0;
	PINODE temp = Head;
    while(temp != NULL)
    {
    	if(temp->iFileType != 0)
    	{
    		printf("%s\t",temp->szFileName);
    		j++;
    	}
    	if(j == 4)
    	{
    		printf("\n");
    		j = 0;
    	}
    	temp = temp->next;
    }
}

// Open given file with specified mode and return file descriptor
int FileSystem :: OpenFile(char *fname, int iMode)
{
	if(fname == NULL || iMode <= 0 || iMode > 3)
	{
		// Invalid parameters
		return -1;
	}
	
	PINODE temp = Head;
	
	// Check if file exists or not, if exists then get its address in passed parameter
	if(!FileExists(fname, &temp))
	{
		// File doesn't exists
		return -2;
	}

	if(temp->iPermission < iMode)
	{
		return -3;
	}

	// Get an available file descriptor
	int iFD = GetFreeFD();

	if(iFD == -1)
	{
		// No file descriptor available right now
		return -4;
	}

	// Create file table
	UFDTArr[iFD].ptrFileTable = (PFILE_TABLE)malloc(sizeof(FileTable));

	if(UFDTArr[iFD].ptrFileTable == NULL)
	{
		// Failed to create file table
		return -5;
	}

	// Create an entry in file table for given file with specified parameters
	UFDTArr[iFD].ptrFileTable->iReadOffset = 0;	
	UFDTArr[iFD].ptrFileTable->iWriteOffset = 0;
	UFDTArr[iFD].ptrFileTable->iCount = 1;
	UFDTArr[iFD].ptrFileTable->iMode = iMode;
	UFDTArr[iFD].ptrFileTable->ptrInode = temp;

	// Increment the reference count by 1
	(UFDTArr[iFD].ptrFileTable->ptrInode->iReferenceCount)++;

	// Return FD to caller
	return iFD;
}

int FileSystem :: ReadFile(int iFD, char *arr, int iSize)
{
	if(UFDTArr[iFD].ptrFileTable == NULL)
	{
		// File not found
		return -1;
	}

	if(!HasReadPermission(iFD))
	{
		return -2;
	}

	if(UFDTArr[iFD].ptrFileTable->iReadOffset == MAX_FILESIZE)
	{
		// Read offset at end of file
		return -2;
	}

	if(UFDTArr[iFD].ptrFileTable->ptrInode->iFileType != REGULAR)
	{
		// File is not regular file
		return -3;
	}

	// Check if given size doesn't exceed actual file size
	// else the garbage values will be displayed to the user
	int iRead_Size = (UFDTArr[iFD].ptrFileTable->ptrInode->iActualFileSize) - (UFDTArr[iFD].ptrFileTable->iReadOffset);

	// So we want to only want to read possible number of bytes
	if(iRead_Size < iSize)
	{
		// Read iRead_Size bytes into given array
		strncpy(arr, (UFDTArr[iFD].ptrFileTable->ptrInode->buffer + UFDTArr[iFD].ptrFileTable->iReadOffset), iRead_Size);
		// Change read offset in the file table
		UFDTArr[iFD].ptrFileTable->iReadOffset = UFDTArr[iFD].ptrFileTable->iReadOffset + iRead_Size;
	}
	else
	{
		// Read iSize bytes into given array
		strncpy(arr, (UFDTArr[iFD].ptrFileTable->ptrInode->buffer + UFDTArr[iFD].ptrFileTable->iReadOffset), iSize);
		// Change read offset in the file table
		UFDTArr[iFD].ptrFileTable->iReadOffset = UFDTArr[iFD].ptrFileTable->iReadOffset + iSize;
	}

	return iSize;
}

int FileSystem :: WriteFile(int iFD, char *arr, int iSize)
{
	if(!HasWritePermission(iFD))
	{
		return -1;
	}

	if(UFDTArr[iFD].ptrFileTable->iWriteOffset == MAX_FILESIZE)
	{
		// Write offset at end of file
		return -2;
	}

	if(UFDTArr[iFD].ptrFileTable->ptrInode->iFileType != REGULAR)
	{
		// File is not regular file
		return -3;
	}

	strncpy((UFDTArr[iFD].ptrFileTable->ptrInode->buffer + UFDTArr[iFD].ptrFileTable->iWriteOffset), arr, iSize);

	// Increase write offset
	UFDTArr[iFD].ptrFileTable->iWriteOffset = UFDTArr[iFD].ptrFileTable->iWriteOffset + iSize;

	// Change actual file size
	UFDTArr[iFD].ptrFileTable->ptrInode->iActualFileSize = UFDTArr[iFD].ptrFileTable->ptrInode->iActualFileSize + iSize;

	// Return number of bytes written to file
	return iSize;
}

// Create a new file with specified permissions
int FileSystem :: CreateFile(char *fname, int iMode)
{
	int iFD  = 0;
	if(fname == NULL || iMode == 0 || iMode > 3)
	{
		// Invalid parameter values
		return -1;
	}

	if(SuperBlockObj.iFreeInodes == 0)
	{
		// There are no free Inodes
		return -2;
	}

	// Check if file exists or not
	if(FileExists(fname))
	{
		// File already exists
		return -3;
	}

	// Get a Free Inode
	PINODE inode = GetFreeInode();

	// Get an available file decriptor from UFDT
	iFD = GetFreeFD();

	if(iFD == -1)
	{
		// No file descriptor avaible right now
		return -4;
	}

	// Create file table
	UFDTArr[iFD].ptrFileTable = (PFILE_TABLE)malloc(sizeof(FileTable));

	if(UFDTArr[iFD].ptrFileTable == NULL)
	{
		// Failed to create file table
		return -5;
	}
	
	// Decrement count of free Inodes by 1
	(SuperBlockObj.iFreeInodes)--;

	// Initialise File Table for the file to be created
	UFDTArr[iFD].ptrFileTable->iCount = 1;
	UFDTArr[iFD].ptrFileTable->iMode = iMode;
	UFDTArr[iFD].ptrFileTable->iReadOffset = 0;
	UFDTArr[iFD].ptrFileTable->iWriteOffset = 0;
	UFDTArr[iFD].ptrFileTable->ptrInode = inode;

	// Initialise Inode with given values for the file to be created
	UFDTArr[iFD].ptrFileTable->ptrInode->iFileType = REGULAR;
	UFDTArr[iFD].ptrFileTable->ptrInode->iFileSize = MAX_FILESIZE;
	UFDTArr[iFD].ptrFileTable->ptrInode->iActualFileSize = 0;
	UFDTArr[iFD].ptrFileTable->ptrInode->iPermission = iMode;
	UFDTArr[iFD].ptrFileTable->ptrInode->iLinkCount = 1;
	UFDTArr[iFD].ptrFileTable->ptrInode->iReferenceCount = 1;
	strcpy(UFDTArr[iFD].ptrFileTable->ptrInode->szFileName, fname);

	UFDTArr[iFD].ptrFileTable->ptrInode->buffer = (char *)malloc(MAX_FILESIZE); // Create file

	// return the file decsriptor on success of all operations
	return iFD;
}

// Create DLIB with list of Inodes with default values
void FileSystem :: CreateDILB()
{
	PINODE newn = NULL;
	PINODE temp = Head;
	Head = NULL;
	int iCnt = 1;

	while(iCnt <= MAX_INODE)
	{
		newn = (PINODE)malloc(sizeof(INODE));
		newn->next = NULL;
		// Initialize all members in Inode to default values
		newn->iLinkCount = newn->iReferenceCount = 0;
		newn->iFileType = newn->iFileSize = 0;
		newn->buffer = NULL;
		newn->iInodeNo = iCnt;
		strcpy(newn->szFileName, "");
		// Append Inode to the InodeList
		if(temp == NULL)
		{
			Head = newn;
			temp = Head;
		}
		else
		{
			temp->next = newn;
			temp = temp->next;
		}
		++iCnt;
	}
}

// Initiliase SuperBlock with default values
void FileSystem :: InitialiseSuperBlock()
{
	for(int i = 0; i < UFDT_ARR_SIZE; ++i)
	{
		UFDTArr[i].ptrFileTable = NULL;
	}
	SuperBlockObj.iTotalInodes = MAX_INODE;
	SuperBlockObj.iFreeInodes = MAX_INODE;	
}
