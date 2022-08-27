#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "prefapi.h"
#ifdef _WIN32
#include <io.h>
#endif // _WIN32
#ifdef __linux__
#include <sys/io.h>
#endif // __linux__
#ifdef __GNUC__
#include <unistd.h>
char* ltoa(long i, char* s, int dummy_radix) {
    sprintf(s, "%ld", i);
    return s;
}
#endif
#ifdef _MSC_VER
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif

static int INI_OpenFile(const char *section, const char *key, const char *defaultValue, char *value, int length, const char *filename)
{
    char *va;
    char v9c[128];
    short v10, i;
    FILE *vs;

    v10 = 0;
    vs = fopen(filename, "r");
    if (vs)
    {

        while (!v10 && fgets(v9c, 128, vs) != 0)
        {
            if (v9c[0] == '[')
            {
                for (i = 0; v9c[i] != 0 && v9c[i] != ']'; i++) {
                }
                if (v9c[i] == ']')
                {
                    v9c[i] = '\0';
                    if (!strcmp(v9c + 1, section))
                        v10 = 1;
                }
            }
        }

        while (v10 && fgets(v9c, 128, vs) != 0)
        {
            if (v9c[0] == '[')
            {
                v10 = 0;
                continue;
            }
            va = strtok(v9c, "=\r\n");
            if (va)
            {
                if (!strcmp(va, key))
                {
                    fclose(vs);
                    va = strtok(NULL, "\r\n");
                    if (va)
                    {
                        strncpy(value, va, length);
                        value[length-1] = '\0';
                    }
                    else
                        value[0] = '\0';
                    return 1;
                }
            }
        }
        fclose(vs);
    }
    if (defaultValue)
        strncpy(value, defaultValue, length);
    else
        value[0] = '\0';
    return 0;
}

static int INI_SaveFile(const char *section, const char *key, const char *value, const char *filename)
{
    char *va;
    char va8[128];
    FILE *vs;
    short vc, i;
    int v28, vbp, vd, v10, l, v1c, v18, vdi, v20;

    vc = 0;
    if (!section || !*section)
        return 0;
    vs = fopen(filename, "rb+");
    if (!vs)
    {
        vs = fopen(filename, "wb+");
        if (!vs)
            return 0;
        fseek(vs, 0, SEEK_SET);
    }
    v10 = 0;

    while (!vc && fgets(va8, 128, vs) != 0)
    {
        if (va8[0] == '\r' || va8[0] == '\n')
            continue;
        if (va8[0] == '[')
        {
            for (i = 0; va8[i] != 0 && va8[i] != ']'; i++) {
            }
            if (va8[i] == ']')
            {
                va8[i] = '\0';
                if (!strcmp(va8 + 1, section))
                {
                    vc = 1;
                    break;
                }
            }
            else
                continue;
        }
        v10 = ftell(vs);
    }
    if (!vc)
    {
        fseek(vs, 0, SEEK_END);
        if (!key || !*key)
        {
            fclose(vs);
            return 1;
        }
        fprintf(vs, "\r\n[%s]\r\n", section);
        v28 = vbp = ftell(vs);
    }
    else
    {
        v28 = ftell(vs);
        while (fgets(va8, 128, vs) != 0)
        {
            vbp = ftell(vs);
            if (va8[0] == '[')
            {
                vc = 0;
                break;
            }
            va = strtok(va8, "=\r\n");
            if (va)
            {
                if (!strcmp(va, key))
                {
                    va = strtok(NULL, "=\r\n");
                    if (!strcmp(value, va))
                    {
                        fclose(vs);
                        return 1;
                    }
                    vc = 1;
                    break;
                }
                else
                    v28 = vbp;
            }
        }
    }
    if (!vc)
    {
        if (vbp == v28)
        {
            if (!key || !*key || !value || !*value)
            {
                fclose(vs);
                return 1;
            }
            fseek(vs, 0, SEEK_END);
            fprintf(vs, "%s=%s\r\n", key, value);
            fclose(vs);
            return 1;
        }
        else
        {
            if (key && *key)
                vd = 0;
            else
            {
                vd = v28 - v10;
                vbp = v28;
            }
        }
    }
    else
    {
        vd = vbp - v28;
    }
    if (key && *key && value && *value)
    {
        sprintf(va8, "%s=%s\r\n", key, value);
        l = strlen(va8);
    }
    else
        l = 0;
    v1c = l - vd;
    if (key && *key)
        v10 = v28;
    if (v1c < 0)
    {
        while (1)
        {
            fseek(vs, vbp, SEEK_SET);
            vdi = fread(va8, 1, 128, vs);
            if (!vdi)
                break;
            v18 = ftell(vs);
            fseek(vs, v1c + vbp, SEEK_SET);
            vbp = v18;
            fwrite(va8, 1, vdi, vs);
        }
        fseek(vs, v1c, SEEK_END);
        #ifdef _MSC_VER
        _chsize(fileno(vs), ftell(vs));
        #endif
        #ifdef __GNUC__
        ftruncate(fileno(vs), ftell(vs));
        #endif


    }
    else if (v1c > 0)
    {
        fseek(vs, 0, SEEK_END);
        v20  = ftell(vs);
        vbp = 128;
        while (1)
        {
            int vd1;
            vd1 = v20 - vbp;
            if (vd1 < v10)
            {
                vbp = v20 - v10;
                if (vbp <= 0)
                    break;
                vd1 = v10;
            }
            v20 = vd1;
            fseek(vs, v20, SEEK_SET);
            vdi = fread(va8, 1, vbp, vs);
            if (!vdi)
                break;
            fseek(vs, v1c + v20, SEEK_SET);
            fwrite(va8, 1, vdi, vs);
            fflush(vs);
        }
    }
    if (key && *key && value && *value)
    {
        fseek(vs, v10, SEEK_SET);
        fprintf(vs, "%s=%s\r\n", key, value);
    }
    fclose(vs);
    return 1;
}

