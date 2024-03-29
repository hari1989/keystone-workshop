/*
 * multicoreLoopback_osal.c
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  =========================== REVISION INFORMATION ===========================
 *  Revision Information
 *   Changed: $LastChangedDate: 2012-02-09 10:29:24 -0500 (Thu, 09 Feb 2012) $
 *   Revision: $LastChangedRevision: 19 $
 *   Last Update By: $Author: danrinkes $
 *  ============================================================================
 *
        1         2         3         4         5         6          7
12345678901234567890123456789012345678901234567890123456789012345678901234567890
*/
#include <multicoreLoopback_osal.h>
#include <xdc/std.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/heaps/HeapBuf.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/cfg/global.h>
#include <ti/drv/srio/srio_drv.h>

/* CSL Include Files */
#include <ti/csl/csl_chip.h>
#include <ti/csl/csl_semAux.h>
#include <ti/csl/csl_cacheAux.h>
#include <ti/csl/csl_xmcAux.h>

/* IPC includes */ 
#include <ti/ipc/GateMP.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/ListMP.h>
#include <ti/ipc/SharedRegion.h>

/**********************************************************************
 ************************** Local Definitions *************************
 **********************************************************************/

#define CPPI_HW_SEM     		1
#define QMSS_HW_SEM     		2
#define SRIO_HW_SEM     		3

#define MAX_MEM_MGR_ENTRIES     30

typedef struct MEM_MGMT_ENTRY
{
    uint8_t*    ptrMemory;
    uint32_t    isFree;
}MEM_MGMT_ENTRY;

MEM_MGMT_ENTRY     gDataBufferMemMgr[MAX_MEM_MGR_ENTRIES];
int32_t            gDataBufferMemMgrMaxSize = 0;

/**********************************************************************
 ************************** Global Variables **************************
 **********************************************************************/
uint32_t malloc_counter	= 0;
uint32_t free_counter	= 0;

/**********************************************************************
 **************************** OSAL Functions **************************
 **********************************************************************/

/**
 *  @b Description
 *  @n  
 *      Utility function which converts a local address to global.
 *
 *  @param[in]  addr
 *      Local address to be converted
 *
 *  @retval
 *      Global Address
 */
static UInt32 Osal_local2Global (UInt32 addr)
{
	UInt32 corenum;

	/* Get the core number. */
	corenum = CSL_chipReadReg(CSL_CHIP_DNUM); 

	/* Compute the global address. */
	return (addr + (0x10000000 + (corenum*0x1000000)));
}

/**
 *  @b Description
 *  @n  
 *      The function is used to allocate a memory block of the specified size.
 *
 *  @param[in]  numBytes
 *      Number of bytes to be allocated.
 *
 *  @retval
 *      Allocated block address
 */
Void* Osal_srioMalloc(UInt32 numBytes)
{
	Error_Block	errorBlock;
    Void*       ptr;

    /* Increment the allocation counter. */
    malloc_counter++;	

    /* Allocate the memory. */
    ptr = Memory_alloc(NULL, numBytes, 0, &errorBlock);

    /* Return the allocated memory block. */
	return ptr;
}

/**
 *  @b Description
 *  @n  
 *      The function is used to clean up a specific memory block and is called
 *      from the SRIO Driver. 
 *
 *  @param[in]  ptr
 *      Pointer to the memory block to be cleaned up.
 *  @param[in]  size
 *      Size of the memory block being cleaned up.
 *
 *  @retval
 *      Not Applicable
 */
Void Osal_srioFree (Void* ptr, UInt32 size)
{
    /* Increment the free counter. */
    free_counter++;	
	Memory_free(NULL, ptr, size);
}

/**
 *  @b Description
 *  @n  
 *      The function is used to allocate a block of memory for all the data buffer
 *      operations. This function is called by the application.
 *
 *  @param[in]  numBuffers
 *      Number of data buffers
 *  @param[in]  dataBufferSize
 *      Size of each data buffer
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
int32_t Osal_dataBufferInitMemory(uint32_t dataBufferSize)
{
	Error_Block	errorBlock;
    uint8_t*    ptrMemory;
    uint32_t    index;

    /* Allocate memory for all the data buffers */
    ptrMemory = (uint8_t*)Memory_alloc(NULL, MAX_MEM_MGR_ENTRIES*dataBufferSize, 16, &errorBlock);
    if (ptrMemory == NULL)
        return -1;

    /* Convert to a global address */
    ptrMemory = (Void*)Osal_local2Global((UInt32)ptrMemory);
 
    /* Now we chop up the memory and add it to the memory manager. */
    for (index = 0; index < MAX_MEM_MGR_ENTRIES; index++)
    {
        /* Populate the data memory management entry. */
        gDataBufferMemMgr[index].isFree    = 1;
        gDataBufferMemMgr[index].ptrMemory = ptrMemory;

        /* Increment the memory to the next address */
        ptrMemory = ptrMemory + dataBufferSize;        
    }

    /* Remember the memory buffer size */
    gDataBufferMemMgrMaxSize = dataBufferSize;

    /* Memory Manager has been created. */
    return 0;
}

