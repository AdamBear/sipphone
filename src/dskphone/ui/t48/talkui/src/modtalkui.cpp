//mod-base
#include "talkui_define.h"
#include "modtalkui.h"

//mod-UIManager
#include "uimanager/basedialog.h"
#include "uimanager/uimanager_inc.h"
#include "uimanager/uimanager_header.h"
#include "qtcommon/qmisc.h"

//mod-MainWnd
#include "mainwnd/mainwnd.h"

//mod-Talk
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/modtalklogic.h"
// #include "talk/talklogic_inc.h"

//mod-this(TalkUI)
#include "talkuimanager.h"
#include "dlgtalkuiframe.h"
#include "talkuiframemanager.h"
#include "dlgdialingui.h"
#include "dlgringingui.h"
#include "dlgtalkingui.h"
#include "dlgconnectui.h"
#include "dlgfinishui.h"
#include "dlgconferenceui.h"
#include "dlgparknotifyui.h"
#include "dlgrtpstatusui.h"
#include "cdlgacdcallinfo.h"
#include "cdlgpickupmenu.h"
#include "cdlgscamenu.h"
#if IF_FEATURE_SHOW_BLF_CALLID
#include "cdlgblfmenu.h"
#endif
#include "favoriteselector.h"
#include "uicommon/messagebox/selectmessagebox.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "commonfunction.h"

// mod-inputMethod
#include "inputmethod/inputmethod.h"

#include "lamp/ledlamp/include/modledlamp.h"

// #include "testmode/modtestmode.h"
#include "dsskey/include/moddsskey.h"
#include "callmanager/modcallmanager.h"

#include "baseui/t4xpicpath.h"
#include "uimanager/uimanager.h"


/************************************************************************/
/* 接口  ： TalkUI_RegisterDlgClass()                                 */
/* 功能  ： 实现窗体类注册                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void talkui_RegisterDlgClass()
{
    REGISTER_DLG(CDlgTalkUIFrame);
    REGISTER_DLG(CDlgDialingUI);
    REGISTER_DLG(CDlgConnectUI);
    REGISTER_DLG(CDlgTalkingUI);
    REGISTER_DLG(CDlgConferenceUI);
    REGISTER_DLG(CDlgRingingUI);
    REGISTER_DLG(CDlgFinishedUI);
    REGISTER_DLG(CDlgParkNotifyUI);
    REGISTER_DLG(CDlgRTPStatusUI);
    REGISTER_DLG(CDlgACDCallInfo);
    REGISTER_DLG(CDlgPickupMenu);
    REGISTER_DLG(CDlgSCAMenu);
#if IF_FEATURE_SHOW_BLF_CALLID
    REGISTER_DLG(CDlgBLFMenu);
#endif
}

/************************************************************************/
/* 接口  ： talkui_Init()                                                */
/* 功能  ： talkUI模块初始化                                                */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkui_Init()
{
    talkui_RegisterDlgClass();
    talklogic_Init();
    _TalkUIManager.InitManager();

    //callmanager注册
    CCallMGRItemInfo callMGRItemInfo;

    callMGRItemInfo.strDisplayName = TRID_KEYPAD;
    callMGRItemInfo.strIcon = PIC_SOFTKEY_CALLMANAGER_DIAL;
    callMGRItemInfo.strNormalBKImg = "";
    callMGRItemInfo.strSelectBKImg = PIC_BTN_ITEM_CLICK_RIGHT;


    callmanager_SetPageInfo(DLG_Dial, 0x00, callMGRItemInfo, NULL, talkui_enterdialingui,
                            talkui_putdialingui, talkui_releasedialinguidlg, CALLMANAGER_DEFAULT_ITEM_INDEX + 2);

}

/************************************************************************/
/* 接口  ： talkui_Uninit()                                              */
/* 功能  ： talkUI模块注销                                               */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkui_Uninit()
{
    _TalkUIManager.UninitManager();
    talklogic_Uninit();
}

