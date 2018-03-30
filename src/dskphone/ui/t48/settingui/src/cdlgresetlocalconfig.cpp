#include "cdlgresetlocalconfig.h"
#include "uikernel/quipushbutton.h"
#include "baseui/t4xpicpath.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include <configserver/configserverinc.h>

namespace
{
enum
{
    MSGBOX_ID_CONFIRM_RESET_LOCAL = 100
};
}

CDlgResetLocalConfig::CDlgResetLocalConfig(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_btnResetLocal(NULL)
{
    InitData();
}

CDlgResetLocalConfig::~CDlgResetLocalConfig()
{

}

void CDlgResetLocalConfig::InitData()
{
    m_btnResetLocal = new qUIPushButton(this);
    if (m_btnResetLocal == NULL)
    {
        //内存已满立即返回
        return;
    }

    //设置按钮的值和位置
    m_btnResetLocal->setText(LANG_TRANSLATE(TRID_RESET_LOCAL_SETTINGS));
    m_btnResetLocal->setTextColor(QColor(222, 0, 37));
    m_btnResetLocal->set3StatePic(PIC_BTN_REBOOT_RESET, PIC_BTN_REBOOT_RESET_PRESS,
                                  PIC_BTN_REBOOT_RESET);
    m_btnResetLocal->setGeometry(BTNSTARTX, BTNSTARTY, BTNWIDTH, BTNHEIGHT);

    m_btnResetLocal->installEventFilter(this);
    this->installEventFilter(this);

    connect(m_btnResetLocal, SIGNAL(clicked()), this, SLOT(OnBtnResetLocalClicked()));
}

bool CDlgResetLocalConfig::IsStatusChanged()
{
    return false;
}

bool CDlgResetLocalConfig::LoadPageData()
{
    return true;
}

// 设置页面的SoftKey
bool CDlgResetLocalConfig::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";
    return true;
}

bool CDlgResetLocalConfig::SavePageData()
{
    return true;
}

void CDlgResetLocalConfig::OnBtnResetLocalClicked()
{
    //点击后，弹出重启提示
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_IF_RESET_LOCAL_SETTING),
                                MESSAGEBOX_OkCancel, -1, "", "",
                                MSGBOX_ID_CONFIRM_RESET_LOCAL);
}

void CDlgResetLocalConfig::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iRet = pMessageBox->GetResult();
    int iMsgID = pMessageBox->GetID();

    if (iMsgID == MSGBOX_ID_CONFIRM_RESET_LOCAL
            && iRet == MessageBox_OK)
    {
        msgPostMsgToThread(msgGetThreadByName(CONFIG_SERVER_THREAD_NAME), CSM_CLEAR_USERCFG, 0, 0);

        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_RESETLOCAL_PLEASE_WAIT),
                                    MESSAGEBOX_NOTE, SHORT_TIME);
    }
}
