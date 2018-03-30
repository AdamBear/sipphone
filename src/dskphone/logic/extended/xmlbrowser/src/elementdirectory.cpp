#include "logic_inc.h"
#include "talk/talklogic/include/modtalklogic.h"


#ifdef IF_FEATURE_XMLBROWSER

CElementDirectory::CElementDirectory()
{
}

CElementDirectory::~CElementDirectory()
{
}

// 创建之后的处理
bool CElementDirectory::AfterCreate()
{
    return CElementHaveUIBase::AfterCreate();
}

// 销毁之前的处理
void CElementDirectory::BeforeDestroy()
{

}

// 是否播放提示音
bool CElementDirectory::IsNeedPlayBeep()
{
    return _GetXData().stAttr.bBeep;
}

// 对UI事件的处理
bool CElementDirectory::OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd,
                                 bool & bUpdate, bool bDestroyOnExit/* = false*/)
{
    bool bHandled = true;
    switch (eAction)
    {
#if IF_FEATURE_XMLDIRECTORY
    case XB_ACTION_SEARCH_PHONEBOOK:
        {
            if (pData != NULL)
            {
                // 提交 submit，提交之后不用更新当前界面，等待跳入新的界面
                bUpdate = false;
                Submit((char *)pData);
            }
        }
        break;
#endif
    case XB_ACTION_CONFIRM:
    case XB_ACTION_DIAL:
    case XB_ACTION_HOOK_OFF:
    case XB_ACTION_PRESS_HANDFREE:
        {
            if (pData == NULL)
            {
                return false;
            }
            if (_GetXData().stAttr.bLockIn
                    && (eAction == XB_ACTION_PRESS_HANDFREE))
            {
                return true;
            }
            int iFocusIndex = *(int *)pData;
            // 获取焦点对应的Item，处理对应的URI
            DirMenuItem * pItem = GetItem(iFocusIndex);
            if (pItem != NULL/* && !pItem->strURI.empty()*/)
            {
                if (!pItem->strURI.empty())
                {
                    //拨号
                    _URIProcesser.ProcessDial(pItem->strURI.c_str());
                    _XMLBrowserManager.Exit();
                }
                else
                {
                    if (eAction == XB_ACTION_HOOK_OFF && _GetXData().stAttr.bLockIn)
                    {
                        return true;
                    }
                    else
                    {
                        // 预拨号
                        talklogic_EnterDialUI("");
                        _XMLBrowserManager.Exit();

                    }
                }
                //声音频道图标是否有更新？T42机型
                return true;
            }
        }
        break;
    case XB_ACTION_CANCEL:
        {
            // 如果cancel值为空，则交给基类处理
            if (_GetXData().stAttr.strCancelAction.empty())
            {
                bHandled = false;
            }
            else
            {
                // cancelAction:执行cancel的URL
                bUpdate = _GetXData().stAttr.bDestroyOnExit;
                ProcessLegalURL(_GetXData().stAttr.strCancelAction, _GetXData().stAttr.bDestroyOnExit);
            }
        }
        break;
    case XB_ACTION_LINEKEY_CALLOUT:
        {
            XDataUI2Logic * pUIData = (XDataUI2Logic *)pData;
            CHECK_NULL(pUIData, false);

            DirMenuItem * pItem = GetItem(pUIData->lActionParam);
            if (pItem == NULL)
            {
                return false;
            }

            int iDsskeyID = pUIData->wActionParam;
            DssKey_Type eDskType = dsskey_GetDsskeyType(iDsskeyID);

            // DSSkey按键类型为Redial
            if (eDskType == DT_LINE)
            {
                if (!pItem->strURI.empty())
                {
                    talklogic_CallOut(pItem->strURI.c_str(), "", dsskey_GetDsskeyLine(iDsskeyID));
                    // 退出
                    _XMLBrowserManager.Exit();
                    return true;
                }
                // 为空不反应
                else
                {
                    return true;
                }
            }
            else
            {
                if (_GetXData().stAttr.bLockIn)
                {
                    return true;
                }
            }
            bHandled = false;
        }
        break;
    case XB_ACTION_PREVIOUS:
        {
            // 调用Previous内的URI命令
            if (!_GetXData().stAttr.strPrevious.empty())
            {
                bUpdate = _GetXData().stAttr.bDestroyOnExit;
                ProcessLegalURL(_GetXData().stAttr.strPrevious, _GetXData().stAttr.bDestroyOnExit);
            }
        }
        break;
    case XB_ACTION_NEXT:
        {
            // 调用Next内的URI命令
            if (!_GetXData().stAttr.strNext.empty())
            {
                bUpdate = _GetXData().stAttr.bDestroyOnExit;
                ProcessLegalURL(_GetXData().stAttr.strNext, _GetXData().stAttr.bDestroyOnExit);
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }

    // 其他事件交给基类处理
    if (!bHandled)
    {
        bHandled = CElementHaveUIBase::OnAction(eAction, pData, bExitWnd, bUpdate,
                                                _GetXData().stAttr.bDestroyOnExit);
    }

    return bHandled;
}

// softkey处理
bool CElementDirectory::ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate)
{
    return false;
}

// 获取焦点Item的数据
DirMenuItem * CElementDirectory::GetItem(int iFocusedItem)
{
    // 找到对应的MenuItem
    List_Dir_Menu_Item & listItem = _GetXData().listItem;
    List_Dir_Menu_Item::iterator it = listItem.begin();
    for (int iIndex = 0; it != listItem.end(); it++, ++iIndex)
    {
        if (iIndex == iFocusedItem)
        {
            return *it;
        }
    }

    return NULL;
}

// 获取指定控件的号码,无号码则返回空
yl::string CElementDirectory::GetItemNumberByIndex(UINT uIndex)
{
#ifdef xww
    List_Dir_Menu_Item::iterator item = m_dataToUI.pListItem->begin();
    for (int i = 0; item != m_dataToUI.pListItem->end(); ++i, item++)
    {
        if (i == uIndex && *item != NULL)
        {
            return (*item)->strURI;
        }
    }
#endif
    return "";
}

#if IF_FEATURE_XMLDIRECTORY
// Submit的处理
bool CElementDirectory::Submit(char * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // 获得要提交的目的URL
    yl::string strURL = _GetXData().stAttr.strUrl;

    // 根据index获得参数名
    yl::string strParam = _GetXData().stInputField.strParam;

    // 获得参数值，可以为空
    yl::string strValue = pData;

    if (!strValue.empty())
    {
        // 在加第一个数据时进行判断URL中是否有"?"
        if (strstr(strURL.c_str(), "?") != NULL)
        {
            // 如果有"?"先加"&"，再添加参数名和参数值
            strURL += "&";
            strURL += strParam;
            strURL += "=";
            strURL += strValue;
        }
        else
        {
            // 如果无"?"先加"?"，再添加参数名和参数值
            strURL += "?";
            strURL += strParam;
            strURL += "=";
            strURL += strValue;
        }
    }

    XMLBROWSER_INFO("CElementDirectory::Submit(%s)", strURL.c_str());
    // 发送相应的URL
    //_URIProcesser.ProcessURI(strURL);
    xmlbrowser_Enter(strURL.c_str(), true);
    return true;
}

bool CElementDirectory::UpdateData(XMLParseData * pParseData, bool & bUpdate)
{
    if (pParseData == NULL)
    {
        return false;
    }

    XBDirectoryData * pData = (XBDirectoryData *)pParseData->pData;
    if (pParseData->eType == XMT_DIRECTORY
            && pData != NULL
            && !pData->stInputField.strParam.empty())
    {
        BeforeDestroy();
        InitData(pParseData);
        bUpdate = true;
        return true;
    }

    return false;
}
#endif

#endif // #ifdef IF_FEATURE_XMLBROWSER

