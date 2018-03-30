#include "commonunits/commonunits_def.h"
#include "iconlist.h"
#include "uimanager/moduimanager.h"
#include "testmode/include/modtestmode.h"
#include "xwindow/xControls.h"
#include "idleuicommon.h"
#include "baseui/include/t2xpicpath.h"

namespace
{
#define ICON_FLASHING_SHOW_TIME_DEFAULT         1500
#define ICON_FLASHING_HIDE_TIME_DEFAULT         1500

#define FIEXD_ICON_FLASHING_SHOW_TIME           2000
#define FIEXD_ICON_FLASHING_HIDE_TIME           1000
// 默认图标个数
#define ICON_MAX_NUM_DEFAULT                    5
#define ICON_SAPCE_DEFAULT                      2

#if IF_IDLE_SHOW_ICON_HINT
#define ICON_HINT_ID                            "icon_hint"
#define ICON_HINT_BACKGRAOUND_ID                "icon_hint_background"
#define ICON_HINT_TEXT_LIMIT                    2
#define ICON_HINT_FONT_SIZE                     11
#define ICON_HINT_TEXT_COLOR                    xColor(0xff, 0xff, 0xff)
#define ICON_HINT_CONTENT_MARGIN_BOTTOM            3
#define ICON_HINT_CONTENT_HEIGHT                   11
#define ICON_HINT_CONTENT_WIDTH                    11
#define ICON_HINT_CONTENT_LONG_WIDTH               16
#endif
};

//////////////////////////////////////////////////////////////////////////
// CIconList
CFlashIcon::CFlashIcon()
    : m_eIconType(IT_UNKNOWN)
    , m_bNeedFlash(false)
{

}

CFlashIcon::~CFlashIcon()
{

}

void CFlashIcon::SetFlash(bool bFlash)
{
    m_bNeedFlash = bFlash;
}

void CFlashIcon::SetIconType(ICON_TYPE eIconType)
{
    m_eIconType = eIconType;
}

//////////////////////////////////////////////////////////////////////////
// CIconList
CIconList::CIconList()
    : m_uShowTime(ICON_FLASHING_SHOW_TIME_DEFAULT)
    , m_uHideTime(ICON_FLASHING_HIDE_TIME_DEFAULT)
    , m_bLeft2Right(false)
    , m_pLayoutView(NULL)
    , m_uIconSpace(ICON_SAPCE_DEFAULT)
    , m_uMaxNum(ICON_MAX_NUM_DEFAULT)
{

}

CIconList::~CIconList()
{

}

void CIconList::SetFlashTime(UINT uShowTime, UINT uHideTime)
{
    m_uShowTime = uShowTime;
    m_uHideTime = uHideTime;
}

void CIconList::InitLayoutView(xViewGroup* pLayoutView)
{
    if (NULL == pLayoutView)
    {
        return;
    }
    m_pLayoutView = pLayoutView;
}

int CIconList::GetIconListSize()
{
    if (NULL == m_pLayoutView)
    {
        return 0;
    }
    return m_pLayoutView->childrenCount();
}

void CIconList::SetIconMax(UINT uMaxNum)
{
    m_uMaxNum = uMaxNum;
}

void CIconList::SetLayoutDirection(bool bLeft2Right)
{
    m_bLeft2Right = bLeft2Right;
}

void CIconList::SetIconSpace(UINT uIconSpace)
{
    m_uIconSpace = uIconSpace;
}

//////////////////////////////////////////////////////////////////////////
// CNormalIconList
CNormalIconList::CNormalIconList()
{

}

CNormalIconList::~CNormalIconList()
{

}

bool CNormalIconList::AddIcon(ICON_TYPE eIconType, bool bNeedFlash, const yl::string& strHint/* = ""*/, xViewGroup* pLayoutView /*= NULL*/)
{
    yl::string strIcon;
    if (!IconType2PicPath(eIconType, strIcon))
    {
        return false;
    }

    if (!AddIcon(strIcon, bNeedFlash, strHint, pLayoutView))
    {
        return false;
    }

    m_listIconType.push_back(eIconType);

    return true;
}

