#include "mtswacddetaildelegate.h"

#if IF_FEATURE_METASWITCH_ACD

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

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"


namespace
{
enum
{
    MSG_ID_SHOW_NOTE_BEFORE_BACK = 1100,
    MSG_ID_CANCEL_DOWNLOAD_MTSW_ACD,
    MSG_ID_LOGIN_LOGOUT_MTSW_ACD,
    MSG_ID_MULTILOGIN_MTSW_ACD,
};
#define ACDLIST_NOTE_MSGBOX_SHOW_TIME     1500
}

CMtswAcdDetailDelegate::CMtswAcdDetailDelegate()
{
    etl_RegisterMsgHandle(MTSW_ACD_MESSAGE_BEGIN, MTSW_ACD_MESSAGE_END,
                          &CMtswAcdDetailDelegate::OnMTSWAcdDetailMsg);
}

CMtswAcdDetailDelegate::~CMtswAcdDetailDelegate()
{
    etl_UnregisterMsgHandle(MTSW_ACD_MESSAGE_BEGIN, MTSW_ACD_MESSAGE_END,
                            &CMtswAcdDetailDelegate::OnMTSWAcdDetailMsg);
}

CSettingUIDelegateBase* CMtswAcdDetailDelegate::CreatMtswAcdDetailDelegate()
{
    CMtswAcdDetailDelegate* pDelegate = new CMtswAcdDetailDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CMtswAcdDetailDelegate::OnMTSWAcdDetailMsg(msgObject& msg)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetTopDelegate();
    if (NULL == pDelegate || kszPageMTSWAcdDetail != pDelegate->GetPageID())
    {
        return FALSE;
    }

    CMtswAcdDetailDelegate* pAcdDetailDelegate = static_cast<CMtswAcdDetailDelegate*>(pDelegate);
    if (NULL == pAcdDetailDelegate)
    {
        return FALSE;
    }

    switch (msg.message)
    {
    case MTSW_ACD_GET_RESULT:
        {
            SETTINGUI_INFO("CDlgMTSWAcdList MTSW_ACD_GET_RESULT w = %d", msg.wParam);
            if (EMTSWLOGDTYPE_Queues == msg.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);
                g_SetUICallBackFun.RefreshUI(true);
            }
        }
        break;
    case MTSW_ACD_GET_FAILED:
        {
            SETTINGUI_INFO("CDlgMTSWAcdList MTSW_ACD_GET_FAILED");
            if (MessageBox_GetMessageBoxCount() > 0)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_Destory);

                // 如果所有的账号都下载失败，则回退界面
                g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWAcdDetail, _LANG(TRID_NOTE), _LANG(TRID_LOAD_FAILED),
                                                       SETTING_MESSAGEBOX_NOTE, "", "", ACDLIST_NOTE_MSGBOX_SHOW_TIME,
                                                       MSG_ID_SHOW_NOTE_BEFORE_BACK);
            }
        }
        break;
    default:
        break;
    }

    return TRUE;
}

void CMtswAcdDetailDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() < MIN_SETTING_SOFTKEY_NUM)
    {
        return;
    }

    vecSoftKey[3].m_iType = ST_MTSW_ACD_MEMBER;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SOFTKEY_MTSW_ACD_MEMBER);
}

bool CMtswAcdDetailDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_MTSW_ACD_MEMBER == iType)
    {
        OnActionMembers();
        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CMtswAcdDetailDelegate::InitExtraData(void* pData)
{
    // 下载数据
    MTSWACD_DownloadData(EMTSWLOGDTYPE_Queues);
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT),
                                           SETTING_MESSAGEBOX_Cancel, "", "", -1,
                                           MSG_ID_CANCEL_DOWNLOAD_MTSW_ACD);
}

bool CMtswAcdDetailDelegate::SavePageData(CSettingUIPageDataBase*  pPageData, bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    return false;
}

bool CMtswAcdDetailDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (bPress && PHONE_KEY_OK == nKeyCode)
    {
        OnActionMembers();
        return true;
    }
    return CMtswAcdMemberDelegate::OnKeyEvent(nKeyCode, bPress, nDataIndex);
}

void CMtswAcdDetailDelegate::GetMLHGDetail()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    const SMLGHQueuesData* pData = MTSWACD_GetCurQueuesData();
    if (NULL == pData)
    {
        return;
    }

    AddBaseItemData(m_pPageData, _LANG(TRID_MTSW_QUEUE_AVG_WAITING_TIME),
                    pData->strAVGWaitingTime);

    yl::string strValue = commonAPI_FormatString("%d", pData->nCountInQueue);
    AddBaseItemData(m_pPageData, _LANG(TRID_MTSW_QUEUE_CALLERS_WAITING),
                    strValue);

    strValue = commonAPI_FormatString("%d", pData->nLogged);
    AddBaseItemData(m_pPageData, _LANG(TRID_MTSW_QUEUE_LOGGEDIN_MEMBERS),
                    strValue);

    strValue = pData->bLoggedIn ? _LANG(TRID_MTSW_ACD_LOGIN) : _LANG(TRID_MTSW_ACD_LOGOUT);
    AddBaseItemData(m_pPageData, _LANG(TRID_MTSW_QUEUE_QUEUE_STATUS),
                    strValue);
}

void CMtswAcdDetailDelegate::OnActionMembers()
{
    //进入组成员列表界面
    SettingUI_EnterPage(kszPageMTSWAcdMembers);
}

bool CMtswAcdDetailDelegate::ProcessMsgCallBack(int nResult, int nID,
        void* pExtraData /*= NULL*/)
{
    switch (nID)
    {
    case MSG_ID_CANCEL_DOWNLOAD_MTSW_ACD:
        {
            if (MessageBox_Cancel == nResult)
            {
                g_SetUICallBackFun.FallBackToPrevousPage();
                return true;
            }
        }
        break;
    case MSG_ID_SHOW_NOTE_BEFORE_BACK:
        {
            g_SetUICallBackFun.FallBackToPrevousPage();
            return true;
        }
    default:
        break;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

CSettingUIPageDataBase* CMtswAcdDetailDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIPageDataBase* pPageData = CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        pPageData->SetPageTitle(MTSWACD_GetCurrentMLHG());
        GetMLHGDetail();
    }

    return pPageData;
}

#endif

