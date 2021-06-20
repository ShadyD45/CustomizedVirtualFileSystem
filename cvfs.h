#ifndef CVFS_H
#define CVFS_H

#include "structs.h"

class FileSystem
{
	private:
		UFDT UFDTArr[UFDT_ARR_SIZE];
		SuperBlock SuperBlockObj;
		PINODE Head;
		
		// --------------- Utility Functions ------------------ //
		PINODE GetFreeInode();
		bool FileExists(char *, PPINODE node = NULL);
		int GetFreeFD();
		bool HasReadPermission(int);
		bool HasWritePermission(int);
		
	
	public:
		FileSystem();			// Constructor
		void DisplayHelp();
		int GetFDByName(char *);
		// --------------- Utility Functions ------------------ //


		// --------------- File System Functions --------------- //
		void CloseAllFiles();
		int CloseFile(char *);
		int stat_file(char *);
		void man(char *);
		int rm_file(char *);
		void ls_files();
		int OpenFile(char *, int);
		int ReadFile(int, char *, int);
		int WriteFile(int, char *, int);
		int CreateFile(char *, int);
		void CreateDILB();
		void InitialiseSuperBlock();
		// --------------- File System Functions --------------- //
};
#endif
