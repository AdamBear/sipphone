#include "mtswacdlistdelegate.h"

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
#include "settinguilogic/include/modsetuilogic.h"


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

CMtswAcdListDelegate::CMtswAcdListDelegate()
{
    etl_RegisterMsgHandle(MTSW_ACD_MESSAGE_BEGIN, MTSW_ACD_MESSAGE_END,
                          &CMtswAcdListDelegate::OnMTSWAcdlistMsg);
}

CMtswAcdListDelegate::~CMtswAcdListDelegate()
{
    etl_UnregisterMsgHandle(MTSW_ACD_MESSAGE_BEGIN, MTSW_ACD_MESSAGE_END,
                            &CMtswAcdListDelegate::OnMTSWAcdlistMsg);
}

CSettingUIDelegateBase* CMtswAcdListDelegate::CreatMtswAcdListDelegate()
{
    CMtswAcdListDelegate* pDelegate = new CMtswAcdListDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CMtswAcdListDelegate::OnMTSWAcdlistMsg(msgObject& msg)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetTopDelegate();
    if (NULL == pDelegate || kszPageMTSWAcdList != pDelegate->GetPageID())
    {
        return FALSE;
    }

    CMtswAcdListDelegate* pAcdListDelegate = static_cast<CMtswAcdListDelegate*>(pDelegate);
    if (NULL == pAcdListDelegate)
    {
        return FALSE;
    }

    switch (msg.message)
    {
    case MTSW_ACD_GET_RESULT:
        {
            SETTINGUI_INFO("CDlgMTSWAcdList MTSW_ACD_GET_RESULT w = %d", msg.wParam);
            if (EMTSWLOGDTYPE_All == msg.wParam
                    || EMTSWLOGDTYPE_MemberList == msg.wParam
                    || EMTSWLOGDTYPE_MLHG == msg.wParam)
            {
                pAcdListDelegate->GetMLHGList();
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
                g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWAcdList, _LANG(TRID_NOTE), _LANG(TRID_LOAD_FAILED),
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

void CMtswAcdListDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    VEC_SMLGG_DATA vecMLGHData;
    MTSWACD_GetMLHGList(vecMLGHData);
    if (vecSoftKey.size() < MIN_SETTING_SOFTKEY_NUM || vecMLGHData.size() <= 0)
    {
        return;
    }

    int iFoucusIndex = GetFocusIndex();

    if (iFoucusIndex < 0 || iFoucusIndex >= vecMLGHData.size()
            || !vecMLGHData[iFoucusIndex].bLoginLogoutSupport)
    {
        vecSoftKey[1].m_iType = ST_EMPTY;
        vecSoftKey[1].m_strSoftkeyTitle = "";
    }
    else if (vecMLGHData[iFoucusIndex].IsLogin())
    {
        vecSoftKey[1].m_iType = ST_LOGOUT;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_LOGOUT);
    }
    else
    {
        vecSoftKey[1].m_iType = ST_MTSW_LOGIN;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_LOGIN);
    }

    bool bShowPreminumAcd = MTSWACD_BeHasPreminumAcd() && !MTSWACD_IsAllMLHGLogout();

    if (bShowPreminumAcd)
    {
        vecSoftKey[2].m_iType = ST_MTSW_ACD_STATE;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_SOFTKEY_MTSW_ACD_STATE);

        vecSoftKey[3].m_iType = ST_MTSW_ACD_DETAIL;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SOFTKEY_MTSW_ACD_DETAIL);

        if (vecSoftKey.size() > MIN_SETTING_SOFTKEY_NUM)
        {
            vecSoftKey[4].m_iType = ST_MTSW_ACD_MEMBER;
            vecSoftKey[4].m_strSoftkeyTitle = _LANG(TRID_SOFTKEY_MTSW_ACD_MEMBER);
        }
        else
        {
            CSoftKeyBarData softkeyData;
            softkeyData.m_iType = ST_MTSW_ACD_MEMBER;
            softkeyData.m_strSoftkeyTitle = _LANG(TRID_SOFTKEY_MTSW_ACD_MEMBER);
            vecSoftKey.push_back(softkeyData);
        }
    }
    else
    {
        vecSoftKey[2].m_iType = ST_MTSW_ACD_DETAIL;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_SOFTKEY_MTSW_ACD_DETAIL);

        vecSoftKey[3].m_iType = ST_MTSW_ACD_MEMBER;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SOFTKEY_MTSW_ACD_MEMBER);
    }

}

