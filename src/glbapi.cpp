#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include "glbapi.h"
#include "vmemapi.h"
#ifdef _WIN32
#include <io.h>
#endif // _WIN32
#ifdef __linux__
#include <sys/io.h>
#endif // __linux__
#ifdef __GNUC__
#include <unistd.h>
char* strupr(char* s)
{
    char* tmp = s;

    for (; *tmp; ++tmp) {
        *tmp = toupper((unsigned char)*tmp);
    }

    return s;
}
#endif
#ifdef _MSC_VER
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif

char prefix[] = "FILE";
char exePath[PATH_MAX];
const char *serial = "32768GLB";

struct fitem_t {
    char name[16];
    meminfo_t mem;
    int length;
    int offset;
    int flags;
    int lock;
};

struct filedesc_t {
    char path[PATH_MAX];
    fitem_t *items;
    int itemcount;
    FILE *handle;
    const char *mode;
};

int fVmem = 0;
int num_glbs = 0;

#define GLBMAXFILES 15

filedesc_t filedesc[GLBMAXFILES];

#pragma pack(push, 1)
struct glbitem_t {
    int crypt;
    int offset;
    int length;
    char name[16];
};
#pragma pack(pop)

void GLB_EnCrypt(const char *key, void *buf, int size)
{
    char *data = (char*)buf;
    int keylen, vbx;
    char vc, vd;
    keylen = strlen(key);
    vbx = 25 % keylen;
    vc = key[vbx];
    while (size--)
    {
        vd = (*data + key[vbx] + vc) % 256;
        *data++ = vd;
        vc = vd;
        if (++vbx >= keylen)
            vbx = 0;
    }
}

void GLB_DeCrypt(const char *key, void *buf, int size)
{
    char *data = (char*)buf;
    int keylen, vbx;
    char vc, vd;
    keylen = strlen(key);
    vbx = 25 % keylen;
    vc = key[vbx];
    while (size--)
    {
        vd = *data;
        *data++ = (vd - key[vbx] - vc) % 256;
        vc = vd;
        if (++vbx >= keylen)
            vbx = 0;
    }
}

FILE *GLB_FindFile(int a1, int a2, const char *mode)
{
    FILE *h;
    char buffer[PATH_MAX];
    sprintf(buffer, "%s%04u.GLB", prefix, a2);
    h = fopen(buffer, mode);
    if (h == NULL)
    {
        sprintf(buffer, "%s%s%04u.GLB", exePath, prefix, a2);
        h = fopen(buffer, mode);
        if (h == NULL)
        {
            if (a1)
                return NULL;
            sprintf(buffer, "%s%04u.GLB", prefix, a2);
            EXIT_Error("GLB_FindFile: %s, Error #%d,%s", buffer, errno, strerror(errno));
        }
    }
    strcpy(filedesc[a2].path, buffer);
    filedesc[a2].mode = mode;
    filedesc[a2].handle = h;
    return h;
}

FILE *GLB_OpenFile(int a1, int a2, const char *mode)
{
    filedesc_t *fd = &filedesc[a2];
    if (!fd->handle)
        return GLB_FindFile(a1, a2, mode);
    if (strcmp(mode, fd->mode) != 0)
    {
        fclose(fd->handle);
        fd->handle = fopen(fd->path, mode);
        if (fd->handle == NULL)
        {
            if (a1)
                return NULL;
            EXIT_Error("GLB_OpenFile: %s, Error #%d,%s", fd->path, errno, strerror(errno));
        }
    }
    else
        fseek(fd->handle, 0, SEEK_SET);
    return fd->handle;
}

void FUN_00276c8(void)
{
    int i;
    for (i = 0; i < GLBMAXFILES; i++)
    {
        if (filedesc[i].handle)
        {
            fclose(filedesc[i].handle);
            filedesc[i].handle = NULL;
        }
    }
}

int GLB_NumItems(int a1)
{
    glbitem_t head;
    FILE *handle;
    handle = GLB_OpenFile(1, a1, "rb");
    if (handle == NULL)
        return 0;
    fseek(handle, 0, SEEK_SET);
    if (!fread(&head, sizeof(head), 1, handle))
        EXIT_Error("GLB_NumItems: Read failed!");
    GLB_DeCrypt(serial, &head, sizeof(head));
    return head.offset;
}

