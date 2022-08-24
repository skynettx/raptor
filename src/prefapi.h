#pragma once
int INI_InitPreference(const char *section);
int INI_GetPreferenceLong(const char *section, const char *key, int defValue);
int INI_GetPreferenceHex(const char* section, const char* key, int defValue);
int INI_GetPreferenceBool(const char *section, const char *key, short defvalue);
char* INI_GetPreference(const char *section, const char *key, char *retValue, int lenght, const char *defValue);
int INI_PutPreferenceLong(const char *section, const char *key, int value);
int INI_PutPreferenceHex(const char *section, const char *key, int value);
int INI_PutPreferenceBool(const char *section, const char *key, short value);
int INI_PutPreference(const char *section, const char *key, const char *value);
int INI_DeletePreference(const char *section, const char *key);
