#include "menutitlemanager.h"

namespace NS_MENU
{
//////////////////////////////////////////////////////////////////////////
// Class CMenuTitleManager.
CMenuTitleManager * CMenuTitleManager::m_pInst = NULL;

// Static
CMenuTitleManager * CMenuTitleManager::GetInstance()
{
    // 创建实例.
    if (m_pInst == NULL)
    {
        m_pInst = new CMenuTitleManager();
    }
    return m_pInst;
}

// Static
void CMenuTitleManager::ReleaseInstance()
{
    // 销毁单键.
    if (m_pInst != NULL)
    {
        delete m_pInst;
        m_pInst = NULL;
    }
}

void CMenuTitleManager::SetTitle(const yl::string & strClickAction,
                                 const yl::string & strTitle)
{
    // 新增title.
    m_mapTitle[strClickAction] = strTitle;
}

void CMenuTitleManager::RemoveTitle(const yl::string & strClickAction)
{
    // 重置title.
    m_mapTitle[strClickAction] = "";
}

yl::string CMenuTitleManager::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    // 获取title.
    TitleMapCIter citer = m_mapTitle.find(strClickAction);
    return citer != m_mapTitle.end() ? citer->second : "";
}

CMenuTitleManager::CMenuTitleManager()
{

}

CMenuTitleManager::~CMenuTitleManager()
{

}

}  // namespace NS_MENU
