#include "directorycommon.h"
#include "bsftpwdadapter.h"
#include "bsftpwdmgr.h"
#include "commonuihelper.h"
#include "dirmsgboxmgr.h"
#include "dlgdirectory.h"
#include "commonunits/modcommonunits.h"
#include "languagehelper/modlangtranslate.h"
#include "baseui/include/baseuicommon.h"
#include "baseui/include/detaillistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "baseui/include/inputmethod.h"
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE
#include "baseui/include/customdetaillistitem.h"
#endif
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "imeedit/imecommon.h"
#include "xwindow/xListView.h"
#include "uimanager/moduimanager.h"


namespace
{
#define BSFT_PWD_ITEM_INDEX_USER    0
#define BSFT_PWD_ITEM_INDEX_PWD     1
#define BSFT_PWD_ITEM_COUNT         2
}

CBsftPwdAdapter::CBsftPwdAdapter()
    : CDirectoryAdapter(DIR_UI_STATE_CHG_PWD)
    , m_pUserItem(NULL)
    , m_pPwdItem(NULL)
{
}

CBsftPwdAdapter::~CBsftPwdAdapter()
{
    ReleaseListItem();
}

CBsftPwdAdapter * CBsftPwdAdapter::GetBsftPwdAdapter(CDirectoryAdapterPtr & pAdapter)
{
    if (pAdapter.IsEmpty() || DIR_UI_STATE_CHG_PWD != pAdapter->GetState())
    {
        return NULL;
    }

    return static_cast<CBsftPwdAdapter *>(pAdapter.Get());
}

void CBsftPwdAdapter::UnInit()
{
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE || IF_HIDE_TITLE_BUT_EDIT
    OnChangeListViewParam(false);
#endif

    CDirectoryAdapter::UnInit();

    ReleaseListItem();
}

void CBsftPwdAdapter::ReleaseListItem()
{
    if (NULL != m_pUserItem)
    {
        g_ItemFactory.ReleaseItem(m_pUserItem);
        m_pUserItem = NULL;
    }

    if (NULL != m_pPwdItem)
    {
        g_ItemFactory.ReleaseItem(m_pPwdItem);
        m_pPwdItem = NULL;
    }
}

void CBsftPwdAdapter::EnterPwdSetting()
{
    if (NULL == m_pUserItem)
    {
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE
        m_pUserItem = static_cast<CDetailListItem *>(g_ItemFactory.ApplyItem(
                          CCustomDetailListItem::GetCustomDetailListItemType()));
#else
        m_pUserItem = static_cast<CDetailListItem *>(g_ItemFactory.ApplyItem(
                          CDetailListItem::GetDetailListItemType()));
#endif
    }

    if (NULL == m_pPwdItem)
    {
        m_pPwdItem = static_cast<CEditListItem *>(g_ItemFactory.ApplyItem(
                         CEditListItem::GetEditListItemType()));
    }

    int nAccountID = 0;

#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable() && CALL_LOG_TYPE_BROADSOFT == _BsftPwdMgr.GetDirType())
    {
        nAccountID = BsftLog_GetAccountIDByGroupID(_BsftPwdMgr.GetGroupId());
    }
#endif

    yl::string strUserID;
    yl::string strPassword;
    yl::string strUserIDValue;

    if (IsBsftAllowSipAuth())
    {
        strUserID = _LANG(TRID_SIP_REGISTER_NAME);
        strPassword = _LANG(TRID_SIP_PASSWORD);
        strUserIDValue = configParser_GetCfgItemStringValue(kszAccountRegisterName, nAccountID);
    }
    else
    {
        strUserID = _LANG(TRID_USER_ID);
        strPassword = _LANG(TRID_PASSWORD);
        strUserIDValue = configParser_GetCfgItemStringValue(kszAccountXSIUserID, nAccountID);
    }

    if (NULL != m_pUserItem)
    {
        m_pUserItem->ClearContent();
        m_pUserItem->SetTitle(strUserID);
        m_pUserItem->SetValue(strUserIDValue);
    }

    if (NULL != m_pPwdItem)
    {
        m_pPwdItem->ClearContent();
        m_pPwdItem->SetMaxLength(MAX_LEN_PASSWORD);
        m_pPwdItem->SetTitle(strPassword);
        m_pPwdItem->SetValue("");
        m_pPwdItem->SetPassword(true);
        m_pPwdItem->SetSuddenPassword(true);

        // 全机型统一，XSI账号密码输入框默认输入法为2aB
#if IF_FEATURE_AUTOP_IME
        yl::string strPasswordIME = commonUnits_GetIMEByType(kszXsiPasswordIME);

        if (strPasswordIME.empty())
        {
            strPasswordIME = IME_2aB_Pwd;
        }

        m_pPwdItem->InstallIME(IME_PWD, strPasswordIME.c_str());
#else
        m_pPwdItem->InstallIME(IME_PWD, IME_2aB_Pwd);
#endif
    }

    SetFocusIndex(BSFT_PWD_ITEM_INDEX_PWD);

    RefreshUI();
}

