#include "dsskeyframedelegate.h"
#include "baseui/t4xpicpath.h"
#include "dsskeyui/src/dsskeyframe.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include "idlescreenui/dlgidlescreen.h"

#include "keyguard/include/modkeyguard.h"

// Dsskey位置定义
#define DSSKEY_LEFT_OFFSET_L    0
#define DSSKEY_TOP_OFFSET       0
#define DSSKEY_WIDTH            156
#define DSSKEY_HEIGHT           55
#define DSSKEY_DISTANCE         57

const QRect RECT_DSSKEYFRAME_L(0, 50, 160, 340);// Dsskey左悬浮框的位置
const QRect RECT_DSSKEYFRAME_R(644, 50, 160, 340);// Dsskey右悬浮框的位置

CDsskeyFrameDelegate::CDsskeyFrameDelegate()
{
    m_pLeftDsskeyFrame = new qDsskeyFrame();
    if (!m_pLeftDsskeyFrame.isNull())
    {
        m_pLeftDsskeyFrame->setObjectName(QLatin1String("DsskeyFrame_L"));
        // 设置按钮
        for (int i = 0; i < 6; ++i)
        {
            // Dsskey 0-5
            qDsskeyButton * pDsskeyBtn = new qDsskeyButton(i, m_pLeftDsskeyFrame);
            if (pDsskeyBtn != NULL)
            {
                pDsskeyBtn->setObjectName(QString("DsskeyBtnLeft%1").arg(i));
                m_pLeftDsskeyFrame->AddDsskeyBtn(pDsskeyBtn);
                // 左排Dsskey按钮
                pDsskeyBtn->setGeometry(DSSKEY_LEFT_OFFSET_L, DSSKEY_TOP_OFFSET + i * DSSKEY_DISTANCE, DSSKEY_WIDTH,
                                        DSSKEY_HEIGHT);
                pDsskeyBtn->setIconPicAlign(Qt::AlignLeft);
                pDsskeyBtn->setTextColor(Qt::black);
            }
            else
            {
                MAINWND_WARN("qDsskeyButton is NULL!.\n");
            }
        }
        // 设置点击按钮动作
        CAbstractDsskeyBtnAction * pAction = new CDsskeyFeatureAction;
        if (pAction != NULL)
        {
            m_pLeftDsskeyFrame->SetBtnClickAction(pAction);
        }
        else
        {
            MAINWND_WARN("pClickAction is NULL!.\n");
        }

        // 设置长按按钮动作
        pAction = NULL;
        pAction = new CDsskeyLongClickAction;
        if (pAction != NULL)
        {
            m_pLeftDsskeyFrame->SetBtnLongPressAction(pAction);
        }
        else
        {
            MAINWND_WARN("pLongAction is NULL!.\n");
        }

    }
    else
    {
        MAINWND_WARN("m_pDsskeyFrame_L is NULL!.\n");
    }


    m_pLeftDsskeyFrame->hide();

    // Dsskey右排按键
    m_pRightDsskeyFrame = new qDsskeyFrame();
    if (!m_pRightDsskeyFrame.isNull())
    {
        m_pRightDsskeyFrame->setObjectName(QLatin1String("DsskeyFrame_R"));
        // 设置按钮
        for (int i = 0; i < 5; ++i)
        {
            // Dsskey 25-29
            qDsskeyButton * pDsskeyBtn = new qDsskeyButton(i + 6, m_pRightDsskeyFrame);
            if (pDsskeyBtn != NULL)
            {
                pDsskeyBtn->setObjectName(QString("DsskeyBtnRight%1").arg(i));
                m_pRightDsskeyFrame->AddDsskeyBtn(pDsskeyBtn);
                pDsskeyBtn->setIconPicAlign(Qt::AlignLeft);
                pDsskeyBtn->setTextColor(Qt::black);
                // 右排Dsskey按钮
                pDsskeyBtn->setGeometry(DSSKEY_LEFT_OFFSET_L, DSSKEY_TOP_OFFSET + i * DSSKEY_DISTANCE, DSSKEY_WIDTH,
                                        DSSKEY_HEIGHT);

            }
            else
            {
                MAINWND_WARN("qDsskeyButton is NULL!.\n");
            }
        }

        // 设置点击按钮动作
        CAbstractDsskeyBtnAction * pAction = new CDsskeyFeatureAction;
        if (pAction != NULL)
        {
            m_pRightDsskeyFrame->SetBtnClickAction(pAction);
        }
        else
        {
            MAINWND_WARN("pClickAction is NULL!.\n");
        }

        // 设置长按按钮动作
        pAction = NULL;
        pAction = new CDsskeyLongClickAction;
        if (pAction != NULL)
        {
            m_pRightDsskeyFrame->SetBtnLongPressAction(pAction);
        }
        else
        {
            MAINWND_WARN("pLongAction is NULL!.\n");
        }

        // 设置切页按钮
        m_pPageBtn = new qDsskeyButton;
        if (!m_pPageBtn.isNull())
        {
            m_pPageBtn->setObjectName("PageBtn");
            m_pRightDsskeyFrame->AddUIPushBtn(m_pPageBtn, this, SLOT(OnDssKeyPageAction()));
            // 设置图标路径
            m_pPageBtn->setIconPic(PIC_DSS_PAGE_EXPAND);
            m_pPageBtn->setCurrIcon(PIC_DSS_PAGE_EXPAND);
            m_pPageBtn->setDownIconPic(PIC_DSS_PAGE_EXPAND_DOWN);
            // 设置按键显示文字
#if IF_BUG_30875
            m_pPageBtn->setText(LANG_TRANSLATE(TRID_SHOW_MORE));
#else
            m_pPageBtn->setText(LANG_TRANSLATE(TRID_DSS_KEY));
#endif
            // 右排Dsskey按钮
            m_pPageBtn->setGeometry(DSSKEY_LEFT_OFFSET_L, DSSKEY_TOP_OFFSET + 5 * DSSKEY_DISTANCE, DSSKEY_WIDTH,
                                    DSSKEY_HEIGHT);

        }
        else
        {
            MAINWND_WARN("pDelegate is NULL!.\n");
        }
    }
    else
    {
        MAINWND_WARN("m_pDsskeyFrame_R is NULL!.\n");
    }

//  m_pRightDsskeyFrame->RefreshAllDsskey();
    m_pRightDsskeyFrame->hide();
}

