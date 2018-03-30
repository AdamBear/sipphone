#include "dlgdisplaygeneral.h"
#include "lamp/backlight/include/modbacklight.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "keymap.h"
#include "uikernel/qcomboboxutility.h"
#include "dsklog/log.h"

CDlgDisplayGeneral::CDlgDisplayGeneral(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_bLoadData(false)
    , m_pCmbBoxBacklightTime(NULL)
    , m_pCmbBoxActiveLevel(NULL)
    , m_pCmbBoxInactiveLevel(NULL)
{
    //初始化数据
    InitData();
}

CDlgDisplayGeneral::~CDlgDisplayGeneral()
{

}

// 初始化数据
void CDlgDisplayGeneral::Init()
{
    backlight_ShowBacklightUI(true);
    backlight_OpenBacklightEvent(false);
    m_bLoadData = false;
}

// 释放数据
void CDlgDisplayGeneral::Uninit()
{
    backlight_ShowBacklightUI(false);
    backlight_OpenBacklightEvent(true);
    m_bLoadData = false;
}

void CDlgDisplayGeneral::showEvent(QShowEvent * pEvent)
{
    // 关闭背光灯事件
    backlight_OpenBacklightEvent(false);

    // 点亮屏幕
    if (m_bLoadData)
    {
        // 若已经正常读取数据则采用当前ActiveLevel的亮度
        ResetBacklight();
    }
    else
    {
        DisplayGeneralData displayGeneralData;
        bool bIsSuccess = g_pDisplayController->GetGeneralData(&displayGeneralData);
        if (bIsSuccess)
        {
            // 使屏幕的亮度和ActiveLevel一样
            ResetBacklight(displayGeneralData.m_nActiveLevel);
        }
        else
        {
            // 读取失败则默认设置为最亮
            ResetBacklight(10);
        }
    }

    //调用父类函数进行进入界面时的焦点设置
    CDlgBaseSubPage::showEvent(pEvent);
}

void CDlgDisplayGeneral::hideEvent(QHideEvent * pEvent)
{
    // 重新开启背光灯事件
    backlight_OpenBacklightEvent(true);
}

//加载数据
bool CDlgDisplayGeneral::LoadPageData()
{
    this->setFocus();

    DisplayGeneralData displayGeneralData;
    bool bIsSuccess =
        g_pDisplayController->GetGeneralData(&displayGeneralData);
    if (!bIsSuccess)
    {
        return false;
    }

    SetCurrentPageData(displayGeneralData);
    return true;
}

//获取数据状态是否改变
bool CDlgDisplayGeneral::IsStatusChanged()
{
    // 获取控制层数据
    DisplayGeneralData objModelData;
    g_pDisplayController->GetGeneralData(&objModelData);

    // 获取页面数据
    DisplayGeneralData objPageData;
    GetCurrentPageData(objPageData);

    //比较控制层数据和UI数据
    return !ComparePageData(objModelData, objPageData);
}

//保存数据
bool CDlgDisplayGeneral::SavePageData()
{
    DisplayGeneralData objPageData;
    GetCurrentPageData(objPageData);
    g_pDisplayController->SetGeneralData(objPageData);

    // 刷新Event
    backlight_RefreshEvent();

    return true;
}

//页面向其数据Mode取数据再次刷新到界面上
void CDlgDisplayGeneral::Refresh()
{

}

//初始化数据，设置控件的属性、样式、数据等都在其中进行
void CDlgDisplayGeneral::InitData()
{
    m_pCmbBoxActiveLevel = new QComboBox(this);
    m_pCmbBoxBacklightTime = new QComboBox(this);
    m_pCmbBoxInactiveLevel = new QComboBox(this);

    //指针判空
    if (m_pCmbBoxActiveLevel == NULL
            || m_pCmbBoxBacklightTime == NULL
            || m_pCmbBoxInactiveLevel == NULL)
    {
        return;
    }

    // Active level. 1-10.
    for (int i = 1; i <= 10; i++)
    {
        m_pCmbBoxActiveLevel->addItem(QString::number(i),
                                      QVariant::fromValue(i));
    }

    // Inactive level. 0, 1.
    m_pCmbBoxInactiveLevel->addItem(LANG_TRANSLATE(TRID_LOW), QVariant::fromValue(1));
    m_pCmbBoxInactiveLevel->addItem(LANG_TRANSLATE(TRID_OFF), QVariant::fromValue(0));

    // Back light time(min). 0-Always on.
    m_pCmbBoxBacklightTime->addItem(LANG_TRANSLATE(TRID_ALWAYS_ON),
                                    QVariant::fromValue(0));
    m_pCmbBoxBacklightTime->addItem(LANG_TRANSLATE(TRID_15_SECONDS),
                                    QVariant::fromValue(1 * 15));
    m_pCmbBoxBacklightTime->addItem(LANG_TRANSLATE(TRID_30_SECONDS),
                                    QVariant::fromValue(2 * 15));
    m_pCmbBoxBacklightTime->addItem(LANG_TRANSLATE(TRID_1_MINUT),
                                    QVariant::fromValue(4 * 15));
    m_pCmbBoxBacklightTime->addItem(LANG_TRANSLATE(TRID_2_MINUTS),
                                    QVariant::fromValue(8 * 15));
    m_pCmbBoxBacklightTime->addItem(LANG_TRANSLATE(TRID_5_MINUTS),
                                    QVariant::fromValue(20 * 15));
    m_pCmbBoxBacklightTime->addItem(LANG_TRANSLATE(TRID_10_MINUTS),
                                    QVariant::fromValue(40 * 15));
    m_pCmbBoxBacklightTime->addItem(LANG_TRANSLATE(TRID_30_MINUTS),
                                    QVariant::fromValue(120 * 15));
    //设置下拉框样式
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxActiveLevel, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxBacklightTime, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxInactiveLevel, Qt::black);

    connect(m_pCmbBoxActiveLevel, SIGNAL(currentIndexChanged(int)),
            this, SLOT(OnActiveLevelChange(int)));
    connect(m_pCmbBoxActiveLevel, SIGNAL(highlighted(int)),
            this, SLOT(OnActiveLevelChange(int)));
    //取消节电模式预览

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_BACKLIGHT_ACTIVE_LEVEL);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxActiveLevel;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_BACKLIGHT_INACTIVE_LEVEL);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxInactiveLevel;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_BACKLIGHT_TIME);
    qPair.first = strLab;
    qPair.second = m_pCmbBoxBacklightTime;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    this->installEventFilter(this);
    m_pCmbBoxActiveLevel->installEventFilter(this);
    m_pCmbBoxBacklightTime->installEventFilter(this);
    m_pCmbBoxInactiveLevel->installEventFilter(this);
}
//用于界面析构时调用。在里面进行一些释放资源的操作，比如注销对信息的监控等。
void CDlgDisplayGeneral::ReleaseData()
{

}
//于界面每次show()出来的时候，重置界面的一些数据,输入法的显示等
void CDlgDisplayGeneral::OnViewShow()
{

}