/**
 *  @b Description
 *  @n  
 *      The function is used to allocate a data buffer of the specified
 *      size. Data buffers should always be allocated from the global
 *      address space.
 *
 *  @param[in]  numBytes
 *      Number of bytes to be allocated.
 *
 *  @retval
 *      Allocated block address
 */
Void* Osal_srioDataBufferMalloc(UInt32 numBytes)
{
    uint32_t    index;
    void*       ptrMemory = NULL;

    /* Basic Validation: Ensure that the memory size requested is within range. */
    if (numBytes > gDataBufferMemMgrMaxSize)
        return NULL;

    /* Increment the allocation counter. */
    malloc_counter++;

    /* Lock out interrupts */
    Hwi_disable();

    /* Cycle through for a free entry. */
    for (index = 0; index < MAX_MEM_MGR_ENTRIES; index++)
    {
        /* Check if the entry is free or not? */
        if (gDataBufferMemMgr[index].isFree == 1) 
        {
            /* Entry was free. We can use it. */
            ptrMemory = gDataBufferMemMgr[index].ptrMemory;

            /* Mark the entry as used. */
            gDataBufferMemMgr[index].isFree = 0;

            /* We have found a match. */
            break;
        }
    }

    /* Unlock interrupts. */
    Hwi_enable();

    /* Return the allocated memory. */
    return ptrMemory;	
}

/**
 *  @b Description
 *  @n  
 *      The function is used to clean up a previously allocated data buffer 
 *      block. All data buffers are in the global address space
 *
 *  @param[in]  ptr
 *      Pointer to the data buffer block to be cleaned up
 *  @param[in]  size
 *      Size of the data buffer
 *
 *  @retval
 *      Not Applicable
 */
void Osal_srioDataBufferFree(void* ptr, uint32_t numBytes)
{
    uint32_t    index;

    /* Increment the free counter. */
    free_counter++;	

    /* Lock out interrupts */
    Hwi_disable();

    /* Cycle through and clean up */
    for (index = 0; index < MAX_MEM_MGR_ENTRIES; index++)
    {
        /* Check if the entry is free or not? */
        if (gDataBufferMemMgr[index].ptrMemory == (uint8_t*)ptr) 
        {
            /* Mark the entry as free. */
            gDataBufferMemMgr[index].isFree = 1;

            /* We have found a match. */
            break;
        }
    }

    /* Unlock interrupts. */
    Hwi_enable();
    return;
}

/**
 *  @b Description
 *  @n  
 *      The function is the SRIO OSAL Logging API which logs 
 *      the messages on the console.
 *
 *  @param[in]  fmt
 *      Formatted String.
 *
 *  @retval
 *      Not Applicable
 */
Void Osal_srioLog( String fmt, ... )
{
}

/**
 *  @b Description
 *  @n  
 *      The function is used to create a critical section.
 *
 *  @retval
 *      Semaphore Handle created
 */
Void* Osal_srioCreateSem(Void)
{
    return (Void*)Semaphore_create(0, NULL, NULL);
}

/**
 *  @b Description
 *  @n  
 *      The function is used to delete a critical section.
 *
 *  @param[in]  semHandle
 *      Semaphore handle to be deleted
 *
 *  @retval
 *      Not Applicable
 */
Void Osal_srioDeleteSem(Void* semHandle)
{
    Semaphore_delete(semHandle);
}

/**
 *  @b Description
 *  @n  
 *      The function is used to pend on a semaphore
 *
 *  @param[in]  semHandle
 *      Semaphore handle on which the API will pend
 *
 *  @retval
 *      Not Applicable
 */
Void Osal_srioPendSem(Void* semHandle)
{
    Semaphore_pend(semHandle, BIOS_WAIT_FOREVER);
}

/**
 *  @b Description
 *  @n  
 *      The function is used to post a semaphore
 *
 *  @param[in]  semHandle
 *      Semaphore handle which will be posted
 *
 *  @retval
 *      Not Applicable
 */
Void Osal_srioPostSem(Void* semHandle)
{
    Semaphore_post(semHandle);
}

