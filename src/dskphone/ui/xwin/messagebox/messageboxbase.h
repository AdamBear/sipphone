#ifndef __messageboxbase_h__
#define __messageboxbase_h__

#include "xwindow/xWindow.h"
#include "xwindow-kernel/include/xthreadtimer.h"
#include "messagebox/messageboxdefine.h"
#include "messageboxcallback.h"


#define MESSAGEBOX_WINDOW_LAYOUT_LEVEL_ABOVE_SOFTKEY 0
#define MESSAGEBOX_WINDOW_LAYOUT_LEVEL_UNDER_SOFTKEY 2

extern const char * MSGBOX_ID_TEXT_TITLE;
extern const char * MSGBOX_ID_TEXT_MSG;

// messageBox的类型
enum MESSAGEBOX_TYPE
{
    MESSAGEBOX_Error       = -1,
    MESSAGEBOX_NOTE        =  0,
    MESSAGEBOX_Cancel,
    MESSAGEBOX_OkCancel,
    MESSAGEBOX_SPECIAL,
    MESSAGEBOX_PWD,
    MESSAGEBOX_UNLOCK,
    MESSAGEBOX_EDK,
    MESSAGEBOX_AUDIO_TEST,
    MESSAGEBOX_MIDDLE,
    MESSAGEBOX_END
};

//弹出框的返回值
enum MESSAGEBOX_RESULT
{
    MessageBox_ERROR        = -1,
    MessageBox_Destory  =  0,       //系统原因导致的非人为关闭
    MessageBox_OK           =  1,
    MessageBox_Cancel,
    MessageBox_TimeOut,
    MessageBox_Close,
    MessageBox_Connect,
    MessageBox_View,
    MessageBox_Edit,
};

#if IF_MSG_BOX_SHOW_ICON
enum MSGBOX_ICON_TYPE
{
    MSGBOX_ICON_NONE = -1, //NONE时的图标为INFO
    MSGBOX_ICON_ERROR, //错误型图标
    MSGBOX_ICON_WARNING, //警告图标
    MSGBOX_ICON_ASK, //询问型图标
    MSGBOX_ICON_INFO, //提示信息图标
    MSGBOX_ICON_SAVESUCCESS //保存成功图标
};
#endif

struct MsgBoxSoftkey
{
public:
    yl::string m_title;
    MESSAGEBOX_RESULT m_result;

public:
    MsgBoxSoftkey(yl::string title = "", MESSAGEBOX_RESULT result = MessageBox_ERROR)
        : m_title(title)
        , m_result(result)
    { }
};
typedef YLVector<MsgBoxSoftkey>     VecMsgBoxSoftkey;

struct MSGBOX_INFO
{
    //弹窗结束后的回调指针
    CMsgBoxCallBackAction * pAction;
    //弹框类型
    MESSAGEBOX_TYPE eType;
    //标题
    yl::string strTitle;
    //提示内容
    yl::string strNote;
    //按钮列表
    VecMsgBoxSoftkey vecSoftkey;
#if IF_MSG_BOX_SHOW_ICON
    //是否显示图标
    MSGBOX_ICON_TYPE eIconType;
#endif
    //自动退出时间(ms)
    int nMsTime;
    //回调id
    int iID;
    // 是否倒计时
    bool bCountDown;
    //额外内容
    void * extraData;

    MSGBOX_INFO()
    {
        pAction = NULL;
        eType = MESSAGEBOX_Error;
        strTitle = "";
        strNote = "";
        vecSoftkey.clear();
#if IF_MSG_BOX_SHOW_ICON
        eIconType = MSGBOX_ICON_NONE;
#endif
        nMsTime = -1;
        iID = -1;
        bCountDown = false;
        extraData = NULL;
    }
};

class CMessageBoxBase : public xFrameLayout, public xSlotHandler, public xTimerHandler
{
public:
    CMessageBoxBase();
    virtual ~CMessageBoxBase();

    virtual void UninitMsgBox() {};

    inline void SetType(int nType)
    {
        m_nType = nType;
    }
    inline int GetType() const
    {
        return m_nType;
    }

