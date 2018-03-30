#ifndef _MESSAGEBOX_MANAGER_H
#define _MESSAGEBOX_MANAGER_H

#include "messageboxbase.h"
#include "uimanager/moduimanager.h"
#include "globalinputhook/modinputhook.h"
#include "xwindow-kernel/include/xsignal.h"

#include <ylvector.h>

typedef CMessageBoxBase * (*FUN_CREATE_MSGBOX)(const MSGBOX_INFO & stMsgInfo);
typedef YLHashMap<MESSAGEBOX_TYPE, FUN_CREATE_MSGBOX> MAP_CREATE_MSGBOX_FUN;

class CMessageBoxManager
{
public:
    CMessageBoxManager();
    ~CMessageBoxManager();

public:
    static CMessageBoxManager & GetInstance();

private:
    // 键盘回调函数
    static bool MessageBoxHookCallbackFun(int nKeyCode, bool bPress);

    static void OnTopWndChangeCallbackFun(const CTopWndChangeInfo & objInfo);

private:
    // msgbox结构体,用于类内部使用
    struct MsgBoxInfo
    {
    public:
        CMessageBoxBase    *    m_pMsgBox;
        CMsgBoxCallBackAction * m_pCallBackAction;

    public:
        MsgBoxInfo(): m_pMsgBox(NULL), m_pCallBackAction(NULL) {}

        MsgBoxInfo(CMessageBoxBase * pMessageBox, CMsgBoxCallBackAction * pCallBack)
            : m_pMsgBox(pMessageBox), m_pCallBackAction(pCallBack) {}

        MsgBoxInfo(const MsgBoxInfo & other)
            : m_pMsgBox(other.m_pMsgBox), m_pCallBackAction(other.m_pCallBackAction) {}

        //是否要释放指针
        ~MsgBoxInfo() {}

        MsgBoxInfo & operator=(const MsgBoxInfo & other)
        {
            // 证同测试
            if (this != &other)
            {
                // 浅拷贝
                this->m_pMsgBox         = other.m_pMsgBox;
                this->m_pCallBackAction = other.m_pCallBackAction;
            }

            return *this;
        }
    };

    // 内部使用类型定义
    typedef YLVector<MsgBoxInfo>            VecMsgInfo;
    typedef VecMsgInfo::iterator                ItMsgInfo;

public:
    // 添加消息框
    bool AddMessageBox(CMessageBoxBase * pMessageBox, CMsgBoxCallBackAction * pCallBack);

    // 以指定方式清除弹出框
    // CallBackAction有可能对应多个弹出框
    bool RemoveMessageBox(CMsgBoxCallBackAction * pCallBack,
                          MESSAGEBOX_RESULT nResultType,
                          bool bCallBack = true,
                          int iID = -1);

    //  以指定方式清除单个弹出框
    bool RemoveMessageBox(CMessageBoxBase * pMessageBox,
                          MESSAGEBOX_RESULT nResultType,
                          bool bCallBack = true);

    // 以指定方式清除当前顶层弹出框，返回值表示是否完成清除
    bool RemoveTopMessageBox(MESSAGEBOX_RESULT nResultType);

    // 以指定方式清除当前所有弹出框
    bool RemoveAllMessageBox(MESSAGEBOX_RESULT nResultType);

    // 获取弹出框个数
    int GetMessageBoxCount();

    CMessageBoxBase * GetTopMessageBox();

    // 处理按键事件
    bool ProcessKeyEvent(int nKeyCode, bool bPress);

    bool RemoveMessageBoxType(MESSAGEBOX_TYPE nType,
                              MESSAGEBOX_RESULT eResult = MessageBox_Cancel);

    void RegisterMsgBox(MESSAGEBOX_TYPE eMsgBoxType, FUN_CREATE_MSGBOX pFunCreateMsgBox);
    CMessageBoxBase * ApplyMsgBox(const MSGBOX_INFO & stMsgInfo);

public:
    xSignal                signalMessageBoxShow;

protected:
    CMessageBoxBase * ApplyDefaultMsgBox(const MSGBOX_INFO & stMsgInfo);

private:
    bool ProcessMsgBoxExit(const MsgBoxInfo & msgboxInfo,
                           MESSAGEBOX_RESULT nResultType,
                           bool bCallBack = true,
                           bool bPlayAnimation = true);

    void UpdateWindow();

private:
    VecMsgInfo     m_vecMsgBoxInfo;

    MAP_CREATE_MSGBOX_FUN       m_mapCreateFun;

};

//获取全局唯一的弹出框管理对象
#define _MessageBoxManager CMessageBoxManager::GetInstance()

#endif // _MESSAGEBOX_MANAGER_H
