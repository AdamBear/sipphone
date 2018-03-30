#ifndef _MESSAGEBOXMANAGER_H
#define _MESSAGEBOXMANAGER_H

#include "uimanager/uimanager_inc.h"
#include "messageboxbase.h"


class CMsgBoxCallBackAction;

class CMessageBoxManager : public QObject
{
    Q_OBJECT

private:
    // 键盘回调函数
    static bool MessageBoxHookCallbackFun(int nKeyCode, bool bPress);

    // 鼠标回调函数
    static bool MessageBoxMouseHookCallbackFun(QObject * pObject, QMouseEvent * pEvent);

    static void OnTopWndChangeCallbackFun(CBaseDialog * dlgPreTop, CBaseDialog * dlgNewTop);

    // 全键盘显示与隐藏回调函数
    static bool MessageBoxKeyBoardShowCallbackFun(bool bKeyBoardShow);

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

        //是否要释放指针
        ~MsgBoxInfo() {}

        MsgBoxInfo & operator=(const MsgBoxInfo & other)
        {
            // 证同测试
            if (this != &other)
            {
                // 浅拷贝
                //......................................
                this->m_pMsgBox         = other.m_pMsgBox;
                this->m_pCallBackAction = other.m_pCallBackAction;
            }

            return *this;
        }
    };

    // 内部使用类型定义
    typedef std::vector<MsgBoxInfo>         VecMsgInfo;
    typedef VecMsgInfo::iterator            ItMsgInfo;
    typedef VecMsgInfo::reverse_iterator    ReverseItMsgInfo;

private:
    // 用于比较msgbox是否相等的函数对象
    class CEqualMsgBox
    {
    public:
        CEqualMsgBox(CMessageBoxBase * pMessageBox): m_pMsgBox(pMessageBox) {}
        bool operator()(const MsgBoxInfo & msgInfo) const
        {
            return (m_pMsgBox == msgInfo.m_pMsgBox);
        }

    private:
        CMessageBoxBase * m_pMsgBox;
    };

    // 用于比较CallBackAction是否相等的函数对象
    class CEqualMsgCBAction
    {
    public:
        CEqualMsgCBAction(CMsgBoxCallBackAction * pCBAction, int iID)
            : m_pCBAction(pCBAction)
            , m_iID(iID)
        {

        }

        bool operator()(const MsgBoxInfo & msgInfo) const
        {
            return (m_pCBAction == msgInfo.m_pCallBackAction
                    && (m_iID == -1
                        || (NULL != msgInfo.m_pMsgBox
                            && m_iID == msgInfo.m_pMsgBox->GetID())));
        }

    private:
        CMsgBoxCallBackAction * m_pCBAction;
        int                    m_iID;
    };

public:
    CMessageBoxManager();
    ~CMessageBoxManager();

    // 添加消息框
    bool AddMessageBox(CMessageBoxBase * pMessageBox, CMsgBoxCallBackAction * pCallBack);

    // 以指定方式清除弹出框
    // CallBackAction有可能对应多个弹出框
    bool RemoveMessageBox(CMsgBoxCallBackAction * pCallBack,
                          MESSAGEBOX_RESULT nResultType,
                          bool bCallBack = false,
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

    // 处理鼠标事件
    bool ProcessMouseEvent(QObject * pObject, QMouseEvent * pEvent);


private Q_SLOTS:
    void MaskFramePressed();

    bool OnKeyboardShow();
    bool OnKeyboardHide();

private:
    bool ProcessMsgBoxExit(const MsgBoxInfo & msgboxInfo,
                           MESSAGEBOX_RESULT nResultType,
                           bool bCallBack = true);

    void UpdateWindow();

private:
    VecMsgInfo     m_vecMsgBoxInfo;
    bool m_bIsKeyboardShow;
};

//获取全局唯一的弹出框管理对象
#define  _MessageBoxManager GetMessageBoxManagerInstance()
CMessageBoxManager & GetMessageBoxManagerInstance();

#endif // _MESSAGEBOXMANAGER_H
