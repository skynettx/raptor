#pragma once

void GLB_UseVM(void);
int GLB_InitSystem(const char *a1, int a2, const char *a3);
char *GLB_LockItem(int a1);
void GLB_UnlockItem(int a1);
char *GLB_GetItem(int a1);
char *GLB_CacheItem(int a1);
void GLB_FreeItem(int a1);
void GLB_FreeAll(void);
int GLB_GetItemID(const char *a1);
int GLB_GetItemSize(int a1);
void GLB_EnCrypt(const char *key, void *buf, int size);
void GLB_DeCrypt(const char *key, void *buf, int size);
int GLB_ReadFile(const char *a1, char *a2);
void GLB_SaveFile(char *a1, char *a2, int a3);
void GLB_SetItemPointer(int a1, char* a2);
void GLB_SetItemSize(int a1, int a2);