#include "fwdinternationalcontroller.h"

#include "ETLLib/ETLLib.hpp"
#include "setting/include/common.h"

#include "basemethod.h"
#include "fwdinternationalmodel.h"
#include "interfacedefine.h"
#include "etlmsghandler/modetlmsghandler.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CFWDInternationalController.
CFWDInternationalController * CFWDInternationalController::m_pInst = NULL;

CFWDInternationalController * CFWDInternationalController::GetInstance()
{
    // // 如果单件不存在则创建
    if (m_pInst == NULL)
    {
        m_pInst = new CFWDInternationalController();
        // 初始化
        if (m_pInst != NULL)
        {
            m_pInst->MyInit();
        }
    }
    return m_pInst;
}

void CFWDInternationalController::ReleaseInstance()
{
    // 单件不存在则释放
    if (m_pInst != NULL)
    {
        // Do finalization.
        m_pInst->MyFinal();

        delete m_pInst;
        m_pInst = NULL;
    }
}

bool CFWDInternationalController::GetPageMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction) const
{
    if (strClickAction.empty() || pData == NULL)
    {
        return false;
    }

    // 调用父类接口
    return NS_SETTING::GetPageMenuListByClickAction(pData, strClickAction);
}

const yl::string CFWDInternationalController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // 调用父类接口
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CFWDInternationalController::GetFWDInternationalData(FWDInternationalData * pData) const
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // 获取开关
    pData->m_isEnable = m_pModel->GetFWDInternationalEnabled();
    return true;
}

void CFWDInternationalController::SetFWDInternationalData(const FWDInternationalData & refData)
{
    if (m_pModel != NULL)
    {
        bool bCurrentEnabled = m_pModel->GetFWDInternationalEnabled();
        if (refData.m_isEnable != bCurrentEnabled)
        {
            // 设置数据
            m_pModel->SetFWDInternationalEnabled(refData.m_isEnable);
            //发送消息
            etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_FORWARD, 0);
        }
    }
}

CFWDInternationalController::CFWDInternationalController()
    : m_pModel(NULL)
{

}

CFWDInternationalController::~CFWDInternationalController()
{

}

bool CFWDInternationalController::MyInit()
{
    // Instantiate the model instance.
    if (m_pModel == NULL)
    {
        m_pModel = new CFWDInternationalModel();
    }
    return m_pModel != NULL;
}

bool CFWDInternationalController::MyFinal()
{
    // Release the model instance.
    DELETE_OBJ(m_pModel);
    return m_pModel != NULL;
}

}  // namespace NS_SETTING
