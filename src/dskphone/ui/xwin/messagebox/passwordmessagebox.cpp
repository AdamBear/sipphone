#include "passwordmessagebox.h"
#include "modmessagebox.h"
#include "keymap.h"
#include "translateiddefine.h"
#include "imeedit/ximeedit.h"
#include "languagehelper/modlangtranslate.h"
#include "xwindow/xWindowManager.h"
#include "baseui/include/inputmethod.h"
#include "feature/include/modfeature.h"
#include "commonapi/systemhelperapi.h"
#include "commonunits/modcommonunits.h"
#include "feature/include/featurecommon.h"
#include "feature/include/modfeature.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

namespace
{
#define MSG_BOX_LAYOUT_FILE_PASSWORD "messagebox/pwdmsgbox.xml"
#define MSG_BOX_ID_EDIT_INPUT "editInput"
#define MSG_BOX_ID_TITLE "txtTitle"
}

CPasswordMessageBox::CPasswordMessageBox()
    : m_pEditPwd(NULL)
    , m_nInputMinLimit(0)
    , m_bProcessOKBtn(true)
{
    SetType(MESSAGEBOX_PWD);
}

CPasswordMessageBox::~CPasswordMessageBox()
{
    if (NULL != m_pEditPwd)
    {
        IME_UninstallIME(m_pEditPwd);
    }
}

CMessageBoxBase * CPasswordMessageBox::CreatePasswordMsgBox(const MSGBOX_INFO & stMsgInfo)
{
    CPasswordMessageBox * pPwdMsgBox = new CPasswordMessageBox;

    if (NULL != pPwdMsgBox)
    {
        pPwdMsgBox->loadContent(MSG_BOX_LAYOUT_FILE_PASSWORD);
        pPwdMsgBox->CreateWindowObject();
        pPwdMsgBox->UpdateSoftkeyData();

        if (!stMsgInfo.strTitle.empty())
        {
            pPwdMsgBox->SetTitle(stMsgInfo.strTitle);
        }
        //如果没有title控件，则需要将title与note连起来，放在note里显示
        if (pPwdMsgBox->GetTitleExist())
        {
            pPwdMsgBox->SetMessage(stMsgInfo.strTitle + " " + stMsgInfo.strNote);
        }
        else
        {
            pPwdMsgBox->SetMessage(stMsgInfo.strNote);
        }

        pPwdMsgBox->SetID(stMsgInfo.iID);
        pPwdMsgBox->SetShowAlways(true);
    }

    return pPwdMsgBox;
}

void CPasswordMessageBox::UpdateSoftkeyData()
{
    VecMsgBoxSoftkey vecSoftkey;
    MsgBoxSoftkey softkey;

    // Cancel按钮
    softkey.m_result = MessageBox_Cancel;
    softkey.m_title = _LANG(TRID_CANCEL);
    vecSoftkey.push_back(softkey);

    //留出两个空位给ime
    softkey.m_result = MessageBox_ERROR;
    softkey.m_title = "";
    vecSoftkey.push_back(softkey);
    softkey.m_result = MessageBox_ERROR;
    softkey.m_title = "";
    vecSoftkey.push_back(softkey);

    // OK按钮
    softkey.m_result = MessageBox_OK;
    softkey.m_title = _LANG(TRID_OK);
    vecSoftkey.push_back(softkey);

    SetSoftkey(vecSoftkey);
}

void CPasswordMessageBox::loadChildrenElements(xml_node & node)
{
    CMessageBoxBase::loadChildrenElements(node);

    m_pEditPwd = static_cast<xEditTextView *>(getViewById(MSG_BOX_ID_EDIT_INPUT));

    if (NULL != m_pEditPwd)
    {
        IME_InstallIME(m_pEditPwd, IME_PWD);

#if IF_FEATURE_AUTOP_IME
        yl::string strIme = commonUnits_GetIMEByType(kszAdvancedSettingPasswordIME);
        m_pEditPwd->SetIme(strIme);
#else
        m_pEditPwd->SetIme(IME_abc_Pwd);
#endif

        m_pEditPwd->SetPassword(true);
        m_pEditPwd->SetSuddenPassword(true);
    }
}

bool CPasswordMessageBox::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (!bPress)
    {
        return false;
    }

    if (PHONE_KEY_CANCEL == nKeyCode)
    {
        ExitMessageBox(MessageBox_Cancel);
        return true;
    }

    if (PHONE_KEY_OK == nKeyCode)
    {
		outputPassword();
        return true;
    }

    if (PHONE_KEY_UP == nKeyCode || PHONE_KEY_DOWN == nKeyCode)
    {
        return true;
    }

    return false;
}

void CPasswordMessageBox::outputPassword()
{
    if (NULL == m_pEditPwd)
    {
        return;
    }

    // 获取用户输入内容
    m_pEditPwd->EndIme();
	m_strPassword = m_pEditPwd->GetText();
	ExitMessageBox(MessageBox_OK);
}

bool CPasswordMessageBox::OnSoftkeyAction(int nType, bool bLongPress)
{
    if (MessageBox_OK == nType)
    {
		outputPassword();
        return true;
    }

    //如果Softkey是取消，判断当前顶层弹窗是否是密码框
    CMessageBoxBase * pTop = MessageBox_GetTopMessageBox();
    if (MessageBox_Cancel == nType && NULL != pTop && MESSAGEBOX_PWD != pTop->GetType())
    {
        MessageBox_RemoveTopMessageBox(MessageBox_Cancel);
        return true;
    }

    return CMessageBoxBase::OnSoftkeyAction(nType, bLongPress);
}

void CPasswordMessageBox::ShowErrorMessageBox(const yl::string & strNote)
{
    //输入的密码有误
    MSGBOX_INFO stMsgInfo;

    stMsgInfo.eType = MESSAGEBOX_NOTE;
    stMsgInfo.strNote = strNote;
    stMsgInfo.nMsTime = LONGER_TIME;

    MessageBox_ShowCommonMsgBox(stMsgInfo);

    if (NULL != m_pEditPwd)
    {
        yl::string strEmpty = "";
        m_pEditPwd->SetInitText(strEmpty);
    }
}
