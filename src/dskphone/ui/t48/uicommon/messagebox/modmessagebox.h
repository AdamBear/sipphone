#ifndef MODMESSAGEBOX_H
#define MODMESSAGEBOX_H

#include "messageboxbase.h"
#include "messageboxmanager.h"
#include "messageboxcallback.h"
#include "baseui/ipopupmenudelegate.h"

#include "optionmessagebox.h"

// 弹出框时间
#ifndef SHORT_TIME
#define SHORT_TIME           1500
#endif

#ifndef LONGER_TIME
#define LONGER_TIME          2000
#endif

#ifndef AUTO_EXIT_TIME
#define AUTO_EXIT_TIME          60000
#endif


/***********************************************************/
// 功能 : 弹出提示框
// 注意：eType: 提示框类型，改API目前支持MESSAGEBOX_NOTE,MESSAGEBOX_Cancel,
//              MESSAGEBOX_OkCancel
// 参数说明 :
//                  pAction: 弹窗结束后的回调指针
//                  strNote: 提示框正文
//                  nTime: 存在时间s，-1表示不会自动消失，单位毫秒
//                  strCancelBtnTitle: 取消按钮上的文字
//                  strConfirmBtnTitle: 确认按钮上的文字
//                  iID: 用来区分同一个回调类，同时存在多个同种类型的弹窗的时候使用
//                  extraData:附件数据
//                  bShowCountDown:是否显示倒计时
// 返回值 :          无
/***********************************************************/
void MessageBox_ShowCommonMsgBox(CMsgBoxCallBackAction * pAction,
                                 const QString & strNote,
                                 MESSAGEBOX_TYPE eType,
                                 int nMsTime = -1,
                                 const QString & strCancelBtnTitle = "",
                                 const QString & strOkBtnTitle = "",
                                 int iID = -1,
                                 const QVariant & extraData = QVariant(),
                                 bool bShowCountDown = false,
                                 MESSAGEBOX_RESULT eTimeResult = MessageBox_Cancel);

/***********************************************************/
// 功能 : 弹出提示框
// 注意：vecSoftkey是完全由外部自定义的按钮
// 参数说明 :
//                  pAction: 弹窗结束后的回调指针
//                  strNote: 提示框正文
//                  vecSoftkey: 要显示的softkey
//                  nMaxAllowedBtn: 允许显示按钮的最大数量，小于vecSoftkey中的数量则只显示前几个，大于则没有影响
//                  nTime: 存在时间s，-1表示不会自动消失，单位毫秒
//                  iID: 用来区分同一个回调类，同时存在多个同种类型的弹窗的时候使用
//                  extraData:附件数据
//                  bShowCountDown:是否显示倒计时
// 返回值 :          无
/***********************************************************/
void MessageBox_ShowCommonMsgBox(CMsgBoxCallBackAction * pAction,
                                 const QString & strNote,
                                 VecMsgBoxSoftkey & vecSoftkey,
                                 int nMsTime = -1,
                                 int iID = -1,
                                 const QVariant & extraData = QVariant(),
                                 bool bShowCountDown = false,
                                 MESSAGEBOX_RESULT eTimeResult = MessageBox_Cancel);

/***********************************************************/
// 功能: 弹出提示框
// 注意：传入MessageBox指针后，该MessageBox就被托管，外界无需释放
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//                  pMessageBox: 弹出框的指针
//                  iID: 用来区分同一个回调类，同时存在多个同种类型的弹出框的时候使用
// 返回值:             弹出框处理的结果
/***********************************************************/
void MessageBox_ShowMessageBox(CMsgBoxCallBackAction * pAction,
                               CMessageBoxBase * pMessageBox);

/***********************************************************/
// 功能: 弹出密码输入框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//                  strUserName: 密码校对使用的用户名
/***********************************************************/
void MessageBox_ShowPasswordBox(CMsgBoxCallBackAction * pAction,
                                const QString & strUserName,
                                const QVariant & extraData = QVariant(),
                                int iID = -1);

/***********************************************************/
// 功能: 弹出键盘锁解锁框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
/***********************************************************/
void MessageBox_ShowUnlockBox(CMsgBoxCallBackAction * pAction,
                              int iID = -1);

