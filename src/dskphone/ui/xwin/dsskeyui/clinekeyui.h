#pragma once
#include <ylvector.h>
#include "xwindow/xWindow.h"
#include "keymap.h"
#include "dsskeyuicommondefine.h"
#include "dsskey/include/dsskey_enumtypes.h"

#include "cdsskeybutton.h"
#include "cpagebutton.h"
#include "dsskeydelegate.h"

XWINDOW_EXPORT xView & currentLinekeyUI();

//////////////////////////////////////////////////////////////////////////

class CLinekeyUI
    : public xFrameLayout
    , public xSlotHandler
{
public:
    static LRESULT OnDlgLineKeyMessage(msgObject& objMessage);

public:
    CLinekeyUI();
    ~CLinekeyUI();

    virtual void Init();
    virtual void Uninit();

    void InitData();

    // 处理定时器消息
    bool OnTimer(UINT uTimerID) DECLARE_ATTR;
public:
    void LoadContentFile();
    virtual void loadAttributes(xml_node& node);
    virtual void loadChildrenElements(xml_node & node);

    static CLinekeyUI* CreateInstance();
    //static CLinekeyUI* GetInstance();

    void slotOnLinekeypress(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                            LPVOID pData);
    void slotOnLineKeyLonngPress(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                 LPVOID pData);

    void slotOnPageButtonClick(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                               LPVOID pData);

    void RefreshDsskeyData(const DsskeyDisplayData& objDisplayData);

    // 根据索引刷新数据
    void RefreshDsskeyData(int iIndex, const DsskeyDisplayData& objDisplayData);

    void SetBtnClickAction(CAbstractDsskeyBtnAction* pAction);
    void SetBtnPressAction(CAbstractDsskeyBtnAction* pAction);
    void SetBtnLongPressAction(CAbstractDsskeyBtnAction* pAction);
    void SetDataDelegate(CDsskeyDataDelegateBase* pDelegate);

    void SetBtnDown(int iIndex, bool bPressed);

    // 设置显示模式
    void SetDisplayMode(DSSKEY_DISPLAY_MODE eDispMode);

    void ChangeDsskeyLabelType();

    bool IsDsskeyInAnimation();
    void PlayShowDsskeyAnimation(DSSKEY_DISPLAY_MODE eDispMode);

#if IF_FEATURE_DSSKEY_ALIGN_MODE
    bool SetBtnAlignByIndex(int iIndex, int iAlign);
#endif

    void SetDsskeysLayout(int nBtnWidth);
    void SetPageBtnLayout(int nBtnWidth);

    CDsskeyButton * ReplaceOldBtn(int iIndex, int iAlign, CDsskeyButton *pBtn);

    // 设置翻页键是否显示
    void SetPageBtnVisible(bool bShow = true);

    // 设置翻页键信息(页码个数、当前页码)
    void SetPageInfo(int iPageNum, int iPageIndex);

    // 设置翻页键位置
    //void SetPageGeometry(const QRect & rc);

    // 设置翻页键闪烁图标
    void FlashPageIcon(int pageNum, DSS_KEY_COLOR_TYPE eColor,
                       DSS_KEY_FLUSH_TYPE eType = REFRESH_TYPE_NO_FLUSH);

    // 根据索引刷新按键位置
    //void SetBtnGeometryByIndex(int iIndex, const QRect & rc);

private:
    void slotKeyEvent(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
    {
        XWINDOW_INFO(" ====== CLinekeyUI slotKeyEvent lParam:%d wParam:%d ====== ", lParam, wParam);
        if (1 == lParam)
        {
            // press
            emitHotkeySignal(wParam - PHONE_KEY_LINE1, sender, true, false);
            SetBtnDown(wParam - PHONE_KEY_LINE1, true);
        }
        else if (0 == lParam)
        {
            SetBtnDown(wParam - PHONE_KEY_LINE1, false);
            // release
            emitHotkeySignal(wParam - PHONE_KEY_LINE1, sender, false, false);
        }
        m_bLongPress = false;
    }

    void emitHotkeySignal(int iItemIndex, xSignal* sender, bool bPress, bool bLongPressed);
    CDsskeyButton* findBtnByIndex(int iBtnIndex);

    int GetDsskeyWidth();

    bool IsShowFrameBg() const;
    void PrepareWidgetBeforePaint();
    void PrepareLeftDssFrameBg();
    void PrepareRightDssFrameBg();
    void ChangeDssFrameGeometory();

protected:
    xLinearLayout * m_pLeftFrame;
    xLinearLayout * m_pRightFrame;
private:

    typedef YLVector<CDsskeyButton *> VEC_DSSKEY_BTN;
    typedef VEC_DSSKEY_BTN::iterator DSSKEY_BTN_IT;
    VEC_DSSKEY_BTN m_vecDsskeyBtns;
    CPageButton * m_pPageBtn;

    CAbstractDsskeyBtnAction*       m_pBtnClickAction;
    CAbstractDsskeyBtnAction*       m_pBtnPressAction;
    CAbstractDsskeyBtnAction*       m_pBtnLongPressAction;
    CDsskeyDataDelegateBase*        m_pDataDelegate;

    int m_iMaxNumPerPage;
    int m_iMaxPage;
    int m_iTotalKey;
    int m_iKeyFullWidth;
    int m_iKeyExtendWidth;
    int m_iKeyNormalWidth;
    int m_iKeyTwoCharWidth;
    int m_iKeyNoLabelWidth;

    bool m_bShowFrameBg;
    bool m_bLongPress;
    bool m_bInAnimation;

    DSSKEY_DISPLAY_MODE m_eDisplayMode;         // 显示模式，是长按键还是短按键
};
#define g_pLinekeyUI CLinekeyUI::CreateInstance()