void CDlgDisplayGeneral::OnActiveLevelChange(int iCmbIndex)
{
    if (m_bLoadData)
    {
        backlight_SetBacklightLevel(m_pCmbBoxActiveLevel->itemData(iCmbIndex, Qt::UserRole).value<int>());
    }
}

//比较页面前后数据
bool CDlgDisplayGeneral::ComparePageData(const DisplayGeneralData & beforePageData,
        const DisplayGeneralData & afterPageData) const
{
    if (beforePageData.m_nActiveLevel != afterPageData.m_nActiveLevel
            || beforePageData.m_nInactiveLevel != afterPageData.m_nInactiveLevel
            || beforePageData.m_nBackLightTime != afterPageData.m_nBackLightTime)
    {
        //前后页面数据不相等返回false
        return false;
    }
    else
    {
        //前后页面数据相等返回true
        return true;
    }
}

//取得当前页面数据
DisplayGeneralData & CDlgDisplayGeneral::GetCurrentPageData(
    DisplayGeneralData & currentPageData) const
{
    //指针判空
    if (m_pCmbBoxActiveLevel != NULL
            && m_pCmbBoxBacklightTime != NULL
            && m_pCmbBoxInactiveLevel != NULL)
    {
        //取得Active Level 值
        currentPageData.m_nActiveLevel = m_pCmbBoxActiveLevel->itemData(
                                             m_pCmbBoxActiveLevel->currentIndex()).value<int>();
        //取得Inactive Level值
        currentPageData.m_nInactiveLevel = m_pCmbBoxInactiveLevel->itemData(
                                               m_pCmbBoxInactiveLevel->currentIndex()).value<int>();
        //取得Backlight Time 值
        currentPageData.m_nBackLightTime = m_pCmbBoxBacklightTime->itemData(
                                               m_pCmbBoxBacklightTime->currentIndex()).value<int>();
    }

    return currentPageData;
}