bool CMtswAcdListDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_MTSW_LOGIN == iType)
    {
        SetCurrentMLHG();

        if (MTSWACD_IsAllowMultipleLogin()
                || IsAllLogout())
        {
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_MTSW_ACD_LOGINING),
                                                   SETTING_MESSAGEBOX_Cancel, "", "", -1,
                                                   MSG_ID_LOGIN_LOGOUT_MTSW_ACD);
            // 请求登录
            ACD_ReqLogin();
        }
        else
        {
            // 提示是否允许登录
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_MTSW_ACD_LOGIN_TIPS),
                                                   SETTING_MESSAGEBOX_OkCancel, "", "", -1,
                                                   MSG_ID_MULTILOGIN_MTSW_ACD);
        }
    }
    else if (ST_LOGOUT == iType)
    {
        SetCurrentMLHG();
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_MTSW_ACD_LOGOUTING),
                                               SETTING_MESSAGEBOX_Cancel, "", "", -1,
                                               MSG_ID_LOGIN_LOGOUT_MTSW_ACD);
        CallCenter_Logout();
    }
    else if (ST_MTSW_ACD_DETAIL == iType)
    {
        SetCurrentMLHG();
        //进入详情界面
        SettingUI_EnterPage(kszPageMTSWAcdDetail);
    }
    else if (ST_MTSW_ACD_MEMBER == iType)
    {
        SetCurrentMLHG();
        //进入组成员列表界面
        SettingUI_EnterPage(kszPageMTSWAcdMembers);
    }
    else if (ST_MTSW_ACD_STATE == iType)
    {
        //进入组成员列表界面
        SettingUI_EnterPage(kszPageMTSWAcdMyStatus);
        SetRefreshDataWhenTopDelegate(true);
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CMtswAcdListDelegate::InitExtraData(void* pData)
{

}

bool CMtswAcdListDelegate::SavePageData(CSettingUIPageDataBase*  pPageData, bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    return false;
}

void CMtswAcdListDelegate::GetMLHGList()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    VEC_SMLGG_DATA vecMLGHData;
    MTSWACD_GetMLHGList(vecMLGHData);

    // 只有当下载完成memberlist时才刷新界面
    if (vecMLGHData.size() <= 0)
    {
        return;
    }

    m_pPageData->ClearItemData();
    m_pPageData->SetPageTitle(GetAcdListTitle());
    for (int i = 0; i < vecMLGHData.size(); ++i)
    {
        yl::string strValue = vecMLGHData[i].IsLogin() ? _LANG(TRID_MTSW_ACD_LOGIN)
                              : _LANG(TRID_MTSW_ACD_LOGOUT);
        AddBaseItemData(m_pPageData, vecMLGHData[i].strName, strValue, "", false, false);
    }

    MessageBox_RemoveTopMessageBox(MessageBox_OK);
    g_SetUICallBackFun.RefreshUI(false);
}

bool CMtswAcdListDelegate::ProcessMsgCallBack(int nResult, int nID,
        void* pExtraData /*= NULL*/)
{
    switch (nID)
    {
    case MSG_ID_MULTILOGIN_MTSW_ACD:
        {
            if (MessageBox_OK == nResult)
            {
                ACD_ReqLogin();
                return true;
            }
        }
        break;
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

bool CMtswAcdListDelegate::SetCurrentMLHG()
{
    // 呼出列表成員
    CSettingUIItemDataBase* pBaseItemData = GetItemDataByDataIndex(GetFocusIndex());
    if (NULL == pBaseItemData)
    {
        return false;
    }

    yl::string strDisplayName = pBaseItemData->GetDisplayName();
    if (!strDisplayName.empty())
    {
        return MTSWACD_SetCurrentMLHG(strDisplayName);
    }

    return false;
}

CSettingUIPageDataBase* CMtswAcdListDelegate::LoadPageData(bool bReLoad /*= false*/)
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
            MTSWACD_DownloadData(EMTSWLOGDTYPE_All);
            g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT),
                                                   SETTING_MESSAGEBOX_Cancel, "", "", -1,
                                                   MSG_ID_CANCEL_DOWNLOAD_MTSW_ACD);
        }
        else
        {
            SettingUI_EnterPage(kszPageMTSWCommportal, (void*)SettingUI_MTSWLoginReturnCallBack);
        }

    }

    return pPageData;
}

//获取ACD状态
yl::string CMtswAcdListDelegate::GetAcdListTitle()
{
    yl::string strTitle = _LANG(TRID_ACD);
    yl::string strAcdStatus;
    SettingUILogic_GetACDStatusString(ACD_GetAcdStatus(), strAcdStatus);
    if (!strAcdStatus.empty())
    {
        strTitle = strAcdStatus;
    }
    return strTitle;
}

bool CMtswAcdListDelegate::IsAllLogout()
{
    VEC_SMLGG_DATA vecMLGHData;
    MTSWACD_GetMLHGList(vecMLGHData);
    for (int i = 0; i < vecMLGHData.size(); ++i)
    {
        if (vecMLGHData[i].IsLogin()
                && vecMLGHData[i].bLoginLogoutSupport)
        {
            return false;
        }
    }

    return true;
}

#endif

