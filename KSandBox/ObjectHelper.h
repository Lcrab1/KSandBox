#pragma once
#include<fltKernel.h>
#include"SystemHelper.h"

typedef void* (*PFN_ObGetObjectType)(void* Object);
typedef ULONG(*PFN_ObQueryNameInfo)(void* Object);



extern PFN_ObQueryNameInfo g_ObQueryNameInfo;
extern PFN_ObGetObjectType g_ObGetObjectType;

BOOLEAN SeInitializeObject(void);
