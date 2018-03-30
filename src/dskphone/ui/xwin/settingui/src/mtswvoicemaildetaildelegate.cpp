#include "mtswvoicemaildetaildelegate.h"

#if IF_FEATURE_METASWITCH_VOICEMAIL

#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"
#include "settinguimanager.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/modmessagebox.h"

#include "account/include/modaccount.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "baselibs/include/taskaction/common.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/include/modsetuilogic.h"

namespace
{
enum
{
    MSG_ID_DELETE_MESSAGE = 1210,
    MSG_ID_LOAD_VOICEMAIL,
};
}

CMtswVoiceMailDetailDelegate::CMtswVoiceMailDetailDelegate()
{
    etl_RegisterMsgHandle(MTSW_MESSAGE_BEGIN, MTSW_MESSAGE_END,
                          &CMtswVoiceMailDetailDelegate::OnMTSWVoiceMailDetailMsg);
}

CMtswVoiceMailDetailDelegate::~CMtswVoiceMailDetailDelegate()
{
    etl_UnregisterMsgHandle(MTSW_MESSAGE_BEGIN, MTSW_MESSAGE_END,
                            &CMtswVoiceMailDetailDelegate::OnMTSWVoiceMailDetailMsg);
    MTSWVM_SetPlayLoading(false);
}

CSettingUIDelegateBase* CMtswVoiceMailDetailDelegate::CreateMtswVoiceMailDetailDelegate()
{
    CMtswVoiceMailDetailDelegate* pDelegate = new CMtswVoiceMailDetailDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CMtswVoiceMailDetailDelegate::OnMTSWVoiceMailDetailMsg(msgObject& msg)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetTopDelegate();
    if (NULL == pDelegate || kszPageMTSWVoiceMailDetail != pDelegate->GetPageID())
    {
        return FALSE;
    }

    CMtswVoiceMailDetailDelegate* pVoiceMailDetailDelegate = static_cast<CMtswVoiceMailDetailDelegate*>(pDelegate);
    if (NULL == pVoiceMailDetailDelegate)
    {
        return FALSE;
    }

    switch (msg.message)
    {
    case MTSW_MESSAGE_DOWNLOADED:
        {
            MTSWVM_SetPlayLoading(false);
            //关闭弹框
            MessageBox_RemoveAllMessageBox();
            pVoiceMailDetailDelegate->EnterPlay();
        }
        break;
    case MTSW_MESSAGE_DOWNLOADFAILED:
        {
            //关闭弹框
            MessageBox_RemoveAllMessageBox();
            if (NS_TA::TA_FILE_EXCEED_MAX_SIZE == msg.wParam)
            {
                g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailDetail, _LANG(TRID_NOTE), _LANG(TRID_DOWNLOADFILE_OUT_OF_MEMORY),
                                                       SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME);
            }
            else
            {
                g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailDetail, _LANG(TRID_NOTE), _LANG(TRID_LOAD_FAILED),
                                                       SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME);
            }
        }
        break;
    default:
        break;
    }

    return TRUE;
}

void CMtswVoiceMailDetailDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() < MIN_SETTING_SOFTKEY_NUM)
    {
        return;
    }

    vecSoftKey[1].m_iType = ST_SEND;
    vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEND);
    vecSoftKey[2].m_iType = ST_DELETE;
    vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_DELETE);
    vecSoftKey[3].m_iType = ST_PLAY;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_PLAY);
}

bool CMtswVoiceMailDetailDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    switch (iType)
    {
    case ST_SEND:
        {
            ProcessCallBack();
            return true;
        }
        break;
    case ST_DELETE:
        {
            g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailDetail, _LANG(TRID_NOTE), _LANG(TRID_IF_DELETE_MESSAGE),
                                                   SETTING_MESSAGEBOX_OkCancel, "", "", -1,
                                                   MSG_ID_DELETE_MESSAGE);
            return true;
        }
        break;
    case ST_PLAY:
        {
            ProcessPlayKey();
            return true;
        }
        break;
    default:
        break;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

bool CMtswVoiceMailDetailDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    switch (nKeyCode)
    {
    case PHONE_KEY_OK:
        {
            ProcessCallBack();
            return true;
        }
        break;
    case PHONE_KEY_LEFT:
    case PHONE_KEY_RIGHT:
        {
            SwitchMessageDetail(PHONE_KEY_RIGHT == nKeyCode);
            return true;
        }
        break;
    default:
        break;
    }
    return CSettinguiDataDelegateBase::OnKeyEvent(nKeyCode, bPress, nDataIndex);
}

