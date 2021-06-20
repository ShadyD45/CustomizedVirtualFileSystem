#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<iostream>
#include<string.h>

#include "cvfs.h"

void printError(int iErrCode)
{	
	if(iErrCode == 1)
	{
		printf("Error: Bad command");
	}
	else if(iErrCode == -1)
	{
		printf("Error: Incorrect Parameters");	
	}	
}

int main()
{
	char szStr[80];
	char buffer[MAX_FILESIZE];
	char *ptr = NULL;
	char szCommand[4][80];
	int iCount = 0, iRet = 0, iFD = 0;

	// Intialize the file system
	CreateDILB();
	InitialiseSuperBlock();

	while(1)
	{
		fflush(stdin);
        strcpy(szStr,"");
        
        printf("\nVFS : >");
        fgets(szStr, 80, stdin);
		
		iCount = sscanf(szStr,"%s %s %s %s",szCommand[0], szCommand[1], szCommand[2], szCommand[3]);
		
		switch(iCount)
		{
			case 1: if(strcmp(szCommand[0], "ls") == 0)
					{
						ls_files();
					}
					else if(strcmp(szCommand[0], "exit") == 0)
					{
						printf("\nThank you for using Marvellous VFS\n");
                		exit(0);
					}
					else if(strcmp(szCommand[0], "clear") == 0)
					{
						system("clear");
					}
					else if(strcmp(szCommand[0], "help") == 0)
					{
						DisplayHelp();
					}
					else if(strcmp(szCommand[0], "closeall") == 0)
					{
						CloseAllFiles();
					}
					else
					{
						printError(1);
					}
					break;

			case 2: if(strcmp(szCommand[0], "rm") == 0)
					{
						iRet = rm_file(szCommand[1]);

						if(iRet == 0)
						{
							printf("\nSUCCESS : File %s deleted successfully\n",szCommand[1]);
						}
						else if(iRet == -1)
						{
							printf("\nERROR: Invalid parameters to rm\n Use man rm for usage information");
						}
						else if(iRet == -2)
						{
							printf("\nERROR: File %s not found", szCommand[1]);
						}
					}
					else if(strcmp(szCommand[0], "man") == 0)
					{
						man(szCommand[1]);
					}
					else if(strcmp(szCommand[0], "write") == 0)
					{
						iFD = GetFDByName(szCommand[1]);
						
						if(iFD == -1)
						{
							printf("\nError : Please open the file first\n");

						}
						else
						{
							printf("\nEnter data: ");
							fgets(buffer, MAX_FILESIZE, stdin);		// scanf(" %[^\n]",buffer);
						
							iRet = strlen(buffer);
						
							if(iRet == 0)
							{
								printf("\nError : No data provided to write\n"); 
							}
							else
							{
								iRet = WriteFile(iFD, buffer, iRet);

								if(iRet == -1)
								{
									printf("\nError : Permission Denied\n");
								}
							}
						}
					}
					else if(strcmp(szCommand[0], "close") == 0)
					{
						int iRet = CloseFile(szCommand[1]);
						if(iRet == -1)
						{
							printf("\nERROR : File %s not found or not open", szCommand[1]);
						}
						else
						{
							printf("\nSUCCESS : File %s closed", szCommand[1]);
						}
					}
					else if(strcmp(szCommand[0], "stat") == 0)
					{
						iRet = stat_file(szCommand[1]);
						if(iRet == -1)
						{
							printf("\nERROR : Incorrect Parameters\n");
						}	
						else if(iRet == -2)
						{
							printf("\nERROR : File %s not found\n",szCommand[1]);
						}
					}
					else
					{
						printError(1);
					}
					break;

			case 3: 
					if(strcmp(szCommand[0], "open") == 0)
					{
						// Syntax of open is open(fname, mode) so we passed fname and mode to our function
						iRet = OpenFile(szCommand[1], atoi(szCommand[2]));
						
						if(iRet >= 0)
						{
							printf("\nSUCCESS : File %s opened successfully with file descriptor: %d\n", szCommand[1], iRet);
						}
						else if(iRet == -1)
						{
							printf("\nERROR : Invalid parameter values\n");
						}
						else if(iRet == -2)
						{
							printf("\nERROR : File %s doesn't exists\n", szCommand[1]);
						}
						else if(iRet == -3)
						{
							printf("\nERROR : Permission Denied\n");
						}
						else if(iRet == -4)
						{
							printf("\nERROR : No file descriptor available\n");
						}
						else if(iRet == -5)
						{
							printf("\nERROR : Memory allocation failure\n");
						}
						else
						{
							printf("\nSome error occured\n");
						}
					}
					else if(strcmp(szCommand[0], "create") == 0)
					{
						// Syntax of create is create(fname, mode) so we passed fname and mode to our function
						iRet = CreateFile(szCommand[1], atoi(szCommand[2]));

						if(iRet >= 0)
						{
							printf("\nSUCCESS :File %s created successfully with file descriptor: %d\n", szCommand[1], iRet);
						}
						else if(iRet == -1)
						{
							printf("\nERROR : Invalid parameter values\n");
						}
						else if(iRet == -2)
						{
							printf("\nERROR : There is no inodes\n");
						}
						else if(iRet == -3)
						{
							printf("\nERROR : File already exists\n");
						}
						else if(iRet == -4)
						{
							printf("\nERROR : No file descriptor available\n");
						}
						else if(iRet == -5)
						{
							printf("\nERROR : Memory allocation failure\n");
						}
					}
					else if(strcmp(szCommand[0], "read") == 0)
					{
						iFD = GetFDByName(szCommand[1]);
						
						if(iFD == -1)
						{
							printf("\nError : Incorrect parameter\n"); 
						}
						else
						{
							ptr = (char *)malloc(sizeof(char) * atoi(szCommand[2])+1);

							if(ptr == NULL)
							{
								printf("\nError : Memory allocation failure\n");
								break;
							}
						
							// Syntax of read is read(fd, buff, size) so we passed fd, buff and size to our function
							iRet = ReadFile(iFD, ptr, atoi(szCommand[2]));

							if(iRet > 0)
							{
								// Write read data on stdout (monitor)
								write(2,ptr,iRet);
							}
							else if(iRet == -1){
								printf("\nERROR : File does not exists\n");
							}
							else if(iRet == -2){
								printf("\nERROR : Permission denied\n");
							}
							else if(iRet == -3){
								printf("\nERROR : Reached at end of file\n");
							}
							else if(iRet == -4){
								printf("\nERROR : It is not regular file\n");
							}
							else if(iRet == 0){
								printf("\nERROR : File empty\n");
							}
						}
					}
					break;
			
			case 4: 
					break;

			default: printf("Error: Bad command default");
					 break;
		}
	}
	return 0;
}
