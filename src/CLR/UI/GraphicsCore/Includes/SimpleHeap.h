
#include "nanoCLR_Types.h"

// Simple Heap is for use by Porting Kit users who need private memory allocation.
/*************************************************************************************
**
** Function: SimpleHeap_Allocate
**
** Synopsis: Initializes simple heap from supplied buffer.
** Pointer to buffer is saved in global variable.
** Later is used for allocation of blocks by SimpleHeap_Allocate
**
** Arguments: [pHeapBuffer] - Pointer to heap buffer. This pointer is saved in global variable,
**                            later used by SimpleHeap_* function.
**            [pHeapBuffer] - Size of memory block pointed by pHeapBuffer
**
**************************************************************************************/
void SimpleHeap_Initialize(void* pHeapBuffer, CLR_UINT32 heapBuufferSize);

/**********************************************************************
**
** Function: SimpleHeap_Allocate
**
** Synopsis: Allocates block of memory from heap buffer initialized by SimpleHeap_Initialize
**
**
** Arguments: [len]                  - Size of block to allocate.
**
** Returns:   Pointer to newly allocated memory
              or NULL if there is no free memory to accomodate block of size len
**********************************************************************/
void* SimpleHeap_Allocate(size_t len);

/**********************************************************************
**
** Function: SimpleHeap_Release
**
** Synopsis: Releases memory block allocated by SimpleHeap_Allocate
**
**
** Arguments: [pHeapBlock] - Memory block to release.
**
**********************************************************************/
void  SimpleHeap_Release(void*  pHeapBlock);


/**********************************************************************
**
** Function: SimpleHeap_ReAllocate
**
** Synopsis: Reallocates memory on an existing pointer and copies bck the
** data
**
** Arguments: [pHeapBlock] - Memory block to reallocate.
** Arguments: [len]        - Size of block to allocate.
**
**********************************************************************/
void* SimpleHeap_ReAllocate(void*  pHeapBlock, size_t len);

/**********************************************************************
**
** Function: SimpleHeap_IsAllocated
**
** Synopsis: Checks if pHeapBlock points to memory block allocated by SimpleHeap_Allocate
**
** Arguments: [pHeapBlock] - Memory block to release.
**
** Returns:   TRUE if pHeapBlock points to memory allocated, FALSE otherwise.
**********************************************************************/
bool  SimpleHeap_IsAllocated(void*  pHeapBlock);