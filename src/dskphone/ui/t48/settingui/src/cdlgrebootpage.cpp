#include "cdlgrebootpage.h"
#include "uikernel/quipushbutton.h"
#include "baseui/t4xpicpath.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "setting/src/resetandrebootcontroller.h"
#include "adapterbox/include/modadapterbox.h"

namespace
{
enum
{
    MSGBOX_ID_CONFIRM_REBOOT = 100
};
}

CDlgRebootPage::CDlgRebootPage(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_qUIBtnReboot(NULL)
{
    InitData();
}

CDlgRebootPage::~CDlgRebootPage()
{

}

void CDlgRebootPage::InitData()
{
    m_qUIBtnReboot = new qUIPushButton(this);
    if (m_qUIBtnReboot == NULL)
    {
        //内存已满立即返回
        return;
    }

    //设置按钮的值和位置
    m_qUIBtnReboot->setText(LANG_TRANSLATE(TRID_REBOOT));
    m_qUIBtnReboot->setTextColor(QColor(222, 0, 37));
    m_qUIBtnReboot->set3StatePic(PIC_BTN_REBOOT_RESET, PIC_BTN_REBOOT_RESET_PRESS,
                                 PIC_BTN_REBOOT_RESET);
    m_qUIBtnReboot->setGeometry(BTNSTARTX, BTNSTARTY, BTNWIDTH, BTNHEIGHT);

    m_qUIBtnReboot->installEventFilter(this);
    this->installEventFilter(this);

    connect(m_qUIBtnReboot, SIGNAL(clicked()), this, SLOT(OnBtnRebootClicked()));
}

bool CDlgRebootPage::IsStatusChanged()
{
    return false;
}

bool CDlgRebootPage::LoadPageData()
{
    return true;
}

// 设置页面的SoftKey
bool CDlgRebootPage::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";
    return true;
}

bool CDlgRebootPage::SavePageData()
{
    return true;
}

void CDlgRebootPage::OnBtnRebootClicked()
{
    //点击后，弹出重启提示
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_IF_REBOOT_THE_PHONE),
                                MESSAGEBOX_OkCancel, -1, "", "",
                                MSGBOX_ID_CONFIRM_REBOOT);
}

void CDlgRebootPage::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iRet = pMessageBox->GetResult();
    int iMsgID = pMessageBox->GetID();

    if (iMsgID == MSGBOX_ID_CONFIRM_REBOOT
            && iRet == MessageBox_OK)
    {
        AdapterBox_Reboot(0);
    }
}

void CDlgRebootPage::UpdateWnd()
{
    if (!m_bPageEnable)
    {
        m_qUIBtnReboot->setDisabled(true);
    }
}
