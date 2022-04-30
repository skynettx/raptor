#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include "vmemapi.h"

struct poolitem_t
{
    uint32_t size : 24;
    uint32_t chain : 7;
    uint32_t status : 1;

    meminfo_t *meminfo;
};

struct pool_t
{
    poolitem_t *start[63];
    poolitem_t *end[63];
    uint32_t count;
    poolitem_t *last;
    uint32_t discard;
    uint32_t age;
};

pool_t pool;

void VM_InitMemory(char *a1, int a2)
{
    poolitem_t *pi = (poolitem_t*)a1;
    poolitem_t *pi2;
    pool.start[pool.count] = pi;
    pi->size = a2 - sizeof (poolitem_t);
    pi->status = 1;
    pi->chain = pool.count;
    pi->meminfo = NULL;
    if (!pool.last)
        pool.last = pi;
    pi2 = (poolitem_t*)(a1 + pi->size);
    pi2->size = 0;
    pi2->chain = 0;
    pi2->status = 0;
    pi2->meminfo = NULL;
    pool.end[pool.count] = pi2;
    if (pool.count > 0)
        pool.end[pool.count - 1]->chain = pool.count;
    pool.count++;
}

uint32_t vm_ColaceMem(poolitem_t *pi)
{
    uint32_t l;
    poolitem_t *pi2;
    l = pi->size;
    pi2 = (poolitem_t*)((char*)pi + l);
    while (pi2->status)
    {
        l += pi2->size;
        pi2 = (poolitem_t*)((char*)pi + l);
    }
    pi->size = l;
    pi->status = 1;
    return l;
}

poolitem_t *vm_DiscardMem(uint32_t a1)
{
    poolitem_t *v20;
    poolitem_t *pi, *pi2;
    uint32_t vb;
    uint32_t vsi;
    uint32_t vdi;
    uint32_t vcx;
    int v24;
    int nb;
    do
    {
        vsi = 0xffffffff;
        vdi = 0xffffffff;
        vcx = 0xffffffff;
        v24 = 0;
        v20 = NULL;
        pi = pool.start[0];
        pool.age--;
        do
        {
            vb = pi->size;
            if (vb == 0)
            {
                if (pi->chain != 0)
                    continue;
                if (!v20)
                    break;
                pool.discard++;
                v20->meminfo->ptr = NULL;
                vm_ColaceMem(v20);
                return v20;
            }
            if (!pi->status && pi->meminfo)
            {
                if (pi->meminfo->age)
                    pi->meminfo->age--;
                if (vb >= a1)
                {
                    if (vsi > pi->meminfo->age || vsi == pi->meminfo->age || vdi > vb)
                    {
                        vdi = vb;
                        v20 = pi;
                        vsi = pi->meminfo->age;
                    }
                    else if (vcx > pi->meminfo->age)
                        vcx = pi->meminfo->age;
                }
            }
            pi = (poolitem_t*)((char*)pi + vb);
        } while (1);
        vcx++;
        pi2 = NULL;
        pi = pool.last = pool.start[0];
        do
        {
            vb = pi->size;
            if (pi->status)
            {
                pi2 = pi;
                if (vm_ColaceMem(pi) >= a1)
                    return pi;
                pi = (poolitem_t*)((char*)pi + vb);
                continue;
            }
            if (vb)
            {
                if (pi->meminfo && vcx >= pi->meminfo->age)
                {
                    pool.discard++;
                    pi->status = 1;
                    pi->meminfo->ptr = NULL;
                    v24 = 1;
                    pi->meminfo = NULL;
                    if (pi2)
                    {
                        pi = pi2;
                        continue;
                    }
                    pi2 = pi;
                }
                else
                    pi2 = pi;
                pi = (poolitem_t*)((char*)pi + vb);
                continue;
            }
            nb = pi->chain;
            pi = pool.start[nb];
            if (!nb)
                break;
        } while (1);
    } while (!v24);
    return NULL;
}

char *VM_Malloc(unsigned int a1, meminfo_t *a2, int a3)
{
    int vsi;
    unsigned int sz;
    unsigned int df;
    poolitem_t *pi;
    a1 = ((a1 + 9) & (~9)) + 24;
    pi = pool.last;
    vsi = 1;
    while (1)
    {
        if (!pi->status)
        {
            sz = pi->size;
            if (vsi == 1)
            {
                if (pi->meminfo && a3)
                    vsi = 0;
            }
            if (sz > 0)
                pi = (poolitem_t*)((char*)pi + sz);
            else
                pi = pool.start[pi->chain];
            if (pi == pool.last)
            {
                if (vsi == 1)
                    return NULL;
                pi = vm_DiscardMem(a1);
                if (!pi)
                    return NULL;
                sz = pi->size;
                break;
            }
        }
        else
        {
            sz = vm_ColaceMem(pi);
            if (sz < a1)
                pi = (poolitem_t*)((char*)pi + sz);
            if (sz >= a1 || pi == pool.last)
            {
                if (sz < a1)
                    return NULL;
                break;
            }
        }
    }
    df = sz - a1;
    if (df < sizeof(poolitem_t) + 4)
    {
        a1 = sz;
        pool.last = (poolitem_t*)((char*)pi + sz);
    }
    else
    {
        pool.last = (poolitem_t*)((char*)pi + a1);
        pool.last->size = df;
        pool.last->chain = pi->chain;
        pool.last->status = 1;
        pool.last->meminfo = NULL;
    }
    pi->size = a1;
    pi->status = 0;
    pi->meminfo = a2;
    if (a2)
    {
        a2->ptr = (char*)(pi + 1);
        a2->age = ++pool.age;
    }
    return (char*)(pi + 1);
}

void VM_Touch(meminfo_t *a1)
{
    if (a1)
        a1->age = ++pool.age;
}

void VM_Free(char *a1)
{
    poolitem_t *pi = (poolitem_t*)a1;
    pi--;
    if (pi->status)
        return;
    if (pi->meminfo)
        pi->meminfo->ptr = NULL;
    pi->meminfo = NULL;
    vm_ColaceMem(pi);
    pool.last = pi;
}

void VM_Lock(char *a1)
{
    poolitem_t* pi = (poolitem_t*)a1;
    pi--;
    if (pi->status)
        return;
    pi->meminfo = NULL;
}

void VM_Unlock(char *a1, meminfo_t *a2)
{
    poolitem_t *pi = (poolitem_t*)a1;
    pi--;
    if (pi->status)
        return;
    pi->meminfo = a2;
    if (a2)
    {
        a2->ptr = a1;
        a2->age = pool.age;
    }
}
