#include "basesettingdialog.h"

#include "baseui/include/titlebase.h"

#include "basesettinguidelegate.h"

#include "common/common_data_define.h"

// manager
#include "settinguidialogmanager.h"
#include "settingactiontypemgr.h"

#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/include/settingdefine.h"
#include "settingui/include/pageiddefine.h"

#include "setuidlgbasecache.h"
#include "setuicachefactory.h"
#include "settingadapterfactory.h"
#include "settinguiadapter.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"

#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"
#include "dsklog/log.h"
#include "keymap.h"
#include "uimanager/moduimanager.h"
#include "common/common_data_define.h"
#include "xwindow/xWindowManager.h"
#include "messagebox/modmessagebox.h"
#include "edk/include/modedk.h"
#include "edkui/include/modedkui.h"


CSettingDialogBase::CSettingDialogBase(xView * pDialogView, EWindowType eWndType /*= eWT_PageMenu*/)
    : CBaseDialog(pDialogView, eWndType)
    , m_pSettingUIDelegate(NULL)
    , m_pFirstBindDelegate(NULL)
    , m_pTitle(NULL)
    , m_nDialogType(SETTTING_DIALOG_TYPE_NONE)
{
}

CSettingDialogBase::~CSettingDialogBase()
{
#if IF_SETTING_FRAME_DEBUG
    SETTINGUI_INFO("[CSettingDialogBase::~CSettingDialogBase()]");
#endif
    if (NULL != m_pTitle)
    {
        CTitleBase * pTitle = m_pTitle;

        m_pTitle = NULL;
        delete pTitle;
    }
}

void CSettingDialogBase::BindDelegate(CSettingUIDelegateBase * pDelegate)
{
    if (NULL == pDelegate || m_pSettingUIDelegate == pDelegate)
    {
        return;
    }

    m_pSettingUIDelegate = pDelegate;
    // 清除标记
    if (NULL == m_pFirstBindDelegate)
    {
        m_pFirstBindDelegate = pDelegate;
    }
}

void CSettingDialogBase::UnBindDelegate()
{
    if (NULL != m_pSettingUIDelegate)
    {
        m_pSettingUIDelegate->SetFirstBindShow(true);
        m_pSettingUIDelegate = NULL;
    }
}

void CSettingDialogBase::BindTitle(xView * pTitleView)
{
    if (NULL == pTitleView)
    {
        return;
    }

    if (NULL == m_pTitle)
    {
        m_pTitle = new CTitleBase;
    }

    if (NULL != m_pTitle)
    {
        m_pTitle->SetTitleView(pTitleView);
    }
}

void CSettingDialogBase::RefreshUI(bool bReLoadData /*= false*/)
{
    if (NULL == m_pSettingUIDelegate)
    {
        return;
    }

    LoadPageData(bReLoadData);

    yl::string strTitle = m_pSettingUIDelegate->GetTitleName();

    RefreshTitle(strTitle);
}

void CSettingDialogBase::RefreshSoftkey()
{
    CArraySoftKeyBarData vecSoftKey;

    // 初始化
    for (int i = 0; i < 4; ++i)
    {
        CSoftKeyBarData objSoftkeyData;

        objSoftkeyData.m_iType = ST_EMPTY;
        objSoftkeyData.m_strSoftkeyTitle = "";

        vecSoftKey.push_back(objSoftkeyData);
    }

    if (NULL != m_pSettingUIDelegate)
    {
        m_pSettingUIDelegate->GetPageXmlSoftkey(vecSoftKey);
        m_pSettingUIDelegate->GetPagePrivateSoftkey(vecSoftKey);
    }

    SetSoftKey(vecSoftKey);
}

//刷新Title
void CSettingDialogBase::RefreshTitle()
{
    yl::string strTitle = "";
    if (NULL != m_pSettingUIDelegate)
    {
        strTitle = m_pSettingUIDelegate->GetTitleName();
    }

    RefreshTitle(strTitle);
}

void CSettingDialogBase::RefreshTitle(const yl::string & strTitle)
{
    if (Edk_IsEnable() && EDK_IsTipModeEnable())
    {
        //显示EDK对应的title
        if (NULL != m_pSettingUIDelegate)
        {
            yl::string strPageID = m_pSettingUIDelegate->GetPageID();
            yl::string strNextPageID = m_pSettingUIDelegate->GetNextPageID();
            int nID =  m_pSettingUIDelegate->GetPageLineID();
            yl::string strEDKTitle = EdkUI_GetMenuIDByPageData(strPageID, strNextPageID, nID);
            m_pTitle->SetTitle(strEDKTitle);
        }

        return;

    }

    if (NULL != m_pTitle)
    {
        m_pTitle->SetTitle(strTitle);
    }
}

void CSettingDialogBase::DropOutCurrentPage(bool bDelDlg /*= true*/)
{
    g_pSettingUIDialogManager->DropOutCurrentPage(this, bDelDlg);
}

