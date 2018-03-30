#include "mtswvoicemaillistdelegate.h"

#if IF_FEATURE_METASWITCH_VOICEMAIL

#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"
#include "settinguimanager.h"
#include "uicallbackfun.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/modmessagebox.h"
#include "uimanager/moduimanager.h"
#include "baseui/include/baseuicommon.h"
#include "settinguitextmessageitemdata.h"
#include "baseui/include/t2xpicpath.h"
#include "messagebox/selectmessagebox.h"
#include "baseui/include/modbaseui.h"

#include "account/include/modaccount.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#include "baselibs/include/taskaction/common.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "contacts/directory/include/moddirinterface.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "directoryui/include/moddirectoryui.h"

namespace
{
enum
{
    MSG_ID_LOAD_VOICEMAIL_LIST = 1200,
    MSG_ID_DELETE_MESSAGE,
    MSG_ID_DELETE_ALL_MESSAGE,
    MSG_ID_LOAD_VOICEMAIL,
    MSG_ID_LOAD_FAIL,
};
#define MAX_LINE_NUM 5
#define VOICEMAIL_LIST_NOTE_MSGBOX_SHOW_LONG_TIME       2000
}

CMtswVoiceMailListDelegate::CMtswVoiceMailListDelegate()
{
    etl_RegisterMsgHandle(MTSW_MESSAGE_BEGIN, MTSW_MESSAGE_END,
                          &CMtswVoiceMailListDelegate::OnMTSWVoiceMailListMsg);
}

CMtswVoiceMailListDelegate::~CMtswVoiceMailListDelegate()
{
    MTSWVM_SetPlayLoading(false);
    etl_UnregisterMsgHandle(MTSW_MESSAGE_BEGIN, MTSW_MESSAGE_END,
                            &CMtswVoiceMailListDelegate::OnMTSWVoiceMailListMsg);
    MTSWVM_Clean();
    UIManager_LockReturnToIdle(false);
}

CSettingUIDelegateBase* CMtswVoiceMailListDelegate::CreateMtswVoiceMailListDelegate()
{
    CMtswVoiceMailListDelegate* pDelegate = new CMtswVoiceMailListDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CMtswVoiceMailListDelegate::OnMTSWVoiceMailListMsg(msgObject& msg)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetTopDelegate();
    if (NULL == pDelegate || kszPageMTSWVoiceMailList != pDelegate->GetPageID())
    {
        return FALSE;
    }

    CMtswVoiceMailListDelegate* pVoiceMailListDelegate = static_cast<CMtswVoiceMailListDelegate*>(pDelegate);
    if (NULL == pVoiceMailListDelegate)
    {
        return FALSE;
    }

    switch (msg.message)
    {
    case MTSW_MESSAGE_LOGIN:
        {
            //登录成功重新下载列表
            MTSWVM_LoadVoiceMailList();
            // 如果所有的账号都下载失败，则回退界面
            g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT),
                                                   SETTING_MessageBox_Cancel, "", "", -1,
                                                   MSG_ID_LOAD_VOICEMAIL_LIST);

            //下载中禁止自动返回Idle界面
            UIManager_LockReturnToIdle(true);
        }
        break;
    case MTSW_MESSAGE_AUTH_ERR:
        {
            //关闭弹框
            MessageBox_RemoveAllMessageBox();
            g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE), _LANG(TRID_LOAD_FAILED),
                                                   SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME,
                                                   MSG_ID_LOAD_FAIL);
        }
        break;
    case MTSW_MESSAGE_DOWNLOADED:
        {
            //关闭弹框
            MessageBox_RemoveAllMessageBox();
            pVoiceMailListDelegate->EnterPlay();
        }
        break;
    case MTSW_MESSAGE_DOWNLOADFAILED:
        {
            //关闭弹框
            MessageBox_RemoveAllMessageBox();
            if (NS_TA::TA_FILE_EXCEED_MAX_SIZE == msg.wParam)
            {
                g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE), _LANG(TRID_DOWNLOADFILE_OUT_OF_MEMORY),
                                                       SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME
                                                      );
            }
            else
            {
                g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE), _LANG(TRID_LOAD_FAILED),
                                                       SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME
                                                      );
            }
        }
        break;
    case MTSW_MESSAGE_VMLIST_RESULT:
        {
            MTSWVM_SetDownLoading(false);
            //关闭弹框
            MessageBox_RemoveAllMessageBox(MessageBox_Destory);

            UIManager_LockReturnToIdle(false);

            if (1 == msg.lParam)
            {
                pVoiceMailListDelegate->LoadVoicemailList();
            }
            else if (NS_TA::TA_FILE_EXCEED_MAX_SIZE == msg.wParam)
            {
                g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE), _LANG(TRID_DOWNLOADFILE_OUT_OF_MEMORY),
                                                       SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME
                                                       , MSG_ID_LOAD_FAIL);
            }
            else
            {

                g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE), _LANG(TRID_LOAD_FAILED),
                                                       SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME
                                                       , MSG_ID_LOAD_FAIL);
            }
        }
        break;
    default:
        break;
    }

    return TRUE;
}

