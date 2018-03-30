#ifndef XML_BROWSER_MESSAGE_MANAGER_H
#define XML_BROWSER_MESSAGE_MANAGER_H

#include <ylstring.h>
#include "ETLLib.hpp"
#include "messagebox/messageboxcallback.h"

#include "extended/xmlbrowser/include/logicanduicommon.h"

#if XML_BROWSER_SUPPORT_UI

// 采用新的弹窗实现
class XBMessageManager : public CMsgBoxCallBackAction
{

public:
    XBMessageManager();

    ~XBMessageManager();

    // 弹窗结束的回调函数
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox) ;

    // 弹出框提示信息
    void ShowMessage(XB_TIP_TYPE eType, UINT uTimeOut = 0);

    // 销毁弹出框
    void DestroyMessage();

    // 解析提示类型，获取提示内容
    yl::string GetNoteByType(XB_TIP_TYPE eType);

    // 根据提示类型，判断点击取消进的操作
    void ClickCancelActionByType(XB_TIP_TYPE eType);

    bool IsXmlMsgBoxPopuping();

    // 延时弹出消息的槽函数
    void OnShowMessage();

private:
    XB_TIP_TYPE     m_eMsgType;
    UINT            m_uTimeOut;
    bool            m_bIsPopuping;
};

XBMessageManager & GetXBMessageManager();

#define _XBMessageManager GetXBMessageManager()

#endif

#endif // XML_BROWSER_MESSAGE_MANAGER_H
