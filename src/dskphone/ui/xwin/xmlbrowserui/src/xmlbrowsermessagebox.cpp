#include "xmlbrowsermessagebox.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"

#include "messagebox/modmessagebox.h"
#include "uimanager/moduimanager.h"
#include "languagehelper/modlangtranslate.h"
#include "dsklog/log.h"
#include "translateiddefine.h"

#if XML_BROWSER_SUPPORT_UI

namespace
{
#define XBMESSAGE_MSG_ID    100 + 6
#define SOFT_KEY_NUM        4
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

    //!!!!! 确认下不延时是否会阻塞
    m_eMsgType = eType;
    m_uTimeOut = uTimeOut;

    m_bIsPopuping = false;

    OnShowMessage();
}

// 延时弹出消息的槽函数
void XBMessageManager::OnShowMessage()
{
    CBaseDialog * pXBDialog = UIManager_GetPrivateDlg(DLG_CDlgXMLBrowser);
    //在talkui界面不弹窗,xmlbrowser 已经存在 但是不是顶层窗口时不弹窗
    bool bBlock = UIManager_IsInTalk() ||
                  (NULL != pXBDialog && pXBDialog != UIManager_GetTopWindow());

    if (bBlock)
    {
        return;
    }

    // 全屏恢复出厂和重启提示特殊处理
    if (XB_TIP_RESET == m_eMsgType || XB_TIP_REBOOT == m_eMsgType)
    {
        // 逻辑内部调用的adapter接口已包含UI的提示与显示
        return;
    }

    yl::string strTitle = _LANG("Note");
    yl::string strNote = "";

    strNote = GetNoteByType(m_eMsgType);

    XMLBROWSERUI_INFO("XBMessageManager::OnShowMessage, message [%s]", strNote.c_str());

    //添加弹出标记，解决Idle界面有弹出框时载入xml闪烁问题
    m_bIsPopuping = true;
    MSGBOX_INFO stMsgInfo;
    stMsgInfo.strTitle = strTitle;
    stMsgInfo.strNote = _LANG(strNote);
    stMsgInfo.pAction = &_XBMessageManager;
    stMsgInfo.nMsTime = m_uTimeOut;
    stMsgInfo.iID = XBMESSAGE_MSG_ID;

    if (XB_TIP_LOADING == m_eMsgType
            || XB_TIP_LOADING_IMAGE == m_eMsgType)
    {
        stMsgInfo.eType = MESSAGEBOX_Cancel;
    }
    else
    {
        stMsgInfo.eType = MESSAGEBOX_OkCancel;
        MsgBoxSoftkey stSoftInfo;
        for (int i = 0; i < SOFT_KEY_NUM - 1; ++i)
        {
            stMsgInfo.vecSoftkey.push_back(stSoftInfo);
        }
        stSoftInfo.m_title = _LANG(TRID_OK);
        stSoftInfo.m_result = MessageBox_OK;
        stMsgInfo.vecSoftkey.push_back(stSoftInfo);
    }

    MessageBox_ShowCommonMsgBox(stMsgInfo);
    //m_bIsPopuping = false;
}

yl::string XBMessageManager::GetNoteByType(XB_TIP_TYPE eType)
{
    // 核对提示语
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
    XMLBROWSERUI_INFO("XBMessageManager::DestroyMessage");
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
    if (NULL == pMessageBox)
    {
        return;
    }

    int nResult = pMessageBox->GetResult();
    // 响应cancel键的处理
    if (MessageBox_Cancel == nResult)
    {
        ClickCancelActionByType(m_eMsgType);
    }

    // 弹窗结束
    m_bIsPopuping = false;
}

#endif
