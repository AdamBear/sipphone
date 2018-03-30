#include "dlgcallmanager.h"

#include "qcallmanagerbar.h"
#include "calluimanager.h"

CDlgCallManager::CDlgCallManager(void)
{
    //创建softkey 实例  即底排按钮栏
    CreateSoftkeyBar();
}

CDlgCallManager::~CDlgCallManager(void)
{
}

// 返回该窗口指针
CBaseDialog * CDlgCallManager::CreateInstance()
{
    return new CDlgCallManager();
}

//创建softkey bar 实例
void CDlgCallManager::CreateSoftkeyBar()
{
    if (NULL == m_pCallManagerBar)
    {
        m_pCallManagerBar = new qCallManagerBar(this);

        if (NULL != m_pCallManagerBar)
        {
            connect(m_pCallManagerBar, SIGNAL(ClickAction(const QString &)),
                    this, SLOT(OnBarClick(const QString &)));

            m_pCallManagerBar->setGeometry(160, 391, 480, 87);
            ////---- 优化界面速度暂时注释
            //m_pCallManagerBar->SetBackgroundPicture(PIC_BTN_ITEM_BAR_BACKGROUND);
            m_pCallManagerBar->raise();
            m_pCallManagerBar->show();
        }
    }
}

//设置窗口位置
void CDlgCallManager::SetRect(const QRect & rect)
{
    //先设置自身窗口位置
    this->setGeometry(rect);

    //设置子窗口的位置
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            m_pCurrentCallMGRInfo->pDlg->setGeometry(4, 53, 792, 336);
        }
    }

}

void CDlgCallManager::OnBarClick(const QString & strClickAction)
{
    _CallUIManager.OnCallManagerBarClick(strClickAction.toUtf8().data());
}
//设置窗口的提示模式命令
void CDlgCallManager::SetHintTitle(const QString & strHint)
{
    if (m_pCurrentCallMGRInfo != NULL && m_pCurrentCallMGRInfo->pDlg != NULL)
    {
        m_pCurrentCallMGRInfo->pDlg->SetHintTitle(strHint);
    }
}

//获取窗口提示模式命令
const QString CDlgCallManager::GetWindowHint() const
{
    if (m_pCurrentCallMGRInfo != NULL && m_pCurrentCallMGRInfo->pDlg != NULL)
    {
        return m_pCurrentCallMGRInfo->pDlg->GetWindowHint();
    }

    return "";
}