/**
 *  @b Description
 *  @n  
 *      This is the Multicore OSAL Implementation to protect the driver shared
 *      resources across multiple cores.
 *
 *  @retval
 *      Semaphore Opaque Handle
 */
void* Osal_srioEnterMultipleCoreCriticalSection(void)
{
    /* Get the hardware semaphore */
    while ((CSL_semAcquireDirect (SRIO_HW_SEM)) == 0);
    return NULL;
}

/**
 *  @b Description
 *  @n  
 *      The function is called to end the critical section which was protecting
 *      shared resources from access across multiple cores.
 *
 *  @param[in]  critSectHandle
 *      Semaphore opaque handle.
 *
 *  @retval
 *      None
 */
Void  Osal_srioExitMultipleCoreCriticalSection(Void* critSectHandle)
{
    CSL_semReleaseSemaphore (SRIO_HW_SEM);
}

/**
 *  @b Description
 *  @n  
 *      The function is used to provide critical section to prevent access of shared
 *      resources from single core and multiple threads.  
 *
 *  @param[in]  drvHandle
 *      Driver Handle which needs critical section to protect its resources.
 *
 *  @retval
 *      Opaque handle
 */
Void* Osal_srioEnterSingleCoreCriticalSection(Srio_DrvHandle drvHandle)
{
    return (Void*)Hwi_disable();
}

/**
 *  @b Description
 *  @n  
 *      The function is called to end the critical section access of shared resources
 *      from single cores.
 *
 *  @param[in]  drvHandle
 *      Driver Handle which needed critical section to protect its resources.
 *  @param[in]  critSectHandle
 *      Opaque handle retreived when the Single Core Protection Enter API was called
 *
 *  @retval
 *      Not Applicable.
 */
Void Osal_srioExitSingleCoreCriticalSection(Void* critSectHandle)
{
    Hwi_enable();
}

/**
 *  @b Description
 *  @n  
 *      The function is used by the SRIO driver to indicate that
 *      its about to access a block of memory and we need to ensure
 *      that the cache contents for this block are invalidated before
 *      we try and use it.
 *
 *  @param[in]  ptr
 *      Pointer to the buffer which is being accessed
 *  @param[in]  size
 *      Size of the buffer which is to be accessed.
 *
 *  @retval
 *      None
 */
void Osal_srioBeginMemAccess(void* ptr, uint32_t size)
{
#if 0        
    CACHE_invL1d (ptr, size, CACHE_WAIT);
    /*  Cleanup the prefectch buffer also. */
    CSL_XMC_invalidatePrefetchBuffer();
#else
    UInt  key;

    /* Disable Interrupts */
    key = Hwi_disable();

    /* Cleanup the prefetch buffer also. */
    CSL_XMC_invalidatePrefetchBuffer();

    /* Invalidate the cache. */
    CACHE_invL1d (ptr, size, CACHE_FENCE_WAIT);

    /* Reenable Interrupts. */
    Hwi_restore(key);
#endif    
}

/**
 *  @b Description
 *  @n  
 *      The function is used by the SRIO driver to indicate that its 
 *      ending access to a block of memory. We need to ensure that the
 *      contents of the cache are written back to the actual memory.
 *
 *  @param[in]  ptr
 *      Pointer to the buffer 
 *  @param[in]  size
 *      Size of the buffer 
 *
 *  @retval
 *      None
 */
void Osal_srioEndMemAccess(void* ptr, uint32_t size)
{
#if 0        
    CACHE_wbL1d (ptr, size, CACHE_WAIT);
    _mfence();
#else
    UInt  key;

    /* Disable Interrupts */
    key = Hwi_disable();

    /* Writeback the cache. */
    CACHE_wbL1d (ptr, size, CACHE_FENCE_WAIT);

    /* Reenable Interrupts. */
    Hwi_restore(key);
#endif    
}

/**
 *  @b Description
 *  @n  
 *      The function is invoked by the SRIO Driver to indicate that a
 *      descriptor is being accessed. 
 *
 *  @param[in]  drvHandle
 *      Driver Instance for which descriptor is being accessed.
 *  @param[in]  ptr
 *      Pointer to the descriptor being accessed
 *  @param[in]  size
 *      Size of the descriptor (Only valid for Driver Managed)
 *
 *  @retval
 *      None
 */
void Osal_srioBeginDescriptorAccess (Srio_DrvHandle drvHandle, void* ptr, uint32_t descSize)
{
    /* In the Application all descriptor are located in LL2 memory we dont need to
     * add any special cache invalidation hook here. However if the descriptors were located
     * in SL2 (MSMC) this would be required. */
    return;
}