bool CMtswVoiceMailListDelegate::IsRejectReturnToIdle()
{
    return true;
}

void CMtswVoiceMailListDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() < MIN_SETTING_SOFTKEY_NUM)
    {
        return;
    }

    vecSoftKey[1].m_iType = ST_SEND;
    vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEND);
    vecSoftKey[2].m_iType = ST_OPTION;
    vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_OPTION);
    vecSoftKey[3].m_iType = ST_PLAY;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_PLAY);

}

bool CMtswVoiceMailListDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    int inFocusIndex = GetFocusIndex();
    MTSWVM_SetItemIndex(inFocusIndex);
    switch (iType)
    {
    case ST_SEND:
        {
            ProcessCallBack();
            return true;
        }
        break;
    case ST_OPTION:
        {
            ShowOption();
            return true;
        }
        break;
    case ST_PLAY:
        {
            ProcessPlay();
            return true;
        }
        break;
    default:
        break;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CMtswVoiceMailListDelegate::InitExtraData(void* pData)
{
    // 下载数据
    MTSWVM_SetDownLoading(true);
}

bool CMtswVoiceMailListDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (bPress && PHONE_KEY_OK == nKeyCode)
    {
        EnterDetail();
        return true;
    }
    return CSettinguiDataDelegateBase::OnKeyEvent(nKeyCode, bPress, nDataIndex);
}

void CMtswVoiceMailListDelegate::MessageBoxCallBack(CMessageBoxBase* pMessageBox)
{
    if (NULL == pMessageBox || MESSAGEBOX_SELECTBOX != pMessageBox->GetType()
            || MessageBox_OK != pMessageBox->GetResult())
    {
        return;
    }

    CSelectMessageBox* pOptionMsgBox = (CSelectMessageBox*)pMessageBox;
    int nAction = pOptionMsgBox->GetFcousAction();
    switch (nAction)
    {
    case OPTION_DETAIL:
        {
            EnterDetail();
        }
        break;
    case OPTION_MARK_NEW:
        {
            MarkReadStatus(STATUS_UNREAD);
        }
        break;
    case OPTION_MARK_HEARD:
        {
            MarkReadStatus(STATUS_READ);
        }
        break;
    case OPTION_MOVE_TO_CONTACT:
        {
            AddToContacts(true);
        }
        break;
    case OPTION_ADD_TO_MTSW_CONTACT:
        {
            AddToContacts(false);
        }
        break;
    case OPTION_CALL_VOICEMAIL:
        {
            ProcessCallBack();
        }
        break;
    case OPTION_DELETE:
        {
            g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE),
                                                   _LANG(TRID_IF_DELETE_MESSAGE),
                                                   SETTING_MESSAGEBOX_OkCancel, "", "", -1
                                                   , MSG_ID_DELETE_MESSAGE);
        }
        break;
    case OPTION_DELETE_ALL:
        {
            g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE),
                                                   _LANG(TRID_IF_DELETE_ALL_MESSAGES),
                                                   SETTING_MESSAGEBOX_OkCancel, "", "", -1
                                                   , MSG_ID_DELETE_ALL_MESSAGE);
        }
        break;
    default:
        break;
    }
}

