//mod-this
#include "modtalkuilogic.h"
#include <ylstring.h>

//mod-talk
#include "talk/talklogic/include/modtalklogic.h"


/************************************************************************/
/* 接口  ： talkui_Init()                                                */
/* 功能  ： talkUI模块初始化                                                */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkuilogic_Init()
{
    _TalkUILogicManager.Init();
    //已经在总初始化入口处进行过初始化
//  talklogic_Init();

    ////callmanager注册
    //CCallMGRItemInfo callMGRItemInfo;

    //callMGRItemInfo.strDisplayName = "Keypad";
    //callMGRItemInfo.strIcon = PIC_SOFTKEY_CALLMANAGER_DIAL;
    //callMGRItemInfo.strNormalBKImg = "";
    //callMGRItemInfo.strSelectBKImg = PIC_BTN_ITEM_CLICK_RIGHT;


    //callmanager_SetPageInfo(CTalkUIDial, 0x00, callMGRItemInfo, NULL, talkui_enterdialingui, talkui_putdialingui, talkui_releasedialinguidlg, CALLMANAGER_DEFAULT_ITEM_INDEX + 2);

}

/************************************************************************/
/* 接口  ： talkui_Uninit()                                              */
/* 功能  ： talkUI模块注销                                               */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkuilogic_Uninit()
{
    talklogic_Uninit();

    _TalkUILogicManager.Unint();

}

// session退出
void talkuilogic_SessionExit(int iSessionID)
{
    TALKUI_INFO("talkuilogic_SessionExit iSessionID=%d", iSessionID);

    _TalkUILogicManager.SessionExit(iSessionID);

    TALKUI_INFO("talkuilogic_SessionExit  end iSessionID=%d", iSessionID);
}

// 根据通话Id清空UI对应的数据指针
// 同一Session 中状态切换时会调用该函数 在内部基本不需要做什么处理  类似一个通知消息
void talkuilogic_ClearSessionData(int iSessionID)
{
    TALKUI_INFO("talkuilogic_ClearSessionData iSessionID=%d", iSessionID);

    _TalkUILogicManager.ClearSessionData(iSessionID);

    TALKUI_INFO("talkuilogic_ClearSessionData  end iSessionID=%d", iSessionID);
}

/************************************************************************/
/* 接口  ： talkui_UpdateWnd(void* pData)                                */
/* 功能  ： 根据Logic层所提供的数据进行界面更新                               */
/* 参数说明 ：[in] void* pData:Logic层所提供的数据，需为DataLogic2UI     */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkuilogic_UpdateWnd(void * pData)
{
    TALKUI_INFO("talkuilogic-----UpdateWnd dataPointer=0x%x", pData);

    if (pData != NULL)
    {
        DataLogic2UI * pLogic2UIData = (DataLogic2UI *)(pData);

        TALKUI_INFO("talkuilogic-----UpdateWnd sessionID=%d", pLogic2UIData->iSessionID);

        _TalkUILogicManager.UpdateUI(pLogic2UIData);

    }

    TALKUI_INFO("talkuilogic-----UpdateWnd end dataPointer=0x%x", pData);
}

/************************************************************************/
/* 接口  ： talkui_UpdateErrorInfo(void* pData)                          */
/* 功能  ： 根据Logic层所提供的数据进行错误信息更新，若有错误信息，         */
/*         则直接显示，无论处于何通话界面，若处于错误界面，                 */
/*         传过来的参数为空，则关闭错误提示界面                               */
/* 参数说明 ：[in] void* pData:Logic层所提供的数据，需为Error_Info           */
/* 返回值 ： 无                                                            */
/************************************************************************/
void talkuilogic_UpdateErrorInfo(void * pData)
{
    if (pData != NULL)
    {
        Error_Info * pErrorInfo = (Error_Info *)(pData);
        _TalkUILogicManager.UpdateErrorInfo(pErrorInfo);
    }
}


/************************************************************************/
/* 接口  ： talkui_GetTalkTimeBySessionId()                              */
/* 功能  ： 根据通话Id获得通话时间数据                                       */
/* 参数说明 ：[in] int nSessionId:所需获取的Session对应的Id                */
/* 返回值 ： 对应的Session通话时间的秒数                                  */
/************************************************************************/
int talkuilogic_GetTalkTimeBySessionId(int nSessionId)
{
    return _TalkUILogicManager.GetTalkTimeBySessionId(nSessionId);

}

/************************************************************************/
/* 接口  ： talkUI_FlushAllEvent()                                       */
/* 功能  ：  将当前所有UI(Qt)事件执行完毕，主要用于拨号呼出，将所有输入完成  */
/* 参数说明 ：无                                                          */
/* 返回值 ： 无                                                            */
/************************************************************************/
yl::string talkuilogic_GetInputContent()
{
    yl::string strNum = "";
    yl::string strDisplayname = "";
    _TalkUILogicManager.GetInputContent(strNum, strDisplayname);
    return strNum;
}

bool talkuilogic_GetInputContent(yl::string & strNum, yl::string & strDisplayname)
{
    return _TalkUILogicManager.GetInputContent(strNum, strDisplayname);
}

void talkui_UpdateCurrentWnd()
{
    _TalkUILogicManager.UpdateCurrentWnd();
}

void talkuilogic_RegisterOnTalkCallBack(ON_TALK_PROC funCallBack)
{
    _TalkUILogicManager.RegisterOnTalkCallBack(funCallBack);
}

void talkuilogic_UnregisterOnTalkCallBack(ON_TALK_PROC funCallBack)
{
    _TalkUILogicManager.UnregisterOnTalkCallBack(funCallBack);
}