void GLB_LoadIDT(filedesc_t *a1)
{
    glbitem_t buf[10];
    fitem_t *ve;
    int i, j, k;
    FILE *v30 = a1->handle;
    ve = a1->items;
    fseek(v30, sizeof(glbitem_t), SEEK_SET);
    for (i = 0; i < a1->itemcount; i += j)
    {
        j = a1->itemcount - i;
        if ((unsigned int)j > 10)
            j = 10;
        fread(buf, sizeof(glbitem_t), j, v30);
        for (k = 0; k < j; k++)
        {
            GLB_DeCrypt(serial, &buf[k], sizeof(glbitem_t));
            if (buf[k].crypt == 1)
                ve->flags |= 0x40000000;
            ve->length = buf[k].length;
            ve->offset = buf[k].offset;
            memcpy(ve->name, buf[k].name, 16);
            ve++;
        }
    }
}

void GLB_UseVM(void)
{
    fVmem = 1;
}

int GLB_InitSystem(const char *a1, int a2, const char *a3)
{
    int i, j, k;
    filedesc_t *fd;
    char *t;
    memset(exePath, 0, sizeof(exePath));
    strcpy(exePath, a1);
    t = strrchr(exePath, '\\');
    if (t)
        t[1] = '\0';
    num_glbs = a2;

    if (a3)
    {
        strcpy(prefix, a3);
        strupr(prefix);
    }
    memset(filedesc, 0, sizeof(filedesc));
    k = 0;
    for (i = 0; i < num_glbs; i++)
    {
        fd = &filedesc[i];
        j = GLB_NumItems(i);
        if (j)
        {
            fd->itemcount = j;
            fd->items = (fitem_t*)calloc(j, sizeof(fitem_t));
            if (!fd->items)
                EXIT_Error("GLB_NumItems: memory ( init )");
            GLB_LoadIDT(fd);
            k++;
        }
    }
    return k;
}

int FUN_000279ec(char *a1, int a2, int a3)
{
    fitem_t *fi;
    FILE *handle = filedesc[a2].handle;
    if (!handle)
        return 0;


    fi = &filedesc[a2].items[a3];
    if (a1)
    {
        if (fi->mem.ptr && a1 != fi->mem.ptr)
        {
            memcpy(a1, fi->mem.ptr, fi->length);
        }
        else
        {
            fseek(handle, fi->offset, SEEK_SET);
            fread(a1, fi->length, 1, handle);
            if (fi->flags & 0x40000000)
                GLB_DeCrypt(serial, a1, fi->length);
        }
    }
    return fi->length;
}

char *GLB_FetchItem(int a1, int a2)
{
    int vb;
    meminfo_t *vd;
    char *m;
    fitem_t *fi;
    if (a1 == -1)
    {
        EXIT_Error("GLB_FetchItem: empty handle.");
        return NULL;
    }
    uint16_t f = (a1 >> 16) & 0xffff;
    uint16_t n = (a1 >> 0) & 0xffff;
    fi = &filedesc[f].items[n];
    if (a2 == 2)
        fi->flags |= 0x80000000;
    if (!fi->mem.ptr)
    {
        fi->lock = 0;
        if (fi->length)
        {
            if (fVmem)
            {
                vb = a2 != 0;
                if (fi->flags & 0x80000000)
                    vd = NULL;
                else
                    vd = &fi->mem;
                m = VM_Malloc(fi->length, vd, vb);
            }
            else
                m = (char*)calloc(fi->length, 1);
            if (a2 == 2)
                fi->lock = 1;
            fi->mem.ptr = m;
            if (fi->mem.ptr)
            {
                FUN_000279ec(fi->mem.ptr, f, n);
            }
        }
    }
    else if (a2 == 2)
    {
        if (fVmem)
        {
            fi->lock++;
            VM_Lock(fi->mem.ptr);
        }
    }
    if (!fi->mem.ptr && a2 != 0)
        EXIT_Error("GLB_FetchItem: failed on %d bytes, mode=%d.", fi->length, a2);
    if (a2 == 1)
    {
        if (fVmem)
            VM_Touch(&fi->mem);
    }
    return fi->mem.ptr;
}

char *GLB_CacheItem(int a1)
{
    return GLB_FetchItem(a1, 0);
}

char *GLB_GetItem(int a1)
{
    return GLB_FetchItem(a1, 1);
}

char *GLB_LockItem(int a1)
{
    return GLB_FetchItem(a1, 2);
}

void GLB_UnlockItem(int a1)
{
    fitem_t *fi;
    if (a1 == -1)
        return;
    uint16_t f = (a1 >> 16) & 0xffff;
    uint16_t n = (a1 >> 0) & 0xffff;
    fi = &filedesc[f].items[n];
    if (fi->mem.ptr && fVmem)
    {
        if (!fi->lock || !--fi->lock)
        {
            fi->flags &= ~0x80000000;
            VM_Unlock(fi->mem.ptr, &fi->mem);
        }
    }
    else
        fi->flags &= ~0x80000000;
}

