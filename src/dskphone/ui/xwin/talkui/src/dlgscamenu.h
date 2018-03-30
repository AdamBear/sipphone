#ifndef _DLG_SCAMENU_H_
#define _DLG_SCAMENU_H_

#include "uimanager/basedialog.h"
#include "dlgtalkmenubase.h"

#include "scamenuadapter.h"
////////////talkuilogic////////
#include "talkuilogic/scamenuuibase.h"

namespace talkui
{
class CDlgSCAMenu : public CDlgTalkMenuBase, public CSCAMenuUIBase
{
public:
    //取得实例
    static CBaseDialog * CreateInstance();

public:
    CDlgSCAMenu();
    virtual ~CDlgSCAMenu();

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗.口清空回复到初始化状态
    virtual void Uninit();

public:
    //设置数据
    void SetData(void * pData);

public:
    //获取焦点 SCAmenuitem 的序号
    virtual int GetFocusSCAMenuItemIndex();

public:
    //获取Softkey数据
    virtual void GetSoftkey(CArraySoftKeyBarData & objSoftkeyData);

public:
    virtual bool onKeyPressedEvent(int key);

public:
    //处理softkey 继承自 talkbaseui
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftType, bool bLongPress);

protected:
    CSCAMenuAdapter m_objSCAMenuAdapter;
};



}           //namespace talkui


#endif



