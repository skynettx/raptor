#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include "vmemapi.h"

#define MAX_BLOCKS  63

typedef struct
{
    uint32_t size;
    VM_OWNER* owner;
}MCBL;

typedef struct
{
    uint8_t* blk[MAX_BLOCKS];
    MCBL* last_mcb[MAX_BLOCKS];
    uint32_t blocks;
    MCBL* rover;
    uint32_t discarded;
    uint32_t age;
}POOL;

#define BLK_FREE    0x80000000L
#define BLK_SIZE    0x00FFFFFFL
#define BLK_ID      0x7F000000L

static POOL pool;

/*************************************************************************
 VM_InitMemory - Assign memory block to be used as virtual memory.
               - Can be called multiple times to add additional memory.
 *************************************************************************/
void
VM_InitMemory(
    char* memory,           // INPUT : Memory to be added to the pool
    int size                // INPUT : Size of memory
)
{
    MCBL* mcb;

    ASSERT(pool.blocks < MAX_BLOCKS);
    ASSERT(size > 1024);

    pool.blk[pool.blocks] = (uint8_t*)memory;
    
    /*
    * Create 1st Memory Block, the size of the block does include
    * the MCB header.
    */
    mcb = (MCBL*)memory;
    mcb->size = size - sizeof(MCBL);
    mcb->size |= BLK_FREE;
    mcb->size |= pool.blocks << 24;
    mcb->owner = NULL;
    if (pool.rover == NULL)
        pool.rover = mcb;
    
    /*
    * Create Last Memory Block
    */
    mcb = (MCBL*)((uint8_t*)mcb + (mcb->size & BLK_SIZE));
    mcb->size = 0;
    mcb->owner = NULL;
    pool.last_mcb[pool.blocks] = mcb;

    /*
    * Chain previous last MCB to this block
    */
    if (pool.blocks > 0)
    {
        mcb = pool.last_mcb[pool.blocks - 1];
        mcb->size = pool.blocks << 24;
    }
    pool.blocks++;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 vm_ColaceMem - Colace small fragments of memory into larger blocks.
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
uint32_t
vm_ColaceMem(
    MCBL* mcb
)
{
    MCBL* next_mcb;
    uint32_t mcb_size;

    mcb_size = (uint32_t)(mcb->size & BLK_SIZE);
    
    /*
    * Merge all free contiguous blocks.
    */
    next_mcb = (MCBL*)((uint8_t*)mcb + mcb_size);
    
    while (next_mcb->size & BLK_FREE)
    {
        mcb_size += (uint32_t)(next_mcb->size & BLK_SIZE);
        next_mcb = (MCBL*)((uint8_t*)mcb + mcb_size);
    }
    mcb->size = (uint32_t)(BLK_FREE | (mcb->size & ~BLK_SIZE) | mcb_size);
    
    return mcb_size;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 vm_DiscardMem - Disacard infrequently used memory
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
MCBL*
vm_DiscardMem(
    uint32_t size
)
{
    MCBL* mcb;
    MCBL* free_mcb;
    VM_OWNER* owner;
    uint32_t mcb_size;
    bool mem_freed;
    uint32_t lowcnt;
    uint32_t lowsize;
    uint32_t oldage;
    MCBL* low_mcb;

    do
    {
        oldage = lowsize = lowcnt = UINT32_MAX;
        low_mcb = NULL;
        mcb = (MCBL*)(pool.blk[0]);
        mem_freed = 0;
        pool.age--;

        for (;; )
        {
            mcb_size = mcb->size & BLK_SIZE;
            if (mcb_size == 0)
            {
                
                /*
                * At end of block, use the block # encoded in the size to
                * advance (or wrap) to the next block of memory.
                */
                mcb_size = mcb->size >> 24;
                mcb = (MCBL*)(pool.blk[mcb_size]);
                if (mcb_size == 0)
                {
                    if (low_mcb != NULL)
                    {
                        pool.discarded++;
                        low_mcb->owner->obj = NULL;
                        vm_ColaceMem(low_mcb);
                        return low_mcb;
                    }
                    break;
                }
                continue;
            }

            if (!(mcb->size & BLK_FREE))
            {
                if ((owner = mcb->owner) != NULL)
                {
                    /*
                    * Keep track of the least recently used memory block that is
                        * large enough to use as we are searching for a free block.
                    */
                    if (owner->age)
                        owner->age--;

                    if (mcb_size >= size &&
                        (owner->age < lowcnt ||
                            (owner->age == lowcnt && lowsize > mcb_size)
                            )
                        )
                    {
                        lowsize = mcb_size;
                        lowcnt = owner->age;
                        low_mcb = mcb;
                    }
                    else if (owner->age < oldage)
                    {
                        oldage = owner->age;
                    }
                }
            }
            mcb = (MCBL*)((uint8_t*)mcb + mcb_size);
        }
        
        /*
        * Now go through and colace the heap
        */
        pool.rover = mcb = (MCBL*)(pool.blk[0]);
        free_mcb = NULL;
        oldage += 2;

        for (;; )
        {
            mcb_size = (mcb->size & BLK_SIZE);
            
            if (mcb->size & BLK_FREE)
            {
                free_mcb = mcb;
                mcb_size = vm_ColaceMem(mcb);
                if (mcb_size >= size)
                    return mcb;

                mcb = (MCBL*)((uint8_t*)mcb + mcb_size);
                continue;
            }
            else if (mcb_size)
            {
                if (mcb->owner != NULL && mcb->owner->age <= oldage)
                {
                    pool.discarded++;
                    mcb->size |= BLK_FREE;
                    mcb->owner->obj = NULL;
                    mcb->owner = NULL;
                    mem_freed = 1;
                    
                    if (free_mcb)
                    {
                        mcb = free_mcb;
                        continue;
                    }
                    free_mcb = mcb;
                }
                else
                {
                    free_mcb = NULL;
                }
                mcb = (MCBL*)((uint8_t*)mcb + (mcb_size & BLK_SIZE));
            }
            else
            {
                /*
                * At end of block, use the block # encoded in the size to
                * advance (or wrap) to the next block of memory.
                */
                free_mcb = NULL;
                mcb_size = mcb->size >> 24;
                mcb = (MCBL*)(pool.blk[mcb_size]);
                
                if (mcb_size == 0)
                {
                    break;
                }
            }
        }
    } while (mem_freed == 1);
    return NULL;
}

/*************************************************************************
 VM_Malloc - Allocates a block of memory - swapping out other blocks if
             needed.
 *************************************************************************/
void*
VM_Malloc(
    uint32_t size,       // INPUT : Size of object
    VM_OWNER* owner,     // INPUT : Owner Structure, NULL=Locked
    bool discard		 // INPUT : Discard memory to satisfy request.
)
{
    MCBL* mcb;
    MCBL* next_mcb;
    uint32_t mcb_size;
    bool all_locked;

    ASSERT(pool.blocks > 0);
    
    /*
    * Round block up to next DWORD in size, and add in overhead of MCB
    */
    size = ((size + 3) & ~3) + sizeof(MCBL);

    /*
    * Search for free memory block across all pools...
    */
    all_locked = 1;

    mcb = pool.rover;
    
    do
    {
        while (!(mcb->size & BLK_FREE))
        {
            mcb_size = (uint32_t)(mcb->size & BLK_SIZE);
            
            if (all_locked == 1 && mcb->owner != NULL && discard)
                all_locked = 0;

            if (mcb_size > 0)
                mcb = (MCBL*)((uint8_t*)mcb + mcb_size);
            else
            {
                
                /*
                * At end of block, use the block # encoded in the size to
                * advance (or wrap) to the next block of memory.
                */
                mcb = (MCBL*)(pool.blk[mcb->size >> 24]);
            }

            if (mcb == pool.rover)
            {
                if (all_locked == 1)
                    return NULL;
                /*
                * Walked all of memory with no luck, start discarding least recently
                * used memory.
                */
                if ((mcb = vm_DiscardMem(size)) == NULL)
                    return NULL;

                mcb_size = mcb->size & BLK_SIZE;
                goto FOUND_MCB;
            }
        }
        mcb_size = vm_ColaceMem(mcb);
        
        if (mcb_size < size)
        {
            
            /*
            * Block is not big enough to satisfy request, goto next block
            */
            mcb = (MCBL*)((uint8_t*)mcb + mcb_size);
        }
    } while (mcb_size < size && mcb != pool.rover);

    if (mcb_size < size)
        return NULL;

FOUND_MCB:
    if (mcb_size - size < (sizeof(MCBL) + 4))
    {
        size = mcb_size;
        next_mcb = (MCBL*)((uint8_t*)mcb + size);
    }
    else
    {
        next_mcb = (MCBL*)((uint8_t*)mcb + size);
        next_mcb->size = (uint32_t)(BLK_FREE | (mcb->size & BLK_ID) | (mcb_size - size));
        next_mcb->owner = NULL;
    }
    pool.rover = next_mcb;
    mcb->size = (uint32_t)((mcb->size & BLK_ID) | size);
    mcb->owner = owner;
    
    if (owner != NULL)
    {
        owner->obj = (char*)mcb + sizeof(MCBL);
        owner->age = ++pool.age;
    }
    
    return (void*)((uint8_t*)mcb + sizeof(MCBL));
}

/*************************************************************************
 VM_Touch - touch a peice of memory to keep track of most recently used.
 *************************************************************************/
void
VM_Touch(
    VM_OWNER* owner		  // INPUT : Owner of memory to touch.
)
{
    if (owner)
    {
        owner->age = ++pool.age;
    }
}

/*************************************************************************
 VM_Free - frees a block of memory allocated by VM_Malloc
 *************************************************************************/
void
VM_Free(
    char* mem             // INPUT : Memory Object to Free
)
{
    MCBL* mcb;

    if (mem != NULL)
    {
        mcb = (MCBL*)((uint8_t*)mem - sizeof(MCBL));
        
        if (!(mcb->size & BLK_FREE))
        {
            if (mcb->owner != NULL)
                mcb->owner->obj = NULL;
            
            mcb->owner = NULL;
            vm_ColaceMem(mcb);
            pool.rover = mcb;
        }
    }
}

/*************************************************************************
 VM_Lock - Locks a block of memory from being swapped out.
 *************************************************************************/
void
VM_Lock(
    char* mem            // INPUT : Memory Object to Free
)
{
    MCBL* mcb;

    ASSERT(mem != NULL);

    mcb = (MCBL*)((uint8_t*)mem - sizeof(MCBL));
    
    if (!(mcb->size & BLK_FREE))
    {
        mcb->owner = NULL;
    }
}

/*************************************************************************
 VM_Unlock - Unlocks a block of memory allowing it to be swapped out.
 *************************************************************************/
void
VM_Unlock(
    char* mem,           // INPUT : Memory Object to Free
    VM_OWNER* owner      // INPUT : Owner Structure, NULL=Locked
)
{
    MCBL* mcb;

    if (mem != NULL)
    {
        mcb = (MCBL*)((uint8_t*)mem - sizeof(MCBL));
        
        if (!(mcb->size & BLK_FREE))
        {
            mcb->owner = owner;
            
            if (owner != NULL)
            {
                owner->obj = (char*)mcb + sizeof(MCBL);
                owner->age = pool.age;
            }
        }
    }
}
