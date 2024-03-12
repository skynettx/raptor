#pragma once

#define ASSERT(exp)	((void)0)
#define ASIZE(a) (sizeof(a)/sizeof((a)[0]))

typedef struct
{
    char* obj;
    uint32_t age;
}VM_OWNER;

void VM_InitMemory(char *memory, int size);
void *VM_Malloc(uint32_t size, VM_OWNER *owner, bool discard);
void VM_Free(char *mem);
void VM_Lock(char *mem);
void VM_Unlock(char *mem, VM_OWNER *owner);
void VM_Touch(VM_OWNER *owner);