bool talkui_IsInTalk()
{
    EWindowType eWndType = _UIManager.GetTopWndType();
#if IF_USE_CALLMANAGER
    if (eWT_CallManager == eWndType)
    {
        if (DLG_Dial == callmanager_GetCurrentPageName())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
#endif
        if (eWT_Talk == eWndType)
        {
            return true;
        }

    return false;
}

// session退出
void talkui_SessionExit(int iSessionID)
{
    _TalkUIManager.ClearSessionData(iSessionID);
//  _TalkUIManager.EraseTalkUIFromMap(iSessionID);

}

// 根据通话Id清空UI对应的数据指针
// 同一Session 中状态切换时会调用该函数 在内部基本不需要做什么处理  类似一个通知消息
void talkui_ClearSessionData(int iSessionID)
{
    // 注释原因：Routine切换的时候，将SessionID设为非法，会导致逻辑层切换成功，UI还未切换的时候，按键概率性失效的问题
    //_TalkUIManager.ClearSessionData(iSessionID);
//  _TalkUIManager.EraseTalkUIFromMap(iSessionID);
}

/************************************************************************/
/* 接口  ： talkui_UpdateWnd(void* pData)                                */
/* 功能  ： 根据Logic层所提供的数据进行界面更新                               */
/* 参数说明 ：[in] void* pData:Logic层所提供的数据，需为DataLogic2UI     */
/* 返回值 ： 无                                                            */
/************************************************************************/

void EnterDialUI()
{
    talklogic_EnterDialUI("", AUTO_SELECT_LINE_ID);
}

void ExitDialingUI()
{
    _TalkUIManager.ExitDialingUI();
}

void talkui_SetCallManagerInfo()
{

}

void talkui_UpdateWnd(void * pData)
{
    if (pData != NULL)
    {
        DataLogic2UI * pLogic2UIData = (DataLogic2UI *)(pData);
        _TalkUIManager.UpdateContentByData(*pLogic2UIData);
    }
}

/************************************************************************/
/* 接口  ： talkui_UpdateErrorInfo(void* pData)                          */
/* 功能  ： 根据Logic层所提供的数据进行错误信息更新，若有错误信息，         */
/*         则直接显示，无论处于何通话界面，若处于错误界面，                 */
/*         传过来的参数为空，则关闭错误提示界面                               */
/* 参数说明 ：[in] void* pData:Logic层所提供的数据，需为Error_Info           */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkui_UpdateErrorInfo(void * pData)
{
    if (pData != NULL)
    {
        Error_Info * pErrorInfo = (Error_Info *)(pData);
        _TalkUIManager.UpdateErrorInfoByData(*pErrorInfo);
    }
}

/************************************************************************/
/* 接口  ： talkui_UpdateChannelIcon()                                   */
/* 功能  ： 更新通道图标                                                   */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
//void talkui_UpdateChannelIcon()
//{
//  _TalkUIManager.UpdateChannelIcon();
//}

/************************************************************************/
/* 接口  ： talkui_GetTalkTimeBySessionId()                              */
/* 功能  ： 根据通话Id获得通话时间数据                                       */
/* 参数说明 ：[in] int nSessionId:所需获取的Session对应的Id                */
/* 返回值 ： 对应的Session通话时间的秒数                                  */
/************************************************************************/
int talkui_GetTalkTimeBySessionId(int nSessionId)
{
    ////---- T48
    CDlgBaseTalkUI * pBaseTalkUI = _TalkUIManager.FindTalkUIBySessionId(nSessionId);
    if (pBaseTalkUI != NULL)
    {
        QCustomTime timeTalk;
        timeTalk = pBaseTalkUI->GetTalkTime();
        return timeTalk.hour() * 3600 + timeTalk.minute() * 60 + timeTalk.second();
    }

    return 0;
}

/************************************************************************/
/* 接口  ： talkui_SetVolumeBarVisible()                             */
/* 功能  ： 关闭开启音量条                                                */
/* 参数说明 ：[in] bool bVisible:是否开启关闭音量条                       */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkui_SetVolumeBarVisible(bool bVisible)
{
    // !!!!!,MainWnd提供C接口
    _MainWND.SetVolumeBarVisible(bVisible);
}

/************************************************************************/
/* 接口  ： talkui_GetInputChar()                                        */
/* 功能  ： 获取赌赢输入法按键的第一个字符                                */
/* 参数说明 ：[in] string strIMEName:输入法的名称                        */
/* 参数说明 ：[in] int iKeyCode:按键的keyCode                           */
/* 返回值 ： 无                                                            */
/************************************************************************/
yl::string talkui_GetInputChar(const yl::string & strIMEName, int iKeyCode)
{
    QString strFirstChar = qGetIMEFirstChar(strIMEName.c_str(), iKeyCode);
    return strFirstChar.toUtf8().data();
}

/************************************************************************/
/* 接口  ： talkUI_FlushAllEvent()                                       */
/* 功能  ：  将当前所有UI(Qt)事件执行完毕，主要用于拨号呼出，将所有输入完成  */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
yl::string talkui_GetInputContent()
{
#if 0 //by kjf
    static bool bEnter = false;//  防止函数重入做的临时变量

    // 判断函数是否重入
    if (bEnter)
    {
        return "";
    }

    bEnter = true;// 给函数上锁
    yl::string strContent = fromQString(_TalkUIManager.GetInputContent());
    bEnter = false;// 给函数解锁
    return strContent;
#else
    yl::string strNum = "";
    yl::string strDisplayname = "";
    _TalkUIManager.GetInputContent(strNum, strDisplayname);
    return strNum;
#endif
}

bool talkui_GetInputContent(yl::string & strNum, yl::string & strDisplayname)
{
    return _TalkUIManager.GetInputContent(strNum, strDisplayname);
}

/************************************************************************/
/* 接口  ： talkui_GetInputCallID()                                      */
/* 功能  ：  获取输入内容的Call id    */
/* 参数说明 ：无                                                          */
/* 返回值 ： 输入框内容的callid，int类型                                   */
/************************************************************************/
int talkui_GetInputCallID()
{
    return _TalkUIManager.GetInputCallID();
}

/************************************************************************/
/* 接口  ： talkui_EnterPickupMenu()                                     */
/* 功能  ： 进入Pickup操作界面     */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
void talkui_EnterPickupMenu()
{
    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_DlgPickupMenu);
    if (pDlg == NULL)
    {
        pDlg = UIManager_CreatePrivateDlg(DLG_DlgPickupMenu);
    }

    if (pDlg != NULL)
    {
        CDlgPickupMenu * pDlgPickupMenu = qobject_cast<CDlgPickupMenu *>(pDlg);
        if (NULL != pDlgPickupMenu)
        {
            pDlgPickupMenu->SetData(NULL);
        }

        if (pDlg != UIManager_GetTopWindow())
        {
            UIManager_SetTopWindow(pDlg);
        }
        else
        {
            UIManager_UpdateWnd(pDlg);
        }

    }
}

