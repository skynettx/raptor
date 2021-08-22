#pragma once
int INI_InitPreference(const char *a1);
int INI_GetPreferenceLong(const char* a1, const char* a2, int a3);
int INI_GetPreferenceHex(const char* a1, const char* a2, int a3);
int INI_GetPreferenceBool(const char* a1, const char* a2, short a3);
char* INI_GetPreference(const char* a1, const char* a2, char* a3, int a4, const char* a5);
int INI_PutPreferenceLong(const char* a1, const char* a2, int a3);
int INI_PutPreferenceHex(const char* a1, const char* a2, int a3);
int INI_PutPreferenceBool(const char* a1, const char* a2, short a3);
int INI_PutPreference(const char* a1, const char* a2, const char* a3);
int INI_DeletePreference(const char* a1, const char* a2);
