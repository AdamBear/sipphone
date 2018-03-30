#ifndef _DLG_PICKUP_MENU_H_
#define _DLG_PICKUP_MENU_H_

#include "uimanager/basedialog.h"
#include "dlgtalkmenubase.h"

#include "pickupadapter.h"
////////////talkuilogic////////
#include "talkuilogic/pickupmenuuibase.h"

namespace talkui
{
class CDlgPickupMenu : public CDlgTalkMenuBase, public CPickUPMenuUIBase
{
public:
    //取得实例
    static CBaseDialog * CreateInstance();

public:
    CDlgPickupMenu();
    virtual ~CDlgPickupMenu();

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗.口清空回复到初始化状态
    virtual void Uninit();

    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strBGPath);

    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle()
    {
        return false;
    }

public:
    //设置数据
    void SetData(void * pData);

public:
    //获取焦点 pickupmenuitem 的序号
    virtual int GetFocusPickUPMenuItemIndex();

public:
    //获取Softkey数据
    virtual void GetSoftkey(CArraySoftKeyBarData & objSoftkeyData);

public:
    virtual bool onKeyPressedEvent(int key);

public:
    //处理softkey 继承自 talkbaseui
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftType, bool bLongPress);

protected:
    CPickUpAdapter      m_objPickUpAdapter;

};



}           //namespace talkui


#endif
