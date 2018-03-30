#include "uimanager_inc.h"
#include "modlogicuimanager.h"

/************************************************************************/
/* 接口  ： LogicUIManager_IsTopWndIdle()                               */
/* 功能  ： 初始化判断顶层窗口是不是Idle                                */
/* 参数说明 ：                                                          */
/*     无                                                               */
/* 返回值 ： 无                                                         */
/************************************************************************/
bool LogicUIManager_IsTopWndIdle()
{
    return (eWT_Idle == _UIManager.GetTopWndType());
}

/************************************************************************/
/* 接口  ： LogicUIManager_IsTopWndTalk()                               */
/* 功能  ： 初始化判断顶层窗口是不是Talk                                */
/* 参数说明 ：                                                          */
/*     无                                                               */
/* 返回值 ： 无                                                         */
/************************************************************************/
bool LogicUIManager_IsTopWndTalk()
{
    return (eWT_Talk == _UIManager.GetTopWndType());
}

/************************************************************************/
/* 接口  ： LogicUIManager_IsTopWndZero()                               */
/* 功能  ： 初始化判断顶层窗口是不是Talk                                */
/* 参数说明 ：                                                          */
/*     无                                                               */
/* 返回值 ： 无                                                         */
/************************************************************************/
bool LogicUIManager_IsTopWndZero()
{
    return (eWT_ZeroSPTouch == _UIManager.GetTopWndType());
}

bool LogicUIManager_IsTopWndXMLBrowser()
{
    return (eWT_XMLBrower == _UIManager.GetTopWndType());
}
