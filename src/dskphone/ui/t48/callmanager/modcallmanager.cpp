#include "modcallmanager.h"
#include "callmanagercommon.h"

#include "calluimanager.h"
#include "dlgcallmanager.h"
//#include "callmanagerfuntion.h"



//窗体类注册
void callmanager_RegisterDlgClass()
{
    REGISTER_DLG(CDlgCallManager);
}

//模块初始化
void callmanager_Init()
{

}

//模块注销
void callmanager_Unint()
{

}

//根据窗口名称获取子界面
CCallManagerBaseDlg * callmanager_GetPageByUIName(const yl::string & strUIName)
{
    return _CallUIManager.GetPageDlgByName(strUIName);
}

//根据窗口名称设置窗口
void callmanager_SetPageDlgByName(const yl::string & strUIName, CCallManagerBaseDlg * pDlg)
{
    _CallUIManager.SetPageDlgByName(strUIName, pDlg);
}

//根据窗口名称显示界面
void callmanager_EnterPage(const yl::string & strUIName)
{
    _CallUIManager.ModelEnterPage(strUIName);
}

//退出子界面显示
void callmanager_PutPage(const yl::string & strUIName)
{
    _CallUIManager.ModelPutPage(strUIName);
}

//创建子窗口并且显示， 是调用回调显示窗口
void callmanager_CreateAndEnterPage(const yl::string & strUIName)
{
    _CallUIManager.OnCallManagerBarClick(strUIName);
}

//释放窗口， 是调用回调释放窗口
void callmanager_ReleasePage(const yl::string & strUIName)
{
    _CallUIManager.OnCallManagerReleasePage(strUIName);
}

//获取当前页的名称
yl::string callmanager_GetCurrentPageName()
{
    return _CallUIManager.GetCurrentPageName();
}

//获取当前子界面
CCallManagerBaseDlg * callmanager_GetCurrentPage()
{
    return _CallUIManager.GetCurrentDlg();
}

//添加页面信息
void callmanager_SetPageInfo(const yl::string & strUIName,
                             int iUIFlag,                       //窗口属性
                             CCallMGRItemInfo callMGRItemInfo,  //Item 相关信息
                             PPageEventCallBack pPageEventCallBack,     //其他事件的回调函数 如show  、  hide
                             PShowPage pShowPage,           //需要显示该界面时调用的函数
                             PHidePage pHidePage,   //隐藏该界面时调用的函数
                             PUnboundCallBack pUnboundCallBack, //解除绑定时的回调函数
                             int iItemIndex         //显示的顺序
                            )
{
    _CallUIManager.SetPageInfo(strUIName,
                               iUIFlag,
                               callMGRItemInfo,
                               pPageEventCallBack,
                               pShowPage,
                               pHidePage,
                               pUnboundCallBack,
                               iItemIndex);
}