void CMtswVoiceMailDetailDelegate::GetVoiceMailDetail()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    VoiceMsgItemDetailData VMIDD;
    int iIndex = MTSWVM_GetItemIndex();
    bool bRet = MTSWVM_GetMessageDetail(&VMIDD, iIndex);

    if (!bRet)
    {
        return;
    }

    MTSWVM_MarkReadStatus(STATUS_READ);

    if (VMIDD.m_bUrgent)
    {
        //设置标题
        m_pPageData->SetPageTitle(_LANG(TRID_MTSW_VOICEMAIL_URGENT_MESSAGE));
    }
    else
    {
        //设置标题
        m_pPageData->SetPageTitle(_LANG(TRID_MTSW_VOICEMAIL_MESSAGE));
    }

    AddBaseItemData(m_pPageData, _LANG(TRID_FROM),
                    VMIDD.m_strName, "", false);

    AddBaseItemData(m_pPageData, _LANG(TRID_NUMBER),
                    VMIDD.m_strNumber, "", false);

    AddBaseItemData(m_pPageData, _LANG(TRID_TIME),
                    SettingUILogic_Time2NewFormatString(VMIDD.m_tTime), "", false);

    AddBaseItemData(m_pPageData, _LANG(TRID_DURATION),
                    VMIDD.m_strDuration, "", false);

}

bool CMtswVoiceMailDetailDelegate::SwitchMessageDetail(bool bNext)
{

    int nTotalCount = MTSWVM_GetTotalCount();
    int nCurIndex = MTSWVM_GetItemIndex();
    int nSwitchIndex;

    if (!SettingUI_GetSwitchIndex(nTotalCount, nCurIndex, bNext, nSwitchIndex))
    {
        return false;
    }

    MTSWVM_SetItemIndex(nSwitchIndex);
    GetVoiceMailDetail();

    return true;
}

void CMtswVoiceMailDetailDelegate::ProcessCallBack()
{
    VoiceMsgItemDetailData VMIDD;
    int iIndex = MTSWVM_GetItemIndex();
    if (MTSWVM_GetMessageDetail(&VMIDD, iIndex))
    {
        talklogic_CallOut(VMIDD.m_strNumber, VMIDD.m_strName);
    }
}

void CMtswVoiceMailDetailDelegate::ProcessPlayKey()
{
    g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT),
                                           SETTING_MESSAGEBOX_Cancel, "", "", -1
                                           , MSG_ID_LOAD_VOICEMAIL);
    MTSWVM_SetPlayLoading(true);
    MTSWVM_DownloadVoiceMail();
    //下载中禁止自动返回Idle界面
    UIManager_LockReturnToIdle(true);
}

void CMtswVoiceMailDetailDelegate::EnterPlay()
{
    SettingUI_EnterPage(kszPageMTSWVoiceMailPlay);
}

bool CMtswVoiceMailDetailDelegate::ProcessMsgCallBack(int nResult, int nID,
        void* pExtraData /*= NULL*/)
{
    switch (nID)
    {
    case MSG_ID_DELETE_MESSAGE:
        {
            if (MessageBox_OK == nResult)
            {
                int iCurrentIndex = MTSWVM_GetItemIndex();
                int iTotal = MTSWVM_GetTotalCount() - 1;
                //删除的是最后一条则焦点上移
                if (iCurrentIndex == iTotal)
                {
                    iCurrentIndex--;
                }
                MTSWVM_Delete();
                g_SetUICallBackFun.FallBackToPrevousPage();
                //发送消息刷新界面
                etl_NotifyApp(false, MTSW_MESSAGE_VMLIST_RESULT, iCurrentIndex, 1);
            }
        }
        break;
    case MSG_ID_LOAD_VOICEMAIL:
        {
            if (MessageBox_Cancel == nResult)
            {
                MTSWVM_SetPlayLoading(false);
            }
        }
        break;
    default:
        break;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

CSettingUIPageDataBase* CMtswVoiceMailDetailDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIPageDataBase* pPageData = CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        GetVoiceMailDetail();
    }

    return pPageData;
}

#endif

