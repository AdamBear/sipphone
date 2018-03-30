#include "cdsskeybutton.h"
#include "baseui/include/t2xpicpath.h"
#include "xwindow/xWindowManager.h"
#include "dsskeyui/dsskeyui_common.h"
namespace
{
const char * DSSKEYBTN_LAYOUT_FILE = "dsskey/dsskeybutton.xml";

const char * DSSKEYBTN_TEXT = "textDsskey";

#define DSSKEY_LONG_PRESS_TIME  1000

}
const char * DSSKEY_UI_SHOW_WITHOUT_CONTENT = "android:showwithoutcontent";
// Dsskey显示Text的模式
typedef enum tagDSSKEY_PIC_MODE
{
    PIC_MODE_NORMAL,    // 正常
    PIC_MODE_HILIGHT,   // 高亮
    PIC_MODE_DISABLE,   // 禁用
    PIC_MODE_USING,     // 正在使用当中
    PIC_MODE_MAX
} DSSKEY_PIC_MODE;

// 更换背景图片
yl::string TransLongOrNormalPic(DSSKEY_PIC_MODE ePicMode, DSSKEY_DISPLAY_MODE eDisplayMode)
{
    yl::string strTemp;
    if (DDM_NO_LABEL == eDisplayMode)
    {
        switch (ePicMode)
        {
        case PIC_MODE_NORMAL:
            {
                strTemp = PIC_THEME1_LINEKEY;
            }
            break;
        case PIC_MODE_HILIGHT:
            {
                strTemp = PIC_THEME1_HIGHLIGHT_LINEKEY;
            }
            break;
        case PIC_MODE_DISABLE:
            {
                strTemp = PIC_THEME1_DISABLED_LINEKEY;
            }
            break;
        case PIC_MODE_USING:
            {
                strTemp = PIC_THEME1_DIAL_LINEKEY;
            }
            break;
        default:
            {
                strTemp = PIC_THEME1_LINEKEY;
            }
            break;
        }
    }
    else
    {
        switch (ePicMode)
        {
        case PIC_MODE_NORMAL:
            {
                strTemp = PIC_THEME1_LINEKEY;
            }
            break;
        case PIC_MODE_HILIGHT:
            {
                strTemp = PIC_THEME1_HIGHLIGHT_LINEKEY;
            }
            break;
        case PIC_MODE_DISABLE:
            {
                strTemp = PIC_THEME1_DISABLED_LINEKEY;
            }
            break;
        case PIC_MODE_USING:
            {
                strTemp = PIC_THEME1_DIAL_LINEKEY;
            }
            break;
        default:
            {
                strTemp = PIC_THEME1_LINEKEY;
            }
            break;
        }
    }

    return strTemp;
}

CDsskeyButton::CDsskeyButton()
    : m_pReceiver(NULL)
    , m_slot(NULL_METHOD)
    , m_pLongPressReceiver(NULL)
    , m_slotLongPress(NULL_METHOD)
    , m_eDisplayMode(DDM_NORMAL)
    , m_eGroundPhoto(GROUNDPHOTO_MODE_NORMAL)
    , m_eShowType(DSSKEY_SHOW_DEFAULT)
    , m_eLabelType(DSS_LT_NORMAL)
    , m_eFlushType(REFRESH_TYPE_NO_FLUSH)
    , m_eDssKeyType(DT_NONE)
    , m_etextMode(TEXT_MODE_FIRST_ROW)
    , m_nDsskeyID(-1)
    , m_bFlush(false)
    , m_bDown(false)
    , m_bShowWithoutContent(true)
    , m_iAlign(gravity_left | gravity_y_center)
    , m_colorFocus(225, 255, 255)
    , m_colorNormal(225, 255, 255)
{
    //loadContent(DSSKEYBTN_LAYOUT_FILE);
}


CDsskeyButton::~CDsskeyButton()
{
}

void CDsskeyButton::loadAttributes(xml_node& node)
{
    xLinearLayout::loadAttributes(node);
    PopupAndroidBool(node, DSSKEY_UI_SHOW_WITHOUT_CONTENT, m_bShowWithoutContent);
    PopupAndroidColor(node, DSSKEY_PAGEBTN_TEXT_COLOR_NORMAL, m_colorNormal);
    PopupAndroidColor(node, DSSKEY_PAGEBTN_TEXT_COLOR_FOCUS, m_colorFocus);
}

void CDsskeyButton::loadChildrenElements(xml_node & node)
{
    xLinearLayout::loadChildrenElements(node);
    m_pText = static_cast<xTextView *>(getViewById(DSSKEYBTN_TEXT));
}

void CDsskeyButton::UpdateDisplayStatus(const DsskeyDisplayData& sData)
{
    SetText(sData.strFirstRowText);
}

void CDsskeyButton::SetText(const yl::string& src)
{
    m_strTitle = src;
}

void CDsskeyButton::SetShortenText(const yl::string& src)
{
    m_strShortenTitle = src;
}

void CDsskeyButton::SetTextColor(const xColor& color)
{
    if (m_pText != NULL)
    {
        m_pText->SetTextColor(color);
    }
}

void CDsskeyButton::SetBtnAlign(int nAlign)
{
    m_iAlign = nAlign;
    if (m_pText != NULL)
    {
        m_pText->SetGravity(nAlign);
    }
}

