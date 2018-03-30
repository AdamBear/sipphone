#ifndef _TALKUI_FACTORY
#define _TALKUI_FACTORY

#include "dlgtalkuiframe.h"

#include "dlgacdcallinfoui.h"
#include "dlgautoredialui.h"
#include "dlgconfui.h"
#include "dlgconfmanagerui.h"
#include "dlgconnectui.h"
#include "dlgdialingui.h"
#include "dlgfinishui.h"
#include "dlgparknotifyui.h"
#include "dlgringui.h"
#include "dlgrtpstatusui.h"
#include "dlgtalkingui.h"
#include "dlgnetconfui.h"
#include "dlgnetconfmgrui.h"

namespace talkui
{

class CTalkUIFactory
{
public:
    CTalkUIFactory();
    virtual ~CTalkUIFactory();

public:
    static CTalkUIFactory & GetInstance();

    void init();

public:
    //创建通话UI实例（提供给uilogic使用）
    CUILogicBaseTalkUI * CreateTalkUI(const yl::string & strName);
    //获取通话UI实例（提供给UI使用）
    CDlgBaseTalkUI * GetTalkUI(const yl::string & strName);

protected:
    //获取通话UI实例（根据参数判断是否解绑）
    CUILogicBaseTalkUI * GetTalkUIFromCache(const yl::string & strName, bool bUnbind);

public:
    //释放通话UI实例指针
    void ReleaseTalkUI(const yl::string & strProcessorName, CUILogicBaseTalkUI * pTalkUI);

    void ReleaseAllUI();

    //因为多继承造成指针强转问题
    //UILogic层的基础对话框转换成UI的基础对话框
    CDlgBaseTalkUI * UILogicTalkUIToBaseTalkUI(yl::string strProcessorName,
            CUILogicBaseTalkUI * pUILogicBaseTalkUI);
    //UI的基础对话框转换成UILogic层的基础对话框
    CUILogicBaseTalkUI * BaseTalkUIToUILogicTalkUI(const yl::string strProcessorName,
            CDlgBaseTalkUI * pDlgBaseTalkUI);

private:
    //窗口缓存
    CDlgACDCallInfoUI*  m_pACDCallInfoUI;
    CDlgAutoRedialUI*  m_pAutoRedialUI;
    CDlgConfUI*  m_pConfUI;
    CDlgConfManagerUI*  m_pConfManagerUI;
    CDlgConnectUI*  m_pConnectUI;
    CDlgDialingUI*  m_pDialingUI;
    CDlgFinishUI*  m_pFinishUI;
    CDlgParkNotifyUI*  m_pParkNotifyUI;
    CDlgRingUI*  m_pRingUI;
    CDlgRTPStatusUI*  m_pRTPStatusUI;
    CDlgTalkingUI*  m_pTalkingUI;
    CDlgNetConfUI* m_pNetConfUI;
    CDlgNetConfMgrUI* m_pNetConfMgrUI;
};

#define _TalkUIFactory CTalkUIFactory::GetInstance()

}
#endif //_TALKUI_FACTORY
