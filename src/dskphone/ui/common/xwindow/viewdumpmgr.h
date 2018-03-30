#ifndef __VIEW_DUMP_MGR_H__
#define __VIEW_DUMP_MGR_H__

#include "viewdumpcommon.h"


#if IF_XWINDOW_DUMP_EXPORT
class xTextView;
class CViewDumpMgr
{
public:
    CViewDumpMgr();
    ~CViewDumpMgr();

    static CViewDumpMgr& GetInstance();

public:
    void Init();

    int RegisterClass(const char* pszClassName);
    bool RegisterViewDumpClass(const char* pszClassName, CViewDumpClassBase* pViewDumpClass);
    bool RegisterDataFormat(int nDataType, FUN_DATA_FORMAT pFunDataFormat);
    bool FormatData(int nDataType, void* pData, yl::string& strData);

    bool DumpView(xView* pView, xView* pFocusView, bool bActive, xml_node& nodeParent);
    xTextView* GetTextView(xView* pView);

protected:
    CViewDumpClassBase* GetViewDumpClass(const yl::string& strClassName);
    yl::string GetClassNameByID(int nClassID);

protected:
    int m_nMaxViewClassID;

    typedef YLHashMap<int, yl::string> MAP_CLASS_NAME;
    MAP_CLASS_NAME m_mapClassName;

    typedef YLHashMap<yl::string, CViewDumpClassBase*> MAP_VIEW_DUMP_CLASS;
    MAP_VIEW_DUMP_CLASS m_mapViewDumpClass;

    typedef YLHashMap<int, FUN_DATA_FORMAT> MAP_FUN_DATA_FORMAT;
    MAP_FUN_DATA_FORMAT m_mapFunDataFormat;

    CViewDumpClassBase* m_pBaseClass;
};

#define g_ViewDumpMgr CViewDumpMgr::GetInstance()
#endif

#endif