fitem_t *FUN_00027c50(int a1)
{
    if (a1 == -1)
        return NULL;
    uint16_t f = (a1 >> 16) & 0xffff;
    uint16_t n = (a1 >> 0) & 0xffff;
    return &filedesc[f].items[n];
}

void FUN_00027c9c(int a1, char *a2)
{
    if (a1 == -1)
        return;
    uint16_t f = (a1 >> 16) & 0xffff;
    uint16_t n = (a1 >> 0) & 0xffff;
    if (a2)
        FUN_000279ec(a2, f, n);
}

int GLB_GetItemID(const char *a1)
{
    fitem_t *fi;
    int fc;
    int i, j;
    if (*a1 != ' ' && *a1 != '\0')
    {
        for (i = 0; i < num_glbs; i++)
        {
            fi = filedesc[i].items;
            fc = filedesc[i].itemcount;
            for (j = 0; j < fc; j++, fi++)
            {
                if (!strcmp(a1, fi->name))
                {
                    return (i << 16) | j;
                }
            }
        }
    }
    return -1;
}

void GLB_FreeItem(int a1)
{
    filedesc_t *fd;
    fitem_t *fi;
    if (a1 == -1)
        return;
    uint16_t f = (a1 >> 16) & 0xffff;
    uint16_t n = (a1 >> 0) & 0xffff;
    fd = &filedesc[f];
    if (n >= (unsigned short)fd->itemcount)
    {
        EXIT_Error("GLB_FreeItem - item out of range: %d > %d file %d.\n", n, fd->itemcount, f);
    }
    fi = &fd->items[n];
    if (fi->mem.ptr)
    {
        fi->flags &= ~0x80000000;
        if (fVmem)
            VM_Free(fi->mem.ptr);
        else
            free(fi->mem.ptr);
        fi->mem.ptr = NULL;
    }
}

void GLB_FreeAll(void)
{
    int i, j;
    fitem_t *fi;
    for (i = 0; i < num_glbs; i++)
    {
        fi = filedesc[i].items;
        for (j = 0; j < filedesc[i].itemcount; j++)
        {
            if (fi->mem.ptr && !(fi->flags & 0x80000000))
            {
                if (fVmem)
                    VM_Free(fi->mem.ptr);
                else
                    free(fi->mem.ptr);
                fi->mem.ptr = NULL;
            }
            fi++;
        }
    }
}

int GLB_GetItemSize(int a1)
{
    uint16_t f = (a1 >> 16) & 0xffff;
    uint16_t n = (a1 >> 0) & 0xffff;
    return filedesc[f].items[n].length;
}

void GLB_SetItemSize(int a1, int a2)
{
    uint16_t f = (a1 >> 16) & 0xffff;
    uint16_t n = (a1 >> 0) & 0xffff;
    filedesc[f].items[n].length = a2;
}

void GLB_SetItemPointer(int a1, char* a2)
{
    uint16_t f = (a1 >> 16) & 0xffff;
    uint16_t n = (a1 >> 0) & 0xffff;
    filedesc[f].items[n].mem.ptr = a2;
}

int GLB_ReadFile(const char *a1, char *a2)
{
    FILE *handle;
    int l;
    char f_a0[PATH_MAX];
    if (access(a1, 0) == -1)
    {
        strcpy(f_a0, exePath);
        strcat(f_a0, a1);
        a1 = f_a0;
    }
    handle = fopen(a1, "rb");
    if (handle == NULL)
        EXIT_Error("LoadFile: Open failed!");
    fseek(handle, 0, SEEK_END);
    l = ftell(handle);
    rewind(handle);
    if (a2 && l)
    {
        if (!fread(a2, l, 1, handle))
        {
            fclose(handle);
            EXIT_Error("GLB_LoadFile: Load failed!");
        }
    }
    fclose(handle);
    return l;
}

void GLB_SaveFile(char *a1, char *a2, int a3)
{
    FILE *handle;

    handle = fopen(a1, "wb");
    if (handle == NULL)
        EXIT_Error("SaveFile: Open failed!");
    if (a3)
    {
        if (!fwrite(a2, a3, 1, handle))
        {
            fclose(handle);
            EXIT_Error("GLB_SaveFile: Write failed!");
        }
    }
    fclose(handle);
}

