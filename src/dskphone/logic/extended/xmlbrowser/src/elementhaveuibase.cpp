#include "logic_inc.h"

#ifdef IF_FEATURE_XMLBROWSER

CElementHaveUIBase::CElementHaveUIBase()
{
    m_iCurrentFocus = -1;
}

// 创建之后的处理
bool CElementHaveUIBase::AfterCreate()
{
    // 取消提示框
    _XMLBrowserManager.ExitMessageBox();
    // 播放提示音
    if (IsNeedPlayBeep())
    {
        _XMLBrowserManager.PlayBeep();
    }

    return true;
}

// Softkey的处理步骤1
bool CElementHaveUIBase::BeginProcessSoftkey(XB_ACTION_TYPE eAction, yl::string strURI,
        int iFocusedItem, bool & bExitWnd, bool & bUpdate)
{
    bool bHandled = false;
    if (eAction == XB_ACTION_ST_FUNCTION)
    {
        // 非SoftKey:xxx类型的功能交给URIProcesser处理
        bHandled = true;
        // 若softkey的URI是服务器的http地址，则发送请求时在http地址后增加selection＝设置的参数
        UpdateURI(strURI, iFocusedItem);

        _URIProcesser.ProcessURI(strURI);
    }
    else if (eAction == XB_ACTION_SOFTKEY)
    {
        // SoftKey:xxx类型的热键处理
        bHandled = Step2ProcessSoftkey(strURI, iFocusedItem, bExitWnd, bUpdate);
    }

    return bHandled;
}

// Softkey的处理步骤2
bool CElementHaveUIBase::Step2ProcessSoftkey(yl::string strURI, int iFocusedItem, bool & bExitWnd,
        bool & bUpdate)
{
    /*
    bool bHandled = false;
    XB_SOFTKEY_TYPE eType = CSoftkeyProcesser::GetTypeByURI(strURI);
    switch (eType)
    {
    case XB_ST_EXIT:
        {
            XMLBROWSER_INFO("Step2ProcessSoftkey XB_ST_EXIT");
            // 退出，统一处理
            bHandled = _ElementManager.DeleteElement(GetID());
        }
        break;
    case XB_ST_MORE:
        {
            XMLBROWSER_INFO("Step2ProcessSoftkey XB_ST_MORE");
        }
        break;
    case XB_ST_FUNCTION:
        {
            _URIProcesser.ProcessURI(strURI);
            bHandled = true;
        }
    case XB_ST_DOT:
    case XB_ST_BACKSPACE:
    case XB_ST_NEXTSPACE:
    case XB_ST_CHANGEMODE:
    case XB_ST_EMPTY:
        {
            // 不处理，交给UI
        }
        break;
    default:
        {
            ProcessSoftkey(eType, iFocusedItem, bUpdate);
        }
        break;
    }

    return bHandled;
    */
    return false;
}

// 对UI事件的处理
bool CElementHaveUIBase::OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd,
                                  bool & bUpdate, bool bDestroyOnExit/* = false*/)
{
    bool bHandled = true;
    switch (eAction)
    {
    case XB_ACTION_SOFTKEY:
    case XB_ACTION_ST_FUNCTION:
        {
            // softkey统一交给SoftkeyProcesser处理
            // XBSoftkeyTreateData *pDD = (XBSoftkeyTreateData *)pData;
            // bHandled = BeginProcessSoftkey(eAction, pDD->_strURI, pDD->_nFocusIndex, bExitWnd, bUpdate);
        }
        break;
    case XB_ACTION_EXIT:
        {
#if IF_BUG_32431
            yl::string strURL = this->GetAutoRefreshURL();
            _XMLBrowserManager.StopAutoRefresh(strURL);
#endif
            bUpdate = _ElementManager.DeleteElement(this->GetID());
        }
        break;
    case XB_ACTION_CANCEL:
        {
            _XMLBrowserManager.Exit();
        }
        break;
    case XB_ACTION_FOCUSCHANGE:
        {
            if (pData != NULL)
            {
                // 默认只改变焦点，不刷新界面
                bUpdate = false;
                // 获得当前焦点，UI提供接口
                m_iCurrentFocus = *(int *)pData;
            }
        }
        break;
    case XB_ACTION_URI:
        {
            if (pData == NULL)
            {
                return false;
            }
            yl::string strURI = (char *)pData;
            bUpdate = bDestroyOnExit;
            GotoURL(strURI, bDestroyOnExit);

            //_URIProcesser.ProcessURI(pURI);
        }
        break;
    case XB_ACTION_HOOK_OFF:
    case XB_ACTION_PRESS_HANDFREE:
        {
            if (!talklogic_SessionExist())
            {
                talklogic_EnterDialUI("");
            }
            else if (talkLogic_IsSessionCallIncoming())
            {
                talklogic_AcceptIncomingCall();
            }

            _XMLBrowserManager.Exit();
            //caijz T4x是否有区别？
            bHandled = true;
        }
        break;
    default:
        bHandled = false;
        break;
    }

    _XMLBrowserManager.StopWav();

    // 其他事件交给各子类处理
    return bHandled;
}

// URI后面添加selection
void CElementHaveUIBase::AddSelectionToURI(yl::string & strURI, const yl::string & strSelection)
{
    if (strSelection.empty())
    {
        // 为空则不处理
        return;
    }

    if (strstr(strURI.c_str(), "?") != NULL)
    {
        strURI += "&selection=";
        strURI += strSelection;
    }
    else
    {
        strURI += "?selection=";
        strURI += strSelection;
    }
}

// 判断并处理合法的URL
void CElementHaveUIBase::ProcessLegalURL(yl::string & strURL, bool bDestroyOnExit/* = false*/)
{
    strURL.trim_both(" \r\n\t"); //兼容开头结尾异常字符
    if (strncmp(strURL.c_str(), "http", 4) == 0
            || strncmp(strURL.c_str(), "https", 5) == 0)
    {
        GotoURL(strURL, bDestroyOnExit);
    }
    else
    {
        XMLBROWSER_WARN("error! not http(s) uri.");
        _XMLBrowserManager.ShowMessageBox(XB_TIP_INVAILID_URI, 3000);
    }
}

bool CElementHaveUIBase::GotoURL(yl::string & strURI, bool bDestroyOnExit /*= false*/)
{
    _URIProcesser.ProcessURI(strURI);

    if ((strncmp(strURI.c_str(), "http", 4) == 0
            || strncmp(strURI.c_str(), "https", 5) == 0)
            && bDestroyOnExit)
    {
        _ElementManager.DeleteElement(this->GetID());
    }

    return true;
}

#endif // #ifdef IF_FEATURE_XMLBROWSER

