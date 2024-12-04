#include "DllHelper.h"

static LIST g_DllList;
static BOOLEAN g_IsDllListInitialized = FALSE;



const WCHAR* g_Ntdll = L"NTDLL";
const WCHAR* g_User32 = L"USER32";

BOOLEAN SeInitializeDll(void)
{
	INITIALIZE_LIST(&g_DllList);   //将结构内存清空
	g_IsDllListInitialized = TRUE;

	if (!SeLoadDll(g_Ntdll))         //内存映射
	{
		return FALSE;
	}
	if (!SeLoadDll(g_User32))
	{
		return FALSE;
	}

	return TRUE;
}

PDLL_ENTRY SeLoadDll(const WCHAR* ImageName)
{

	PDLL_ENTRY pDllEntry = NULL;


	//static const WCHAR* _DotDll = L".dll";
	NTSTATUS Status;

	WCHAR path[128];
	UNICODE_STRING uni;
	OBJECT_ATTRIBUTES objattrs;
	IO_STATUS_BLOCK MyIoStatusBlock;
	FILE_STANDARD_INFORMATION info;
	IMAGE_DOS_HEADER* dos_hdr;
	IMAGE_DATA_DIRECTORY* data_dirs;
	ULONG LastError;
	SIZE_T ViewSize;


	pDllEntry = GET_LIST_HEAD(&g_DllList);
	while (pDllEntry) {
		if (_wcsicmp(pDllEntry->ImageBase, ImageName) == 0)
			return pDllEntry;
		pDllEntry = GET_LIST_FLINK(pDllEntry);
	}

	return pDllEntry;
}