    inline void SetResult(MESSAGEBOX_RESULT iResult)
    {
        m_eResult = iResult;
    }
    inline MESSAGEBOX_RESULT GetResult()
    {
        return m_eResult;
    }

    inline void SetID(int iID)
    {
        m_iID = iID;
    }
    inline int GetID()
    {
        return m_iID;
    }

    inline void SetIsRemovedByManager(bool bRemovedByManager)
    {
        m_bRemovedByManager = bRemovedByManager;
    }
    inline bool IsRemovedByManager()
    {
        return m_bRemovedByManager;
    }

    //////////////////////////////////////////
    inline void SetExtraData(void * extraData)
    {
        m_ExtraData = extraData;
    }
    inline void * GetExtraData()
    {
        return m_ExtraData;
    }
    //////////////////////////////////////////

    //设置单独存在
    //////////////////////////////////////////
    inline void SetIsExistOnly(bool bExistOnly)
    {
        m_bIsExistOnly = bExistOnly;
    }
    inline bool GetIsExistOnly()
    {
        return m_bIsExistOnly;
    }
    //////////////////////////////////////////

    //设置限制返回idle
    //////////////////////////////////////////
    inline void SetIsForbitAutoReturnToIdle(bool bIsForbitAutoReturnToIdle)
    {
        m_bIsForbitAutoReturnToIdle = bIsForbitAutoReturnToIdle;
    }
    inline bool GetIsForbitAutoReturnToIdle()
    {
        return m_bIsForbitAutoReturnToIdle;
    }
    //////////////////////////////////////////

    inline void SetShowAlways(bool bShowAlways)
    {
        m_bShowAlways = bShowAlways;
    }
    inline bool IsShowAlways()
    {
        return m_bShowAlways;
    }

public:
    virtual void SetTitle(const yl::string & strTitle);
    virtual void SetMessage(const yl::string & strMessage);

    bool GetTitleExist();

    virtual void SetWidgetFocus() {};

    virtual void DeleteThisBox();

public:
    virtual void loadChildrenElements(xml_node & node);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

public:
    // 设置窗口销毁的时间
    void SetExitTime(int iTimeToExit);

    virtual void RestartExitTime();

    inline int GetExitTime()
    {
        return m_iTimeToDie;
    }

    void SetRestartDieTimeByKey(bool bEnable);

    inline bool IsRestartDieTimeByKey()
    {
        return m_bRestartDieTimeByKey;
    }

    void SetRemoveByKey(bool bEnable);
    inline bool IsRemoveByKey()
    {
        return m_bRemoveByKey;
    }

    void OnExitTimeOut(xThreadTimer* timer, LPVOID pExtraData);

public:
    void SetSoftkey(const VecMsgBoxSoftkey & vecSoftkey);

private:
    void slotOnSoftkeyPress(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                            LPVOID pData);
    void slotOnSoftKeyLongPress(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                LPVOID pData);

protected:
    virtual bool OnSoftkeyAction(int nType, bool bLongPress);

    bool CreateWindowObject(bool bAboveSoftkey = false);
    virtual void ExitMessageBox(MESSAGEBOX_RESULT eResult);

private:
    //弹出框通用属性
    MESSAGEBOX_RESULT           m_eResult;
    int                         m_nType;
    int                         m_iID;
    void*                       m_ExtraData;

    int                         m_iTimeToDie;
    bool                        m_bRestartDieTimeByKey;
    bool                        m_bRemovedByManager;
    bool                        m_bRemoveByKey;
    //弹出框特制属性
    //是否单独存在（如果该值为真，则弹出新的弹出框时，会把该弹出框清掉）
    bool        m_bIsExistOnly;
    //是否需要限制自动回到idle
    bool        m_bIsForbitAutoReturnToIdle;
    //是否总是显示（在弹出新弹窗时判断旧是否显示）
    bool        m_bShowAlways;

protected:
    xThreadTimer                m_timer;

protected:
    xTextView*      m_pTitle;
    xTextView*      m_pMessage;
};

#endif