void CSettingDialogBase::FallBackToThePreviousInterface()
{
    g_pSettingUIDialogManager->FallBackToThePreviousInterface(this);
}

bool CSettingDialogBase::LoadPageData(bool bReLoadData /*= false*/)
{
    if (NULL == m_pSettingUIDelegate)
    {
        return false;
    }

    CSettingUIPageDataBasePtr pPageInfo = m_pSettingUIDelegate->LoadPageData(bReLoadData);
    if (NULL == pPageInfo)
    {
        SETTINGUI_WARN("CSettingDialogBase LoadPageData fail");
        return false;
    }
    m_pSettingUIDelegate->PageDataFilter(pPageInfo);
    m_pSettingUIDelegate->UpdateShowIndex();

    return true;
}

bool CSettingDialogBase::SavePageData()
{
    return true;
}

bool CSettingDialogBase::OnAction(int eActionType, int nDataIndex)
{
    if (NULL == m_pSettingUIDelegate)
    {
        return false;
    }

    return m_pSettingUIDelegate->OnAction(eActionType, nDataIndex);
}

void CSettingDialogBase::InitExtraData(void * pData)
{
    if (NULL != m_pSettingUIDelegate)
    {
        m_pSettingUIDelegate->InitExtraData(pData);
    }
}

void CSettingDialogBase::DealWithDataValid(CSettingUIDataValidInfo & dataValid)
{
    yl::string strNote(dataValid.GetValidNote());
    if (strNote.empty())
    {
        strNote = _LANG(TRID_VALUE_IS_INVALID);
    }

    PopCommonMessageBox(_LANG(TRID_NOTE), strNote, dataValid.GetMsgBoxType()
                        , "", "", dataValid.GetShowtime(), MSG_ID_VALID_DATA, NULL, this);
}

void CSettingDialogBase::PopCommonMessageBox(const yl::string & strTitle
        , const yl::string & strNote, int eType
        , const yl::string & strCancelBtnTitle /*= ""*/
        , const yl::string & strConfirmBtnTitle /*= ""*/
        , int nMsTime /*= -1*/, int iID /*= -1*/
        , void * pExtraData /*= NULL*/
        , CMsgBoxCallBackAction * pAction /*= NULL*/)
{
    MSGBOX_INFO msgBoxInfo;
    msgBoxInfo.pAction = (NULL == pAction) ? this : pAction;
    msgBoxInfo.eType = SettingType2MsgType((SETTING_MESSAGEBOX_TYPE)eType);
    msgBoxInfo.strTitle = strTitle;
    msgBoxInfo.strNote = strNote;
    msgBoxInfo.nMsTime = nMsTime;
    msgBoxInfo.iID = iID;
    msgBoxInfo.extraData = pExtraData;
    MessageBox_ShowCommonMsgBox(msgBoxInfo);
}

void CSettingDialogBase::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iResult = MsgResultType2SettingResultType(pMessageBox->GetResult());
    int nID = pMessageBox->GetID();
    void * pExtraData = pMessageBox->GetExtraData();

    if (NULL != m_pSettingUIDelegate
            && m_pSettingUIDelegate->ProcessMsgCallBack(iResult, nID, pExtraData))
    {
        return;
    }
}

bool CSettingDialogBase::OnSoftkeyAction(int iType, bool bLongPress)
{
    if (!bLongPress && NULL != m_pSettingUIDelegate)
    {
        return m_pSettingUIDelegate->OnSoftkeyAction(iType, bLongPress, INVALID_ITEM_INDEX);
    }

    return false;
}

/////////////////////////////////////////// CBaseDialog st ///////////////////////////////////////////
bool CSettingDialogBase::IsRejectReturnToIdle()
{
    if (NULL != m_pSettingUIDelegate)
    {
        return m_pSettingUIDelegate->IsRejectReturnToIdle();
    }

    return false;
}

void CSettingDialogBase::OnReturnToIdle()
{
#if IF_SETTING_FRAME_DEBUG
    SETTINGUI_INFO("[CSettingDialogBase::OnReturnToIdle()] dialog[%p] =====", this);
#endif
    if (NULL != m_pSettingUIDelegate)
    {
        m_pSettingUIDelegate->OnReturnToIdle();
    }
    // 时间到返回Idle会删除界面指针，这边不删除，不然话机会挂掉
    g_pSettingUIDialogManager->DropOutCurrentPage(this, false);

    CBaseDialog::OnReturnToIdle();
}

bool CSettingDialogBase::IsSubDialogRemovable()
{
    if (NULL != m_pSettingUIDelegate)
    {
        return m_pSettingUIDelegate->IsCurStatusRemovable();
    }

    return true;
}

/////////////////////////////////////////// CBaseDialog en ///////////////////////////////////////////
/////////////////////////////////////////// 界面属性 st ///////////////////////////////////////////
const yl::string & CSettingDialogBase::GetPageID() const
{
    if (NULL != m_pSettingUIDelegate)
    {
        return m_pSettingUIDelegate->GetPageID();
    }

    return NS_SET::kszEmptyString;
}

