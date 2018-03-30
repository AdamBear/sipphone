#ifndef __DSSKEYBUTTON_H__
#define __DSSKEYBUTTON_H__

#include "xwindow/xWindow.h"
#include "xwindow-kernel/include/xthreadtimer.h"
#include "dsskeyui/dsskeyui_common.h"
#include "dsskeyui/dsskeyuicommondefine.h"

enum DSSKEY_SHOW_TYPE
{
    DSSKEY_SHOW_DEFAULT = 0, // 无icon
    DSSKEY_SHOW_SIGICON_LEFT = 1, // 单个icon
    DSSKEY_SHOW_SIGICON_RIGHT = 2,
    DSSKEY_SHOW_MULTIICON_LEFT = 3, //icon上还有信息
    DSSKEY_SHOW_MULTIICON_RIGHT = 4,
};

enum BtnStatus
{
    BTN_STASTUS_NORMAL = 0,
    BTN_STASTUS_DOWN,
    BTN_STASTUS_CHECKED = BTN_STASTUS_DOWN,
    BTN_STASTUS_DISABLE,
    BTN_STASTUS_HIGHLIGHT,
    BTN_STASTUS_USING
};

class CDsskeyButton
    : public xLinearLayout
    , public xTimerHandler
{
public:
    CDsskeyButton();
    ~CDsskeyButton();
public:
    virtual void loadAttributes(xml_node& node);
    virtual void loadChildrenElements(xml_node & node);
    //更新显示状态
    virtual void UpdateDisplayStatus(const DsskeyDisplayData& sData);

    virtual void SetText(const yl::string& src);
    virtual void SetShortenText(const yl::string& src);

    virtual void SetTextColor(const xColor& color);

    void SetTitle(const yl::string & strTitle)
    {
        m_strTitle = strTitle;
        SetText(m_strTitle);
    }
    void SetType(DssKey_Type nType)
    {
        m_eDssKeyType = nType;
    }
    void SetIndex(int nIndex)
    {
        m_nIndex = nIndex;
    }

    const yl::string & GetTitle() const
    {
        if (m_pText != NULL)
        {
            return m_pText->GetText();
        }
        return m_strTitle;
    }
    int GetObjectType() const
    {
        return m_nObjectType;
    }
    int GetDsskeyType() const
    {
        return m_eDssKeyType;
    }
    int GetIndex() const
    {
        return m_nIndex;
    }
    int GetDsskeyShowType() const
    {
        return m_eShowType;
    }

    void SetBtnAlign(int nAlign);
    int GetAlign() const;
    // 设置显示模式
    void SetDisplayMode(DSSKEY_DISPLAY_MODE eDispMode);

    void SetDssLabelType(DSSKEY_LABEL_TYLE eLabelType);

    void SetupPressSlot(xSlotHandler * pReceiver, slotMethod slot);
    void SetupLongPressSlot(xSlotHandler * pReceiver, slotMethod slot);

    void OnBtnClick(bool bLongPressed);

    void OnBtnLongClick(xThreadTimer* timer, LPVOID pExtraData);

    virtual void SetBtnDown(bool bPressed);
    virtual void ChangeDisplay();

    bool IsShowTwoCharsDsskey();
    bool IsShowNoLabelDsskey();
protected:

    yl::string m_strTitle;
    yl::string m_strShortenTitle;
    bool m_bShowIcon;
    bool m_bDown;
    bool m_bShowWithoutContent;

    int m_nObjectType;
    DssKey_Type m_eDssKeyType;  // dsskey业务类型
    int m_nIndex;       // dsskey按键索引值
    int m_nDsskeyID;    // DsskeyID

    DSS_KEY_FLUSH_TYPE  m_eFlushType;       // 闪动的类型
    bool                m_bFlush;           // 是否处于闪动状态
    int                 m_nFlushCnt;        // 闪动定时计数

    xTextView * m_pText;

    DSSKEY_LABEL_TYLE   m_eLabelType;           // Label显示类型
    DSSKEY_DISPLAY_MODE m_eDisplayMode;         // 显示模式，是长按键还是短按键
    DSS_KEY_TEXT_MODE   m_etextMode;        // 文字显示模式
    DSS_KEY_GROUNDPHOTO_MODE    m_eGroundPhoto;   // 背景显示模式

    DSSKEY_SHOW_TYPE m_eShowType;

    BtnStatus   m_LastbtnStatus;        // 上次按键的状
    int m_iAlign;

    xColor m_colorNormal;
    xColor m_colorFocus;

    xSlotHandler  *  m_pReceiver;
    slotMethod          m_slot;
    xSlotHandler  *  m_pLongPressReceiver;
    slotMethod          m_slotLongPress;

    xThreadTimer m_timerLongPress;
};

#endif
