#include "cdlgfulldsskeyui.h"

#include "uimanager/uimanager_inc.h"
#include "dsskeyui/include/moddsskeyui.h"
#include "dsskey/include/moddsskey.h"
#include "uikernel/languagehelper.h"
// mod_Xmlbrowser
//#include "xmlbrowser/modxmlbrowser.h"
#include "picpath.h"
// 图片ICON宏定义
#include "baseui/t4xpicpath.h"
#include "baseui/transwidgetbgcontroler.h"
// DsskeyUI
#include "dsskeyui/src/dsskeyframe.h"
#include "keymap.h"
#include "qtcommon/qmisc.h"
#include "imagemanager/modimagemanager.h"
#include "uimanager/rectdefine.h"
#include "uimanager/moduimanager.h"
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
#include "setting/src/displaycontroller.h"
#endif

#define SAFE_DELETE_POINTER(p)      if (p != NULL) \
                                    { \
                                    delete p; \
                                    p = NULL; \
                                    }

#define DSSKEY_DSSKEYUI_LEFT_OFFSET_L   0
#define DSSKEY_DSSKEYUI_TOP_OFFSET      0
#define DSSKEY_DSSKEYUI_WIDTH           156
#define DSSKEY_DSSKEYUI_HEIGHT          55
#define DSSKEY_DSSKEYUI_V_DISTANCE      57
#define DSSKEY_DSSKEYUI_H_DISTANCE      161
const QRect RECT_IDLE_DSSKEYFRAME(0, 7, 800, 340);// Dsskey悬浮框的位置

// Idle界面位置
const QRect RECT_DSSKEY(0, TITLE_HEIGHT, PHONE_WIDTH, 347);

CDlgFullDsskeyUI::CDlgFullDsskeyUI(QWidget * parent/* = 0*/)
    : CBaseDialog(parent)
    , m_pDsskeyFrame(NULL)
{
    // 设置小窗口的风格为MENU
    SetWindowType(eWT_Dsskey);

    //设置顶层窗口属性
    SetTopWnd();

    // 设置窗体位置
    SetDlgGeometry(RECT_DSSKEY);

    InitData();

    UIManager_InstallTransChangeSignalSlot(this, SLOT(OnTransparencyChange()));
}

CDlgFullDsskeyUI::~CDlgFullDsskeyUI()
{
    SAFE_DELETE_POINTER(m_pDsskeyFrame);

    UIManager_UnInstallTransChangeSignalSlot(this, SLOT(OnTransparencyChange()));
}