//设置当前页面数据
void CDlgDisplayGeneral::SetCurrentPageData(DisplayGeneralData & currentPageData)
{
    //指针判空
    if (m_pCmbBoxActiveLevel == NULL
            || m_pCmbBoxBacklightTime == NULL
            || m_pCmbBoxInactiveLevel == NULL)
    {
        return;
    }

    //定义nCount用于记录combobox的项数
    int nCount = m_pCmbBoxActiveLevel->count();
    //设置Active Level值
    int nActiveLevel = currentPageData.m_nActiveLevel;
    if (nActiveLevel < 0 || nActiveLevel > 10)
    {
        // 超出范围,使用默认值
        nActiveLevel = 8;
    }
    for (int i = 0; i < nCount; i++)
    {
        //查找相等项
        if (m_pCmbBoxActiveLevel->itemData(i).value<int>()
                == nActiveLevel)
        {
            //设置值匹配的项
            m_pCmbBoxActiveLevel->setCurrentIndex(i);
        }
    }

    nCount = m_pCmbBoxInactiveLevel->count();
    int nInActiveLevel = currentPageData.m_nInactiveLevel;
    if (nInActiveLevel < 0 || nInActiveLevel > 1)
    {
        // 超出范围,使用默认值
        nInActiveLevel = 1;
    }
    for (int i = 0; i < nCount; i++)
    {
        //查找相等项
        if (m_pCmbBoxInactiveLevel->itemData(i).value<int>()
                == nInActiveLevel)
        {
            //设置值匹配的项
            m_pCmbBoxInactiveLevel->setCurrentIndex(i);
        }
    }

    nCount = m_pCmbBoxBacklightTime->count();
    // 判断定时区间在哪个段
    int CurBackTime = currentPageData.m_nBackLightTime;
    SETTINGUI_INFO("CurBackTimeis %d ", CurBackTime);
    if (CurBackTime == 0)
    {
        CurBackTime = 0;
    }
    else if (CurBackTime < 0)
    {
        CurBackTime = 0;
    }
    else if (CurBackTime >= 1 && CurBackTime <= 22)
    {
        CurBackTime = 15;
    }
    else if (CurBackTime > 22 && CurBackTime <= 44)
    {
        CurBackTime = 30;
    }
    else if (CurBackTime > 44 && CurBackTime <= 89)
    {
        CurBackTime = 60;
    }
    else if (CurBackTime > 89 && CurBackTime <= 209)
    {
        CurBackTime = 120;
    }
    else if (CurBackTime > 209 && CurBackTime <= 449)
    {
        CurBackTime = 300;
    }
    else if (CurBackTime > 449 && CurBackTime <= 1199)
    {
        CurBackTime = 600;
    }
    else
    {
        CurBackTime = 1800;
    }
    SETTINGUI_INFO("CurBackTimeis %d ", CurBackTime);
    for (int i = 0; i < nCount; i++)
    {
        //查找相等项
        if (m_pCmbBoxBacklightTime->itemData(i).value<int>()
                == CurBackTime)
        {
            //设置值匹配的项
            m_pCmbBoxBacklightTime->setCurrentIndex(i);
        }
    }
    // 加载完数据后设置其标志位为0
    m_bLoadData = true;
}

// 重设背景的亮度
void CDlgDisplayGeneral::ResetBacklight(int iLightLeve/* = -1*/)
{
    if (iLightLeve == -1)
    {
        int iActiveLevel = -1;
        if (NULL != m_pCmbBoxActiveLevel)
        {
            iActiveLevel = m_pCmbBoxActiveLevel->itemData(
                               m_pCmbBoxActiveLevel->currentIndex(), Qt::UserRole).value<int>();
        }

        backlight_SetBacklightLevel(iActiveLevel);
    }
    else
    {
        backlight_SetBacklightLevel(iLightLeve);
    }
}

bool CDlgDisplayGeneral::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 不处理非自身的消息
    if (!isChildOf(pObject, this) && pObject != this)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::MouseButtonPress:
    {
        ResetBacklight();
    }
    break;
    case QEvent::FocusIn:
    {
        //!!!!! T46为什么删除
        if (pObject == m_pCmbBoxBacklightTime)
        {
            ResetBacklight();
        }
        else if (pObject == m_pCmbBoxActiveLevel && m_bLoadData)
        {
            // 设置背光灯为iActiveLevel
            int iActiveLevel = -1;
            if (NULL != m_pCmbBoxActiveLevel)
            {
                iActiveLevel = m_pCmbBoxActiveLevel->itemData(
                                   m_pCmbBoxActiveLevel->currentIndex(), Qt::UserRole).value<int>();
            }

            backlight_SetBacklightLevel(iActiveLevel);
        }
        //取消节电模式预览，Bug #30722
        else
        {
            // 上下移动焦点需要重新点亮背光灯
            ResetBacklight();
        }
        //!!!!! T46为什么删除
    }
    break;
    case QEvent::KeyPress:
    {
        QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
        int nKey = pKeyEvent->key();
        switch (nKey)
        {
        case PHONE_KEY_LEFT:
        case PHONE_KEY_UP:
        case PHONE_KEY_RIGHT:
        case PHONE_KEY_DOWN:
        case PHONE_KEY_OK:
        {
            // none
        }
        break;
        default:
        {
            ResetBacklight();
        }
        break;
        }
    }
    break;
    default:
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}