static char preference[PATH_MAX];

int INI_InitPreference(const char *section)
{
    if (section)
        strcpy(preference, section);
    return !access(preference, 4);
}

int INI_GetPreferenceLong(const char *section, const char *key, int defValue)
{
    char s1[20], s2[20];
    ltoa(defValue, s1, 10);
    INI_GetPreference(section, key, s2, 20, s1);
    return atol(s2);
}

int INI_GetPreferenceHex(const char* section, const char* key, int defValue)
{
    char s1[32], s2[32];
    unsigned int t;

    ltoa(defValue, s1, 10);
    INI_GetPreference(section, key, s2, 32, s1);
    sscanf(s2, "%x", &t);

    return t & 0xFF;
}

int INI_GetPreferenceBool(const char *section, const char *key, short defvalue)
{
    char s2[10];
    if (defvalue)
        INI_GetPreference(section, key, s2, 10, "TRUE");
    else
        INI_GetPreference(section, key, s2, 10, "FALSE");
        
    if (s2[0] == '1' || !strcmp(s2, "TRUE"))
        return 1;
    if (s2[0] == '0' || !strcmp(s2, "FALSE"))
        return 0;
    return atoi(s2);
}

char *INI_GetPreference(const char *section, const char *key, char *retValue, int lenght, const char *defValue)
{
    if (!section || !key || !retValue)
    {
        if (retValue)
            *retValue = 0;
        return NULL;
    }
    INI_OpenFile(section, key, defValue, retValue, lenght, preference);
    return retValue;
}

int INI_PutPreferenceLong(const char *section, const char *key, int value)
{
    char s1[32];
    if (value == -1)
    {
        INI_DeletePreference(section, key);
        return 0;
    }
    ltoa(value, s1, 10);
    return INI_PutPreference(section, key, s1);
}

int INI_PutPreferenceHex(const char *section, const char *key, int value)
{
    char s1[36];
    if (value == -1)
    {
        INI_DeletePreference(section, key);
        return 0;
    }
    sprintf(s1, "%x", value);
    return INI_PutPreference(section, key, s1);
}

int INI_PutPreferenceBool(const char *section, const char *key, short value)
{
    if (value)
        return INI_PutPreference(section, key, "TRUE");
    return INI_PutPreference(section, key, "FALSE");
}

int INI_PutPreference(const char *section, const char *key, const char *value)
{
    if (!section || !key || !value)
        return 0;
    return INI_SaveFile(section, key, value, preference);
}

int INI_DeletePreference(const char *section, const char *key)
{
    if (!section)
        return 0;
    return INI_SaveFile(section, key, NULL, preference);
}
