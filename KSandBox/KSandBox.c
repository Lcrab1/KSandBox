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
		g_MemoryPool = SeCreateMemoryPool();   //�����ڴ��
		if (!g_MemoryPool) {
			//�����ӡ
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