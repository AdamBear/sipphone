#include "titlebardelegate.h"
#include "titlebar.h"
#include "uimanager/rectdefine.h"
#include "baseui/t4xpicpath.h"
#include "baseui/fontcolor.h"

#include "uicommon/messagebox/modmessagebox.h"

#include "uimanager/moduimanager.h"
#include "account/include/modaccount.h"
#include "edkui/modedkui.h"
#include "qtcommon/qmisc.h"
#include "edk/include/modedk.h"
Q_DECLARE_METATYPE(TitleLayoutInfo);

const QRect RECT_STATUSBAR(0, 0, PHONE_WIDTH, TITLE_HEIGHT);//  StatusBar位置

CTitleBarDelegate::CTitleBarDelegate()
{
    m_pTitleBar = new CTitleBar();
    if (!m_pTitleBar.isNull())
    {
        m_pTitleBar->SetBackgroundImage(PIC_THEME1_TITLEBIG);
        connect(m_pTitleBar, SIGNAL(SelectAccount(int)), this, SLOT(OnTitleBarSelectAccount(int)));
        SetWgtFontColor(m_pTitleBar, FONT_TYPE_TITLE);
    }
}

CTitleBarDelegate::~CTitleBarDelegate()
{

}

void CTitleBarDelegate::DrawWidget(CBaseDialog * pDialog)
{
    if (NULL == pDialog
            || m_pTitleBar.isNull())
    {
        return;
    }

    TitleLayoutInfo objInfo;
    bool bShow = pDialog->IsShowTitle(objInfo);

    // 隐藏Title
    if (!bShow)
    {
        m_pTitleBar->hide();
        return;
    }

    // 设置Title的信息
    m_pTitleBar->SetLayoutInfo(objInfo);

    //根据提示模式是否开启，显示不同值
    if (EDK_IsTipModeEnable())
    {
        yl::string strHintTmp =  EDKUI_GetHintNameByUIAction(EDKUI_GetCurPageAction());

        // 设置Title中间显示内容
        m_pTitleBar->SetTitle((objInfo.eMidType == TITLE_MODE_TITLE) ? toQString(strHintTmp) : "");
    }
    else
    {
        // 设置Title中间显示内容
        m_pTitleBar->SetTitle((objInfo.eMidType == TITLE_MODE_TITLE) ? pDialog->GetWindowTitle() : "");
    }


    // 显示Title
    if (0 == MessageBox_GetMessageBoxCount())
    {
        m_pTitleBar->raise();
    }

    if (m_pTitleBar->isHidden())
    {
        m_pTitleBar->show();
    }

    pDialog->AfterShowTitle();
}

QWidget * CTitleBarDelegate::GetWidget()
{
    return m_pTitleBar.data();
}

void CTitleBarDelegate::SetParent(QWidget * pWgt)
{
    if (!m_pTitleBar.isNull())
    {
        m_pTitleBar->setParent(pWgt);
        m_pTitleBar->setGeometry(RECT_STATUSBAR);
    }

    setParent(pWgt);
}

void CTitleBarDelegate::SetVisible(bool bVisible)
{
    if (!m_pTitleBar.isNull())
    {
        bVisible ? m_pTitleBar->show() : m_pTitleBar->hide();
    }
}

void CTitleBarDelegate::Raise()
{
    if (!m_pTitleBar.isNull())
    {
        m_pTitleBar->raise();
    }
}

bool CTitleBarDelegate::Operator(const QString & strAction, const QVariant & var)
{
    MAINWND_INFO("CTitleBarDelegate::Operator[%s]", strAction.toUtf8().data());
    if (m_pTitleBar.isNull())
    {
        return false;
    }

    if (TITLE_BAR_SET_TITLE == strAction)
    {
        if (!var.canConvert<QString>())
        {
            return false;
        }

        QString strTitle = var.value<QString>();
        m_pTitleBar->SetMidAreaType(TITLE_MODE_TITLE);

        //根据提示模式是否开启显示不同的内容
        if (EDK_IsTipModeEnable())
        {
            yl::string strHintTmp =  EDKUI_GetHintNameByUIAction(EDKUI_GetCurPageAction());

            m_pTitleBar->SetTitle(toQString(strHintTmp));
        }
        else
        {
            m_pTitleBar->SetTitle(strTitle);
        }

        if (!strTitle.isEmpty() && 0 == MessageBox_GetMessageBoxCount())
        {
            m_pTitleBar->raise();
        }

        m_pTitleBar->show();
    }
    else if (TITLE_BAR_SET_LEFT_AREA_TEXT == strAction)
    {
        if (!var.canConvert<QString>())
        {
            return false;
        }

        m_pTitleBar->SetAccountDisplayName(var.value<QString>());
    }
    else if (TITLE_BAR_SET_LEFT_AREA_ICON == strAction)
    {
        if (!var.canConvert<QString>())
        {
            return false;
        }

        m_pTitleBar->SetAccountIcon(var.value<QString>());
    }
    else if (TITLE_BAR_SET_SELECT_ACCOUNT_ID == strAction)
    {
        if (!var.canConvert<int>())
        {
            return false;
        }

        m_pTitleBar->SetSelectAccountID(var.value<int>());
    }
    else if (TITLE_BAR_SET_TITLE_INFO == strAction)
    {
        if (!var.canConvert<TitleLayoutInfo>())
        {
            return false;
        }

        m_pTitleBar->SetLayoutInfo(var.value<TitleLayoutInfo>());
    }
    else
    {
        MAINWND_WARN("CTitleBarProxy::Operator action error[%s]", strAction.toUtf8().data());
    }

    return true;
}

// 当选择弹出账号列表时做的操作
void CTitleBarDelegate::OnTitleBarSelectAccount(int nAccountID)
{
    MAINWND_INFO("CTitleBarDelegate::OnTitleBarSelectAccount[%d]", nAccountID);
    CBaseDialog * pDlg = UIManager_GetTopWindow();
    if (NULL == pDlg)
    {
        return;
    }

    // 选择状态先交由顶层窗口处理
    if (pDlg->ProcessTitleBarSelectAccount(nAccountID))
    {
        return;
    }

    // 默认切换账号  现在机制改为逻辑层设置默认账号接口调用成功则逻辑层发送消息通知，失败则UI自己刷新。
    if (!acc_SetDefaultAccount(nAccountID, true) && !m_pTitleBar.isNull())
    {
        m_pTitleBar->RefreshAccount();
    }
}