#if IF_FEATURE_METASWITCH
/***********************************************************/
// 功能: 弹出键盘锁解锁框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
/***********************************************************/
void MessageBox_ShowMTSWCommportalBox(CMsgBoxCallBackAction * pAction,
                                      int iID = -1);
#endif

/***********************************************************/
// 功能: 弹出Dsskey 密码框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
/***********************************************************/
void MessageBox_ShowDsskeyLockPasswordBox(CMsgBoxCallBackAction * pAction,
        const QVariant & extraData = QVariant(),
        int iID = -1);

/***********************************************************/
// 功能: 弹出XSI登陆框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//                  将由调用的回调函数来完成验证密码
/***********************************************************/
void MessageBox_ShowXSIPasswordBox(CMsgBoxCallBackAction * pAction,
                                   const QString & strUserTitle,
                                   const QString & strUserName,
                                   const QString & strPasswordTitle,
                                   int iID = -1);
#if IF_FEATURE_34162
void MessageBox_ShowLdapPasswordBox(CMsgBoxCallBackAction * pAction,
                                    const QString & strUserName,
                                    const QString & strPassword,
                                    int iID = -1);
#endif
/***********************************************************/
// 功能: 弹出选择框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//                  eType: 弹出框的类型，将决定弹出框是否有cancel、ok按钮
//                  vecSoftkey: 要显示的softkey
//                  nMaxAllowedBtn: 允许显示按钮的最大数量，小于vecSoftkey中的数量则只显示前几个，大于则没有影响
//                  nSelectBoxType: 大、小选择框
//                  pDelegate: 选择框内容代理
/***********************************************************/
CMessageBoxBase * MessageBox_ShowSelectBox(CMsgBoxCallBackAction * pAction,
        VecMsgBoxSoftkey & vecSoftkey,
        int nSelectBoxType,
        IFrameListDelegate * pDelegate,
        const QString & strTitle,
        int iID = -1);

/***********************************************************/
// 功能: 弹出选择框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//          //////iID、eType: 弹出框的ID和类型
//                  nRowCount: 选择框为一行或两行
//                  pDelegate: 选择框内容代理
/***********************************************************/
void MessageBox_ShowImageSelectBox(CMsgBoxCallBackAction * pAction,
                                   int nRowCount,
                                   IFrameListDelegate * pDelegate,
                                   int iID = -1);

/***********************************************************/
// 功能: option类型弹框
// 参数说明:
/***********************************************************/
void MessageBox_ShowOptionBox(CMsgBoxCallBackAction * pAction
                              , const vecOptionMsgBoxItem & vecOptionItem
                              , int iID = -1
                              , const QVariant & extraData = QVariant()
                              , bool bDefaultFocus = false
                              , int nDefaultIndex = -1
                              , int nTimeTodie = 0);

/***********************************************************/
// 功能: 弹出蓝牙框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//                  ieType: 弹出框的类型
//              //  lpszText: 标题
//                  bAutoConnect: 自动连接的默认值
//                  nMsTime: 消失时间
/***********************************************************/
void MessageBox_ShowBluetoothBox(CMsgBoxCallBackAction * pAction,
                                 const QString & lpszText,
                                 bool bAutoConnect,
                                 int nMsTime = -1,
                                 int iID = -1);

/***********************************************************/
// 功能: Wi-Fi弹出密码输入框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//                  strName: 密码校对使用的用户名
/***********************************************************/
void MessageBox_ShowWiFiPasswordBox(CMsgBoxCallBackAction * pAction,
                                    const QString & strName,
                                    const QVariant & extraData = QVariant(),
                                    int iID = -1);

/***********************************************************/
// 功能: 弹出Wi-Fi Detail框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//                  ieType: 弹出框的类型
//              //  lpszText: 标题
//                  bAutoConnect: 自动连接的默认值
//                  nMsTime: 消失时间
/***********************************************************/
void MessageBox_ShowWifiDetailBox(CMsgBoxCallBackAction * pAction,
                                  const QString & lpszText,
                                  int nMsTime = -1,
                                  int iID = -1);

