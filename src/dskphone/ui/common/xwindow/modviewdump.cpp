#include "modviewdump.h"
#include "viewdumpmgr.h"


#if IF_XWINDOW_DUMP_EXPORT
int ViewDump_RegisterClass(const char* pszClassName)
{
    return g_ViewDumpMgr.RegisterClass(pszClassName);
}

bool ViewDump_RegisterDataFormat(int nDataType, FUN_DATA_FORMAT pFunDataFormat)
{
    return g_ViewDumpMgr.RegisterDataFormat(nDataType, pFunDataFormat);
}

bool ViewDump_RegisterViewDumpClass(const char* pszClassName, CViewDumpClassBase* pViewDumpClass)
{
    return g_ViewDumpMgr.RegisterViewDumpClass(pszClassName, pViewDumpClass);
}

bool ViewDump_FormatData(int nDataType, void* pData, yl::string& strData)
{
    return g_ViewDumpMgr.FormatData(nDataType, pData, strData);
}
#endif