CDsskeyFrameDelegate::~CDsskeyFrameDelegate()
{

}

void CDsskeyFrameDelegate::DrawWidget(CBaseDialog * pDialog)
{
    bool  bExpend = false;
    // 判断页面的风格
    if (pDialog != NULL
            && pDialog->IsShowDsskey(bExpend))
    {
        // 根据风格显示LineKey
        if (!m_pLeftDsskeyFrame.isNull())
        {
            if (UIManager_IsInTalk())
            {
                m_pLeftDsskeyFrame->raise();
            }

            if (m_pLeftDsskeyFrame->isHidden())
            {
                m_pLeftDsskeyFrame->show();
            }
        }

        // 根据风格显示LineKey
        if (!m_pRightDsskeyFrame.isNull())
        {
            if (UIManager_IsInTalk())
            {
                m_pRightDsskeyFrame->raise();
            }

            if (m_pRightDsskeyFrame->isHidden())
            {
                m_pRightDsskeyFrame->show();
            }
        }

        if (bExpend)
        {
            // 根据当前IDLE界面Dsskey展开状态设置翻页键的图标
            m_pPageBtn->setIconPic(PIC_DSS_PAGE_CONTRACT);
            m_pPageBtn->setCurrIcon(PIC_DSS_PAGE_CONTRACT);
            m_pPageBtn->setDownIconPic(PIC_DSS_PAGE_CONTRACT_DOWN);
#if IF_BUG_30875
            m_pPageBtn->SetPageDsskeyText(LANG_TRANSLATE(TRID_SHOW_LESS));
#endif
        }
        else
        {
            // 设置图标路径
            m_pPageBtn->setIconPic(PIC_DSS_PAGE_EXPAND);
            m_pPageBtn->setCurrIcon(PIC_DSS_PAGE_EXPAND);
            m_pPageBtn->setDownIconPic(PIC_DSS_PAGE_EXPAND_DOWN);
#if IF_BUG_30875
            m_pPageBtn->SetPageDsskeyText(LANG_TRANSLATE(TRID_SHOW_MORE));
#endif
        }
        m_pPageBtn->update();
    }
    else
    {
        // 根据风格隐藏LineKey
        if (!m_pLeftDsskeyFrame.isNull())
        {
            m_pLeftDsskeyFrame->hide();
        }
        // 根据风格隐藏LineKey
        if (!m_pRightDsskeyFrame.isNull())
        {
            m_pRightDsskeyFrame->hide();
        }
    }
}

