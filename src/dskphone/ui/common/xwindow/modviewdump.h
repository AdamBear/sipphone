#ifndef __MOD_VIEW_DUMP_H__
#define __MOD_VIEW_DUMP_H__

#include "viewdumpcommon.h"

#if IF_XWINDOW_DUMP_EXPORT
int ViewDump_RegisterClass(const char* pszClassName);

bool ViewDump_RegisterDataFormat(int nDataType, FUN_DATA_FORMAT pFunDataFormat);

bool ViewDump_RegisterViewDumpClass(const char* pszClassName, CViewDumpClassBase* pViewDumpClass);

bool ViewDump_FormatData(int nDataType, void* pData, yl::string& strData);
#endif

#endif