void CBsftPwdAdapter::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(BSFT_PWD_ITEM_COUNT);
    pListView->SetItemAttach();
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE || IF_HIDE_TITLE_BUT_EDIT
    OnChangeListViewParam(true);
#endif
}

bool CBsftPwdAdapter::PrecheckDataIndex(int & nDataIndex)
{
    return CheckPageIndex(BSFT_PWD_ITEM_COUNT, nDataIndex);
}

xListViewItem * CBsftPwdAdapter::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (BSFT_PWD_ITEM_INDEX_USER == nDataIndex)
    {
        return m_pUserItem;
    }

    if (BSFT_PWD_ITEM_INDEX_PWD == nDataIndex)
    {
        return m_pPwdItem;
    }

    return NULL;
}

bool CBsftPwdAdapter::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    return true;
}

bool CBsftPwdAdapter::GetTitle(yl::string & strTitle)
{
    if (IsBsftAllowSipAuth())
    {
        strTitle = _LANG(TRID_PLEASE_ENTER_YOUR_SIP_CREDENTIAL_PASSWROD);
    }
    else
    {
        strTitle = _LANG(TRID_INPUT_YOUR_WEB_ACCESS_PASSWORD);
    }

    return true;
}

bool CBsftPwdAdapter::GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey)
{
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_OK);
    vecSoftKey[3].m_iType = ST_OK;

    return true;
}

bool CBsftPwdAdapter::ProcessKeyPress(int nKey)
{
    if (PHONE_KEY_OK == nKey)
    {
        return ProcessAction(ST_OK);
    }

    return CDirectoryAdapter::ProcessKeyPress(nKey);
}

bool CBsftPwdAdapter::ProcessLineKeyPress(int nKey)
{
    return false;
}

bool CBsftPwdAdapter::ProcessAction(int nAction)
{
    if (ST_OK == nAction)
    {
        // 密码如果为空，进行提示
        yl::string strPwd;

        if (NULL != m_pPwdItem)
        {
            m_pPwdItem->ReturnAllInput();
            strPwd = m_pPwdItem->GetValue();
        }

        if (strPwd.empty())
        {
            CDirMsgBoxMgr::ShowMsgBox(TRID_PASSWORD_CANNOT_BE_EMPTY);

            if (NULL != m_pPwdItem)
            {
                m_pPwdItem->FocusItem(true);
            }

            return true;
        }

        _BsftPwdMgr.SetPassword(strPwd);

        CDirectoryControlerPtr pControler = GetControler();

        if (!pControler.IsEmpty())
        {
            pControler->OnAction(DIR_ACTION_BSFT_CHG_PWD, INVALID_ITEM_INDEX, knInvalidRId);
        }

        return true;
    }

    return CDirectoryAdapter::ProcessAction(nAction);
}
