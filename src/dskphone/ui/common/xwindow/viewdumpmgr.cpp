#include "viewdumpmgr.h"
#include "xWindow.h"
#include "dsklog/log.h"


#if IF_XWINDOW_DUMP_EXPORT
CViewDumpMgr::CViewDumpMgr()
    : m_nMaxViewClassID(0)
    , m_pBaseClass(NULL)
{

}

CViewDumpMgr::~CViewDumpMgr()
{

}

CViewDumpMgr& CViewDumpMgr::GetInstance()
{
    static CViewDumpMgr objViewDumpMgr;

    return objViewDumpMgr;
}

void CViewDumpMgr::Init()
{
    RegisterDataFormat(VIEW_DUMP_DATA_TYPE_INT, &FormatIntData);
    RegisterDataFormat(VIEW_DUMP_DATA_TYPE_LONG, &FormatLongData);
    RegisterDataFormat(VIEW_DUMP_DATA_TYPE_BOOL, &FormatBoolData);
    RegisterDataFormat(VIEW_DUMP_DATA_TYPE_RECT, &FormatRectData);
    RegisterDataFormat(VIEW_DUMP_DATA_TYPE_WSTRING, &FormatWStringData);
}

int CViewDumpMgr::RegisterClass(const char* pszClassName)
{
    if (NULL == pszClassName)
    {
        XWINDOW_ERR("RegisterClass pszClassName is null");
        return 0;
    }

    MAP_CLASS_NAME::iterator iter = m_mapClassName.begin();
    yl::string strClassName = pszClassName;

    for (; iter != m_mapClassName.end(); ++iter)
    {
        if (strClassName == iter->second)
        {
            return iter->first;
        }
    }

    ++m_nMaxViewClassID;

#if VIEW_DUMP_MGR_DEBUG
    printf("Register Class:%d %s \n", m_nMaxViewClassID, strClassName.c_str());
#endif

    m_mapClassName[m_nMaxViewClassID] = strClassName;

    return m_nMaxViewClassID;
}

bool CViewDumpMgr::RegisterViewDumpClass(const char* pszClassName, CViewDumpClassBase* pViewDumpClass)
{
    if (NULL == pszClassName || NULL == pViewDumpClass)
    {
        return false;
    }

    yl::string strClassName = pszClassName;
    MAP_VIEW_DUMP_CLASS::iterator iter = m_mapViewDumpClass.find(strClassName);

    if (iter != m_mapViewDumpClass.end())
    {
        XWINDOW_ERR("RegisterViewDumpClass duplicated class name");
        return false;
    }

#if VIEW_DUMP_MGR_DEBUG
    printf("Register ViewDump Class:%s %p \n", pszClassName, pViewDumpClass);
#endif

    m_mapViewDumpClass[strClassName] = pViewDumpClass;

    if (NULL == m_pBaseClass && pViewDumpClass->GetParentClass().empty())
    {
        m_pBaseClass = pViewDumpClass;
    }

    return true;
}

bool CViewDumpMgr::RegisterDataFormat(int nDataType, FUN_DATA_FORMAT pFunDataFormat)
{
    MAP_FUN_DATA_FORMAT::iterator iter = m_mapFunDataFormat.find(nDataType);

    if (iter != m_mapFunDataFormat.end())
    {
        XWINDOW_ERR("Duplicated data formate:%d", nDataType);
        return false;
    }

#if VIEW_DUMP_MGR_DEBUG
    printf("Register Data Format:%d %p \n", nDataType, pFunDataFormat);
#endif

    m_mapFunDataFormat[nDataType] = pFunDataFormat;

    return true;
}

bool CViewDumpMgr::FormatData(int nDataType, void* pData, yl::string& strData)
{
    if (VIEW_DUMP_DATA_TYPE_STRING == nDataType)
    {
        yl::string* pString = (yl::string*)pData;

        if (NULL != pString)
        {
            strData = *pString;

            return true;
        }

        return false;
    }

    MAP_FUN_DATA_FORMAT::iterator iter = m_mapFunDataFormat.find(nDataType);

    if (iter != m_mapFunDataFormat.end())
    {
        return (*(iter->second))(pData, strData);
    }

    return false;
}

CViewDumpClassBase* CViewDumpMgr::GetViewDumpClass(const yl::string& strClassName)
{
    MAP_VIEW_DUMP_CLASS::iterator iterClass = m_mapViewDumpClass.find(strClassName);

    if (iterClass != m_mapViewDumpClass.end())
    {
        return iterClass->second;
    }

    return NULL;
}

yl::string CViewDumpMgr::GetClassNameByID(int nClassID)
{
    MAP_CLASS_NAME::iterator iter = m_mapClassName.find(nClassID);

    if (iter != m_mapClassName.end())
    {
        return iter->second;
    }

    return yl::string("");
}

bool CViewDumpMgr::DumpView(xView* pView, xView* pFocusView, bool bActive, xml_node& nodeParent)
{
    if (NULL == pView || !pView->checkVisible())
    {
        return false;
    }

    yl::string strClassName = GetClassNameByID(pView->m_nClassID);

    if (strClassName.empty())
    {
        return false;
    }

    xml_node nodeView = nodeParent.append_child(strClassName.c_str());

    if (bActive)
    {
        if (NULL == pView->m_pParent)
        {
            nodeView.append_attribute("active", 1);
        }

        if (pFocusView == pView)
        {
            nodeView.append_attribute("focus", 1);
        }
    }

    int nLoop = 0;

    while (!strClassName.empty())
    {
        DEAD_LOOP_BREAK(nLoop)

        CViewDumpClassBase* pViewDumpClass = GetViewDumpClass(strClassName);

        if (NULL == pViewDumpClass)
        {
            if (NULL != m_pBaseClass)
            {
                m_pBaseClass->DumpProperty(pView, nodeView);
            }

            break;
        }

        pViewDumpClass->DumpProperty(pView, nodeView);

        strClassName = pViewDumpClass->GetParentClass();
    }

    for (xViewIterator itView = pView->xViewTree::child_head(); itView.hasData(); ++itView)
    {
        xView* pChildView = itView.operator ->();

        DumpView(pChildView, pFocusView, bActive, nodeView);
    }

    return true;
}

xTextView* CViewDumpMgr::GetTextView(xView* pView)
{
    if (NULL == pView)
    {
        return NULL;
    }

    yl::string strClassName = GetClassNameByID(pView->m_nClassID);

    int nLoop = 0;

    while (!strClassName.empty())
    {
        DEAD_LOOP_BREAK(nLoop)

        if ("TextView" == strClassName)
        {
            return static_cast<xTextView*>(pView);
        }

        CViewDumpClassBase* pViewDumpClass = GetViewDumpClass(strClassName);

        if (NULL == pViewDumpClass)
        {
            break;
        }

        strClassName = pViewDumpClass->GetParentClass();
    }

    return NULL;
}
#endif