void CMtswVoiceMailListDelegate::LoadVoicemailList()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    m_pPageData->ClearItemData();
    int nTotalCount = MTSWVM_GetTotalCount();
    VoiceMsgItemDetailData VMIDD;

    for (int i = 0; i < nTotalCount; ++i)
    {
        if (!MTSWVM_GetMessageDetail(&VMIDD, i))
        {
            continue;
        }

        CSettingUITextMessageItemDataPtr pItemData = new CSettingUITextMessageItemData;
        if (NULL == pItemData)
        {
            return;
        }

        yl::string strTime = SettingUILogic_Time2NewFormatString(VMIDD.m_tTime);
        yl::string strID = commonAPI_FormatString("%d", i);
        bool bRead = false;
        if (STATUS_READ == VMIDD.m_eStatus)
        {
            bRead = true;
        }
        AddVoiceMailItem(m_pPageData, strID, VMIDD.m_strName, strTime, bRead, VMIDD.m_bUrgent);
    }

    g_SetUICallBackFun.RefreshUI(false);
}

void CMtswVoiceMailListDelegate::ProcessCallBack()
{
    VoiceMsgItemDetailData VMIDD;
    int iIndex = GetFocusIndex();
    if (MTSWVM_GetMessageDetail(&VMIDD, iIndex))
    {
        talklogic_CallOut(VMIDD.m_strNumber, VMIDD.m_strName);
    }
}

void CMtswVoiceMailListDelegate::ProcessPlay()
{
    g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT),
                                           SETTING_MESSAGEBOX_Cancel, "", "", -1
                                           , MSG_ID_LOAD_VOICEMAIL);
    MTSWVM_SetPlayLoading(true);
    MTSWVM_DownloadVoiceMail();
    //下载中禁止自动返回Idle界面
    UIManager_LockReturnToIdle(true);
}

void CMtswVoiceMailListDelegate::EnterDetail()
{
    SetRefreshDataWhenTopDelegate(true);
    SettingUI_EnterPage(kszPageMTSWVoiceMailDetail);
}

void CMtswVoiceMailListDelegate::EnterPlay()
{
    SetRefreshDataWhenTopDelegate(true);
    SettingUI_EnterPage(kszPageMTSWVoiceMailPlay);
}

void CMtswVoiceMailListDelegate::AddToContacts(bool bIsLocal)
{
    VoiceMsgItemDetailData VMIDD;
    int iIndex = MTSWVM_GetItemIndex();
    MTSWVM_GetMessageDetail(&VMIDD, iIndex);
    yl::string strName = VMIDD.m_strName;
    yl::string strNum = VMIDD.m_strNumber;

    if (bIsLocal)
    {
        //添加到本地联系人 TBD-liuqy
        DM_AddContact(strName, strNum, DIR_ACTION_LOCAL_COPY_TO_CONTACTS, DIRECTORY_TYPE_LOCAL);
    }
    else
    {
        //添加到metaswitch联系人TBD-liuqy
        DM_AddContact(strName, strNum, DIR_ACTION_MTSW_ADD_TO_CONTACTLIST, DIRECTORY_TYPE_METASWICTH);
    }
}

void CMtswVoiceMailListDelegate::MarkReadStatus(ReadStatus readStatus)
{
    if (!netIsNetworkReady())
    {
        g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE), _LANG(TRID_FAILED_PLEASE_RETRY),
                                               SETTING_MESSAGEBOX_NOTE, "", "",
                                               VOICEMAIL_LIST_NOTE_MSGBOX_SHOW_LONG_TIME);
    }
    else
    {
        MTSWVM_MarkReadStatus(readStatus);
        g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailList, _LANG(TRID_NOTE),
                                               _LANG(TRID_LOADING_PLEASE_WAIT),
                                               SETTING_MESSAGEBOX_NOTE, "", "",
                                               SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME);

        g_SetUICallBackFun.RefreshUI(true);
    }
}