void CDsskeyFrameDelegate::SetParent(QWidget * pWgt)
{
    if (!m_pLeftDsskeyFrame.isNull())
    {
        m_pLeftDsskeyFrame->setParent(pWgt);
        m_pLeftDsskeyFrame->setGeometry(RECT_DSSKEYFRAME_L);
    }

    if (!m_pRightDsskeyFrame.isNull())
    {
        m_pRightDsskeyFrame->setParent(pWgt);
        m_pRightDsskeyFrame->setGeometry(RECT_DSSKEYFRAME_R);
    }

    setParent(pWgt);
}

void CDsskeyFrameDelegate::OnDssKeyPageAction()
{
    //如果键盘锁不分类型，则此处应上锁，因为dsskey展开后进入了CDlgFullDsskeyUI，所有点击屏幕事件都被拦截了
    //但是如果改成basedialog默认不拦鼠标点击事件，则此处可以放过，因为在moddsskey有拦截dsskey按键
    if (!keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY))
    {
        return;
    }

    MAINWND_INFO("CDsskeyFrameDelegate::OnDssKeyPageAction");

    CBaseDialog * pDlg = UIManager_GetTopWindow();
    if (NULL != pDlg
            && pDlg->inherits("CDlgIdleScreen"))
    {
        // Idle界面不新建窗口，直接显示隐藏Dsskey
        CDlgIdleScreen * pIdleDlg = (CDlgIdleScreen *)pDlg;
        pIdleDlg->OnDssKeyPageAction();
    }
    else
    {
        // 获取Menu界面的指针
        CBaseDialog * pDsskeyDlg = UIManager_GetPrivateDlg(DLG_FullDssKey);
        if (NULL == pDsskeyDlg)
        {
            pDsskeyDlg = UIManager_CreatePrivateDlg(DLG_FullDssKey, DLG_FullDssKey);
        }
        if (NULL == pDsskeyDlg)
        {
            MAINWND_INFO("Create pDsskeyDlg failed!.\n");
            return;
        }
        // 将窗口设置为顶层窗口
        UIManager_SetTopWindow(pDsskeyDlg);
        // 刷新窗口
        UIManager_UpdateWnd(pDsskeyDlg);
    }
}

bool CDsskeyFrameDelegate::Operator(const QString & strAction, const QVariant & var)
{
    if (DSSKEY_FRAME_CLICK_DSSKEY == strAction)
    {
        if (!var.canConvert<int>())
        {
            return false;
        }
        return XmlBrowserUI_LinekeyCallOut(var.value<int>());
    }

    //默认返回false才能让事件继续被处理
//  return true;
    return false;
}

void CDsskeyFrameDelegate::SetVisible(bool bVisible)
{
    if (!m_pLeftDsskeyFrame.isNull())
    {
        bVisible ? m_pLeftDsskeyFrame->show() : m_pLeftDsskeyFrame->hide();
    }

    if (!m_pRightDsskeyFrame.isNull())
    {
        bVisible ? m_pRightDsskeyFrame->show() : m_pRightDsskeyFrame->hide();
    }
}

void CDsskeyFrameDelegate::Raise()
{
    if (!m_pLeftDsskeyFrame.isNull())
    {
        m_pLeftDsskeyFrame->raise();
    }

    if (!m_pRightDsskeyFrame.isNull())
    {
        m_pRightDsskeyFrame->raise();
    }
}
