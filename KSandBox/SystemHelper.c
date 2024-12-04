#include "SystemHelper.h"

ULONG			g_OsVersion = 0;
ULONG			g_BuildNumber = 0;
unsigned int	g_TrapFrameOffset = 0;
BOOLEAN SeCheckOsVersion(void)
{
	ULONG MajorVersion, MinorVersion;

#ifdef _WIN64
	const ULONG BASE_MAJOR_VERSION = 6;		//主版本号基准值
	const ULONG BASE_MINOR_VERSION = 0;		//次版本号基准值
#else
	const ULONG BASE_MAJOR_VERSION = 5;
	const ULONG BASE_MINOR_VERSION = 1;
#endif

	PsGetVersion(&MajorVersion, &MinorVersion, &g_BuildNumber, NULL);

	if (MajorVersion > BASE_MAJOR_VERSION ||
		(MajorVersion == BASE_MAJOR_VERSION
			&& MinorVersion >= BASE_MINOR_VERSION)) {

		if (MajorVersion == 10) {
			g_OsVersion = WINDOWS_10;
#ifdef _WIN64
			__TrapFrameOffset = 0x90;
#endif
		}
		else if (MajorVersion == 6) {

			if (MinorVersion == 3 && g_BuildNumber >= 9600) {
				g_OsVersion = WINDOWS_81;
#ifdef _WIN64
				__TrapFrameOffset = 0x90;
#endif
			}
			else if (MinorVersion == 2 && g_BuildNumber >= 9200) {
				g_OsVersion = WINDOWS_8;
#ifdef _WIN64
				__TrapFrameOffset = 0x90;
#endif
			}

			else if (MinorVersion == 1 && g_BuildNumber >= 7600) {
				g_OsVersion = WINDOWS_7;
#ifdef _WIN64
				__TrapFrameOffset = 0x1d8;
#endif
			}
			else if (MinorVersion == 0 && g_BuildNumber >= 6000) {
				g_OsVersion = WINDOWS_VISTA;
				g_TrapFrameOffset = 0x00;
			}

		}
		else {
			g_TrapFrameOffset = 0x00;

			if (MinorVersion == 2)
				g_OsVersion = WINDOWS_2003;

			else if (MinorVersion == 1)
				g_OsVersion = WINDOWS_XP;
		}

		if (g_OsVersion)
		{
			return TRUE;
		}
	}
	return FALSE;
}