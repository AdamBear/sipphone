#include "cpagebutton.h"
#include "dsskeyui/dsskeyui_common.h"
#include "xwindow/xWindowManager.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "baseui/include/t2xpicpath.h"

namespace
{
//慢闪设置时间 500 ms
#define     PAGE_FLASH_SLOW_TIME    500
//快闪设置时间 300 ms
#define     PAGE_FLASH_FAST_TIME    300

//图标动画状态标识
#define     NO_FLASH_ICON_FLAG      -10

#define     PAGE_ICON_SIZE_T52T54           24
#define     PAGE_ICON_EXTEND_MODE_ICON_WIDTH_T52    12
#define     PAGE_ICON_EXTEND_MODE_ICON_HEIGHT_T52   22

#define PAGE_ICON_BAR_MARGIN_T52 10
#define PAGE_ICON_BAR_EXTEND_MODE_MARGIN_T52 1
}

extern const char * DSSKEY_PAGEBTN_TEXT_COLOR_NORMAL = "normaltextcolor";
extern const char * DSSKEY_PAGEBTN_TEXT_COLOR_FOCUS = "focustextcolor";

CPageButton::CPageButton()
    : m_iActualPageNum(3)
    , m_iCurrentPage(1)
    , m_eDisplayMode(DDM_FIRST)
    , m_colorNormal(0, 0, 0)
    , m_colorFocus(225, 255, 255)
#if !IS_COLOR
    , m_pOldStyle(NULL)
    , m_pNewStyle(NULL)
#endif
{

}


CPageButton::~CPageButton()
{
}

void CPageButton::Init()
{
    //针对每个Icon设一个定时器，用于图标闪烁
    for (int i = 0; i < 3; i++)
    {
        m_eFlashPageStatus[i] = LK_FLASH_BACKGROUND;
        m_iFlashTimerID[i] = -1;
    }
#if !IS_COLOR
    m_pNewStyle = xStyle::getObjectByName("RoundBtnStyle");
    if (m_pNewStyle == NULL)
    {
        m_pNewStyle = m_pOldStyle;
    }
#endif
    //将页面Icon放到Button上
    PaintPageLine();
}

void CPageButton::loadChildrenElements(xml_node& node)
{
    xLinearLayout::loadChildrenElements(node);
    for (xViewIterator it = child_head(); it.hasData(); ++it)
    {
#if IF_XWINDOW_RTTI_CHECK
        xTextView* btn = dynamic_cast<xTextView*>(it.operator ->());
#else
        xTextView* btn = static_cast<xTextView*>(it.operator ->());
#endif
        if (btn != NULL)
        {
            m_vecPageIcon.push_back(btn);
        }
    }
#if !IS_COLOR
    if (m_vecPageIcon.size() != 0)
    {
        m_pOldStyle = (*m_vecPageIcon.begin())->m_pStyle;
    }
#endif
    Init();
}

void CPageButton::loadAttributes(xml_node& node)
{
    xLinearLayout::loadAttributes(node);
    PopupAndroidColor(node, DSSKEY_PAGEBTN_TEXT_COLOR_NORMAL, m_colorNormal);
    PopupAndroidColor(node, DSSKEY_PAGEBTN_TEXT_COLOR_FOCUS, m_colorFocus);
}

