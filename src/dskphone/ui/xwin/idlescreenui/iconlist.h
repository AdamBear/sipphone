#ifndef __ICONLIST_H__
#define __ICONLIST_H__

#include "talk/talklogic/include/uiandlogic_common.h"
#include "uimanager/uimanager_common.h"
#include <yllist.h>
#include "xwindow/xWindow.h"
#include "xwindow-kernel/include/xthreadtimer.h"
#include "idleuilogic/include/idlehandledefine.h"
#include "idlescreenui/idlescreenuidefine.h"

//#define FIXED_ICON_LIST

//////////////////////////////////////////////////////////////////////////
// 图标信息 CFlashIcon
class CFlashIcon
{
public:
    CFlashIcon();
    ~CFlashIcon();

    void SetFlash(bool bFlash);
    void SetIconType(ICON_TYPE eIconType);

    bool IsNeedFlash()
    {
        return m_bNeedFlash;
    }
    ICON_TYPE GetIconType()
    {
        return m_eIconType;
    }

private:
    ICON_TYPE       m_eIconType;    // Icon类型
    bool            m_bNeedFlash;   // 是否需要闪烁
};

//////////////////////////////////////////////////////////////////////////
// CIconList
class CIconList
{
public:
    CIconList();
    virtual ~CIconList();

    // 添加图标
    virtual bool AddIcon(ICON_TYPE eIconType, bool bNeedFlash = false, const yl::string& strHint = "", xViewGroup* pLayoutView = NULL) = 0;

    // 删除指定类型
    virtual bool RemoveByType(ICON_TYPE eIconType) = 0;

    // 删除所有
    virtual bool RemoveAll() = 0;

    // 获取ICON列表大小
    virtual int GetIconListSize();

    void InitLayoutView(xViewGroup* pLayoutView);

public:
    ///////////////////////// 辅助函数 //////////////////////
    // 设置闪动时间间隔
    void SetFlashTime(UINT uShowTime, UINT uHideTime);
    // 设置最大显示图标个数
    void SetIconMax(UINT uMaxNum);
    // 设置布局方向
    void SetLayoutDirection(bool bLeft2Right);
    // 设置图标间隔
    void SetIconSpace(UINT uIconSpace);

protected:
    UINT                m_uShowTime;            // 闪动时间间隔, 亮起的时间
    UINT                m_uHideTime;            // 闪动时间间隔, 关掉的时间
    bool                m_bLeft2Right;          // ICON是否从右向左布局
    UINT                m_uMaxNum;              // 最大显示图标个数
    UINT                m_uIconSpace;           // 图标间隔
    xViewGroup*         m_pLayoutView;          // 默认使用的布局视图指针
};

//////////////////////////////////////////////////////////////////////////
// CIconList

class CNormalIconList : public CIconList
{
public:
    CNormalIconList();
    virtual ~CNormalIconList();

    // 添加图标
    virtual bool AddIcon(ICON_TYPE eIconType, bool bNeedFlash = false, const yl::string& strHint = "", xViewGroup* pLayoutView = NULL);

    // 删除
    virtual bool RemoveByType(ICON_TYPE eIconType);

    // 删除所有
    virtual bool RemoveAll();

protected:
    bool AddIcon(const yl::string& strFileName, bool bFlashIcon, const yl::string& strHint, xViewGroup* pLayoutView);
#if IF_IDLE_SHOW_ICON_HINT
    void AddHintView(xViewGroup* pView, const yl::string& strHint, bool bTextBackground);
    void AddHintChildView(xViewGroup* pParentView, xView* pChildView,
                          const yl::string& strId, bool bExceedLimit);
#endif
private:
    LIST_ICON_TYPE m_listIconType;
};

//////////////////////////////////////////////////////////////////////////
// CFiexdIconList
typedef YLList<CFlashIcon*>     LIST_FLASH_ICON;
class CFiexdIconList : public CIconList, public xTimerHandler
{
public:
    CFiexdIconList();
    virtual ~CFiexdIconList();

    // 添加图标
    virtual bool AddIcon(ICON_TYPE eIconType, bool bNeedFlash = false, const yl::string& strHint = "", xViewGroup* pLayoutView = NULL);

    // 删除
    virtual bool RemoveByType(ICON_TYPE eIconType);

    // 删除所有
    virtual bool RemoveAll();

    // 获取ICON列表大小
    virtual int GetIconListSize();

protected:
    // 用于T20显示图标
    LCD_ICON_TYPE GetIconLcdType(ICON_TYPE eType);
    // 清空列表
    void ClearIconList();

    void OnFlashTimer(xThreadTimer* timer, LPVOID pExtraData);

    void UpdateFlashIcon();

private:
    void SetIconDisplay(LCD_ICON_TYPE eIconIndex, bool bShowIcon);
    void StopFlashTimer();
    void ResetFlashTimer();

protected:
    LIST_FLASH_ICON     m_listIcon;         // ICON 图标列表
    xThreadTimer        m_timerFlash;
    UINT                m_uFlashCount;      // 需要闪烁的图标个数
    bool                m_bFlashShow;
};

CIconList* GetIconListInstance();

#define g_pIconList GetIconListInstance()

#endif

