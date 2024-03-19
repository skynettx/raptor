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
#endif // __GNUC__
#ifdef _MSC_VER
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif // _MSC_VER

#define _SCOTTGAME

static const char* serial = "32768GLB";
static char exePath[PATH_MAX];
static int num_glbs;
static KEYFILE g_key;
static char prefix[5] = "FILE";
static bool fVmem = 0;

/*
* define file descriptor used to access file.
*/
typedef struct
{
	char name[16];
	VM_OWNER vm_mem;
	uint32_t size;
	uint32_t offset;
	uint32_t flags;
	uint32_t lock_cnt;
}ITEMINFO;

typedef struct
{
	char filepath[PATH_MAX];
	ITEMINFO* item;
	int      items;
	FILE     *handle;
	const char *permissions;
}FILEDESC;

#if __GNUC__ && __BIG_ENDIAN__
typedef struct
{
	uint16_t filenum;
	uint16_t itemnum;
}ITEM_ID;
#else
typedef struct
{
	uint16_t itemnum;
	uint16_t filenum;
}ITEM_ID;
#endif

typedef union
{
	ITEM_ID id;
	uint32_t handle;
}ITEM_H;

static FILEDESC  filedesc[MAX_GLB_FILES];

#define ITF_LOCKED  0x80000000L
#define ITF_ENCODED 0x40000000L

typedef enum
{
	FI_CACHE,
	FI_DISCARD,
	FI_LOCK
}FI_MODE;

/***************************************************************************
  GLB_EnCrypt - Encrypt Data
 ***************************************************************************/
void
GLB_EnCrypt(
	const char* key,             // INPUT : Key that will allow Decryption
	void* buf,                   // INPUT : Buffer to Encrypt
	int length                   // INPUT : Length of Buffer
)
{
	char* buffer = (char*)buf;
	int klen = strlen(key);
	int prev_byte;
	int kidx;

	kidx = SEED % klen;
	prev_byte = key[kidx];

	while (length--)
	{
		prev_byte = (*buffer + key[kidx] + prev_byte) % 256;
		*buffer++ = prev_byte;

		if (++kidx >= klen)
			kidx = 0;
	}
}

/***************************************************************************
  GLB_DeCrypt - Decrypt Data
 ***************************************************************************/
void
GLB_DeCrypt(
	const char* key,             // INPUT : Key that will allow Decryption
	void* buf,                   // INPUT : Buffer to Encrypt
	int length                   // INPUT : Length of Buffer
)
{
	char* buffer = (char*)buf;
	int klen = strlen(key);
	int prev_byte;
	int kidx;
	char dchr;

	kidx = SEED % klen;
	prev_byte = key[kidx];

	while (length--)
	{
		dchr = (*buffer - key[kidx] - prev_byte) % 256;
		prev_byte = *buffer;
		*buffer++ = dchr;

		if (++kidx >= klen)
			kidx = 0;
	}
}

/*------------------------------------------------------------------------
   GLB_FindFile() - Finds a file, opens it, and stores it's path
 ------------------------------------------------------------------------*/
static FILE*
GLB_FindFile(
	int	return_on_failure,	         // INPUT : Don't bomb if file not open
	int	filenum,                     // INPUT : file number
	const char *permissions		     // INPUT : file access permissions
)
{
	const char* routine = "GLB_FindFile";
	char filename[PATH_MAX];
	FILE *handle;
	FILEDESC* fd;

	/*
	* Scott, the ASSERT function is a macro that will be compiled-out when
	* not debugging ( DEBUG not defined ), otherwise it calls EXIT_Assert
	* if the expression is false ( which bails out of your program and displays
	* the expression, file & line number of the assertion ).  This is a standard
	* way of checking variables for correct boundries during development, and
	* makes it easy to remove extra logic for doing so by recompiling without
	* DEBUG defined.
	*/
	ASSERT(filenum >= 0 && filenum < MAX_GLB_FILES);

	/*
	* create a file name and attempt to open it local first, then if it
	* fails use the exe path and try again.
	*/
	sprintf(filename, "%s%04u.GLB", prefix, filenum);
	if ((handle = fopen(filename, permissions)) == NULL)
	{
		sprintf(filename, "%s%s%04u.GLB", exePath, prefix, filenum);

		if ((handle = fopen(filename, permissions)) == NULL)
		{
			if (return_on_failure)
				return NULL;

			sprintf(filename, "%s%04u.GLB", prefix, filenum);
			EXIT_Error("GLB_FindFile: %s, Error #%d,%s",
				filename, errno, strerror(errno));
		}
	}

	/*
	* Keep file handle
	*/
	fd = &filedesc[filenum];

	strcpy(fd->filepath, filename);
	fd->permissions = permissions;
	fd->handle = handle;

	return handle;
}