bool CNormalIconList::AddIcon(const yl::string& strFileName, bool bFlashIcon, const yl::string& strHint, xViewGroup* pLayoutView)
{
    if (pLayoutView == NULL)
    {
        pLayoutView = m_pLayoutView;
    }
    if (NULL == pLayoutView)
    {
        return false;
    }

    if (pLayoutView->childrenCount() >= m_uMaxNum && m_uMaxNum > 0)
    {
        return false;
    }

    xImageView* pImage = static_cast<xImageView*>(pLayoutView->getViewById(strFileName));

    if (NULL != pImage)
    {
        IDLEUI_INFO("CNormalIconList::AddIcon file [%s] already extist", strFileName.c_str());
        return false;
    }

    IDLEUI_INFO("CNormalIconList::AddIcon file [%s]", strFileName.c_str());

    pImage = new xImageView();
    pImage->layout_width = LayoutParams::wrap_content;
    pImage->layout_height = LayoutParams::wrap_content;
    pImage->setPixmap(strFileName);

    if (bFlashIcon
#if IF_IDLE_SHOW_ICON_HINT
            || !strHint.empty()
#endif
       )
    {
        xFlashingBox* pFlashBox = new xFlashingBox();
        if (NULL == pFlashBox)
        {
            return false;
        }

        pFlashBox->m_strId = strFileName;
        pFlashBox->layout_width = LayoutParams::wrap_content;
        pFlashBox->layout_height = LayoutParams::match_parent;
        pFlashBox->paddingLeft = m_uIconSpace;
        pFlashBox->addChildView(pImage);

        FrameLayoutParams* layoutInParent = FrameLayoutParams::GetLayoutParams(pImage->m_pLayoutInParent);
        if (layoutInParent != NULL)
        {
            layoutInParent->layout_gravity = gravity_center;
        }

        if (m_bLeft2Right)
        {
            pLayoutView->addChildView(pFlashBox);
        }
        else
        {
            pLayoutView->insertChildView(0, pFlashBox);
        }
#if IF_IDLE_SHOW_ICON_HINT
        AddHintView(pFlashBox, strHint, true);
        if (bFlashIcon)
        {
            pFlashBox->setupFlashTime(m_uShowTime, m_uHideTime);
        }
#else
        pFlashBox->setupFlashTime(m_uShowTime, m_uHideTime);
#endif

    }
    else
    {
        pImage->m_strId = strFileName;
        pImage->paddingLeft = m_uIconSpace;
        if (m_bLeft2Right)
        {
            pLayoutView->addChildView(pImage);
        }
        else
        {
            pLayoutView->insertChildView(0, pImage);
        }
    }

    pLayoutView->show();

    return true;
}

#if IF_IDLE_SHOW_ICON_HINT
void CNormalIconList::AddHintView(xViewGroup* pViewGroup, const yl::string& strHint, bool bTextBackground)
{
    if (NULL == pViewGroup || strHint.empty())
    {
        return;
    }

    bool bExceedLimit = false;
    if (strHint.length() >= ICON_HINT_TEXT_LIMIT)
    {
        bExceedLimit = true;
    }

    xImageView* pTextBk = NULL;
    if (bTextBackground)
    {
        pTextBk = static_cast<xImageView*>(pViewGroup->getViewById(ICON_HINT_BACKGRAOUND_ID));
        if (NULL == pTextBk)
        {
            pTextBk = new xImageView();
            if (NULL == pTextBk)
            {
                return;
            }
            AddHintChildView(pViewGroup, pTextBk, ICON_HINT_BACKGRAOUND_ID, bExceedLimit);
        }

        if (bExceedLimit)
        {
            pTextBk->setPixmap(PIC_ICON_STATUS_MORE_WORDS_HINTS_BG);
        }
        else
        {
            pTextBk->setPixmap(PIC_ICON_STATUS_ONE_WORD_HINTS_BG);
        }
    }

    // Hint
    xTextView* pTextFootnote = static_cast<xTextView*>(pViewGroup->getViewById(ICON_HINT_ID));
    if (NULL == pTextFootnote)
    {
        pTextFootnote = new xTextView();
        if (NULL == pTextFootnote)
        {
            return;
        }

        AddHintChildView(pViewGroup, pTextFootnote, ICON_HINT_ID, bExceedLimit);
        pTextFootnote->SetGravity(gravity_center);
        xFont fontFootnote = pTextFootnote->GetFont();
        fontFootnote.setPointSize(ICON_HINT_FONT_SIZE);
        pTextFootnote->SetTextColor(ICON_HINT_TEXT_COLOR);
        pTextFootnote->SetFont(fontFootnote);
    }

    pTextFootnote->SetText(strHint);
}

void CNormalIconList::AddHintChildView(xViewGroup* pParentView, xView* pChildView,
                                       const yl::string& strId, bool bExceedLimit)
{
    if (NULL == pParentView || NULL == pChildView)
    {
        return;
    }

    pChildView->m_strId = strId;
    if (bExceedLimit)
    {
        pChildView->layout_width = ICON_HINT_CONTENT_LONG_WIDTH;
    }
    else
    {
        pChildView->layout_width = ICON_HINT_CONTENT_WIDTH;
    }
    pChildView->layout_height = ICON_HINT_CONTENT_HEIGHT;
    pChildView->layout_marginBottom = ICON_HINT_CONTENT_MARGIN_BOTTOM;
    pParentView->addChildView(pChildView);
    FrameLayoutParams* layoutInParent = FrameLayoutParams::GetLayoutParams(pChildView->m_pLayoutInParent);
    if (layoutInParent != NULL)
    {
        layoutInParent->layout_gravity = gravity_right | gravity_bottom;
    }
}

