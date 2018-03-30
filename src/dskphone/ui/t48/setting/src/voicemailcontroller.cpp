#include "voicemailcontroller.h"
#include <ylstring.h>
#include <ylvector.h>
#include "common/common_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "setting/include/common.h"
#include "lamp/powerled/include/powerledmessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "commonapi/stringhelperapi.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "account/include/accountmessagedefine.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "voicemailmodel.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "settingui/include/modsettingui.h"
// #include "define/phonestatemessagedefine.h"
#include "lamp/backlight/include/modbacklight.h"
#include "account/include/account_datadefine.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CVoiceMailController.
CVoiceMailController * CVoiceMailController::m_pInst = NULL;
bool CVoiceMailController::m_bDelayVoiceMailTone = true;
bool CVoiceMailController::m_bVoicePoping = false;
// Static.
CVoiceMailController * CVoiceMailController::GetInstance()
{
    // 实例化单键.
    if (m_pInst == NULL)
    {
        m_pInst = new CVoiceMailController();

        if (m_pInst != NULL)
        {
            m_pInst->MyInit();
        }
    }

    return m_pInst;
}

// Static.
void CVoiceMailController::ReleaseInstance()
{
    // 销毁单键.
    if (m_pInst != NULL)
    {
        m_pInst->MyFinal();

        delete m_pInst;
        m_pInst = NULL;
    }
}

bool CVoiceMailController::GetConfigList(VoiceMailConfigListData * pData) const
{
    if (pData == NULL)
    {
        return false;
    }

    // 从Model中获取数据.
    return m_pModel != NULL ? m_pModel->GetConfigList(pData) : false;
}

void CVoiceMailController::SetConfigList(VoiceMailConfigListData & refData)
{
    // 向Model中设置数据.
    if (m_pModel != NULL)
    {
        m_pModel->SetConfigList(refData);
    }
}

bool CVoiceMailController::GetInfoList(VoiceMailInfoListData * pData) const
{
    if (pData == NULL)
    {
        return false;
    }

    // 从Model获取当前的Voice Mail.
    return m_pModel != NULL ? m_pModel->GetInfoList(pData) : false;
}

// 是否有未读的VoiceMail
bool CVoiceMailController::HasUnRead()
{
    return m_pModel != NULL ? m_pModel->HasUnRead() : false;
}

// 获取所有账号未读Voice Mail的数量
int CVoiceMailController::GetTotalUnRead() const
{
    int iConut = 0;
    VoiceMailInfoListData vomdata;
    if (GetInfoList(&vomdata))
    {
        for (YLList<VoiceMailInfoData>::const_iterator iter = vomdata.m_listInfo.begin();
                iter != vomdata.m_listInfo.end(); ++iter)
        {
            iConut += (*iter).m_nUnReadCount;
        }
    }

    return iConut;
}

void CVoiceMailController::ClickInfoByIndex(int nIndex)
{
    // 获取下标对应的Voice Mail信息.
    VoiceMailInfoData stVMID;
    if (m_pModel == NULL || !m_pModel->GetInfoByAccountId(&stVMID, nIndex))
    {
        return;
    }

    // 只处理账号注册成功则情况.
    if (stVMID.m_nAccountStaus == LS_REGISTERED
            || stVMID.m_nAccountStaus == LS_SYSTEM_DEFAULT_ACCOUNT)
    {
        VoiceMailConfigData stVMCD;
        m_pModel->GetMailNumberByAccountId(&stVMCD, nIndex);
        if (!stVMCD.m_strMailNumber.empty())
        {
            // 呼出.
            talklogic_CallOut(stVMCD.m_strMailNumber, "", nIndex);
        }
        else
        {
            // 其他情况不能呼出.
        }
    }
}

// Message按键处理
bool CVoiceMailController::OnMsgKeyPress()
{
    if (m_pModel == NULL)
    {
        return false;
    }

    VoiceMailInfoListData tInfoList;
    if (!m_pModel->GetInfoList(&tInfoList))
    {
        return false;
    }

    YLList<VoiceMailInfoData>::const_iterator citer = tInfoList.m_listInfo.begin();
    for (int nIndex = 0; citer != tInfoList.m_listInfo.end(); ++citer, nIndex++)
    {
        const VoiceMailInfoData & refVMID = *citer;
        if (refVMID.m_nAccountStaus != LS_SYSTEM_DEFAULT_ACCOUNT)
        {
            continue;
        }

        //语音信箱号码呼出。
        SettingUI_VoiceMailCallOut(nIndex);

//      if (refVMID.m_strMailNumber.empty())
//      {
//          // 调用UI的接口进入设置账号VoiceMailNumber的界面
//          SETTING_WARN("Warning! No UI interface to set VoiceMailNumber, Skipped !!");
//      }
//      else
//      {
//          // 呼出VoiceMail
//          talkLogic_CallOut(refVMID.m_strMailNumber, "", nIndex);
//      }
        break;
    }  // for.

    return true;
}

CVoiceMailController::CVoiceMailController()
    : m_pModel(NULL)
{

}

CVoiceMailController::~CVoiceMailController()
{

}

bool CVoiceMailController::MyInit()
{
    // 创建Model实例.
    if (m_pModel == NULL)
    {
        m_pModel = new CVoiceMailModel();
    }

    return m_pModel != NULL;
}

bool CVoiceMailController::MyFinal()
{
    // 销毁Model实例.
    if (m_pModel != NULL)
    {
        delete m_pModel;
        m_pModel = NULL;
    }

    return m_pModel == NULL;
}

}
