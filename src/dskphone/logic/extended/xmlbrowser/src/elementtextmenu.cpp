#include "logic_inc.h"

#include "dsskey/include/moddsskey.h"
#include "talk/talklogic/include/modtalklogic.h"


#ifdef IF_FEATURE_XMLBROWSER

CElementTextMenu::CElementTextMenu()
{
}

CElementTextMenu::~CElementTextMenu()
{
    m_ListItem.clear();
}

// 创建之后的处理
bool CElementTextMenu::AfterCreate()
{
    return CElementHaveUIBase::AfterCreate();
}

// 销毁之前的处理
void CElementTextMenu::BeforeDestroy()
{

}

// 是否播放提示音
bool CElementTextMenu::IsNeedPlayBeep()
{
    return _GetXData().stAttr.bBeep;
}

// 根据各Elment的情况更新URI
void CElementTextMenu::UpdateURI(yl::string & strURI, int iFocusedItem)
{
    // 若softkey的URI是服务器的http地址，则发送请求时在http地址后增加selection＝设置的参数
    if (strstr(strURI.c_str(), "http://") != NULL
            || strstr(strURI.c_str(), "https://") != NULL)
    {
        MenuItem * pItem = GetItem(iFocusedItem);
        if (pItem != NULL)
        {
            // 在URI后面增加Selection项
            AddSelectionToURI(strURI, pItem->strSelection);
        }
    }
}

// softkey处理
bool CElementTextMenu::ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate)
{
    bool bHandled = false;
    switch (eType)
    {
    case XB_ST_SELECT:
        {
            bUpdate = false;
            XMLBROWSER_INFO("ProcessSoftkey iFocusedItem(%d)", iFocusedItem);
            // 获取焦点对应的Item，处理对应的URI
            MenuItem * pItem = GetItem(iFocusedItem);
            if (pItem != NULL && !pItem->strURI.empty())
            {
                bHandled = true;
                yl::string strTemp = pItem->strURI;
                UpdateURI(strTemp, iFocusedItem);
                strTemp.trim_both(" \r\n\t"); //兼容开头异常字符
                if (strncmp(strTemp.c_str(), "http", 4) == 0
                        || strncmp(strTemp.c_str(), "https", 5) == 0)
                {
                    //_URIProcesser.ProcessURI(strTemp);
                    bUpdate = _GetXData().stAttr.bDestroyOnExit;
                    GotoURL(strTemp, _GetXData().stAttr.bDestroyOnExit);
                }
                else
                {
                    _XMLBrowserManager.ShowMessageBox(XB_TIP_INVAILID_URI, 2000);
                }
            }
            //bUpdate = false;
        }
        break;
    default:
        break;
    }

    return bHandled;
}

// 对UI事件的处理
bool CElementTextMenu::OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd,
                                bool & bUpdate, bool bDestroyOnExit/* = false*/)
{
    bool bHandled = true;
    switch (eAction)
    {
    case XB_ACTION_CONFIRM:// 处理同softkey select
    case XB_ACTION_SELECT:
        {
            if (pData != NULL)
            {
                // 处理同softkey select
                m_iCurrentFocus = *(int *)pData;
                bHandled = ProcessSoftkey(XB_ST_SELECT, m_iCurrentFocus, bUpdate);
            }
        }
        break;
    case XB_ACTION_HOOK_OFF:
    case XB_ACTION_PRESS_HANDFREE:
    case XB_ACTION_DIAL:
        {
            if (_GetXData().stAttr.bLockIn && eAction == XB_ACTION_PRESS_HANDFREE)
            {
//              voice_OnChannelKeyPress(PHONE_KEY_HANDSET_OFF_HOOK);
                return true;
            }

            if (pData == NULL)
            {
                return false;
            }

            MenuItem * pItem = GetItem(*(int *)pData);
            if (pItem != NULL)
            {
                if (!pItem->strDial.empty())
                {
                    //拨号
                    _URIProcesser.ProcessDial(pItem->strDial.c_str());
                    // 退出
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
                        // 退出
                        _XMLBrowserManager.Exit();

                    }
                }
                // T42是否需要刷新通道图标

                return true;
            }
        }
        break;
    case XB_ACTION_URI:
        {
            ActionSoftkeyData * pSKData = (ActionSoftkeyData *)pData;
            if (pSKData != NULL)
            {
                XMLBROWSER_INFO("CElementTextMenu:: eAction = XB_ACTION_URI, iFocusedItem = %d, strURI = %s",
                                pSKData->iFocusedItem, pSKData->strURI.c_str());

                yl::string strTemp = pSKData->strURI;
                // 若softkey的URI是服务器的http地址，则发送请求时在http地址后增加selection＝设置的参数
                UpdateURI(strTemp, pSKData->iFocusedItem);
                //_URIProcesser.ProcessURI(strTemp);
                bUpdate = _GetXData().stAttr.bDestroyOnExit;
                GotoURL(strTemp, _GetXData().stAttr.bDestroyOnExit);
            }
        }
        break;
    case XB_ACTION_CANCEL:
        {
            // 如果cancel值为空，则交给基类处理
            if (_GetXData().stAttr.strCancelAction.empty())
            {
                bHandled = false;
                break;
            }

            bUpdate = _GetXData().stAttr.bDestroyOnExit;
            // cancelAction:执行cancel的URL
            ProcessLegalURL(_GetXData().stAttr.strCancelAction, _GetXData().stAttr.bDestroyOnExit);
        }
        break;
    case XB_ACTION_LINEKEY_CALLOUT:
        {
            XDataUI2Logic * pUIData = (XDataUI2Logic *)pData;
            CHECK_NULL(pUIData, false);

            MenuItem * pItem = GetItem(pUIData->lActionParam);
            if (pItem == NULL)
            {
                return false;
            }

            int iDsskeyID = pUIData->wActionParam;

            //int iDsskeyID = dsskey_GetDsskeyID(DMT_LINEKEY_MODULE, 0, iKey - PHONE_KEY_LINE1);
            DssKey_Type eDskType = dsskey_GetDsskeyType(iDsskeyID);
            // DSSkey按键类型为line
            if (eDskType == DT_LINE)
            {
                if (!pItem->strDial.empty())
                {
                    talklogic_CallOut(pItem->strDial.c_str(), "", dsskey_GetDsskeyLine(iDsskeyID));
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

// 获取焦点Item
MenuItem * CElementTextMenu::GetItem(int iFocusedItem)
{
    // 找到对应的MenuItem
    List_Menu_Item & listItem = _GetXData().listItem;
    List_Menu_Item::iterator it = listItem.begin();
    for (int iIndex = 0; it != listItem.end(); ++it, ++iIndex)
    {
        if (iIndex == iFocusedItem)
        {
            return (*it);
        }
    }

    return NULL;
}

// 获取指定控件的号码,无号码则返回空
yl::string CElementTextMenu::GetItemNumberByIndex(UINT uIndex)
{
#ifdef xww
    // 遍历Item链表，获取对应的号码
    List_Menu_Item::iterator item = m_dataToUI.pListItem->begin();
    for (int i = 0; item != m_dataToUI.pListItem->end(); ++i, ++item)
    {
        if (i == uIndex && (*item) != NULL)
        {
            return (*item)->strDial;
        }
    }
#endif
    return "";
}

#endif // #ifdef IF_FEATURE_XMLBROWSER

