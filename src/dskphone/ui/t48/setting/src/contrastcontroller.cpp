#include "contrastcontroller.h"

#include <ETLLib/ETLLib.hpp>
#include "setting/include/common.h"

#include "basemethod.h"
#include "fwdinternationalmodel.h"
#include "interfacedefine.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CContrastController.
CContrastController * CContrastController::m_pInst = NULL;

CContrastController * CContrastController::GetInstance()
{
    // 如果单件不存在则创建
    if (m_pInst == NULL)
    {
        m_pInst = new CContrastController();
    }
    return m_pInst;
}

void CContrastController::ReleaseInstance()
{
    // 如果单件存在则释放
    if (m_pInst != NULL)
    {
        delete m_pInst;
        m_pInst = NULL;
    }
}

bool CContrastController::GetPageMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction) const
{
    if (strClickAction.empty() || pData == NULL)
    {
        return false;
    }

    // 调用父类接口
    return NS_SETTING::GetPageMenuListByClickAction(pData, strClickAction);
}

const yl::string CContrastController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // 调用父类接口
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

int CContrastController::GetCurrentContrastLevel()
{
    return configParser_GetConfigInt(kszContrastLevel);
}

void CContrastController::SetCurrentContrastLevel(int iLevel)
{
    configParser_SetConfigInt(kszContrastLevel, iLevel, CONFIG_LEVEL_PHONE);
}

CContrastController::CContrastController()
{

}

CContrastController::~CContrastController()
{

}

}  // namespace NS_SETTING