//将代表页面的Icon放到翻页Button上
void CPageButton::PaintPageLine()
{
    DSSKEYUI_INFO("CDssKeyPageButton::PaintPageLine %d m_iCurrentPage: %d....", m_iActualPageNum, m_iCurrentPage);

    //大于2页才开始显示
    if (m_iActualPageNum < 2)
    {
        for (PageIconBtnVector::iterator iter = m_vecPageIcon.begin();
                iter != m_vecPageIcon.end(); ++iter)
        {
            (*iter)->hide(true);
        }
        return;
    }

    //绘画之前先擦除原先画的PageIcon
    ErasePageIcon();
    int i = 0;
    PageIconBtnVector::iterator iter;
    xTextView * pPageIconBtn = NULL;
    xPixmap pmIcon;

    for (i = 0, iter = m_vecPageIcon.begin();
            iter != m_vecPageIcon.end(); ++iter, ++i)
    {
        pPageIconBtn = *iter;

        if (i < m_iActualPageNum)
        {
            pPageIconBtn->show();
            if (NO_FLASH_ICON_FLAG == m_iFlashTimerID[i])
            {
                yl::string iconPath;
                switch (m_eFlashIconColor[i])
                {
                case LED_COLOR_TYPE_RED:
                    iconPath = PAGE_ICON_RED;
                    break;
                case LED_COLOR_TYPE_GREEN:
                    iconPath = PAGE_ICON_GREEN;
                    break;
                default:
                    break;
                }
                g_WorkStation.GetImageByPath(iconPath, pmIcon);
                pPageIconBtn->SetTextColor(m_colorNormal);
                pPageIconBtn->setBackground(pmIcon);
            }
            else
            {
                // 当前页绘制页号和灰色实心图标，非当前页绘制灰色圆环图标
                if (i == (m_iCurrentPage - 1))
                {

                    pPageIconBtn->SetTextColor(m_colorFocus);
#if IS_COLOR
                    g_WorkStation.GetImageByPath(PAGE_ICON_GRAY, pmIcon);
                    pPageIconBtn->setBackground(pmIcon);
#else
                    pPageIconBtn->m_pStyle = m_pNewStyle;
                    pPageIconBtn->setBackground(m_colorNormal);
#endif
                }
                else
                {
                    g_WorkStation.GetImageByPath(PAGE_ICON_GRAY_CIRCLE, pmIcon);
                    pPageIconBtn->SetTextColor(m_colorNormal);
#if IS_COLOR
                    pPageIconBtn->setBackground(pmIcon);// toQString(Trans_GetDsskeyPabeBtnNormalBg()));
#else
                    pPageIconBtn->m_pStyle = m_pOldStyle;
                    pPageIconBtn->setBackground(m_colorFocus);
#endif
                }
            }
        }
        else
        {
            pPageIconBtn->hide(true);
        }
    }

    //update();
}

//根据实际页数重设翻页按钮图标位置
void CPageButton::ReSize()
{

}

void CPageButton::SetDisplayMode(DSSKEY_DISPLAY_MODE eDispMode)
{
    if (m_eDisplayMode == eDispMode)
    {
        return;
    }

    m_eDisplayMode = eDispMode;

    if (PT_T52 == devicelib_GetPhoneType())
    {

        for (PageIconBtnVector::iterator iter = m_vecPageIcon.begin();
                iter != m_vecPageIcon.end(); ++iter)
        {
            if (DDM_EXTEND == m_eDisplayMode)
            {
                (*iter)->layout_width = PAGE_ICON_EXTEND_MODE_ICON_WIDTH_T52;
                (*iter)->layout_height = PAGE_ICON_EXTEND_MODE_ICON_HEIGHT_T52;
                PageIconBtnVector::iterator _it = iter;
                if (++_it != m_vecPageIcon.end())
                {
                    (*iter)->layout_marginRight = PAGE_ICON_BAR_EXTEND_MODE_MARGIN_T52;
                }
                else
                {
                    (*iter)->layout_marginRight = 0;
                }
            }
            else
            {
                (*iter)->layout_width = PAGE_ICON_SIZE_T52T54;
                (*iter)->layout_height = PAGE_ICON_SIZE_T52T54;
                (*iter)->layout_marginRight = PAGE_ICON_BAR_MARGIN_T52;
            }
        }
        onChildrenLayoutChanged();
    }
}

//擦除Page按钮上的PageIcon
void CPageButton::ErasePageIcon()
{
    PageIconBtnVector::iterator iter;
    for (iter = m_vecPageIcon.begin();
            iter != m_vecPageIcon.end(); ++iter)
    {
        //(*iter)->SetText("");
#if !IS_COLOR
        (*iter)->m_pStyle = m_pOldStyle;
        (*iter)->setBackground(m_colorFocus);
#else
        (*iter)->setBackground(xColor(0, 0, 0, 0));
#endif
    }
}