void CMtswVoiceMailListDelegate::ShowOption()
{
    VEC_OPTION_ITEM_DATA vecOption;
    ST_OPTION_ITEM_DATA stOption;

    stOption.m_nAction = OPTION_DETAIL;
    stOption.m_strTitle = _LANG(TRID_DETAIL);
    vecOption.push_back(stOption);

    ReadStatus eStatus = MTSWVM_GetMessageStatus();
    if (STATUS_READ == eStatus)
    {
        stOption.m_nAction = OPTION_MARK_NEW;
        stOption.m_strTitle = _LANG(TRID_MTSW_VOICEMAIL_OPTION_MARK_NEW);
        vecOption.push_back(stOption);
    }
    else if (STATUS_UNREAD == eStatus)
    {
        stOption.m_nAction = OPTION_MARK_HEARD;
        stOption.m_strTitle = _LANG(TRID_MTSW_VOICEMAIL_OPTION_MARK_HEARD);
        vecOption.push_back(stOption);
    }

    stOption.m_nAction = OPTION_MOVE_TO_CONTACT;
    stOption.m_strTitle = _LANG(TRID_ADD_TO_PHONEBOOK);
    vecOption.push_back(stOption);

    if (MtswDir_IsEnable())
    {
        stOption.m_nAction = OPTION_ADD_TO_MTSW_CONTACT;
        stOption.m_strTitle = _LANG(TRID_ADD_TO_MTSW_CONTACT);
        vecOption.push_back(stOption);
    }

    stOption.m_nAction = OPTION_CALL_VOICEMAIL;
    stOption.m_strTitle = _LANG(TRID_MTSW_VOICEMAIL_OPTION_CALL_VOICEMAIL);
    vecOption.push_back(stOption);

    stOption.m_nAction = OPTION_DELETE;
    stOption.m_strTitle = _LANG(TRID_DELETE);
    vecOption.push_back(stOption);

    stOption.m_nAction = OPTION_DELETE_ALL;
    stOption.m_strTitle = _LANG(TRID_DELETE_ALL);
    vecOption.push_back(stOption);

    BaseUI_ShowOptionMenu(this, vecOption);
}

void CMtswVoiceMailListDelegate::AddVoiceMailItem(CSettingUIPageDataBase* pPageData,
        const yl::string& strItemID, const yl::string& strDisp, const yl::string& strValue, bool bRead, bool bUrgent)
{
    if (NULL == pPageData)
    {
        return;

    }
    CSettingUITextMessageItemDataPtr pItemData = new CSettingUITextMessageItemData;
    if (NULL == pItemData)
    {
        return;
    }

    pItemData->SetItemID(strItemID);
    pItemData->SetDisplayName(strDisp);
    pItemData->SetDateString(strValue);
    pItemData->SetShowColon(false);
    pItemData->SetUrgent(bUrgent);
    yl::string strIconPath = bRead ? PIC_VOICE_MAIL_READ : PIC_VOICE_MAIL_UNREAD;
    pItemData->SetStatusIcon(strIconPath);

    if (!pPageData->AddItem(pItemData))
    {
        delete pItemData;
    }
}


bool CMtswVoiceMailListDelegate::ProcessMsgCallBack(int nResult, int nID,
        void* pExtraData /*= NULL*/)
{
    switch (nID)
    {
    case MSG_ID_LOAD_VOICEMAIL_LIST:
        {
            if (MessageBox_Cancel == nResult)
            {
                MTSWVM_CancelDownload();
                g_SetUICallBackFun.FallBackToPrevousPage();
                return true;
            }
        }
        break;
    case MSG_ID_DELETE_MESSAGE:
        {
            if (MessageBox_OK == nResult)
            {
                MTSWVM_Delete();
                LoadVoicemailList();
                return true;
            }
        }
        break;
    case MSG_ID_DELETE_ALL_MESSAGE:
        {
            if (MessageBox_OK == nResult)
            {
                MTSWVM_DeleteAll();
                g_SetUICallBackFun.RefreshUI(true);
                return true;
            }
        }
        break;
    case MSG_ID_LOAD_VOICEMAIL:
        {
            if (MessageBox_Cancel == nResult)
            {
                UIManager_LockReturnToIdle(false);
                MTSWVM_SetPlayLoading(false);
            }
        }
        break;
    case MSG_ID_LOAD_FAIL:
        {
            //下载失败,退出界面
            if (MessageBox_Cancel == nResult)
            {
                g_SetUICallBackFun.FallBackToPrevousPage();
            }
        }
        break;
    default:
        break;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

CSettingUIPageDataBase* CMtswVoiceMailListDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIPageDataBase* pPageData = CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        if (MTSW_IsLogined())
        {
            // 下载数据
            MTSWVM_LoadVoiceMailList();
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT),
                                                   SETTING_MESSAGEBOX_Cancel, "", "", -1,
                                                   MSG_ID_LOAD_VOICEMAIL_LIST);
        }
        else
        {
            SettingUI_EnterPage(kszPageMTSWCommportal, (void*)SettingUI_MTSWLoginReturnCallBack);
        }
    }

    return pPageData;
}

#endif