int CDsskeyButton::GetAlign() const
{
    return m_iAlign;
}

void CDsskeyButton::SetDisplayMode(DSSKEY_DISPLAY_MODE eMode)
{
    if (m_eDisplayMode == eMode)
    {
        return;
    }
    // TODO 改变btn长度
    m_eDisplayMode = eMode;
    // 按键大小变化，需要重新创建绘制文本缓存

    yl::string strNormalPic = "";

    if (PT_T52 != devicelib_GetPhoneType())
    {
        strNormalPic = TransLongOrNormalPic(PIC_MODE_NORMAL, m_eDisplayMode);
    }

    ChangeDisplay();
}

void CDsskeyButton::SetDssLabelType(DSSKEY_LABEL_TYLE eLabelType)
{
    if (eLabelType == m_eLabelType)
    {
        return;
    }

    m_eLabelType = eLabelType;

    ChangeDisplay();
}

bool CDsskeyButton::IsShowTwoCharsDsskey()
{
    return devicelib_GetPhoneType() == PT_T52 && DDM_TWO_CHARS == m_eDisplayMode;
}

bool CDsskeyButton::IsShowNoLabelDsskey()
{
    return devicelib_GetPhoneType() == PT_T52 && DDM_NO_LABEL == m_eDisplayMode;
}

void CDsskeyButton::SetupPressSlot(xSlotHandler * pReceiver, slotMethod slot)
{
    m_pReceiver = pReceiver;
    m_slot = slot;
}

void CDsskeyButton::SetupLongPressSlot(xSlotHandler * pReceiver, slotMethod slot)
{
    m_pLongPressReceiver = pReceiver;
    m_slotLongPress = slot;
}

void CDsskeyButton::OnBtnClick(bool bLongPressed)
{
    {
        if (m_pReceiver != NULL && m_slot != NULL_METHOD)
        {
            ((m_pReceiver)->*(m_slot))(NULL, (WPARAM)m_eDssKeyType, m_nDsskeyID, 0, this);
        }
        SetBtnDown(false);
    }
}

void CDsskeyButton::OnBtnLongClick(xThreadTimer* timer, LPVOID pExtraData)
{
    if (m_pLongPressReceiver != NULL && m_slotLongPress != NULL_METHOD)
    {
        ((m_pLongPressReceiver)->*(m_slotLongPress))(NULL, (WPARAM)m_eDssKeyType, m_nDsskeyID, 0, this);
    }
}

void CDsskeyButton::SetBtnDown(bool bPressed)
{
    m_bDown = bPressed;
    if (m_bDown)
    {
        SetTextColor(m_colorFocus);
        m_timerLongPress.SingleShot(DSSKEY_LONG_PRESS_TIME, TIMER_METHOD(this, CDsskeyButton::OnBtnLongClick));
    }
    else
    {
        SetTextColor(m_colorNormal);
        if (m_timerLongPress.IsTimerRuning())
        {
            m_timerLongPress.KillTimer();
        }
    }
    ChangeDisplay();
}

void CDsskeyButton::ChangeDisplay()
{
    if (!isVisible())
    {
        return;
    }

    if (NULL != m_pText)
    {
        if (DDM_NO_LABEL == m_eDisplayMode)
        {
            m_pText->SetText("");
            m_pText->hide(true);
        }
        else if (DDM_TWO_CHARS == m_eDisplayMode)
        {
            m_pText->SetText(m_strShortenTitle);
            m_pText->SetGravity(gravity_center);
            m_pText->show();
        }
        else
        {
            m_pText->SetText(m_strTitle);
            m_pText->SetGravity(m_iAlign);
            m_pText->show();
        }
    }

    yl::string strBkGround;
    BtnStatus CurbtnStatus;

    if (m_eGroundPhoto == GROUNDPHOTO_MODE_NORMAL)
    {
        // 判断button的状态，设置对应的图片
        if (!this->IsEnable())
        {
            strBkGround = TransLongOrNormalPic(PIC_MODE_DISABLE, m_eDisplayMode);
            CurbtnStatus = BTN_STASTUS_NORMAL;
        }
        else if (m_bDown)
        {
            strBkGround = TransLongOrNormalPic(PIC_MODE_HILIGHT, m_eDisplayMode);
            CurbtnStatus = BTN_STASTUS_DOWN;
        }
        else
        {
            strBkGround = strBkGround = TransLongOrNormalPic(PIC_MODE_NORMAL, m_eDisplayMode);
            CurbtnStatus = BTN_STASTUS_DISABLE;
        }
    }
    else if (m_eGroundPhoto == GROUNDPHOTO_MODE_HIGHLIGHT)
    {
        // 高亮状态背景
        strBkGround = TransLongOrNormalPic(PIC_MODE_HILIGHT, m_eDisplayMode);// getEnablePic();
        CurbtnStatus = BTN_STASTUS_HIGHLIGHT;
    }
    else if (m_eGroundPhoto == GROUNDPHOTO_MODE_USING)
    {
        // 使用状态背景
        strBkGround = PIC_THEME1_DIAL_LINEKEY;
        CurbtnStatus = BTN_STASTUS_USING;
    }

#if IS_COLOR
    xPixmap pixBack;
    g_WorkStation.GetImageByPath(strBkGround, pixBack);
    setBackground(pixBack);
#endif
}