//定时器闪烁图标事件
void CPageButton::OnFlashTimer(UINT uTimerID)
{
    yl::string iconPath = "";
    xPixmap pmIcon;
    for (int i = 0; i < m_iActualPageNum; i++)
    {
        switch (m_eFlashIconColor[i])
        {
        case LED_COLOR_TYPE_RED:
            iconPath = PAGE_ICON_RED;
            break;
        case LED_COLOR_TYPE_GREEN:
            iconPath = PAGE_ICON_GREEN;
            break;
        default:
            break;
        }

        if (uTimerID == (UINT)&m_iFlashTimerID[i])
        {
            if (m_eFlashPageStatus[i] == LK_FLASH_BACKGROUND)
            {
                g_WorkStation.GetImageByPath(iconPath, pmIcon);
                m_vecPageIcon[i]->setBackground(pmIcon);
                m_eFlashPageStatus[i] = LK_FLASH_FOREGROUND;
            }
            else
            {
                m_vecPageIcon[i]->setBackground(xColor(
#if IF_PIXEL_ALPHA
                                                    0,
#endif
                                                    0, 0, 0));
                m_eFlashPageStatus[i] = LK_FLASH_BACKGROUND;
            }
        }

    }
}

void CPageButton::SetupPressSlot(xSlotHandler * pReceiver, slotMethod slot)
{
    m_pReceiver = pReceiver;
    m_slot = slot;
}


//闪烁指定页面的图标
void CPageButton::FlashPageIcon(int pageNum, DSS_KEY_COLOR_TYPE eColor,
                                DSS_KEY_FLUSH_TYPE eType)
{
    DSSKEYUI_INFO(" ====== FlashPageIcon %d %d %d ===== ", pageNum, eColor, eType);
#if !IF_DSSKEY_PAGEBTN_FLASH
    return;
#endif
    if (pageNum < 1
            || pageNum > m_iActualPageNum)
    {
        return;
    }

    //当前页不闪烁
    if (pageNum == m_iCurrentPage)
    {
        return;
    }

    //先停止旧的闪烁
    StopFlashPageIcon(pageNum);

    //页码从0开始计数
    pageNum -= 1;

    m_eFlashIconColor[pageNum] = eColor;

    switch (eType)
    {
    case REFRESH_TYPE_NO_FLUSH:
        //常亮不闪烁
        {
            yl::string iconPath = "";

            switch (eColor)
            {
            case LED_COLOR_TYPE_RED:
                iconPath = PAGE_ICON_RED;
                break;
            case LED_COLOR_TYPE_GREEN:
                iconPath = PAGE_ICON_GREEN;
                break;
            default:
                break;
            }
            DSSKEYUI_INFO("_+_+page btn: %s", iconPath.c_str());
            xPixmap pmIcon;
            g_WorkStation.GetImageByPath(iconPath, pmIcon);
            m_vecPageIcon[pageNum]->setBackground(pmIcon);
            m_iFlashTimerID[pageNum] = NO_FLASH_ICON_FLAG;
        }
        break;
    case REFRESH_TYPE_FLUSH:
        //慢闪0.5s
        m_iFlashTimerID[pageNum] = PAGE_FLASH_SLOW_TIME;
        SetTimerEx(m_iFlashTimerID[pageNum]);
        break;
    case REFRESH_TYPE_FAST_FLUSH:
        //快闪0.3s
        m_iFlashTimerID[pageNum] = PAGE_FLASH_FAST_TIME;
        SetTimerEx(m_iFlashTimerID[pageNum]);
        break;
    default:
        break;
    }
}

