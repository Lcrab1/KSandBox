#pragma once
#include <fltKernel.h>
#define WINDOWS_XP       2
#define WINDOWS_2003     3
#define WINDOWS_VISTA    4
#define WINDOWS_7        5
#define WINDOWS_8        6
#define WINDOWS_81       7
#define WINDOWS_10       8



extern ULONG g_OsVersion;
extern ULONG g_BuildNumber;
extern unsigned int g_TrapFrameOffset;


BOOLEAN SeCheckOsVersion(void);