#endif

bool CNormalIconList::RemoveByType(ICON_TYPE eIconType)
{
    if (NULL == m_pLayoutView)
    {
        return false;
    }

    bool bFind = false;
    LIST_ICON_TYPE::iterator it = m_listIconType.begin();
    for (; it != m_listIconType.end(); it++)
    {
        ICON_TYPE eIType = *it;
        if (eIType == eIconType)
        {
            m_listIconType.erase(it);
            bFind = true;
            break;
        }
    }

    if (!bFind)
    {
        return false;
    }

    yl::string strIcon;
    if (!IconType2PicPath(eIconType, strIcon))
    {
        return false;
    }

    xView* pView = m_pLayoutView->getViewById(strIcon);
    if (NULL == pView)
    {
        return false;
    }

    m_pLayoutView->popup_child(pView);
    return true;
}

bool CNormalIconList::RemoveAll()
{
    if (NULL == m_pLayoutView)
    {
        return false;
    }
    m_pLayoutView->closeAllChildren();
    m_pLayoutView->update();
    m_listIconType.clear();
    return true;
}

//////////////////////////////////////////////////////////////////////////
// CFiexdIconList
CFiexdIconList::CFiexdIconList()
    : m_uFlashCount(0)
    , m_bFlashShow(true)
{
    SetFlashTime(FIEXD_ICON_FLASHING_SHOW_TIME, FIEXD_ICON_FLASHING_HIDE_TIME);

#ifdef IF_FEATURE_FORWARD_OPT
    SetIconDisplay(LCD_ICON_FWD, false);
#endif
}

CFiexdIconList::~CFiexdIconList()
{

}

// 添加图标
bool CFiexdIconList::AddIcon(ICON_TYPE eIconType, bool bNeedFlash, const yl::string& strHint /*= ""*/, xViewGroup* pLayoutView /*= NULL*/)
{
#ifdef IF_FEATURE_FORWARD_OPT
    if (IT_STATE_FWD == eIconType)
    {
        return false;
    }
#endif

    // 遍历列表，寻找Icon的归属
    LIST_FLASH_ICON::iterator it = m_listIcon.begin();
    for (; it != m_listIcon.end(); it++)
    {
        CFlashIcon* pIconInfo = *it;
        if (NULL == pIconInfo
                || pIconInfo->GetIconType() != eIconType)
        {
            continue;
        }

        if (bNeedFlash == pIconInfo->IsNeedFlash())
        {
            // 完全一样不再添加
            return false;
        }

        pIconInfo->SetFlash(bNeedFlash);

        return true;
    }

    // 未知图标类型也不显示
    if (0 != m_uMaxNum
            && GetIconListSize() >= m_uMaxNum || IT_UNKNOWN == eIconType)
    {
        return false;
    }

    CFlashIcon* pIconInfo = new CFlashIcon;
    if (NULL == pIconInfo)
    {
        return false;
    }

    pIconInfo->SetIconType(eIconType);
    pIconInfo->SetFlash(bNeedFlash);
    m_listIcon.push_back(pIconInfo);

    if (bNeedFlash)
    {
        m_uFlashCount++;
        m_bFlashShow = true;
        UpdateFlashIcon();
        ResetFlashTimer();
    }
    else
    {
        SetIconDisplay(GetIconLcdType(pIconInfo->GetIconType()), true);
    }

    return true;
}

// 删除
bool CFiexdIconList::RemoveByType(ICON_TYPE eIconType)
{
    LIST_FLASH_ICON::iterator itr = m_listIcon.begin();
    for (; itr != m_listIcon.end(); ++itr)
    {
        CFlashIcon* pIconInfo = (*itr);
        if (NULL == pIconInfo
                || eIconType != pIconInfo->GetIconType())
        {
            continue;
        }

        LCD_ICON_TYPE eLcdIconType = GetIconLcdType(pIconInfo->GetIconType());

        if (LCD_ICON_LAST == eLcdIconType)
        {
            UIMANAGER_WARN("Remove IconType [%d] failed!", eIconType);
            return false;
        }

        SetIconDisplay(eLcdIconType, false);

        if (pIconInfo->IsNeedFlash())
        {
            m_uFlashCount--;
        }

        delete pIconInfo;
        pIconInfo = NULL;
        m_listIcon.erase(itr);
        break;
    }

    ResetFlashTimer();

    return (itr != m_listIcon.end());
}