//停止指定页面的图标闪烁
void CPageButton::StopFlashPageIcon(int pageNum)
{
#if !IF_DSSKEY_PAGEBTN_FLASH
    return;
#endif
    //如果pageNum<0，清空所有的Page动画状态
    if (pageNum < 0)
    {
        int tmpPageCount = m_iActualPageNum;
        while (tmpPageCount > 0)
        {
            StopFlashPageIcon(tmpPageCount--);
        }
        return;
    }

    if (pageNum < 1
            || pageNum > m_iActualPageNum)
    {
        return;
    }
    pageNum -= 1;

    if (NO_FLASH_ICON_FLAG == m_iFlashTimerID[pageNum])
    {
        m_iFlashTimerID[pageNum] = -1;
    }
    //定时器存在（慢闪或快闪），杀掉定时器
    else if (m_iFlashTimerID[pageNum] != -1)
    {
        timerKillThreadTimer((UINT)&m_iFlashTimerID[pageNum]);
        m_iFlashTimerID[pageNum] = -1;
    }

    //如果停止闪烁的页面为当前页（翻页到闪烁页面），则设置灰色实心图标及表示页数的数字，否则设置灰色圆环
    xPixmap pmIcon;
    if (pageNum + 1 == m_iCurrentPage)
    {
        g_WorkStation.GetImageByPath(PAGE_ICON_GRAY, pmIcon);
        m_vecPageIcon[pageNum]->setBackground(pmIcon);
        m_vecPageIcon[pageNum]->SetTextColor(m_colorFocus);
    }
    else
    {
        g_WorkStation.GetImageByPath(PAGE_ICON_GRAY_CIRCLE, pmIcon);
        m_vecPageIcon[pageNum]->setBackground(pmIcon);
    }

}

//翻页
void CPageButton::PageTurning()
{
    DSSKEYUI_INFO("ActualPageNum:%d", m_iActualPageNum);
    if (m_iActualPageNum > 0)
    {
        //翻页前，将当前页设置图标为灰色圆环

#if IS_COLOR
        xPixmap pmIcon;
        g_WorkStation.GetImageByPath(PAGE_ICON_GRAY_CIRCLE, pmIcon);
        //m_vecPageIcon[m_iCurrentPage - 1]->SetTipsIcon(toQString(Trans_GetDsskeyPabeBtnNormalBg()));
        m_vecPageIcon[m_iCurrentPage - 1]->setBackground(pmIcon);
#else
        m_vecPageIcon[m_iCurrentPage - 1]->m_pStyle = m_pOldStyle;
        m_vecPageIcon[m_iCurrentPage - 1]->SetTextColor(m_colorNormal);
        m_vecPageIcon[m_iCurrentPage - 1]->setBackground(m_colorFocus);
#endif
        //m_vecPageIcon[m_iCurrentPage - 1]->SetText("");

        m_iCurrentPage = m_iCurrentPage >= m_iActualPageNum ? 1 : m_iCurrentPage + 1;

        //翻页后，如果当前页有图标闪烁，则停止闪烁，否则直接设置灰色实心图标和数字
        if (m_iFlashTimerID[m_iCurrentPage - 1] != -1)
        {
            StopFlashPageIcon(m_iCurrentPage);
        }
        else
        {

#if IS_COLOR
            g_WorkStation.GetImageByPath(PAGE_ICON_GRAY, pmIcon);
            m_vecPageIcon[m_iCurrentPage - 1]->setBackground(pmIcon);
#else
            m_vecPageIcon[m_iCurrentPage - 1]->m_pStyle = m_pNewStyle;
            m_vecPageIcon[m_iCurrentPage - 1]->setBackground(m_colorNormal);
#endif
            m_vecPageIcon[m_iCurrentPage - 1]->SetTextColor(m_colorFocus);
        }

        //发出当前页面改变的消息
        //emit CurrentPageTurned(m_iCurrentPage);

        PaintPageLine();
    }
}

//翻到指定页面
void CPageButton::TurnToPage(int pageNum)
{
    if (m_iActualPageNum > 0)
    {
        if (pageNum == m_iCurrentPage)
        {
            return;
        }

        //翻页前，将当前页设置图标为灰色圆环

#if IS_COLOR
        xPixmap pmIcon;
        g_WorkStation.GetImageByPath(PAGE_ICON_GRAY_CIRCLE, pmIcon);
        m_vecPageIcon[m_iCurrentPage - 1]->setBackground(pmIcon);
#else
        m_vecPageIcon[m_iCurrentPage - 1]->m_pStyle = m_pNewStyle;
        m_vecPageIcon[m_iCurrentPage - 1]->SetTextColor(m_colorFocus);
        m_vecPageIcon[m_iCurrentPage - 1]->setBackground(m_colorNormal);
#endif
        //m_vecPageIcon[m_iCurrentPage - 1]->SetText("");

        m_iCurrentPage = pageNum > m_iActualPageNum ? m_iActualPageNum : (pageNum < 1 ? 1 : pageNum);

        //翻页后，如果当前页有图标闪烁，则停止闪烁，否则直接设置灰色实心图标和数字
        if (m_iFlashTimerID[m_iCurrentPage - 1] != -1)
        {
            StopFlashPageIcon(m_iCurrentPage);
        }
        else
        {

#if IS_COLOR
            g_WorkStation.GetImageByPath(PAGE_ICON_GRAY, pmIcon);
            m_vecPageIcon[m_iCurrentPage - 1]->setBackground(pmIcon);
#else
            m_vecPageIcon[m_iCurrentPage - 1]->m_pStyle = m_pNewStyle;
            m_vecPageIcon[m_iCurrentPage - 1]->setBackground(m_colorNormal);
#endif
            m_vecPageIcon[m_iCurrentPage - 1]->SetTextColor(m_colorFocus);
        }

        //发出当前页面改变的消息

        PaintPageLine();
    }
}

