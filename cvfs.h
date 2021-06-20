//#pragma once

#ifndef CVFS_H
#define CVFS_H

#include "structs.h"

// --------------- Utility Functions ------------------ //
PINODE GetFreeInode();
bool FileExists(char *, PPINODE node = NULL);
int GetFDByName(char *);
int GetFreeFD();
bool HasReadPermission(int);
bool HasWritePermission(int);
void DisplayHelp();
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

#endif
