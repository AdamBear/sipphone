#include "dlgcallmanagerbase.h"
#include "uicommon/qtcommon/qmisc.h"
#include "uimanager/moduimanager.h"
#include "calluimanager.h"

//mod-globalInputHook
#include "keymap.h"

CDlgCallManagerBase::CDlgCallManagerBase(void)
{
    //设置为顶层窗口
    SetTopWnd();

    m_eWndType = eWT_CallManager;

    //是否 returntoidle 状态
    m_bIsReturnToIdle = false;

    //当前子窗口信息 设置为空
    m_pCurrentCallMGRInfo = NULL;

    m_pCallManagerBar = NULL;       //底排按钮栏设置为空

    //创建softkey 实例  即底排按钮栏
//  CreateSoftkeyBar();
}

CDlgCallManagerBase::~CDlgCallManagerBase(void)
{

}

// 初始化
void CDlgCallManagerBase::Init()
{
    //当前选中的界面设置为空
    m_pCurrentCallMGRInfo = NULL;

    m_bIsReturnToIdle = false;

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgCallManagerBase::Uninit()
{
    //returntoidle 复位
    m_bIsReturnToIdle = false;
    //当前选中的界面设置为空
    m_pCurrentCallMGRInfo = NULL;
}

// 焦点切换
void CDlgCallManagerBase::OnFocusChanged(bool bFocus)
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            m_pCurrentCallMGRInfo->pDlg->OnFocusChanged(bFocus);
        }
    }
}

bool CDlgCallManagerBase::IsShowTitle(TitleLayoutInfo & objInfo)
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            return m_pCurrentCallMGRInfo->pDlg->IsShowTitle(objInfo);
        }
    }

    return true;
}

void CDlgCallManagerBase::AfterShowTitle()
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            m_pCurrentCallMGRInfo->pDlg->AfterShowTitle();
        }
    }

}

bool CDlgCallManagerBase::IsRejectReturnToIdle(RETURN_IDLE_TYPE
        eReturnIdle/* = RETURN_IDLE_TYPE_NORMAL*/)
{
    if (RETURN_IDLE_TYPE_HOME_PRESS == eReturnIdle)
    {
        return false;
    }

    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            return m_pCurrentCallMGRInfo->pDlg->IsRejectReturnToIdle(eReturnIdle);
        }
    }

    return false;
}

void CDlgCallManagerBase::UpdateWnd()
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            m_pCurrentCallMGRInfo->pDlg->UpdateWnd();
        }
    }
}

CBaseDialog * CDlgCallManagerBase::CreateInstance()
{
    return NULL;
}

//回到Idle前的处理函数
void CDlgCallManagerBase::OnReturnToIdle()
{
    //先设置当前状态是 returntoidle 的
    m_bIsReturnToIdle = true;

    _CallUIManager.OnReturnToIdle();
}

// 设置小窗口数据
void CDlgCallManagerBase::SetData(void * pData)
{

}

bool CDlgCallManagerBase::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            if (m_pCurrentCallMGRInfo->pDlg->eventFilter(pObject, pEvent))
            {
                return true;
            }
        }

        //只有x键 需要特殊处理
        if (NULL != pEvent)
        {
            if (pEvent->type() == QEvent::KeyPress)
            {
                QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
                int nKey = pKeyEvent->key();

                if (PHONE_KEY_CANCEL == nKey)
                {
                    //如果对话框没处理 或者没有对话框  那么直接退出对话框显示
                    _CallUIManager.OnCallManagerReleasePage(m_pCurrentCallMGRInfo->strUIName);

                    return true;
                }

                //http://10.3.5.199/Bug.php?BugID=63931
                //不让bar控件处理  否则会出现问题
                if (PHONE_KEY_OK == nKey)
                {
                    return true;
                }

            }
            else if (pEvent->type() == QEvent::KeyRelease)
            {
                QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);

                //http://10.3.5.199/Bug.php?BugID=63931
                //不让bar控件处理  否则会出现问题
                if (PHONE_KEY_OK == pKeyEvent->key())
                {
                    return true;
                }
            }
        }

    }

    return false;
}

//设置Softkey数据
void CDlgCallManagerBase::SetSoftKey(const CArraySoftKeyBarData & objSoftkeyData)
{

}

bool CDlgCallManagerBase::IsShowSoftkey()
{
    return false;
}

bool CDlgCallManagerBase::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
        QRect & rtVolume, yl::string & strBGPath) const
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            return m_pCurrentCallMGRInfo->pDlg->IsShowVolumeBar(iVolumeType, iMin, iMax, rtVolume, strBGPath);
        }
    }

    return false;
}

QPixmap CDlgCallManagerBase::GetCustomBackGround()
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            return m_pCurrentCallMGRInfo->pDlg->GetCustomBackGround();
        }
    }

    return QPixmap();
}

void CDlgCallManagerBase::SetWindowTitle(const QString & strTitle)
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            m_pCurrentCallMGRInfo->pDlg->SetWindowTitle(strTitle);
        }
    }

}

const QString CDlgCallManagerBase::GetWindowTitle() const
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            return m_pCurrentCallMGRInfo->pDlg->GetWindowTitle();
        }
    }

    return m_strTitle;
}

bool CDlgCallManagerBase::AllowMouseEventOnKeyLock(QObject * pObject, QMouseEvent * pEvent)
{
    // 外部区域的鼠标事件允许通过
    if (!isChildOf(pObject, this))
    {
        return true;
    }

    if (NULL != m_pCurrentCallMGRInfo)
    {
        // 非子页面的鼠标事件,不允许Bar的通过
        if (!isChildOf(pObject, m_pCurrentCallMGRInfo->pDlg))
        {
            return !isChildOf(pObject, m_pCallManagerBar);
        }

        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            return m_pCurrentCallMGRInfo->pDlg->AllowMouseEventOnKeyLock(pObject, pEvent);

        }
    }

    return CBaseDialog::AllowMouseEventOnKeyLock(pObject, pEvent);
}