//根据Dsskey配置情况设置当前页数
bool CPageButton::SetActualPageNum(int pageNum, bool bFlush)
{
    DSSKEYUI_INFO("CDssKeyPageButton::SetActualPageNum:%d", pageNum);
    bool bRet = false;
    int iActualPageNum = pageNum > m_iPageLimit ? m_iPageLimit : (pageNum < 0 ? 0 : pageNum);

    if (iActualPageNum != m_iActualPageNum)
    {
        m_iActualPageNum = iActualPageNum;

        //m_iActualPageNum大于1时才显示样式
        setDisable(m_iActualPageNum <= 1);

        //改变页数后重设当前页
        if (m_iActualPageNum > 0
                && m_iCurrentPage > pageNum)
        {
            m_iCurrentPage = pageNum;
            //emit CurrentPageTurned(m_iCurrentPage);
        }
        if (bFlush)
        {
            // 重新绘制翻页按钮
            PaintPageLine();
        }
        bRet = true;
    }
    return bRet;
}

// 设置当前最大允许的页数
void CPageButton::SetMaxPage(int iMaxPage)
{
    if (iMaxPage < 0)
    {
        DSSKEYUI_WARN("CDssKeyPageButton::SetMaxPage(%d)", iMaxPage);
        return;
    }
    m_iPageLimit = iMaxPage;
}

// 设置页码信息
void CPageButton::SetPageInfo(int iPageNum, int iPageIndex)
{
    bool bPageNum = SetActualPageNum(iPageNum, false);
    bool bCurPage = SetCurrentPage(iPageIndex, false);
    if (bPageNum || bCurPage)
    {
        // 重新绘制翻页按钮
        PaintPageLine();
    }
}

bool CPageButton::SetCurrentPage(int iCurrentPageIndex, bool bFlush)
{
    bool bRet = false;

    if (iCurrentPageIndex < 0
            || iCurrentPageIndex > m_iActualPageNum - 1)
    {
        DSSKEYUI_WARN("CDssKeyPageButton::SetCurrentPage(%d)", iCurrentPageIndex);
    }
    else
    {
        int nCurpage = m_iCurrentPage % m_iPageLimit;
        //  m_vecIconState[nCurpage] = BK_BLACK;
        m_iCurrentPage = iCurrentPageIndex + 1;
        nCurpage = m_iCurrentPage % m_iPageLimit;
        //  m_vecIconState[nCurpage] = BK_WHITE;
        if (bFlush)
        {
            // 重新绘制翻页按钮
            PaintPageLine();
        }
        bRet = true;
    }

    return bRet;
}

void CPageButton::SetBackgroundImage(const yl::string & strNormalBkg,
                                     const yl::string & strDisabledBkg)
{
    m_strNormalBkgImg = strNormalBkg;
    m_strDisabledBkgImg = strDisabledBkg;

    update();
}

//鼠标点击翻页
void CPageButton::OnBtnClick(bool bPress)
{
    if (bPress)
    {
        PageTurning();

        if (m_pReceiver != NULL && m_slot != NULL_METHOD)
        {
            ((m_pReceiver)->*(m_slot))(NULL, (WPARAM)0, 0, 0, this);
        }
    }
}

