#pragma once
#include <fltKernel.h>
#include <ntimage.h>
#include "ListHelper.h"

typedef struct _DLL_ENTRY_ {

	NODE  Node;
	WCHAR ImageName[32];
	HANDLE FileHandle;
	HANDLE SectionHandle;
	ULONG_PTR ImageBase;
	ULONG SizeOfImage;
	UCHAR* BaseAddress;
	IMAGE_NT_HEADERS* ImageNtHeaders;
	IMAGE_EXPORT_DIRECTORY* ImageExportDirectory;

}DLL_ENTRY, * PDLL_ENTRY;


BOOLEAN SeInitializeDll(void);
PDLL_ENTRY SeLoadDll(const WCHAR* ImageName);
