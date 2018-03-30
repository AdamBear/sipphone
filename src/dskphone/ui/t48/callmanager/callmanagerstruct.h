#ifndef _CALL_MANAGER_STRUCT_H
#define _CALL_MANAGER_STRUCT_H

#include "uicommon/uimanager/basedialog.h"

#include "callmanagerptstruct.h"

typedef CBaseDialog CCallManagerBaseDlg;


typedef void (* PPageEventCallBack)(const yl::string & strUIName,
                                    CallManagerEventType eventType);       //其他事件的回调函数 如 show ， hide

typedef void (* PShowPage)(const yl::string & strUIName);       //显示页面调用的函数
typedef void (* PHidePage)(const yl::string & strUIName);       //隐藏页面调用的函数


typedef void (* PUnboundCallBack)(const yl::string & strUIName,
                                  CCallManagerBaseDlg * pDlg); //解除绑定后的回调函数



//typedef void (* PEnterPageCallBack)(const yl::string& strUIName);     //进入子界面时回调函数
//typedef void (* PPutPageCallBack)(const yl::string& strUIName);       //退出界面时回调函数
//
//typedef void (* PChangeItemShowCallBack)(const yl::string& strUIName);    //切换Item时 显示窗口的回调函数
//typedef void (* PChangeItemHideCallBack)(const yl::string& strUIName);    //切换Item时 隐藏窗口的回调函数

#define CALLMANAGER_DEFAULT_ITEM_INDEX 1000     //默认显示顺序
//子窗口信息结构体
struct tagCallMGRInfo
{
    tagCallMGRInfo()
    {
        strUIName = "";         //UI名称
        iUIFlag = 0;
        iItemIndex = CALLMANAGER_DEFAULT_ITEM_INDEX;    //设置默认显示顺序
        pPageEventCallBack = NULL;      //进入该子界面回调
        pShowPage = NULL;           //退出该子界面回调
        pHidePage = NULL;   //切换界面显示回调
        pUnboundCallBack = NULL;    //切换界面隐藏回调

        bIsUsed = false;            //是否已经有使用 主要是在退出 callmanager时 用来标识 是否调用退出函数   这个标志位与是否有pdlg 无关  //主要是防止重复调用外部模块的清除数据函数
        pDlg = NULL;            //子界面窗口
    }

    yl::string strUIName;           //UI名称
    int iUIFlag;                    //该窗口属性
    int iItemIndex;                 //显示的顺序
    PPageEventCallBack pPageEventCallBack;      //其他事件的回调函数 如show  、  hide
    PShowPage pShowPage;            //需要显示该界面时调用的函数
    PHidePage pHidePage;            //隐藏该界面时调用的函数
    PUnboundCallBack pUnboundCallBack;  //解除绑定时的回调函数


    //PEnterPageCallBack pEnterPage;        //进入该子界面回调
    //PPutPageCallBack pPutPage;            //退出该子界面回调
    //PChangeItemShowCallBack pChangeItemShow;  //切换界面显示回调
    //PChangeItemHideCallBack pChangeItemHide;  //切换界面隐藏回调

    bool bIsUsed;           //是否已经有使用 主要是在退出 callmanager时 用来标识 是否调用退出函数   这个标志位与是否有pdlg 无关  //主要是防止重复调用外部模块的清除数据函数
    CCallManagerBaseDlg * pDlg;         //子界面窗口

    //结构体可能修改
    CCallMGRItemInfo callMGRItemInfo;   //item相关信息  不同项目 该类可能不同
};

typedef struct tagCallMGRInfo CCallMGRInfo;
//
//class CCallManagerBaseDlg: public CBaseDialog
//{
//
//};



class CCallManagerStruct
{
public:
    CCallManagerStruct(void);
    virtual ~CCallManagerStruct(void);
};

#endif