// 删除所有
bool CFiexdIconList::RemoveAll()
{
    // 清空内容
    ClearIconList();
    StopFlashTimer();
    m_uFlashCount = 0;
    m_bFlashShow = true;

    // 清空图标
    for (int i = 0; i < LCD_ICON_LAST; ++i)
    {
        SetIconDisplay((LCD_ICON_TYPE)i, false);
    }

    return true;
}

void CFiexdIconList::SetIconDisplay(LCD_ICON_TYPE eIconIndex, bool bShowIcon)
{
#warning
//     if (testmode_IsTestMode())
//     {
//         return ;
//     }

    devicelib_SetIcon(eIconIndex, bShowIcon);
}

// 用于T20显示图标
LCD_ICON_TYPE CFiexdIconList::GetIconLcdType(ICON_TYPE eType)
{
    switch (eType)
    {
    case IT_STATE_VPN:
        {
            return LCD_ICON_VPN;
        }
        break;
    case IT_STATE_DND:
        {
            return LCD_ICON_DND;
        }
        break;
    case IT_STATE_AA:
        {
            return LCD_ICON_AA;
        }
        break;
    case IT_STATE_FWD:
        {
            return LCD_ICON_FWD;
        }
        break;
    case IT_ENCRY:
    case IT_STATE_LOCK:
        {
            return LCD_ICON_LOCK;
        }
        break;
    case IT_STATE_RING_MUTE:
        {
            return LCD_ICON_MUTE;
        }
        break;
    case IT_STATE_INTERNET:
        {
            return LCD_ICON_NET1;
        }
        break;
    case IT_STATE_MISSCALL:
        {
            return LCD_ICON_MISS;
        }
        break;
    case IT_STATE_VOICEMAIL:
        {
            return LCD_ICON_VOICE;
        }
        break;
#warning IT_STATE_HANDSET
        //case IT_STATE_HANDSET:
        //    {
        //        return LCD_ICON_HS;
        //    }
        //    break;
#warning IT_STATE_HANDFREE
        //case IT_STATE_HANDFREE:
        //    {
        //        return LCD_ICON_HF;
        //    }
        //    break;
#warning IT_STATE_HEADSET
    //case IT_STATE_HEADSET:
    //    {
    //        return LCD_ICON_HEADS;
    //    }
    //    break;
    default:
        break;
    }

    return LCD_ICON_LAST;
}

int CFiexdIconList::GetIconListSize()
{
    return (int)m_listIcon.size();
}

void CFiexdIconList::ClearIconList()
{
    //
    LIST_FLASH_ICON::iterator it = m_listIcon.begin();
    for (; it != m_listIcon.end(); ++it)
    {
        CFlashIcon* pIconInfo = (*it);
        if (NULL == pIconInfo)
        {
            continue;
        }
        delete pIconInfo;
        pIconInfo = NULL;
    }
    m_listIcon.clear();
}

void CFiexdIconList::StopFlashTimer()
{
    if (m_timerFlash.IsTimerRuning())
    {
        m_timerFlash.KillTimer();
    }
}

void CFiexdIconList::ResetFlashTimer()
{
    if (0 == m_uFlashCount)
    {
        StopFlashTimer();
    }
    else if (!m_timerFlash.IsTimerRuning())
    {
        UINT uFlashTimer = -1;
        if (m_bFlashShow)
        {
            uFlashTimer = m_uShowTime;
        }
        else
        {
            uFlashTimer = m_uHideTime;
        }
        m_timerFlash.SingleShot(uFlashTimer, TIMER_METHOD(this, CFiexdIconList::OnFlashTimer));
    }
}

void CFiexdIconList::OnFlashTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    m_bFlashShow = !m_bFlashShow;
    UpdateFlashIcon();
    ResetFlashTimer();
}

void CFiexdIconList::UpdateFlashIcon()
{
    LIST_FLASH_ICON::iterator it = m_listIcon.begin();
    // 遍历列表，寻找Timer的归属
    for (UINT uIterIndex = 0; it != m_listIcon.end(); ++it, ++uIterIndex)
    {
        CFlashIcon* pIconInfo = (*it);
        if (NULL != pIconInfo
                && pIconInfo->IsNeedFlash())
        {
            LCD_ICON_TYPE eLcdIconType = GetIconLcdType(pIconInfo->GetIconType());
            if (LCD_ICON_LAST == eLcdIconType)
            {
                UIMANAGER_WARN("Unkonwn eIconType[%d]!", pIconInfo->GetIconType());
                return;
            }
            SetIconDisplay(eLcdIconType, m_bFlashShow);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// 创建IconList对象
CIconList* GetIconListInstance()
{
    static CIconList* s_pIconList = NULL;

    if (NULL == s_pIconList)
    {
        if (DEVICELIB_IS_T42(devicelib_GetPhoneType()))
        {
            s_pIconList =  new CFiexdIconList();
        }
        else
        {
            s_pIconList =  new CNormalIconList();
        }
    }

    return s_pIconList;
}
