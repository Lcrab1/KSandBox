#include"KSandBox.h"

PDRIVER_OBJECT g_DriverObject = NULL;

PPOOL g_MemoryPool = NULL;

NTSTATUS DriverEntry(
	IN  DRIVER_OBJECT* DriverObject,
	IN  UNICODE_STRING* RegistryPath)
{
	BOOLEAN IsOk = TRUE;
	g_DriverObject = DriverObject;
	g_DriverObject->DriverUnload = NULL;

	if (IsOk)
	{
		IsOk = SeCheckOsVersion();
	}
	if (IsOk) {
		g_MemoryPool = SeCreateMemoryPool();   //创建内存池
		if (!g_MemoryPool) {
			//输出打印
			IsOk = FALSE;
		}
	}
	if (IsOk)
	{
		IsOk = SeInitializeObject();
	}

	if (IsOk)
	{
		IsOk = SeInitializeDll();
	}

}