bool CDlgCallManagerBase::AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress)
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            return m_pCurrentCallMGRInfo->pDlg->AllowKeyPadEventOnKeyLock(nKeyCode, bPress);
        }
    }

    return CBaseDialog::AllowKeyPadEventOnKeyLock(nKeyCode, bPress);
}

void CDlgCallManagerBase::OnBackButtonClick()
{
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            m_pCurrentCallMGRInfo->pDlg->OnBackButtonClick();
        }
    }
}

void CDlgCallManagerBase::OnHomeButtonClick()
{
    UIManager_ReturnToIdle(true);
}

void CDlgCallManagerBase::OnCallManagerBarClick(const yl::string & strUIName)
{
    _CallUIManager.OnCallManagerBarClick(strUIName);
}

//给UI 插入子界面信息  主要是为了 设置 显示的item名称 和选中背景图片等等
void CDlgCallManagerBase::SetCallMGRListInfo(YLList<CCallMGRInfo *> & listCallMGRInfo)
{
    if (NULL != m_pCallManagerBar)
    {
        m_pCallManagerBar->SetCallMGRListInfo(listCallMGRInfo);
    }
}

//根据信息显示界面
void CDlgCallManagerBase::EnterPage(CCallMGRInfo * pCallMGRInfo)
{
    //两个都为空 那么直接返回
    if (NULL == pCallMGRInfo && NULL == m_pCurrentCallMGRInfo)
    {
        return ;
    }

    //当前的窗口是 完全 相同的 那么直接返回就可以了
    if (NULL != pCallMGRInfo
            && NULL != m_pCurrentCallMGRInfo
            && pCallMGRInfo->strUIName == m_pCurrentCallMGRInfo->strUIName
            && pCallMGRInfo->pDlg ==
            m_pCurrentCallMGRInfo->pDlg)           //这说明是完全同一个子界面
    {
        return ;
    }

    //先隐藏当前的窗口  这边只是为了容错
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            //一定要顶层窗口 才需要调用onfocuschange
            if (this == UIManager_GetTopWindow())
            {
                //不是returntoidle 的才需要调用 免得重复调用了
                if (false == m_bIsReturnToIdle)
                {
                    m_pCurrentCallMGRInfo->pDlg->OnFocusChanged(false);
                }
            }

            m_pCurrentCallMGRInfo->pDlg->hide();
            m_pCurrentCallMGRInfo->pDlg->setParent(NULL);
        }
    }

    //赋值当前窗口
    m_pCurrentCallMGRInfo = pCallMGRInfo;

    //显示当前窗口  有显示当前窗口的时候 才需要更新 softkeybar
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            //先显示出来 在调用focuschange
            m_pCurrentCallMGRInfo->pDlg->setParent(this);
            m_pCurrentCallMGRInfo->pDlg->show();


            if (this == UIManager_GetTopWindow())
            {
                m_pCurrentCallMGRInfo->pDlg->OnFocusChanged(true);
            }
        }

        //更新当前的焦点
        if (NULL != m_pCallManagerBar)
        {
            m_pCallManagerBar->SetFocusMGRInfo(m_pCurrentCallMGRInfo->strUIName);
        }

        //刷新item 显示
        update();
    }

}

//退出显示界面
void CDlgCallManagerBase::PutPage(CCallMGRInfo * pCallMGRInfo)
{
    //当前窗口为空 直接返回
    if (NULL == m_pCurrentCallMGRInfo)
    {
        return ;
    }

    //两个窗口不相同直接返回
    if (pCallMGRInfo != m_pCurrentCallMGRInfo)
    {
        return ;
    }

    //先隐藏当前的窗口  这边只是加个判断  其实也可以不加
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            //一定要顶层窗口 才需要调用onfocuschange
            if (this == UIManager_GetTopWindow())
            {
                //不是returntoidle 的才需要调用 免得重复调用了  因为returntoidle 的时候 会先调用一次
                if (false == m_bIsReturnToIdle)
                {
                    m_pCurrentCallMGRInfo->pDlg->OnFocusChanged(false);
                }
            }

            m_pCurrentCallMGRInfo->pDlg->setParent(NULL);
            m_pCurrentCallMGRInfo->pDlg->hide();
        }

        //这边要把当前的信息 设置为空
        m_pCurrentCallMGRInfo = NULL;
    }
}

//设置对话框为空
void CDlgCallManagerBase::SetPageDlgNULL(CCallMGRInfo * pCallMGRInfo)
{
    //当前窗口为空 直接返回
    if (NULL == m_pCurrentCallMGRInfo)
    {
        return ;
    }

    //两个窗口不相同直接返回
    if (pCallMGRInfo != m_pCurrentCallMGRInfo)
    {
        return ;
    }

    //先隐藏当前的窗口  这边只是加个判断  其实也可以不加
    if (NULL != m_pCurrentCallMGRInfo)
    {
        if (NULL != m_pCurrentCallMGRInfo->pDlg)
        {
            m_pCurrentCallMGRInfo->pDlg->setParent(NULL);
            m_pCurrentCallMGRInfo->pDlg->hide();
        }

        //这边要把当前的信息 设置为空
        m_pCurrentCallMGRInfo = NULL;
    }
}

//当前是否是returntoidle 退出  如果是returntoidle 退出的 那么在calluimanager就不应该再调用退出了
bool CDlgCallManagerBase::IsReturnToIdle()
{
    return m_bIsReturnToIdle;
}

//创建softkey bar 实例
//void CDlgCallManagerBase::CreateSoftkeyBar()
//{
//
//}
