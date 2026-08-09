#pragma once

int SE_INI_InitPreference(const char *section);
int SE_INI_GetPreferenceLong(const char* section, const char* key, int defValue);
int SE_INI_GetPreferenceHex(const char* section, const char* key, int defValue);
int SE_INI_GetPreferenceBool(const char* section, const char* key, short defValue);
char* SE_INI_GetPreference(const char* section, const char* key, char* retValue, int length, const char* defValue);
int SE_INI_PutPreferenceLong(const char* section, const char* key, int value);
int SE_INI_PutPreferenceHex(const char* section, const char* key, int value);
int SE_INI_PutPreferenceBool(const char* section, const char* key, short value);
int SE_INI_PutPreference(const char* section, const char* key, const char* value);
int SE_INI_DeletePreference(const char* section, const char* key);