/*------------------------------------------------------------------------
   GLB_OpenFile() - Opens & Caches file handle
 ------------------------------------------------------------------------*/
static FILE*
GLB_OpenFile(
	int	return_on_failure,	     // INPUT : Don't bomb if file not open
	int	filenum,                 // INPUT : file number
	const char *permissions		 // INPUT : file access permissions
)
{
	FILEDESC* fd;

	ASSERT(filenum >= 0 && filenum < MAX_GLB_FILES);

	fd = &filedesc[filenum];

	if (fd->handle == 0)
		return GLB_FindFile(return_on_failure, filenum, permissions);
	else if (fd->permissions != permissions)
	{
		fclose(fd->handle);

		if ((fd->handle = fopen(fd->filepath, permissions)) == NULL)
		{
			if (return_on_failure)
				return NULL;

			EXIT_Error("GLB_OpenFile: %s, Error #%d,%s",
				fd->filepath, errno, strerror(errno));
		}
	}
	else
	{
		fseek(fd->handle, 0L, SEEK_SET);
	}

	return fd->handle;
}

/*------------------------------------------------------------------------
   GLB_CloseFiles() - Closes all cached files.
 ------------------------------------------------------------------------*/
static void
GLB_CloseFiles(
	void
)
{
	int j;

	for (j = 0; j < MAX_GLB_FILES; j++)
	{
		if (filedesc[j].handle)
		{
			fclose(filedesc[j].handle);
			filedesc[j].handle = 0;
		}
	}
}

/*------------------------------------------------------------------------
   GLB_NumItems() - Returns number of items in a .GLB file
 ------------------------------------------------------------------------*/
static int
GLB_NumItems(
	int filenum
)
{
	KEYFILE key;
	FILE *handle;

	ASSERT(filenum >= 0 && filenum < num_glbs);

	handle = GLB_OpenFile(1, filenum, "rb");

	if (handle == NULL)
		return 0;

	fseek(handle, 0L, SEEK_SET);

	if (!fread(&key, sizeof(KEYFILE), 1, handle))
	{
		EXIT_Error("GLB_NumItems: Read failed!");
	}

#ifdef _SCOTTGAME
	GLB_DeCrypt(serial, (uint8_t*)&key, sizeof(KEYFILE));
#endif

	return ((int)key.offset);
}

/*--------------------------------------------------------------------------
 GLB_LoadIDT() Loads a item descriptor table from a GLB file.
 --------------------------------------------------------------------------*/
static void
GLB_LoadIDT(
	FILEDESC* fd               // INPUT: file to load
)
{
	FILE *handle;
	int j;
	int k;
	int n;
	KEYFILE key[10];
	ITEMINFO* ii;

	handle = fd->handle;
	ii = fd->item;

	fseek(handle, sizeof(KEYFILE), SEEK_SET);

	for (j = 0; j < fd->items; )
	{
		k = fd->items - j;

		if (k > ASIZE(key))
			k = ASIZE(key);

		fread(key, sizeof(KEYFILE), k, handle);

		for (n = 0; n < k; n++)
		{
#ifdef _SCOTTGAME
			GLB_DeCrypt(serial, (void*)&key[n], sizeof(KEYFILE));
#endif
			if (key[n].opt == GLB_ENCODED)
				ii->flags |= ITF_ENCODED;

			ii->size = key[n].filesize;
			ii->offset = key[n].offset;
			memcpy(ii->name, key[n].name, sizeof(ii->name));
			ii++;
		}
		j += k;
	}
}

/*************************************************************************
   GLB_UseVM - Use virtual memory functions for heap managment.
 *************************************************************************/
void
GLB_UseVM(
	void
)
{
	fVmem = 1;
}

/*************************************************************************
   GLB_InitSystem() - Starts up .GLB file system
 *************************************************************************/
