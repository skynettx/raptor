#pragma once

struct meminfo_t
{
    char* ptr;
    uint32_t age;
};

void VM_InitMemory(char *a1, int a2);
char *VM_Malloc(unsigned int a1, meminfo_t *a2, int a3);
void VM_Free(char *a1);
void VM_Lock(char *a1);
void VM_Unlock(char *a1, meminfo_t *a2);
void VM_Touch(meminfo_t *a1);
