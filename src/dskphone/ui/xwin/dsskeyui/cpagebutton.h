#ifndef _PAGE_BUTTON_H_
#define _PAGE_BUTTON_H_
#include <ylvector.h>
#include "xwindow/xWindow.h"
#include "dsskeyuicommondefine.h"

enum FlashPageStatus
{
    LK_FLASH_BACKGROUND,
    LK_FLASH_FOREGROUND
};

class CPageButton : public xLinearLayout
{
public:
    CPageButton();
    ~CPageButton();

public:
    virtual void loadChildrenElements(xml_node& node);
    virtual void loadAttributes(xml_node& node);
    void Init();

    //闪烁图标
    void FlashPageIcon(int pageNum, DSS_KEY_COLOR_TYPE eColor, DSS_KEY_FLUSH_TYPE eType);
    //停止闪烁
    void StopFlashPageIcon(int pageNum);
    //翻页
    void PageTurning();
    //翻到指定页
    void TurnToPage(int pageNum);

    // 设置页码信息
    void SetPageInfo(int iPageNum, int iPageIndex);

    //根据Dsskey配置情况设置当前页数
    bool SetActualPageNum(int pageNum, bool bFlush = true);

    // 设置当前页
    bool SetCurrentPage(int iCurrentPageIndex, bool bFlush = true);

    // 设置翻页按钮背景
    void SetBackgroundImage(const yl::string & strNormalBkg, const yl::string & strDisabledBkg);

    // 设置最大显示页码个数
    void SetMaxPage(int iMaxPage);

    //根据实际页数重设翻页按钮图标位置
    void ReSize();

    void SetDisplayMode(DSSKEY_DISPLAY_MODE eDispMode);

    // 处理定时器消息
    void OnFlashTimer(UINT uTimerID);

    void SetupPressSlot(xSlotHandler * pReceiver, slotMethod slot);

//signals:

    void CurrentPageTurned(int currentPage);

//private slots:
    void OnBtnClick(bool bPress);

private:
    //绘制翻页按钮
    void PaintPageLine();

    //擦除Page按钮上的PageIcon
    void ErasePageIcon();

    void timerEvent();

protected:
    typedef YLVector<xTextView*> PageIconBtnVector;
    yl::string m_strNormalBkgImg;          // normal状态背景图
    yl::string m_strDisabledBkgImg;        // disabled状态背景图
    int m_iCurrentPage;             // 当前页
    int m_iActualPageNum;           // 实际页数
    int m_iPageLimit;               // 最大可显示页数个数
    int m_iFlashTimerID[3]; // 三个页面的图标闪烁定时器ID
    PageIconBtnVector m_vecPageIcon;              // 翻页按钮上的图标
    FlashPageStatus m_eFlashPageStatus[3];      // 三个页面的图标闪烁状态（前景色/背景色）
    DSS_KEY_COLOR_TYPE m_eFlashIconColor[3];       // 三个页面图标闪烁时用的颜色（图标）
    DSSKEY_DISPLAY_MODE m_eDisplayMode;            // 显示模式

    xColor m_colorNormal;
    xColor m_colorFocus;

    xSlotHandler  *  m_pReceiver;
    slotMethod          m_slot;
#if !IS_COLOR
    xStyle *m_pOldStyle;
    xStyle *m_pNewStyle;
#endif
};
#endif