// 初始化
void CDlgFullDsskeyUI::Init()
{
    // 设置Dsskey数据代理
    m_pDsskeyFrame->RefreshAllDsskey();
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgFullDsskeyUI::Uninit()
{

}


qDsskeyFrame * CDlgFullDsskeyUI::GetDssKeyFrame()
{
    //获取DssKey Frame，Idle界面会使用
    return m_pDsskeyFrame;
}

// 返回该窗口指针
CBaseDialog * CDlgFullDsskeyUI::CreateInstance()
{
    return new CDlgFullDsskeyUI();
}

// 初始化数据
void CDlgFullDsskeyUI::InitData()
{
    InitSoftKeyData();
    InitDsskeyFrame();
}


//初始化Dsskey按钮
void CDlgFullDsskeyUI::InitDsskeyFrame()
{
    // Dsskey部分初始化
    m_pDsskeyFrame = new qDsskeyFrame(this);


    int i = 0;
    qDsskeyButton * pDsskeyBtn = NULL;
    qDsskeyButton * pPushBtn = NULL;
    CAbstractDsskeyBtnAction * pAction = NULL;
    //
    if (m_pDsskeyFrame != NULL)
    {
        m_pDsskeyFrame->setGeometry(RECT_IDLE_DSSKEYFRAME);
        // 设置按钮
        for (i = 0; i < 29; ++i)
        {
            if (i < 6)
            {
                pDsskeyBtn = new qDsskeyButton(i, m_pDsskeyFrame);
            }
            else if (i < 12)
            {
                pDsskeyBtn = new qDsskeyButton(i + 5, m_pDsskeyFrame);
            }
            else if (i < 18)
            {
                pDsskeyBtn = new qDsskeyButton(i + 5, m_pDsskeyFrame);
            }
            else if (i < 24)
            {
                pDsskeyBtn = new qDsskeyButton(i + 5, m_pDsskeyFrame);
            }
            else
            {
                pDsskeyBtn = new qDsskeyButton(i - 18, m_pDsskeyFrame);
            }

            if (pDsskeyBtn != NULL)
            {
                connect(pDsskeyBtn, SIGNAL(btnClicked(int)), this, SLOT(OnDsskeyBtnClick(int)));
                m_pDsskeyFrame->AddDsskeyBtn(pDsskeyBtn);
                if (i < 6)
                {
                    // 第1排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_DSSKEYUI_LEFT_OFFSET_L,
                                            DSSKEY_DSSKEYUI_TOP_OFFSET + i * DSSKEY_DSSKEYUI_V_DISTANCE, DSSKEY_DSSKEYUI_WIDTH,
                                            DSSKEY_DSSKEYUI_HEIGHT);
                }
                else if (i < 12)
                {
                    // 第2排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_DSSKEYUI_LEFT_OFFSET_L + DSSKEY_DSSKEYUI_H_DISTANCE,
                                            DSSKEY_DSSKEYUI_TOP_OFFSET + (i - 6) * DSSKEY_DSSKEYUI_V_DISTANCE, DSSKEY_DSSKEYUI_WIDTH,
                                            DSSKEY_DSSKEYUI_HEIGHT);
                }
                else if (i < 18)
                {
                    // 第2排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_DSSKEYUI_LEFT_OFFSET_L + 2 * DSSKEY_DSSKEYUI_H_DISTANCE,
                                            DSSKEY_DSSKEYUI_TOP_OFFSET + (i - 12) * DSSKEY_DSSKEYUI_V_DISTANCE, DSSKEY_DSSKEYUI_WIDTH,
                                            DSSKEY_DSSKEYUI_HEIGHT);
                }
                else if (i < 24)
                {
                    // 第2排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_DSSKEYUI_LEFT_OFFSET_L + 3 * DSSKEY_DSSKEYUI_H_DISTANCE,
                                            DSSKEY_DSSKEYUI_TOP_OFFSET + (i - 18) * DSSKEY_DSSKEYUI_V_DISTANCE, DSSKEY_DSSKEYUI_WIDTH,
                                            DSSKEY_DSSKEYUI_HEIGHT);
                }
                else
                {
                    // 第3排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_DSSKEYUI_LEFT_OFFSET_L + 4 * DSSKEY_DSSKEYUI_H_DISTANCE,
                                            DSSKEY_DSSKEYUI_TOP_OFFSET + (i - 24) * DSSKEY_DSSKEYUI_V_DISTANCE, DSSKEY_DSSKEYUI_WIDTH,
                                            DSSKEY_DSSKEYUI_HEIGHT);
                }

                pDsskeyBtn->setIconPicAlign(Qt::AlignLeft);
                pDsskeyBtn->setTextColor(Qt::black);
            }
            else
            {
                DSSKEY_WARN("qDsskeyButton is NULL!.\n");
                return;
            }
        }
        // 设置按钮动作
        // 设置点击按钮动作
        pAction = new CDsskeyFeatureAction;
        if (pAction != NULL)
        {
            m_pDsskeyFrame->SetBtnClickAction(pAction);
        }
        else
        {
            DSSKEY_WARN("pClickAction is NULL!.\n");
            return;
        }
        // 设置长按按钮动作
        pAction = NULL;
        pAction = new CDsskeyLongClickAction;
        if (pAction != NULL)
        {
            m_pDsskeyFrame->SetBtnLongPressAction(pAction);
        }
        else
        {
            DSSKEY_WARN("pLongAction is NULL!.\n");
            return;
        }

        // 设置切页按钮
        pPushBtn = new qDsskeyButton;
        if (pPushBtn != NULL)
        {
            m_pDsskeyFrame->AddUIPushBtn(pPushBtn, this, SLOT(OnDsskeyBtnClick()));
            // 设置图标路径
            pPushBtn->setIconPic(PIC_DSS_PAGE_CONTRACT);
            pPushBtn->setCurrIcon(PIC_DSS_PAGE_CONTRACT);
            pPushBtn->setDownIconPic(PIC_DSS_PAGE_CONTRACT_DOWN);
            // 设置按键显示文字
#if IF_BUG_30875
            pPushBtn->setText(LANG_TRANSLATE(TRID_SHOW_LESS));
#else
            pPushBtn->setText(LANG_TRANSLATE(TRID_DSS_KEY));
#endif
            // 右排Dsskey按钮
            pPushBtn->setGeometry(DSSKEY_DSSKEYUI_LEFT_OFFSET_L + 4 * DSSKEY_DSSKEYUI_H_DISTANCE,
                                  DSSKEY_DSSKEYUI_TOP_OFFSET + 5 * DSSKEY_DSSKEYUI_V_DISTANCE, DSSKEY_DSSKEYUI_WIDTH,
                                  DSSKEY_DSSKEYUI_HEIGHT);

        }
    }
    else
    {
        IDLE_WARN("m_pDsskeyFrame is NULL!.\n");
        return;
    }
    m_pDsskeyFrame->show();
    m_pDsskeyFrame->raise();
}