bool CSettingDialogBase::IsOnlyBindOneDelegate()
{
    if (NULL == m_pFirstBindDelegate || m_pFirstBindDelegate == m_pSettingUIDelegate)
    {
        return true;
    }

    return false;
}

void CSettingDialogBase::ClearFirstBindDelegate()
{
    m_pFirstBindDelegate = NULL;
}

/////////////////////////////////////////// 界面属性 en ///////////////////////////////////////////
MESSAGEBOX_TYPE CSettingDialogBase::SettingType2MsgType(SETTING_MESSAGEBOX_TYPE eSetType)
{
    if (SETTING_MESSAGEBOX_NOTE == eSetType)
    {
        return MESSAGEBOX_NOTE;
    }
    else if (SETTING_MESSAGEBOX_Cancel == eSetType)
    {
        return MESSAGEBOX_Cancel;
    }
    else if (SETTING_MESSAGEBOX_OkCancel == eSetType)
    {
        return MESSAGEBOX_OkCancel;
    }
    else if (SETTING_MESSAGEBOX_SPECIAL == eSetType)
    {
        return MESSAGEBOX_SPECIAL;
    }
    else if (SETTING_MESSAGEBOX_PWD == eSetType)
    {
        return MESSAGEBOX_PWD;
    }
    else if (SETTING_MESSAGEBOX_UNLOCK == eSetType)
    {
        return MESSAGEBOX_UNLOCK;
    }
    else if (SETTING_MESSAGEBOX_EDK == eSetType)
    {
        return MESSAGEBOX_EDK;
    }

    return MESSAGEBOX_Error;
}

SETTING_MESSAGEBOX_RESULT CSettingDialogBase::MsgResultType2SettingResultType(
    MESSAGEBOX_RESULT eResultType)
{
    if (MessageBox_Destory == eResultType)
    {
        return SETTING_MessageBox_Destory;
    }
    else if (MessageBox_OK == eResultType)
    {
        return SETTING_MessageBox_OK;
    }
    else if (MessageBox_Cancel == eResultType)
    {
        return SETTING_MessageBox_Cancel;
    }
    else if (MessageBox_TimeOut == eResultType)
    {
        return SETTING_MessageBox_TimeOut;
    }
    else if (MessageBox_Close == eResultType)
    {
        return SETTING_MessageBox_Close;
    }

    return SETTING_MessageBox_ERROR;
}

#if IF_MSG_BOX_SHOW_ICON
MSGBOX_ICON_TYPE CSettingDialogBase::SettingIconType2MsgIconType(SETTING_MESSAGEBOX_ICON_TYPE
        eSetIconType)
{
    if (SETTING_MSG_ICON_TYPE_NONE == eSetIconType)
    {
        return MSGBOX_ICON_NONE;
    }
    else if (SETTING_MSG_ICON_TYPE_ERROR == eSetIconType)
    {
        return MSGBOX_ICON_ERROR;
    }
    else if (SETTING_MSG_ICON_TYPE_WARNING == eSetIconType)
    {
        return MSGBOX_ICON_WARNING;
    }
    else if (SETTING_MSG_ICON_TYPE_ASK == eSetIconType)
    {
        return MSGBOX_ICON_ASK;
    }
    else if (SETTING_MSG_ICON_TYPE_INFO == eSetIconType)
    {
        return MSGBOX_ICON_INFO;
    }
    else if (SETTING_MSG_ICON_TYPE_SAVE_SUCCESS == eSetIconType)
    {
        return MSGBOX_ICON_SAVESUCCESS;
    }

    return MSGBOX_ICON_NONE;
}

SETTING_MESSAGEBOX_ICON_TYPE CSettingDialogBase::MsgIconType2SettingIconType(
    MSGBOX_ICON_TYPE eMsgIconType)
{
    if (MSGBOX_ICON_NONE == eMsgIconType)
    {
        return SETTING_MSG_ICON_TYPE_NONE;
    }
    else if (MSGBOX_ICON_ERROR == eMsgIconType)
    {
        return SETTING_MSG_ICON_TYPE_ERROR;
    }
    else if (MSGBOX_ICON_WARNING == eMsgIconType)
    {
        return SETTING_MSG_ICON_TYPE_WARNING;
    }
    else if (MSGBOX_ICON_ASK == eMsgIconType)
    {
        return SETTING_MSG_ICON_TYPE_ASK;
    }
    else if (MSGBOX_ICON_INFO == eMsgIconType)
    {
        return SETTING_MSG_ICON_TYPE_INFO;
    }
    else if (MSGBOX_ICON_SAVESUCCESS == eMsgIconType)
    {
        return SETTING_MSG_ICON_TYPE_SAVE_SUCCESS;
    }

    return SETTING_MSG_ICON_TYPE_NONE;
}

#endif
