
#pragma once

typedef	void(*PFN_ON_GMEM_TRAVERSAL)(DWORD dwParam, const void* pAddr, const DWORD dwSize, const BOOL bIsFree);

BOOL GMem_IsActive(void);
DWORD GMem_GetPageSize(void);
DWORD GMem_SetPageSize(DWORD dwSize);
DWORD GMem_GetPageNumber(void);
DWORD GMem_GetTotalBytes(void);
DWORD GMem_SetTotalBytes(DWORD dwBytes);
DWORD GMem_GetUsedBytes(void);
DWORD GMem_GetAvailPhyBytes(DWORD* pdwTotalPhyBytes);
DWORD GMem_GetPhyPageSize(void);
DWORD GMem_GetUsedPhyPageNumber(void);
DWORD GMem_GetUsedPhyBytes(void);
DWORD GMem_GetSysUsedBytes(void);

void GMem_Traversal(DWORD dwParam, PFN_ON_GMEM_TRAVERSAL pOnProc);

void* GMem_Alloc(DWORD dwSize);
void GMem_Free(void* pMem);

void GMem_Create(void);
void GMem_Destroy(void);


  