/**
 *  @b Description
 *  @n  
 *      The function is invoked by the SRIO Driver to indicate that a
 *      descriptor is finished being accessed.
 *
 *  @param[in]  drvHandle
 *      Driver Instance for which descriptor is being accessed.
 *  @param[in]  ptr
 *      Pointer to the descriptor being accessed
 *  @param[in]  size
 *      Size of the descriptor (Only valid for Driver Managed)
 *
 *  @retval
 *      None
 */
void Osal_srioEndDescriptorAccess (Srio_DrvHandle drvHandle, void* ptr, uint32_t descSize)
{
    /* In the Application the descriptors are located in Core 0 Global address space
     * If they are accessed from other cores we need to ensure that there is an MFENCE so 
     * that all the access to the descriptors are complete before we start pushing them
     * out. */
    _mfence();
}

/**
 *  @b Description
 *  @n  
 *      The function is used to allocate a memory block of the specified size.
 *      from shared memory.
 *
 *  @param[in]  num_bytes
 *      Number of bytes to be allocated.
 *
 *  @retval
 *      Allocated block address
 */
Ptr Osal_cppiMalloc (UInt32 num_bytes)
{
    Error_Block	errorBlock;

    /* Allocate a buffer from the default HeapMemMp */
    return Memory_alloc ((xdc_runtime_IHeap_Handle) SharedRegion_getHeap(0), num_bytes, 0, &errorBlock);
}

/**
 *  @b Description
 *  @n  
 *      The function is used to free a memory block of the specified size allocated 
 *      using Osal_cppiMalloc() API.
 *
 *  @param[in]  ptr
 *      Pointer to the memory block to be cleaned up.
 *
 *  @param[in]  size
 *      Size of the memory block to be cleaned up.
 *
 *  @retval
 *      Not Applicable
 */
Void Osal_cppiFree (Ptr ptr, UInt32 size)
{
    Memory_free ((xdc_runtime_IHeap_Handle) SharedRegion_getHeap(0), ptr, size);
}

/**
 *  @b Description
 *  @n  
 *      The function is used to enter a critical section.
 *      Function protects against 
 *      access from multiple cores 
 *      and 
 *      access from multiple threads on single core
 *
 *  @retval
 *      Handle used to lock critical section
 */
Ptr Osal_cppiCsEnter (Void)
{
    /* Get the hardware semaphore for protection against multiple core access */
    while ((CSL_semAcquireDirect (CPPI_HW_SEM)) == 0);
    return NULL;
}

/**
 *  @b Description
 *  @n  
 *      The function is used to exit a critical section 
 *      protected using Osal_cppiCsEnter() API.
 *
 *  @param[in]  CsHandle
 *      Handle for unlocking critical section.
 *
 *  @retval
 *      Not Applicable
 */
Void Osal_cppiCsExit (Ptr CsHandle)
{
    /* Release the hardware semaphore */ 
    CSL_semReleaseSemaphore (CPPI_HW_SEM);
    return;
}

/**
 *  @b Description
 *  @n  
 *      The function is used to indicate that a block of memory is 
 *      about to be accessed. If the memory block is cached then this 
 *      indicates that the application would need to ensure that the 
 *      cache is updated with the data from the actual memory.
 *
 *  @param[in]  ptr
 *       Address of memory block
 *  @param[in]  size
 *       Size of memory block
 *
 *  @retval
 *      Not Applicable
 */
void Osal_cppiBeginMemAccess (void *ptr, uint32_t size)
{
#if 0        
    /* Invalidate L1D cache and wait until operation is complete. 
     * Use this approach if L2 cache is not enabled */    
    CACHE_invL1d (ptr, size, CACHE_WAIT);
    /*  Cleanup the prefectch buffer also. */
    CSL_XMC_invalidatePrefetchBuffer();    
#else
    UInt  key;

    /* Disable Interrupts */
    key = Hwi_disable();

    /*  Cleanup the prefetch buffer also. */
    CSL_XMC_invalidatePrefetchBuffer();

    /* Invalidate the cache. */
    CACHE_invL1d (ptr, size, CACHE_FENCE_WAIT);

    /* Reenable Interrupts. */
    Hwi_restore(key);
#endif
}

/**
 *  @b Description
 *  @n  
 *      The function is used to indicate that the block of memory has 
 *      finished being accessed. If the memory block is cached then the 
 *      application would need to ensure that the contents of the cache 
 *      are updated immediately to the actual memory. 
 *
 *  @param[in]  ptr
 *       Address of memory block
 *
 *  @param[in]  size
 *       Size of memory block
 *
 *  @retval
 *      Not Applicable
 */
