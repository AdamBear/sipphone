#ifndef MESSAGEBOXBASE_H
#define MESSAGEBOXBASE_H

#include <QtGui>
#include <ylstring.h>
#include "uikernel/quipushbutton.h"
#include "imsgboxeventfilter.h"
#include "messageboxbuttonset.h"
#include "baseui/maskframe.h"

class qUIPushButton;
class CMaskFrame;

// messageBox的类型
enum MESSAGEBOX_TYPE
{
    MESSAGEBOX_Error = -1,
    MESSAGEBOX_NOTE = 0,
    MESSAGEBOX_Cancel,
    MESSAGEBOX_Ok,
    MESSAGEBOX_OkCancel,
    MESSAGEBOX_OkBlock,

    //特殊类型，按钮个数等于或超过三个，用来实现voicemail和DiscardSave等
    MESSAGEBOX_SPECIAL,

    MESSAGEBOX_PWD,             // 密码框
    MESSAGEBOX_UNLOCK,          // 解锁框
    // 以下为T48使用
    MESSAGEBOX_SELECT,          // 选择框
    MESSAGEBOX_IMAGE_SELECT,            //图片选择框
    MESSAGEBOX_BLUETOOTH,       // bluetooth
    MESSAGEBOX_WIFIDETAIL,      // Wi-Fi detail
    MESSAGEBOX_BSFT_PWD,        // BroadSoft密码
    MESSAGEBOX_LDAP_PWD,        // Ldap密码

    MESSAGEBOX_LINE_SELECT,         //坐上角选择默认话路的“弹出框”

    MESSAGEBOX_EDK,     //EDK使用的输入框
    MESSAGEBOX_GUEST_UNLOCK,    //GUEST 解锁框

    //使用SPECIAL类型来实现voicemail
    //  MESSAGEBOX_VoiceMail,
    //使用SPECIAL类型来实现DiscardSave
    //  MESSAGEBOX_DiscardSave,     // 取消、放弃、保存

    MESSAGEBOX_OPTION,      //Option弹框
    MESSAGEBOX_GPICKUP,        // 包含Gpickup

    MESSAGEBOX_MTSW_COMMPORTAL,     // MTSW密码
    MESSAGEBOX_MTSWACDLIST,     // mtsw acd list

    MESSAGEBOX_MTSW_VOICEMAIL,      //metaswitch 选项框

};

//弹出框的返回值
enum MESSAGEBOX_RESULT
{
    MessageBox_ERROR        = -1,
    MessageBox_Destory  = 0,
    MessageBox_OK           = 1,
    MessageBox_Cancel,
    MessageBox_View,
    MessageBox_Block,
    MessageBox_Close, // 人为的关闭

    MessageBox_Admin,               //在键盘锁解锁框中输入了管理员密码

    MessageBox_Discard, //放弃保存

    //蓝牙
    MessageBox_Delete,
    MessageBox_MoveUp,
    MessageBox_MoveDown,
    MessageBox_On,
    MessageBox_Off,
    MessageBox_Edit,
    MessageBox_MobileSyncOn,
    MessageBox_MobileSyncOff,
    MessageBox_GPickup, // GPickup
    MessageBox_Account,
    // MTSW ACD List
    MessageBox_MTSWACDList_Login,
    MessageBox_MTSWACDList_Logout,
    MessageBox_MTSWACDList_Detail,
    MessageBox_MTSWACDList_Member,

    //metaswitch voicemail
    MessageBox_CallBack,
    MessageBox_MarkAsNew,
    MessageBox_MarkAsHeard,
    MessageBox_AddToLocal,
    MessageBox_AddToNetwork,
    MessageBox_Timeout,
    // transfer type
    MessageBox_Send,
    MessageBox_BTransfer,
};

enum MESSAGEBOX_DEFAULT_INPUT_TYPE
{
    MessageBox_Input_Type_abc,
    MessageBox_Input_Type_123,
    MessageBox_Input_Type_Error,
};

struct MsgBoxSoftkey
{
public:
    QString m_title;
    MESSAGEBOX_RESULT m_result;

public:
    MsgBoxSoftkey(QString title = "", MESSAGEBOX_RESULT result = MessageBox_Destory)
        : m_title(title), m_result(result)    {}

};

typedef std::vector<MsgBoxSoftkey>      VecMsgBoxSoftkey;
typedef VecMsgBoxSoftkey::iterator          ItMsgBoxSoftkey;

class CMessageBoxBase : public QDialog
{
    Q_OBJECT

public:
    CMessageBoxBase(CMaskFrame * pMaskFrame);
    virtual ~CMessageBoxBase();

    inline void SetType(MESSAGEBOX_TYPE eType)
    {
        m_eType = eType;
    }
    inline MESSAGEBOX_TYPE GetType() const
    {
        return m_eType;
    }

    inline void SetResult(MESSAGEBOX_RESULT iResult)
    {
        m_eResult = iResult;
    }
    inline void SetTimeoutResult(MESSAGEBOX_RESULT iResult)
    {
        m_eTimeoutResult = iResult;
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

    //////////////////////////////////////////
    inline void SetExtraData(const QVariant & extraData)
    {
        m_ExtraData = extraData;
    }
    inline QVariant GetExtraData()
    {
        return m_ExtraData;
    }
    //////////////////////////////////////////

    void SetMaskRect(const QRect & rcMask);

    void SetEventFilter(IMsgBoxEventFilter * pFilter);
    IMsgBoxEventFilter * GetEventFilter() const;

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    virtual bool IfClickOutExit()
    {
        return false;
    }

    virtual void PrintMsgBoxInfo(bool bAdd) = 0;

protected:
    void ExitMessageBox(MESSAGEBOX_RESULT eResult);

protected Q_SLOTS:
    void OnCancelPressed();
    void OnOkPressed();

protected Q_SLOTS:
    void OnButtonPressed(const QString & title);

protected:
    // 设置窗口销毁的时间
    void SetExitTime(int iTimeToExit);

protected slots:
    //销毁窗口
    void OnExitTimeOut();

protected:
    qUIPushButton   *   m_pBtnOk;
    qUIPushButton   *   m_pBtnCancel;

    CMessageBoxButtonSet    m_BtnSet;
    CMaskFrame     *    m_pMaskFrame;

    IMsgBoxEventFilter * m_pFilter;

    VecMsgBoxSoftkey m_vecSoftkey;

    bool            m_bAllowOkButton;

private:
    MESSAGEBOX_RESULT           m_eResult;
    MESSAGEBOX_TYPE             m_eType;
    MESSAGEBOX_RESULT           m_eTimeoutResult;
    int                         m_iID;
    QVariant                    m_ExtraData;

    QTimer      m_timer;

};

#endif // MESSAGEBOXBASE_H
