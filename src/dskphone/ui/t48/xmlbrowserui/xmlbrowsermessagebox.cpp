#include "xmlbrowsermessagebox.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"

#include "messagebox/modmessagebox.h"
#include "uimanager/modlogicuimanager.h"
#include "uimanager/moduimanager.h"
#include "uikernel/languagehelper.h"
#include "dsklog/log.h"
#include "translateiddefine.h"

namespace
{
#define XBMESSAGE_MSG_ID    1
}

XBMessageManager & GetXBMessageManager()
{
    static XBMessageManager objXBMessageManager;
    return objXBMessageManager;
}

XBMessageManager::XBMessageManager()
    : m_bIsPopuping(false)
    , m_eMsgType(XB_TIP_UNKOWN)
    , m_uTimeOut(0)
    , m_bInterrupt(false)
{

}

XBMessageManager::~XBMessageManager()
{

}

bool XBMessageManager::IsXmlMsgBoxPopuping()
{
    return m_bIsPopuping;
}

// 弹出框提示信息
void XBMessageManager::ShowMessage(XB_TIP_TYPE eType, UINT uTimeOut/* = 0*/)
{
    XMLBROWSERUI_INFO("XBMessageManager::ShowMessage(%d, %d)", eType, uTimeOut);
    // 删除已经存在的弹出框
    DestroyMessage();

    //清除其他可能存在的提示框
    MessageBox_RemoveAllMessageBox(MessageBox_Destory);

    m_eMsgType = eType;
    m_uTimeOut = uTimeOut;
    m_bInterrupt = false;

    m_bIsPopuping = false;

    CBaseDialog * pXBDialog = UIManager_GetPrivateDlg(DLG_CDlgXMLBrowser);
    //在talkui界面不弹窗,xmlbrowser 已经存在 但是不是顶层窗口时不弹窗
    bool bBlock = LogicUIManager_IsTopWndTalk() ||
                  (NULL != pXBDialog && pXBDialog != UIManager_GetTopWindow());

    if (bBlock)
    {
        return;
    }

    QString strTitle = LANG_TRANSLATE(TRID_NOTE);
    yl::string strNote = "";

    strNote = GetNoteByType(m_eMsgType);

    XMLBROWSERUI_INFO("XBMessageManager::OnShowMessage message [%s]", strNote.c_str());

    //添加弹出标记，解决Idle界面有弹出框时载入xml闪烁问题
    m_bIsPopuping = true;
    MessageBox_ShowCommonMsgBox(&_XBMessageManager, LANG_TRANSLATE(strNote.c_str()), MESSAGEBOX_Cancel,
                                m_uTimeOut, "", "", XBMESSAGE_MSG_ID);
}

#if IF_FEATURE_XMLBROWSER_HOTELING
void XBMessageManager::ShowTips(const yl::string & strMessage, UINT uTimeOut/* = 3000*/)
{
    if (strMessage.empty())
    {
        return;
    }
    XMLBROWSERUI_INFO("XBMessageManager::ShowTips [%s] (%d)", strMessage.c_str(), uTimeOut);

    // 删除已经存在的弹出框
    DestroyMessage();

    //清除其他可能存在的提示框
    MessageBox_RemoveAllMessageBox(MessageBox_Destory);

    m_bIsPopuping = true;
    m_eMsgType = XB_TIP_CONFIG_TIPS;
    MessageBox_ShowCommonMsgBox(&_XBMessageManager, strMessage.c_str(), MESSAGEBOX_Cancel, uTimeOut,
                                LANG_TRANSLATE(TRID_EXIT));
}
#endif