void Osal_cppiEndMemAccess (void *ptr, uint32_t size)
{
#if 0        
    /* Writeback L1D cache and wait until operation is complete. 
     * Use this approach if L2 cache is not enabled */    
    CACHE_wbL1d (ptr, size, CACHE_WAIT);
    _mfence();
#else
    UInt  key;

    /* Disable Interrupts */
    key = Hwi_disable();

    /* Writeback the contents of the cache. */
    CACHE_wbL1d (ptr, size, CACHE_FENCE_WAIT);

    /* Reenable Interrupts. */
    Hwi_restore(key);    
#endif    
}

/**
 *  @b Description
 *  @n  
 *      The function is used to enter a critical section.
 *      Function protects against 
 *      
 *      access from multiple cores 
 *      and 
 *      access from multiple threads on single core
 *
 *  @retval
 *      Handle used to lock critical section
 */
Ptr Osal_qmssCsEnter (Void)
{
    /* Get the hardware semaphore */
    while ((CSL_semAcquireDirect (QMSS_HW_SEM)) == 0);
    return NULL;
}

/**
 *  @b Description
 *  @n  
 *      The function is used to exit a critical section 
 *      protected using Osal_qmssCsEnter() API.
 *
 *  @param[in]  CsHandle
 *      Handle for unlocking critical section.
 *
 *  @retval
 *      Not Applicable
 */
Void Osal_qmssCsExit (Ptr CsHandle)
{
    CSL_semReleaseSemaphore (QMSS_HW_SEM);
    return;
}

/**
 *  @b Description
 *  @n  
 *      The function is used to enter a critical section.
 *      Function protects against 
 *      access from multiple threads on single core
 *
 *  @retval
 *      Handle used to lock critical section
 */
Ptr Osal_qmssMtCsEnter (Void)
{
    return NULL;
}

/**
 *  @b Description
 *  @n  
 *      The function is used to exit a critical section
 *      protected using Osal_qmssCsEnter() API.
 *
 *  @param[in]  CsHandle
 *      Handle for unlocking critical section.
 *
 *  @retval
 *      Not Applicable
 */
Void Osal_qmssMtCsExit (Ptr CsHandle)
{
    /* Release Semaphore using handle */
    return;
}

/**
 *  @b Description
 *  @n  
 *      The function is used to indicate that a block of memory is 
 *      about to be accessed. If the memory block is cached then this 
 *      indicates that the application would need to ensure that the 
 *      cache is updated with the data from the actual memory.
 *
 *  @param[in]  ptr
 *       Address of memory block
 *  @param[in]  size
 *       Size of memory block
 *
 *  @retval
 *      Not Applicable
 */
void Osal_qmssBeginMemAccess (void *ptr, uint32_t size)
{
#if 0        
    /* Invalidate L1D cache and wait until operation is complete. 
     * Use this approach if L2 cache is not enabled */    
    CACHE_invL1d (ptr, size, CACHE_WAIT);
    /*  Cleanup the prefectch buffer also. */
    CSL_XMC_invalidatePrefetchBuffer();    
#else
    UInt  key;

    /* Disable Interrupts */
    key = Hwi_disable();

    /*  Cleanup the prefetch buffer also. */
    CSL_XMC_invalidatePrefetchBuffer();    

    /* Invalidate the cache. */
    CACHE_invL1d (ptr, size, CACHE_FENCE_WAIT);

    /* Reenable Interrupts. */
    Hwi_restore(key);    
#endif
}

/**
 *  @b Description
 *  @n  
 *      The function is used to indicate that the block of memory has 
 *      finished being accessed. If the memory block is cached then the 
 *      application would need to ensure that the contents of the cache 
 *      are updated immediately to the actual memory. 
 *
 *  @param[in]  ptr
 *       Address of memory block
 *  @param[in]  size
 *       Size of memory block
 *
 *  @retval
 *      Not Applicable
 */
void Osal_qmssEndMemAccess (void *ptr, uint32_t size)
{
#if 0        
    /* Writeback L1D cache and wait until operation is complete. 
     * Use this approach if L2 cache is not enabled */    
    CACHE_wbL1d (ptr, size, CACHE_WAIT);
    _mfence();
#else
    UInt  key;

    /* Disable Interrupts */
    key = Hwi_disable();

    /* Writeback the cache. */
    CACHE_wbL1d (ptr, size, CACHE_FENCE_WAIT);

    /* Reenable Interrupts. */
    Hwi_restore(key);    
#endif    
}