/************************************************************************/
/* 接口  ： talkui_UpdatePickupMenu()                                        */
/* 功能  ： 更新Pickup操作界面数据       */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                */
/************************************************************************/
void talkui_UpdatePickupMenu()
{
    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_DlgPickupMenu);
    if (pDlg != NULL)
    {
        CDlgPickupMenu * pDlgPickupMenu = qobject_cast<CDlgPickupMenu *>(pDlg);
        if (NULL != pDlgPickupMenu)
        {
            pDlgPickupMenu->SetData(NULL);
        }

        if (pDlg != UIManager_GetTopWindow())
        {
            UIManager_SetTopWindow(pDlg);
        }
        else
        {
            UIManager_UpdateWnd(pDlg);
        }
    }
}

/************************************************************************/
/* 接口  ： talkui_ExitPickupMenu()                                      */
/* 功能  ：  退出Pickup操作界面    */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
void talkui_ExitPickupMenu()
{
    CDlgPickupMenu * pDlg = qobject_cast<CDlgPickupMenu *>(UIManager_GetPrivateDlg(DLG_DlgPickupMenu));
    if (pDlg != NULL)
    {
        UIManager_PutPrivateDlg(pDlg);
    }
}

/************************************************************************/
/* 接口  ： talkui_OpenPool()                                        */
/* 功能  ：  进入选择界面  */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
bool talkui_OpenPool(bool bIsEnterFromIdle /*= false*/)
{
    //这边一定要 用静态变量
    static CFavoriteSelector selector;

    selector.Init();

    if (!selector.IsHadFavorites())
    {
        return false;
    }

    if (selector.IsCanDirectEnter())
    {
        selector.DirectEnterFavorite();
        return true;
    }

    QString strTitle = LANG_TRANSLATE(TRID_FAVOURITE_DIRECTORIES);


    //更换弹出框
    MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
    MsgBoxSoftkey skOk(LANG_TRANSLATE(TRID_OK), MessageBox_OK);
    VecMsgBoxSoftkey vecSoftKey;

    vecSoftKey.push_back(skCancel);
    vecSoftKey.push_back(skOk);
    //
    MessageBox_ShowSelectBox(&selector, vecSoftKey, SELECT_POP_BOX_TYPE_NORMAL, &selector, strTitle);
    //if (PopupBox_OK != MsgBox_PopupSelectBox(PB_TYPE_OkCancel, SELECT_POP_BOX_TYPE_NORMAL, &selector, strTitle))
    //{
    //  return false;
    //}

    //selector.EnterSelectedFavorite();
    return true;
}