/***********************************************************/
// 功能:
// 参数说明:
/***********************************************************/
void MessageBox_ShowTitleMenuBox(CMsgBoxCallBackAction * pAction,
                                 IPopupMenuDelegate * pDelegate,
                                 const QPoint & ptMenu,
                                 int iID = -1);

/***********************************************************/
// 功能: EDK使用的弹出框
// 参数说明:
/***********************************************************/
void MessageBox_ShowEDKInputBox(CMsgBoxCallBackAction * pAction
                                , int nMaxInput
                                , const QString & strHint = ""
                                , MESSAGEBOX_DEFAULT_INPUT_TYPE nDefalutInputType = MessageBox_Input_Type_abc
                                , bool bIsMaskFeedback = false
                                , int iID = -1
                                , const QVariant & extraData = QVariant());

/***********************************************************/
// 功能: 获取顶层弹出框
//返回值: 顶层弹出框指针
/***********************************************************/
CMessageBoxBase * MessageBox_GetTopMessageBox();

/**********************************************************/
// 功能: 删除指定的弹出框
// 参数说明: 无
//          eResult : 弹出框消失方式
//          pAction : 弹出框结束后的回调指针
//          iID ： 表示弹出框的ID，为-1表示删除Action对应的所有弹出框
// 返回值: 成功清除返回true，否则返回false
/**********************************************************/
bool MessageBox_RemoveMessageBox(CMsgBoxCallBackAction * pAction,
                                 MESSAGEBOX_RESULT eResult = MessageBox_Cancel,
                                 int iID = -1);

/***********************************************************/
// 功能: 以指定方式清除当前顶层弹出框
// 参数说明:
//                  nResultType : 弹出框消失方式
//返回值: 成功清除返回true，否则返回false
/***********************************************************/
bool MessageBox_RemoveTopMessageBox(MESSAGEBOX_RESULT eResult = MessageBox_Cancel);

/***********************************************************/
// 功能: 以指定方式清除当前所有弹出框
// 参数说明:
//                  nResultType : 弹出框消失方式
//返回值: 成功清除返回true，否则返回false
/***********************************************************/
bool MessageBox_RemoveAllMessageBox(MESSAGEBOX_RESULT eResult = MessageBox_Cancel);

/**********************************************************/
// 功能 : 获取当前弹出框总数
// 参数说明 : 无
// 返回值 : 当前存在的弹出框总数
/**********************************************************/
int MessageBox_GetMessageBoxCount();

/***********************************************************/
// 功能: 弹出Guest Unlock键盘锁解锁框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
/***********************************************************/
void MessageBox_ShowGuestUnlockBox(CMsgBoxCallBackAction * pAction,
                                   int iID = -1);


#if IF_FEATURE_METASWITCH_ACD
enum EMTSWAcdListShowLoggedMode
{
    EMTSWACDLIST_SHOW_NONE, //不显示logIn/logout 按钮
    EMTSWACDLIST_SHOW_LOGOUT, // 显示logout按钮
    EMTSWACDLIST_SHOW_LOGIN //显示login按钮
};

/***********************************************************/
// 功能: 弹出MTSW Acd 功能选择框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//                  strMlghName: 标题，显示当前点击的acd组名
//                  eShowLoggedMode: 显示login/logout按钮的模式
//                  iID: 表示弹出框的ID
/***********************************************************/
void MessageBox_ShowMTSWAcdListBox(CMsgBoxCallBackAction * pAction,
                                   const QString & strMlghName,
                                   EMTSWAcdListShowLoggedMode eShowLoggedMode,
                                   int iID = -1);

#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL
/***********************************************************/
// 功能: 弹出Metaswitch 选项框
// 参数说明:
//                  pAction: 弹出框结束后的回调指针
//                  lpszText: 标题
//                  bRead: 是否已读
//                  bAddToNetwork: 是否添加到metaswitch联系人
//                  nMsTime: 消失时间
/***********************************************************/
void MessageBox_ShowMTSWVoiceMailOptionBox(CMsgBoxCallBackAction * pAction,
        const QString & lpszText,
        bool bRead,
        bool bAddToNetwork,
        int nMsTime = -1,
        int iID = -1);
#endif

#endif // _MOD_MESSAGE_BOX_H_
