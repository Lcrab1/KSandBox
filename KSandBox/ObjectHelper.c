#include "ObjectHelper.h"

PFN_ObQueryNameInfo g_ObQueryNameInfo = NULL;
PFN_ObGetObjectType g_ObGetObjectType = NULL;


BOOLEAN SeInitializeObject(void)
{
	if (g_OsVersion >= WINDOWS_7)
	{
		UNICODE_STRING functionName;
		void* functionAddress;

		RtlInitUnicodeString(&functionName, L"ObQueryNameInfo");
		functionAddress = MmGetSystemRoutineAddress(&functionName);
		if (!functionAddress)
		{
			//输出打印
			return FALSE;
		}

		g_ObQueryNameInfo = (PFN_ObQueryNameInfo)functionAddress;

		RtlInitUnicodeString(&functionName, L"ObGetObjectType");
		functionAddress = MmGetSystemRoutineAddress(&functionName);
		if (!functionAddress) {

			//输出打印
			return FALSE;
		}

		g_ObGetObjectType = (PFN_ObGetObjectType)functionAddress;
	}

	return TRUE;
}
