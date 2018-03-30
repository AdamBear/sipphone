#include "cdlgupdateconfigurationpage.h"
#include "uikernel/quipushbutton.h"
#include "baseui/t4xpicpath.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "adapterbox/include/modadapterbox.h"
#include "autopcontrol/include/modautopcontrol.h"

namespace
{
enum
{
    MSGBOX_ID_UPDATE_CONFIG = 111
};
}

CDlgUpdateConfigurationPage::CDlgUpdateConfigurationPage(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qUIBtnUpdate(NULL)
{
    InitData();
}

CDlgUpdateConfigurationPage::~CDlgUpdateConfigurationPage()
{

}

void CDlgUpdateConfigurationPage::InitData()
{
    m_qUIBtnUpdate = new qUIPushButton(this);
    if (m_qUIBtnUpdate == NULL)
    {
        //内存已满立即返回
        return;
    }

    //设置按钮的值和位置
    m_qUIBtnUpdate->setText(LANG_TRANSLATE(TRID_AUTO_PROVISION));
    m_qUIBtnUpdate->setTextColor(QColor(222, 0, 37));
    //仍用reboot的按钮图片
    m_qUIBtnUpdate->set3StatePic(PIC_BTN_REBOOT_RESET, PIC_BTN_REBOOT_RESET_PRESS,
                                 PIC_BTN_REBOOT_RESET);
    m_qUIBtnUpdate->setGeometry(BTNSTARTX, BTNSTARTY, BTNWIDTH, BTNHEIGHT);

    m_qUIBtnUpdate->installEventFilter(this);
    this->installEventFilter(this);

    connect(m_qUIBtnUpdate, SIGNAL(clicked()), this, SLOT(OnBtnUpdateClicked()));
}

bool CDlgUpdateConfigurationPage::IsStatusChanged()
{
    return false;
}

bool CDlgUpdateConfigurationPage::LoadPageData()
{
    return true;
}

// 设置页面的SoftKey
bool CDlgUpdateConfigurationPage::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";
    return true;
}

bool CDlgUpdateConfigurationPage::SavePageData()
{
    return true;
}

void CDlgUpdateConfigurationPage::OnBtnUpdateClicked()
{
    //点击后，弹出重启提示
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_AUTO_PROVISION_NOW),
                                MESSAGEBOX_OkCancel, -1, "", "", MSGBOX_ID_UPDATE_CONFIG);
}

void CDlgUpdateConfigurationPage::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iRet = pMessageBox->GetResult();
    int iMsgID = pMessageBox->GetID();

    if (iMsgID == MSGBOX_ID_UPDATE_CONFIG
            && iRet == MessageBox_OK)
    {
        Autop_NotifyAutoServer(false, ATP_MSG_REQ_START_BY_DEFAULT, 0, 1);
    }
}

void CDlgUpdateConfigurationPage::UpdateWnd()
{
    if (!m_bPageEnable)
    {
        m_qUIBtnUpdate->setDisabled(true);
    }
}