yl::string XBMessageManager::GetNoteByType(XB_TIP_TYPE eType)
{
    //!!!!! 核对提示语
    yl::string strNote = TRID_UNKNOWN_ERROR;
    switch (eType)
    {
    case XB_TIP_INVAILID_URI:
    {
        strNote = TRID_INVALID_URI;
    }
    break;
    case XB_TIP_LARGE_FILE:
    {
        strNote = TRID_FILE_IS_TOO_LARGE;
    }
    break;
    case XB_TIP_LARGE_IMAGE:
    {
        strNote = TRID_IMAGE_IS_TOO_LARGE;
    }
    break;
    case XB_TIP_LOAD_FAIL:
    {
        strNote = TRID_LOADING_ERROR;
    }
    break;
    case XB_TIP_LOAD_IMAGE_FAIL:
    {
        strNote = TRID_IMAGE_LOAD_ERROR;
    }
    break;
    case XB_TIP_LOADING:
    {
        strNote = TRID_LOADING_PLEASE_WAIT;
    }
    break;
    case XB_TIP_LOADING_IMAGE:
    {
        strNote = TRID_LOADING_IMAGE;
    }
    break;
    case XB_TIP_NET_UNAVAILABLE:
    {
        strNote = TRID_NET_UNAVAILABLE;
    }
    break;
    case XB_TIP_NOT_EXIST:
    {
        strNote = TRID_DO_NOT_EXIST;
    }
    break;
    case XB_TIP_ITEMS_OUT_OF_LIMIT:
    {
        strNote = TRID_ITEMS_OUT_OF_LIMIT;
    }
    break;
    case XB_TIP_PARSE_FAIL:
    {
        strNote = TRID_FILE_LAYOUT_ERROR;
    }
    break;
    case XB_TIP_NO_SUBMIT_URI:
    {
        strNote = TRID_NO_SUBMIT_URL;
    }
    break;
    case XB_TIP_REBOOT:
    {
        strNote = TRID_REBOOT;
    }
    break;
    case XB_TIP_RESET:
    {
        strNote = TRID_RESETTING_TO_FACTORY_PLEASE_WAIT;
    }
    break;
    case XB_TIP_INVALID_IP:
    {
        strNote = TRID_INVALID_IP_ADDRESS;
    }
    break;
    case XB_TIP_INVALID_DATE_INT:
    {
        strNote = TRID_INVALID_DATE_FORMAT;
    }
    break;
    case XB_TIP_INVALID_DATE_US:
    {
        strNote = TRID_INVALID_US_DATE_FORMAT;
    }
    break;
    case XB_TIP_INVALID_TIME_US:
    {
        strNote = TRID_INVALID_US_TIME_FORMAT;
    }
    break;
    case XB_TIP_INVALID_TIME_INT:
    {
        strNote = TRID_INVALID_TIME_FORMAT;
    }
    break;
    case XB_TIP_COMPRESS_FAIL:
    {
        strNote = TRID_COMPRESS_ERROR;
    }
    break;
#ifdef IF_FEATURE_GENBAND
    case XB_TIP_ACTIVATED:
    {
        strNote = TRID_FEATURE_ACTIVATED;
    }
    break;
    case XB_TIP_NOT_ACTIVATED:
    {
        strNote = TRID_FEATURE_NOT_ACTIVATED;
    }
    break;
#endif
#if IF_FEATURE_XMLBROWSER_HOTELING
    case XB_TIP_HOTELING_LOGIN_FAIL:
    {
        strNote = TRID_LOGIN_FAILED;
    }
    break;
    case XB_TIP_HOTELING_LOGOUT_FAIL:
    {
        strNote = TRID_LOGOUT_FAILED;
    }
    break;
#endif
    default:
        break;
    }

    return strNote;
}

void XBMessageManager::ClickCancelActionByType(XB_TIP_TYPE eType)
{
    switch (eType)
    {
    case XB_TIP_LOADING:
    case XB_TIP_LOADING_IMAGE:
    {
        XMLBROWSERUI_INFO("XBMessageManager  xmlbrowser_CancelDownload");
        xmlbrowser_CancelDownload();
    }
    break;
    case XB_TIP_INVAILID_URI:
    case XB_TIP_LARGE_FILE:
    case XB_TIP_LARGE_IMAGE:
    case XB_TIP_LOAD_FAIL:
    case XB_TIP_LOAD_IMAGE_FAIL:
    case XB_TIP_NET_UNAVAILABLE:
    case XB_TIP_NOT_EXIST:
    case XB_TIP_PARSE_FAIL:
    default:
        break;
    }
}

// 销毁弹出框
void XBMessageManager::DestroyMessage()
{
    //移除所有弹出框，包括已经弹出的和正在计时弹出的。
    XMLBROWSERUI_INFO("XMLBrowserMessageBox::DestroyMessage, IsPopuping[%d]", m_bIsPopuping);
    //中断弹出计时中的框
    m_bInterrupt = true;

    //移除已经弹出的框
    if (m_bIsPopuping)
    {
        MessageBox_RemoveMessageBox(&_XBMessageManager, MessageBox_Destory, XBMESSAGE_MSG_ID);
        m_bIsPopuping = false;
    }
}

// 弹窗结束的回调函数
void XBMessageManager::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    XMLBROWSERUI_INFO("XMLBrowserMessageBox::MessageBoxCallBack");
    if (pMessageBox == NULL)
    {
        return;
    }

    int nResult = pMessageBox->GetResult();
    // 响应cancel键的处理
    if (nResult == MessageBox_Cancel)
    {
        ClickCancelActionByType(m_eMsgType);
    }

    // 弹窗结束
    m_bIsPopuping = false;
}
