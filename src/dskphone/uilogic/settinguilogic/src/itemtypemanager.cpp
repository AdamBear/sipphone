#include "itemtypemanager.h"
#include "dsklog/log.h"

IMPLEMENT_GETINSTANCE(CItemTypeMgr)


CItemTypeMgr::CItemTypeMgr()
{
}

CItemTypeMgr::~CItemTypeMgr()
{
    m_mapItemType.clear();
}

bool CItemTypeMgr::AddItemType(const yl::string & strItemType, int eItemType)
{
    if (m_mapItemType.end() == m_mapItemType.find(strItemType))
    {
        m_mapItemType[strItemType] = eItemType;
        return true;
    }

    SETTINGUI_INFO("[CItemTypeMgr::AddItemType] type[%s] has exist!!!", strItemType.c_str());
    return false;
}

bool CItemTypeMgr::DelItemType(const yl::string & strItemType)
{
    m_mapItemType.erase(strItemType);
    return true;
}

int CItemTypeMgr::GetItemType(const yl::string & strItemType)
{
    return m_mapItemType[strItemType];
}

