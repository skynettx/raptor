#pragma once
int INI_InitPreference(const char *profile);
int INI_GetPreferenceLong(const char *section, const char *option, int def);
int INI_GetPreferenceHex(const char *section, const char *option, int def);
int INI_GetPreferenceBool(const char *section, const char *option, short def);
char* INI_GetPreference(const char *section, const char *option, char *buf, int buflen, const char *def);
int INI_PutPreferenceLong(const char *section, const char *option, int val);
int INI_PutPreferenceHex(const char *section, const char *option, int val);
int INI_PutPreferenceBool(const char *section, const char *option, short val);
int INI_PutPreference(const char *section, const char *option, const char *val);
int INI_DeletePreference(const char *section, const char *option);
