#include "logic_inc.h"
#include "dsskey/src/dsskeyactionmanager.h"
#include "dsskey/src/dsskeyaction.h"

#ifdef IF_FEATURE_XMLBROWSER

CElementInputScreen::CElementInputScreen()
{
}

CElementInputScreen::~CElementInputScreen()
{
    m_ListInput.clear();
}

// 创建之后的处理
bool CElementInputScreen::AfterCreate()
{
    return CElementHaveUIBase::AfterCreate();
}

// 销毁之前的处理
void CElementInputScreen::BeforeDestroy()
{

}

// 是否播放提示音
bool CElementInputScreen::IsNeedPlayBeep()
{
    return _GetXData().stAttr.bBeep;
}

// 根据各Elment的情况更新URI
void CElementInputScreen::UpdateURI(yl::string & strURI, int iFocusedItem)
{
    // 若softkey的URI是服务器的http地址，则发送请求时在http地址后增加selection＝设置的参数
    if (strstr(strURI.c_str(), "http://") != NULL
            || strstr(strURI.c_str(), "https://") != NULL)
    {
        yl::string strSelection = GetSelectionByFocus(iFocusedItem);
        if (!strSelection.empty())
        {
            // 在URI后面增加Selection项
            AddSelectionToURI(strURI, strSelection);
        }
    }
}

// softkey处理
bool CElementInputScreen::ProcessSoftkey(XB_SOFTKEY_TYPE eType, int iFocusedItem, bool & bUpdate)
{
    bool bHandled = false;
    bUpdate = false;
    /*
        switch (eType)
        {
        case XB_ST_SUBMIT:
            {

            }
            break;
        default:
            break;
        }
    */

    return bHandled;
}

// 对UI事件的处理
bool CElementInputScreen::OnAction(XB_ACTION_TYPE eAction, void * pData, bool & bExitWnd,
                                   bool & bUpdate, bool bDestroyOnExit/* = false*/)
{
    bool bHandled = false;
    switch (eAction)
    {
    case XB_ACTION_FOCUSCHANGE:
        {
        }
        break;
    // 该功能在UI那边处理
    case XB_ACTION_CONFIRM:
        {
            if (pData != NULL)
            {
                // 提交 submit，提交之后不用更新当前界面，等待跳入新的界面
                bUpdate = _GetXData().stAttr.bDestroyOnExit;
                bHandled = Submit((SubmitDataMap *)pData);
            }
        }
        break;
    case XB_ACTION_URI:
        {
            bUpdate = false;
            ActionSoftkeyData * pSKData = (ActionSoftkeyData *)pData;
            if (pSKData != NULL)
            {
                XMLBROWSER_INFO("CElementInputScreen:: eAction = XB_ACTION_URI, iFocusedItem = %d, strURI = %s",
                                pSKData->iFocusedItem, pSKData->strURI.c_str());

                yl::string strTemp = pSKData->strURI;
                // 若softkey的URI是服务器的http地址，则发送请求时在http地址后增加selection＝设置的参数
                UpdateURI(strTemp, pSKData->iFocusedItem);
                //_URIProcesser.ProcessURI(strTemp);
                bUpdate = _GetXData().stAttr.bDestroyOnExit;
                bHandled = GotoURL(strTemp, _GetXData().stAttr.bDestroyOnExit);
            }
        }
        break;
    case XB_ACTION_CANCEL:
        {
            // 如果cancel值不为空，执行URL，为空退出界面
            if (!_GetXData().stAttr.strCancelAction.empty())
            {
                bUpdate = _GetXData().stAttr.bDestroyOnExit;
                // cancelAction:执行cancel的URL
                ProcessLegalURL(_GetXData().stAttr.strCancelAction, _GetXData().stAttr.bDestroyOnExit);
                bHandled = true;
            }
        }
        break;
    case XB_ACTION_LINEKEY_CALLOUT:
        {
            // 上锁时不响应，没上锁进入预拨号
            if (_GetXData().stAttr.bLockIn)
            {
                return true;
            }
            else
            {
                XDataUI2Logic * pUIData = (XDataUI2Logic *) pData;
                CHECK_NULL(pUIData, false);
                if (DT_LINE == dsskey_GetDsskeyType(pUIData->wActionParam))
                {
                    _XMLBrowserManager.Exit();
                    return false;
                }
                //return true;
            }
        }
        break;
    default:
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

// 获得当前焦点所在输入框的类型
INPUT_TYPE CElementInputScreen::GetCurrentType()
{
    // 遍历链表
    List_Input_Field::iterator iter = m_ListInput.begin();
    for (int i = 0; iter != m_ListInput.end(); ++i, ++iter)
    {
        // 焦点符合就返回类型
        if (i == m_iCurrentFocus)
        {
            return (*iter)->eType;
        }
    }

    return INPUT_EMPTY;
}

// Submit的处理
bool CElementInputScreen::Submit(SubmitDataMap * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // 获得要提交的目的URL
    yl::string strURL = _GetXData().stAttr.strURI;
    // 遍历map
    SubmitDataMap::iterator iter = pData->begin();
    for (; iter != pData->end(); ++iter)
    {
        // 获得index
        int iIndex = iter->first;
        // 根据index获得参数名
        yl::string strParam = GetParamByIndex(iIndex);

        // 获得参数值，可以为空
        yl::string strValue = iter->second;

        if (iter == pData->begin())
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
        else
        {
            // 添加参数名和参数值
            strURL += "&";
            strURL += strParam;
            strURL += "=";
            strURL += strValue;
        }
    }

    yl::string strSelection = GetSelectionByFocus(m_iCurrentFocus);
    // 判断selection是否为空
    if (!strSelection.empty())
    {
        // 不空就添加selection及其值
        strURL += "&selection=";
        strURL += strSelection;
    }

    XMLBROWSER_INFO("CElementInputScreen::Submit(%s)", strURL.c_str());
    // 发送相应的URL
    //_URIProcesser.ProcessURI(strURL);
    GotoURL(strURL, _GetXData().stAttr.bDestroyOnExit);
    return true;
}

// 根据index获得参数名Param
yl::string CElementInputScreen::GetParamByIndex(int iIndex)
{
    List_Input_Field & ListInput = _GetXData().listItem;
    List_Input_Field::iterator iter = ListInput.begin();
    // index从0开始
    for (int i = 0; iter != ListInput.end(); ++iter, ++i)
    {
        if (i == iIndex)
        {
            return (*iter)->strParam;
        }
    }

    return "";
}

// 根据当前焦点获得selection
yl::string CElementInputScreen::GetSelectionByFocus(int iIndex)
{
    List_Input_Field & listInput = _GetXData().listItem;
    List_Input_Field::iterator iter = listInput.begin();
    // index从0开始
    for (int i = 0; iter != listInput.end(); ++iter, ++i)
    {
        if (i == iIndex)
        {
            return (*iter)->strSelection;
        }
    }
    return "";
}

#endif // #ifdef IF_FEATURE_XMLBROWSER
