#ifndef _CALL_UI_MANAGER_H
#define _CALL_UI_MANAGER_H

#include "dlgcallmanagerbase.h"


void CallUIManager_NormalShowPage(const yl::string &
                                  strUIName);    //默认调用的进入子界面时回调函数   //当设置的回调函数指针为空时调用
void CallUIManager_NormalHidePage(const yl::string &
                                  strUIName);        //默认调用的 退出子界面时回调函数    //当设置的回调函数指针未空时调用
void CallUIManager_NormalUnboundCallBack(const yl::string & strUIName,
        CCallManagerBaseDlg * pDlg);   //解除绑定时的回调函数

class CCallUIManager
{
    //不让外部new 对象
protected:
    CCallUIManager(void);

public:
    virtual ~CCallUIManager(void);

    //获取CallUIManager 实例
    static CCallUIManager & GetInstance();

public:
    //设置子界面相关信息
    void SetPageInfo(const yl::string & strUIName,
                     int iUIFlag,                        //窗口属性
                     CCallMGRItemInfo callMGRItemInfo,   //Item 相关信息
                     PPageEventCallBack pPageEventCallBack,      //其他事件的回调函数 如show  、  hide
                     PShowPage pShowPage,            //需要显示该界面时调用的函数
                     PHidePage pHidePage,    //隐藏该界面时调用的函数
                     PUnboundCallBack pUnboundCallBack,  //解除绑定时的回调函数
                     int iItemIndex          //显示的顺序
                    );

    //是否存在 该元素
    bool IsExistItem(const yl::string & strUIName);
    //根据ItemIndex 序号获取 插入的迭代器
    YLList<CCallMGRInfo *>::iterator GetItemInsertItorByIndex(int
            iItemIndex);      //根据ItemIndex 序号获取 插入的迭代器
    //设置界面窗口
    void SetPageDlgByName(const yl::string & strUIName, CCallManagerBaseDlg * pDlg);
    //根据名称获取界面窗口
    CCallManagerBaseDlg * GetPageDlgByName(const yl::string & strUIName);
    //获取当前界面窗口
    CCallManagerBaseDlg * GetCurrentDlg();

    //获取当前页的名称
    yl::string GetCurrentPageName();

public:
    //由外部模块调用的 进入某个子界面
    void ModelEnterPage(const yl::string & strUIName);
    //由外部模块调用的 退出某个子界面
    void ModelPutPage(const yl::string & strUIName);

//protected:
//  //内部调用的 显示
//  void ManagerShowPage(CCallMGRInfo* pCallMGRInfo);
//  //内部调用的 隐藏
//  void ManagerHidePage(CCallMGRInfo* pCallMGRInfo);
//
//  //这个是去除某个窗口
//  void ManagerPutPage(CCallMGRInfo* pCallMGRInfo);

protected:
    //这边只是简单的隐藏一下窗口
    //根据名称信息显示对话框界面
    void DlgShowPage(CCallMGRInfo * pCallMGRInfo);
    //退出某个对护矿界面
    void DlgHidePage(CCallMGRInfo * pCallMGRInfo);

public:
    //获取第一个非焦点窗口  子对话框指针不能为空
    CCallMGRInfo * GetFirstNoFocusItem();

    //回到Idle前的处理函数
    virtual void OnReturnToIdle();

//切换窗口显示
    virtual void OnCallManagerBarClick(const yl::string & strUIName);

    //退出某个窗口显示
    virtual void OnCallManagerReleasePage(const yl::string & strUIName);

protected:
    //获取CallManager 对话框指针
    CDlgCallManagerBase * GetDlgCallManager();

    //释放CallManager 对话框指针
    void ReleaseDlgCallManager(CDlgCallManagerBase * pDlgCallManager);

    //检测 是否退出 dlgcallmanager ， 如果需要退出则退出
    void CheckReleaseDlgCallManager();



protected:
    //是否当前焦点窗口
    bool IsFocusPage(const yl::string & strUIName);
    //是否所有的子界面都是为空   这个用于在当前界面的对话框为空的时候判断是否要退出 callmanager 界面
    bool IsAllItemPageNull();

protected:
    CDlgCallManagerBase * m_pDlgCallManager;        //CallManager 对话框实例指针

    YLList<CCallMGRInfo *> m_listCallMGRInfo;       //保存的界面信息

    CCallMGRInfo * m_pCurrentCallMGRInfo;       //当前的CallMGR 信息

};



#define _CallUIManager CCallUIManager::GetInstance()

#endif