// 点击了最后一个Dsskey按键
void  CDlgFullDsskeyUI::OnDsskeyBtnClick()
{
    CBaseDialog * pBaseDialog = UIManager_GetTopWindow();
    if (NULL != pBaseDialog)
    {
        pBaseDialog->OnBackButtonClick();
    }
}

void CDlgFullDsskeyUI::OnDsskeyBtnClick(int iIndex)
{
    if (dsskey_GetDsskeyType(iIndex) != DT_CUSTOM)
    {
        return;
    }

    OnDsskeyBtnClick();
}


bool CDlgFullDsskeyUI::OnTransparencyChange()
{
    DrawDsskeyBtnOnBackground();

    return true;
}

// 更新界面背景
void CDlgFullDsskeyUI::RefreshBackGroundPic()
{
    yl::string strBkTmp;

#if IF_FEATURE_T48_DSSKEY_WALLPAPER
    strBkTmp = g_pDisplayController->GetDsskeyWallpaper();
    if (strcasecmp(strBkTmp.c_str(), "auto") == 0)
    {
        strBkTmp = g_pDisplayController->GetCurrentWallpaper();
    }
#else
    strBkTmp = PIC_THEME1_OTHER_BACKGROUND;
#endif

    if (strBkTmp == m_strBGPicPath || strBkTmp.empty())
    {
        DSSKEYUI_INFO("CDlgFullDsskeyUI::RefreshBackGroundPic() wallpaper no change\
					   or dsskey wallpaper no exist!");
        return;
    }

    m_strBGPicPath = strBkTmp;

    DrawDsskeyBtnOnBackground();
}

void CDlgFullDsskeyUI::DrawDsskeyBtnOnBackground()
{
    if (!m_strBGPicPath.empty())
    {
        m_pmBG = STOCK_GET_BMP(m_strBGPicPath.c_str());
    }

    if (m_pmBG.isNull())
    {
#if IF_FEATURE_T48_DSSKEY_WALLPAPER
        m_strBGPicPath = g_pDisplayController->GetCurrentWallpaper();
        m_pmBG = STOCK_GET_BMP(m_strBGPicPath.c_str());
        if (m_pmBG.isNull())
        {
            m_pmBG = THEME_GET_BMP(PIC_THEME1_OTHER_BACKGROUND);
        }
#else
        m_pmBG = STOCK_GET_BMP(PIC_THEME1_OTHER_BACKGROUND);
#endif
    }

    if (m_pmBG.size().width() != PHONE_WIDTH
            || m_pmBG.size().height() != PHONE_HEIGHT)
    {
        m_pmBG = m_pmBG.scaled(PHONE_WIDTH, PHONE_HEIGHT);
    }

    QPainter painter(&m_pmBG);

    // 绘制Title背景
    QPixmap pmTitleBg = THEME_GET_BMP(g_TransWidgetBgControler.GetTitleBarBg().c_str());
    painter.drawPixmap(QRect(0, 0, PHONE_WIDTH, TITLE_HEIGHT), pmTitleBg);

    // 绘制Dsskey图标
    QPixmap pmBtnBg = THEME_GET_BMP(g_TransWidgetBgControler.GetNormalDsskeyBg().c_str());

    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            painter.drawPixmap(DSSKEY_DSSKEYUI_LEFT_OFFSET_L + i * DSSKEY_DSSKEYUI_H_DISTANCE,
                               50 + j * DSSKEY_DSSKEYUI_V_DISTANCE, pmBtnBg);
        }
    }
}

QPixmap CDlgFullDsskeyUI::GetCustomBackGround()
{
    RefreshBackGroundPic();

    return m_pmBG;
}

//事件过滤器
bool CDlgFullDsskeyUI::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL
            || pEvent == NULL)
    {
        return false;
    }

    if (isChildOf(pObject, this)
            || isChildOf(this, pObject))
    {
        //按X键退出
        if (pEvent->type() == QEvent::KeyPress)
        {
            QKeyEvent * pKeyEvent  = (QKeyEvent *)pEvent;
            if (pKeyEvent->key() == PHONE_KEY_CANCEL
#if IF_BUG_32044
                    || pKeyEvent->key() == PHONE_KEY_FULLDSSKEY_BACK
#endif
               )
            {
                CBaseDialog * pBaseDialog = UIManager_GetTopWindow();
                if (NULL != pBaseDialog)
                {
                    pBaseDialog->OnBackButtonClick();
                }
                return true;
            }
        }
    }
    return CBaseDialog::eventFilter(pObject, pEvent);
}