int			                   // Returns number of GLB resources opened.
GLB_InitSystem(
	const char* exepath,       // INPUT: Where program was run from
	int innum,                 // INPUT: MAX .GLB FILES TO LOOK FOR
	const char* iprefix        // INPUT: FILENAME PREFIX ( NULL for "FILE" )
)
{
	int	opened;
	int	filenum;
	int num;
	char* p;
	FILEDESC* fd;

	/*
	* Extract path from program source of execution.
	*/
	memset(exePath, 0, sizeof(exePath));
	strcpy(exePath, exepath);

	if ((p = strrchr(exePath, '\\')) != NULL)
		*(p + 1) = '\0';

	num_glbs = innum;
	ASSERT(num_glbs >= 1 && num_glbs <= MAX_GLB_FILES);

	if (iprefix)
	{
		ASSERT(strlen(iprefix) < sizeof(prefix) - 1);

		strcpy(prefix, iprefix);
		strupr(prefix);
	}
	memset(filedesc, 0, sizeof(filedesc));

	/*
	* Next, read in header of each file and allocate cache
	*/
	opened = 0;
	for (filenum = 0; filenum < num_glbs; filenum++)
	{
		fd = &filedesc[filenum];

		if ((num = GLB_NumItems(filenum)) != 0)
		{
			fd->items = num;

			/*
			 * Note: calloc zeros out all memory that it allocates
			*/
			fd->item = (ITEMINFO*)calloc(num, sizeof(ITEMINFO));

			if (fd->item == NULL)
				EXIT_Error("GLB_NumItems: memory ( init )");

			/*
			* Load Item Descriptor Table for file
			*/
			GLB_LoadIDT(fd);
			opened++;
		}
	}

	return (opened);
}

/*************************************************************************
 GLB_Load() Loads a file to a pointer from a .GLB file
 *************************************************************************/