/************************************************************************/
/* 接口  ： talkui_ExitPool()                                        */
/* 功能  ：  退出选择界面  */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
void talkui_ExitPool()
{
    //UIManager_PutPrivateDlg(DLG_TalkPool);
}
/************************************************************************/
/* 接口  ： talkui_EnterSCAMenu()                                        */
/* 功能  ： 进入SCA操作界面        */
/* 参数说明 ：无                                                      */
/*              [in]pData:   传给界面的数据                   */
/* 返回值 ： 无                                    */
/************************************************************************/
bool talkui_EnterSCAMenu(void * pData)
{
    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_DlgSCAMenu);
    if (pDlg == NULL)
    {
        pDlg = UIManager_CreatePrivateDlg(DLG_DlgSCAMenu);
    }

    if (pDlg != NULL)
    {
        //设置数据
        pDlg->SetData(pData);

        if (pDlg != UIManager_GetTopWindow())
        {
            UIManager_SetTopWindow(pDlg);
        }
        else
        {
            UIManager_UpdateWnd(pDlg);
        }
    }
    else
    {
        return false;
    }

    return true;
}

/************************************************************************/
/* 接口  ： talkui_IsExistSCAMenu()                                      */
/* 功能  ： 是否存在SCA操作界面  */
/* 参数说明 ：无                                                          */
/* 返回值 ： true-存在                                    */
/************************************************************************/
bool talkui_IsExistSCAMenu()
{
    return (UIManager_GetPrivateDlg(DLG_DlgSCAMenu) != NULL);
}

/************************************************************************/
/* 接口  ： talkui_ExitSCAMenu()                                     */
/* 功能  ：  退出SCA操作界面   */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                    */
/************************************************************************/
void talkui_ExitSCAMenu()
{
    UIManager_PutPrivateDlg(DLG_DlgSCAMenu);
}

#if IF_FEATURE_SHOW_BLF_CALLID
bool talkui_EnterBLFMenu(void* pData)
{
    CBaseDialog* pDlg = UIManager_GetPrivateDlg(DLG_DlgBLFMenu);
    if (pDlg == NULL)
    {
        pDlg = UIManager_CreatePrivateDlg(DLG_DlgBLFMenu);
    }

    if (pDlg != NULL)
    {
        //设置数据
        pDlg->SetData(pData);

        if (pDlg != UIManager_GetTopWindow())
        {
            UIManager_SetTopWindow(pDlg);
        }

        UIManager_UpdateWnd(pDlg);
    }
    else
    {
        return false;
    }

    return true;
}

bool talkui_IsExistBLFMenu(const yl::string& strKey)
{
    CDlgBLFMenu* pBLFMenu = qobject_cast<CDlgBLFMenu*>(UIManager_GetPrivateDlg(DLG_DlgBLFMenu));
    if (NULL != pBLFMenu)
    {
        return pBLFMenu->GetBLFKey() == strKey;
    }
    return false;
}

void talkui_ExitBLFMenu()
{
    UIManager_PutPrivateDlg(DLG_DlgBLFMenu);
}
#endif
