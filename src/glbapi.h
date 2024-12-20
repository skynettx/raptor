#pragma once
#include <stdint.h>

typedef struct
{
	uint32_t opt;           // option (encode on/off)
	uint32_t offset;        // offset into file
	uint32_t filesize;      // filesize
	char name[16];          // text name ( end with null )
}KEYFILE;

typedef struct
{
	uint16_t itemnum;
	uint16_t filenum;
}ITEMS;

#define  MAX_GLB_FILES       0x000F
#define  SEED                0x0019

#define GLB_NORMAL   0
#define GLB_ENCODED  1

void GLB_UseVM(void);
int GLB_InitSystem(const char *exepath, int innum, const char *iprefix);
char *GLB_LockItem(int handle);
void GLB_UnlockItem(int handle);
char *GLB_GetItem(int handle);
char *GLB_CacheItem(int handle);
void GLB_FreeItem(int handle);
void GLB_FreeAll(void);
int GLB_GetItemID(const char *in_name);
int GLB_ItemSize(int handle);
void GLB_EnCrypt(const char *key, void *buf, int length);
void GLB_DeCrypt(const char *key, void *buf, int length);
int GLB_ReadFile(const char *name, char *buffer);
void GLB_SaveFile(char *name, char *buffer, int length);
int GLB_Load(char *inmem, int filenum, int itemnum);
int GLB_IsLabel(int handle);
void GLB_ReadItem(int handle, char *mem);
char * GLB_FindFilePath(char *file);
//char *GLB_GetPtr(int handle);
//void GLB_SetItemPointer(int a1, char* a2);
//void GLB_SetItemSize(int a1, int a2);