int                               // RETURN : size of item read
GLB_Load(
	char* inmem,                  // INPUT: memory pointer or NULL
	int filenum,                  // INPUT: file number
	int itemnum                   // INPUT: item in file number
)
{
	FILE *handle;
	ITEMINFO* ii;

	ASSERT(filenum >= 0 && filenum < num_glbs);

	handle = filedesc[filenum].handle;

	if (handle == 0)
		return 0;

	ASSERT(itemnum < (WORD)filedesc[filenum].items);

	ii = filedesc[filenum].item;
	ii += itemnum;

	if (inmem != NULL)
	{
		if (ii->vm_mem.obj != NULL && inmem != ii->vm_mem.obj)
			memcpy(inmem, ii->vm_mem.obj, ii->size);
		else
		{
			fseek(handle, ii->offset, SEEK_SET);
			fread(inmem, ii->size, 1, handle);
#ifdef _SCOTTGAME
			if (ii->flags & ITF_ENCODED)
			{
				GLB_DeCrypt(serial, inmem, ii->size);
			}
#endif
		}
	}

	return ii->size;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*
 GLB_FetchItem() - Loads item into memory only if free core exists.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
static char*
GLB_FetchItem(
	uint32_t handle,
	FI_MODE	mode
)
{
	char* obj;
	ITEM_H itm;
	ITEMINFO* ii;

	if (handle == ~0)
	{
		EXIT_Error("GLB_FetchItem: empty handle.");
		return NULL;
	}

	itm.handle = handle;

	ASSERT(itm.id.filenum < (WORD)num_glbs);
	ASSERT(itm.id.itemnum < (WORD)filedesc[itm.id.filenum].items);

	ii = filedesc[itm.id.filenum].item;
	ii += itm.id.itemnum;

	if (mode == FI_LOCK)
		ii->flags |= ITF_LOCKED;

	if ((obj = ii->vm_mem.obj) == NULL)
	{
		ii->lock_cnt = 0;

		if (ii->size == 0)
			ii->vm_mem.obj = NULL;
		else
		{
			if (fVmem)
			{
				obj = (char*)VM_Malloc(ii->size,
					(ii->flags & ITF_LOCKED) ? NULL : &ii->vm_mem,
					(mode == FI_CACHE) ? 0 : 1);
			}
			else
			{
				obj = (char*)calloc(ii->size, sizeof(uint8_t));
			}

			if (mode == FI_LOCK)
				ii->lock_cnt = 1;

			ii->vm_mem.obj = obj;

			if (obj != NULL)
			{
				GLB_Load(obj, itm.id.filenum, itm.id.itemnum);
			}
		}
	}
	else if (mode == FI_LOCK && fVmem)
	{
		ii->lock_cnt++;
		VM_Lock(obj);
	}

	if (ii->vm_mem.obj == NULL && mode != FI_CACHE)
	{
		EXIT_Error("GLB_FetchItem: failed on %d bytes, mode=%d.", ii->size, mode);
	}

	if (mode == FI_DISCARD && fVmem)
		VM_Touch(&ii->vm_mem);

	return ii->vm_mem.obj;
}

/***************************************************************************
 GLB_CacheItem() - Loads item into memory only if free core exists.
 ***************************************************************************/
char*
GLB_CacheItem(
	int handle
)
{
	return GLB_FetchItem(handle, FI_CACHE);
}

/***************************************************************************
 GLB_GetItem() - Loads and allocates memory for a .GLB item
 ***************************************************************************/
char*
GLB_GetItem(
	int handle               // INPUT : handle of item
)
{
	return GLB_FetchItem(handle, FI_DISCARD);
}

/***************************************************************************
 GLB_LockItem () - Keeps Item From being discarded.
 ***************************************************************************/
char*
GLB_LockItem(
	int handle
)
{
	return GLB_FetchItem(handle, FI_LOCK);
}

/***************************************************************************
 GLB_UnlockItem () - Allows item to be discarded from memory.
 ***************************************************************************/
void
GLB_UnlockItem(
	int handle
)
{
	ITEM_H itm;
	ITEMINFO* ii;

	if (handle == ~0)
		return;

	itm.handle = handle;

	ASSERT(itm.id.filenum < (WORD)num_glbs);
	ASSERT(itm.id.itemnum < (WORD)filedesc[itm.id.filenum].items);

	ii = filedesc[itm.id.filenum].item;
	ii += itm.id.itemnum;

	if (ii->vm_mem.obj != NULL && fVmem)
	{
		if (ii->lock_cnt)
		{
			ii->lock_cnt--;

			if (ii->lock_cnt)
				return;
		}
		ii->flags &= ~ITF_LOCKED;
		VM_Unlock(ii->vm_mem.obj, &ii->vm_mem);
	}
	else
	{
		ii->flags &= ~ITF_LOCKED;
	}
}

/***************************************************************************
 GLB_IsLabel () - tests to see if ID is a label or an Item
 ***************************************************************************/
int                        // RETURN: TRUE = Label
GLB_IsLabel(
	int handle             // INPUT : handle of item
)
{
	ITEM_H itm;
	ITEMINFO* ii;

	if (handle == ~0)
		return 0;

	itm.handle = handle;

	ASSERT(itm.id.filenum < (WORD)num_glbs);
	ASSERT(itm.id.itemnum < (WORD)filedesc[itm.id.filenum].items);

	ii = filedesc[itm.id.filenum].item;
	ii += itm.id.itemnum;

	return (ii->size == 0 ? 1 : 0);
}

/***************************************************************************
 GLB_ReadItem() - Loads Item into user memory for a .GLB item
 ***************************************************************************/
void
GLB_ReadItem(
	int handle,                   // INPUT : handle of item
	char* mem                     // INPUT : pointer to memory
)
{
	ITEM_H   itm;
	ITEMINFO* ii;

	if (handle == ~0)
		return;

	ASSERT(mem != NULL);

	itm.handle = handle;

	ASSERT(itm.id.filenum < (WORD)num_glbs);
	ASSERT(itm.id.itemnum < (WORD)filedesc[itm.id.filenum].items);

	ii = filedesc[itm.id.filenum].item;
	ii += itm.id.itemnum;

	if (mem != NULL)
	{
		GLB_Load(mem, itm.id.filenum, itm.id.itemnum);
	}
}

/***************************************************************************
   GLB_GetItemID () - Gets Item ID from the text name
 ***************************************************************************/
int                              // RETURN: Handle
GLB_GetItemID(
	const char* in_name          // INPUT : pointer to text name
)
{
	ITEMINFO* ii;
	ITEM_H itm;
	int filenum;
	int itemnum;
	int maxloop;

	ASSERT(in_name != NULL);

	itm.handle = ~0;
	if (*in_name != ' ' && *in_name != '\0')
	{
		for (filenum = 0; filenum < num_glbs; filenum++)
		{
			maxloop = filedesc[filenum].items;
			ii = filedesc[filenum].item;

			for (itemnum = 0; itemnum < maxloop; itemnum++)
			{
				if (strcmp(ii->name, in_name) == 0)
				{
					itm.id.filenum = filenum;
					itm.id.itemnum = itemnum;
					return itm.handle;
				}
				ii++;
			}
		}
	}

	return itm.handle;
}

#if 0
/***************************************************************************
 GLB_GetPtr() - Returns a pointer to item ( handle )
 ***************************************************************************/
char*                           // RETURN: pointer to item
GLB_GetPtr(
	int handle                  // INPUT : handle of item
)
{
	ITEM_H itm;
	ITEMINFO* ii;

	if (handle == ~0)
		return NULL;

	itm.handle = handle;

	ASSERT(itm.id.filenum < (uint16_t)num_glbs);
	ASSERT(itm.id.itemnum < (uint16_t)filedesc[itm.id.filenum].items);

	ii = filedesc[itm.id.filenum].item;
	ii += itm.id.itemnum;

	return ii->memory;
}
#endif

/***************************************************************************
 GLB_FreeItem() - Frees memory for items and places items < MAX SIZE
 ***************************************************************************/
void
GLB_FreeItem(
	int handle               // INPUT : handle of item
)
{
	ITEM_H itm;
	ITEMINFO* ii;

	if (handle == ~0)
		return;

	itm.handle = handle;

	ASSERT(itm.id.filenum < (uint16_t)num_glbs);

	if (itm.id.itemnum >= (uint16_t)filedesc[itm.id.filenum].items)
	{
		EXIT_Error("GLB_FreeItem - item out of range: %d > %d file %d.\n",
			itm.id.itemnum, filedesc[itm.id.filenum].items, itm.id.filenum);
	}
	//   ASSERT( itm.id.itemnum < ( WORD ) filedesc[ itm.id.filenum ].items );

	ii = filedesc[itm.id.filenum].item;
	ii += itm.id.itemnum;

	if (ii->vm_mem.obj != NULL)
	{
		ii->flags &= ~ITF_LOCKED;

		if (fVmem)
			VM_Free(ii->vm_mem.obj);
		else
			free(ii->vm_mem.obj);

		ii->vm_mem.obj = NULL;
	}
}

/***************************************************************************
 GLB_FreeAll() - Frees All memory used by GLB items
 ***************************************************************************/
void
GLB_FreeAll(
	void
)
{
	int		filenum;
	int		itemnum;
	ITEMINFO* ii;

	for (filenum = 0; filenum < num_glbs; filenum++)
	{
		ii = filedesc[filenum].item;

		for (itemnum = 0; itemnum < filedesc[filenum].items; itemnum++)
		{
			if (ii->vm_mem.obj && (ii->flags & ITF_LOCKED) == 0)
			{
				if (fVmem)
					VM_Free(ii->vm_mem.obj);
				else
					free(ii->vm_mem.obj);

				ii->vm_mem.obj = NULL;
			}
			ii++;
		}
	}
}

/***************************************************************************
 GLB_ItemSize() - Returns Size of Item
 ***************************************************************************/
int                               // RETURN: sizeof ITEM
GLB_ItemSize(
	int handle                    // INPUT : handle of item
)
{
	ITEM_H itm;
	ITEMINFO* ii;

	if (handle == ~0)
		return 0;

	itm.handle = handle;

	ASSERT(itm.id.filenum < (WORD)num_glbs);
	ASSERT(itm.id.itemnum < (WORD)filedesc[itm.id.filenum].items);

	ii = filedesc[itm.id.filenum].item;
	ii += itm.id.itemnum;

	return ii->size;
}

/***************************************************************************
   GLB_ReadFile () reads in a normal file
 ***************************************************************************/
int                             // RETURN: size of record
GLB_ReadFile(
	const char* name,           // INPUT : filename
	char* buffer                // OUTPUT: pointer to buffer or NULL
)
{
	char fqp[PATH_MAX];
	FILE *handle;
	uint32_t sizerec;

	if (access(name, 0) == -1)
	{
		strcpy(fqp, exePath);
		strcat(fqp, name);
		name = fqp;
	}

	if ((handle = fopen(name, "rb")) == NULL)
		EXIT_Error("LoadFile: Open failed!");

	fseek(handle, 0, SEEK_END);
	sizerec = ftell(handle);
	rewind(handle);

	if (buffer && sizerec)
	{
		if (!fread(buffer, sizerec, 1, handle))
		{
			fclose(handle);
			EXIT_Error("GLB_LoadFile: Load failed!");
		}
	}

	fclose(handle);

	return (sizerec);
}

/***************************************************************************
   GLB_SaveFile () saves buffer to a normal file ( filename )
 ***************************************************************************/
void
GLB_SaveFile(
	char* name,                // INPUT : filename
	char* buffer,              // INPUT : pointer to buffer
	int length                 // INPUT : length of buffer
)
{
	FILE *handle;

	if ((handle = fopen(name, "wb")) == NULL)
		EXIT_Error("SaveFile: Open failed!");

	if (length)
	{
		if (!fwrite(buffer, length, 1, handle))
		{
			fclose(handle);
			EXIT_Error("GLB_SaveFile: Write failed!");
		}
	}

	fclose(handle);
}
