#include "callcompletionidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"
#include "dsklog/log.h"

#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "voice/include/modvoice.h"
#include "talk/callcompletion/include/modcallcompletion.h"

CCallCompletionHandle::CCallCompletionHandle(int nType /*= PS_STATE_CALL_COMPLETION*/)
    : CBaseIdleHandle(nType)
{
    m_pCallCompletionData = new CallCompletionData();
}

CCallCompletionHandle::~CCallCompletionHandle()
{
    if (NULL != m_pCallCompletionData)
    {
        delete m_pCallCompletionData;
        m_pCallCompletionData = NULL;
    }
}

bool CCallCompletionHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    if (NULL == m_pCallCompletionData)
    {
        return false;
    }

    m_pCallCompletionData->Reset();
    //获取订阅内容成功
    if (CallCompletion_GetSubscribeData(m_pCallCompletionData))
    {
        yl::string strTarget = "";
        //这里只需要处理有空闲状态
        if (CCS_REMOTE_CALLAVALIABLE == m_pCallCompletionData->m_eStatus)
        {
            //如果显示名不为空使用显示名，否则使用用户名
            if (!m_pCallCompletionData->m_strCalleeDisplayName.empty())
            {
                strTarget = m_pCallCompletionData->m_strCalleeDisplayName;
            }
            else
            {
                strTarget = m_pCallCompletionData->m_strCalleeUsername;
            }
            popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_DIALING);
        }
        else if (CCS_WAIT_COMFIMING == m_pCallCompletionData->m_eStatus)
        {
            // 如果显示名不为空使用显示名，否则使用用户名
            if (!m_pCallCompletionData->m_strCalleeDisplayName.empty())
            {
                strTarget = m_pCallCompletionData->m_strCalleeDisplayName;
            }
            else
            {
                strTarget = m_pCallCompletionData->m_strCalleeUsername;
            }
            // 播放提示音
            voice_PlayTone(TONE_BUSY, 3000);
            popData.m_strNote = _UILOGIC_LANG_TRAN(TRID_WAIT_FOR);
        }

        if (strTarget.empty())
        {
            IDLEUI_INFO("CIdleNoteManager::GetCallCompletionTipInfo: no content to show and return !!!");
        }
        else
        {
            popData.m_strNote = popData.m_strNote + yl::string(" ") + strTarget + yl::string("?");
            popData.m_strTitle = _UILOGIC_LANG_TRAN(TRID_CALL_COMPLETION);
            popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //messagebox
            popData.m_vecSoftkey[0] = ST_CANCEL;
            popData.m_vecSoftkey[1] = ST_EMPTY;
            popData.m_vecSoftkey[2] = ST_EMPTY;
            popData.m_vecSoftkey[3] = ST_OK;
        }
    }
    return !popData.m_strNote.empty();
}

bool CCallCompletionHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
    unsigned int nNotifyType = GetNeedClearNotifyType();
    if (NULL == m_pCallCompletionData)
    {
        return bRet;
    }

    if (ST_OK == eSoftkey
            || PHONE_KEY_OK == eKeyCode)
    {
        if (CCS_REMOTE_CALLAVALIABLE == m_pCallCompletionData->m_eStatus)
        {
            //点击了确定，拨打号码
            CallCompletion_CallOut(m_pCallCompletionData->m_nCallId);
        }
        else if (CCS_WAIT_COMFIMING == m_pCallCompletionData->m_eStatus)
        {
            // 停止提示音
            voice_StopTone(TONE_BUSY);
            CallCompletion_Wait(m_pCallCompletionData->m_nCallId);
        }
        bRet = true;
    }
    else if (ST_CANCEL == eSoftkey || ST_EXIT == eSoftkey
             || PHONE_KEY_CANCEL == eKeyCode)
    {
        // 停止提示音
        voice_StopTone(TONE_BUSY);
        //取消.
        CallCompletion_UnsubscribeByCallId(m_pCallCompletionData->m_nCallId);
        idleScreen_ClearNotifyTypeFlag(m_iType, nNotifyType);
        bRet = true;
    }
    return bRet;
}
