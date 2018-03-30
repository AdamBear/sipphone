#include "xValueHandler.h"
#include "xWindow.h"
#include "xcombobox.h"
#include "gsCallMethod.h"
#include "xWindowManager.h"


//////////////////////////////////////////////////////////////////////////
// class xPresentGroup
bool xPresentGroup::GetPresentByValue(const yl::string& strValue, yl::string& strPresent)
{
    LIST_VALUE_PRESENT::iterator iter = m_listValuePresent.begin();

    for (; iter != m_listValuePresent.end(); ++iter)
    {
        ST_VALUE_PRESENT& stValuePresent = *iter;

        if (stValuePresent.strValue == strValue)
        {
            strPresent = stValuePresent.strPresent;

            return true;
        }
    }

    return false;
}

bool xPresentGroup::GetValueByPresent(const yl::string& strPresent, yl::string& strValue)
{
    LIST_VALUE_PRESENT::iterator iter = m_listValuePresent.begin();

    for (; iter != m_listValuePresent.end(); ++iter)
    {
        ST_VALUE_PRESENT& stValuePresent = *iter;

        if (stValuePresent.strPresent == strPresent)
        {
            strValue = stValuePresent.strValue;

            return true;
        }
    }

    return false;
}

bool xPresentGroup::GetValuePresentByIndex(int nIndex, yl::string& strValue, yl::string& strPresent)
{
    LIST_VALUE_PRESENT::iterator iter = m_listValuePresent.begin();

    for (int i = 0; iter != m_listValuePresent.end(); ++i, ++iter)
    {
        ST_VALUE_PRESENT& stValuePresent = *iter;

        if (i == nIndex)
        {
            strValue = stValuePresent.strValue;
            strPresent = stValuePresent.strPresent;

            return true;
        }
    }

    return false;
}

int xPresentGroup::GetValueIndex(const yl::string& strValue)
{
    LIST_VALUE_PRESENT::iterator iter = m_listValuePresent.begin();

    for (int i = 0; iter != m_listValuePresent.end(); ++i, ++iter)
    {
        ST_VALUE_PRESENT& stValuePresent = *iter;

        if (strValue == stValuePresent.strValue)
        {
            return i;
        }
    }

    return -1;
}

void xPresentGroup::DeleteValue(const yl::string& strValue)
{
    LIST_VALUE_PRESENT::iterator iter = m_listValuePresent.begin();

    for (; iter != m_listValuePresent.end(); ++iter)
    {
        ST_VALUE_PRESENT& stValuePresent = *iter;

        if (stValuePresent.strValue == strValue)
        {
            m_listValuePresent.erase(iter);
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// class xValueHandler
EXPORT_VIEW_HANDLER_CLASS(xValueHandler);
xValueHandler::xValueHandler(int nType/* = VIEW_HANDLE_TYPE_VALUE*/)
    : xViewHandler(nType)
{
    m_pHostView = NULL;
    m_bAsyncUpdate = false;
}

xValueHandler* xValueHandler::GetHandle(xViewHandler* pHandle)
{
    if (NULL != pHandle && VIEW_HANDLE_TYPE_VALUE == (VIEW_HANDLE_TYPE_MARK & pHandle->m_nType))
    {
        return static_cast<xValueHandler*>(pHandle);
    }

    return NULL;
}

bool xValueHandler::setValue(const yl::string& newValue)
{
    if (m_strValue != newValue)
    {
        m_strValue = newValue;
        signalValueChanged.emitSignal(0, 0, 0, 0);
        asyncUpdateValueToViews();
        return true;
    }
    return false;
}

const yl::string& xValueHandler::getValue() const
{
    return m_strValue;
}

//////////////////////////////////////////////////////////////////////////
// class xVEnumHandler

void xValueHandler::loadChildrenElements(xml_node& node)
{
    xml_node nodeData = node.child(XWINDOW_TAG_NAME_DATA);

    if (!nodeData.empty())
    {
        xXmlLoader::loadChildrenElements(node);
        return;
    }

    xml_node nodeGroup = nodeData.child(XWINDOW_TAG_NAME_GROUP);

    while (!nodeGroup.empty())
    {
        yl::string id;

        PopupAndroidString(nodeGroup, XWINDOW_NAME_ID, id);

        xPresentGroup objGroup;

        objGroup.m_strGroupName = id;

        xml_node nodeValue = nodeGroup.first_child();

        while (!nodeValue.empty())
        {
            ST_VALUE_PRESENT stValuePresent;

            stValuePresent.strValue = nodeValue.attribute(XWINDOW_NAME_VALUE).as_string();
            stValuePresent.strPresent = nodeValue.attribute(XWINDOW_NAME_PRESENT).as_string();

            nodeValue = nodeValue.next_sibling();
        }

        m_listPresentGroup.push_back(objGroup);

        nodeGroup = nodeGroup.next_sibling(XWINDOW_TAG_NAME_GROUP);
    }

    fetchPresentViews();

    xXmlLoader::loadChildrenElements(node);
}

void xValueHandler::loadAttributes(xml_node& node)
{
    xXmlLoader::loadAttributes(node);

    xml_attribute attrValue = node.attribute(XWINDOW_NAME_VALUE);

    if (!attrValue.empty())
    {
        PopupAndroidString(node, XWINDOW_NAME_VALUE, m_strValue);

#if IF_VIEW_USER_ACTION
        if (!m_strValue.empty() && m_strValue[0] == '`')
        {
            yl::string strValue = m_strValue.substr(1);
            size_t uSplitPos = strValue.find('`');

            if (yl::string::npos != uSplitPos)
            {
                yl::string strScript = strValue.substr(0, uSplitPos);

                m_strValue = call_text_method(strScript.c_str());
            }
        }
#endif

        asyncUpdateValueToViews();
    }
}

void xValueHandler::fetchPresentViewsToGroup(xView* pView, xPresentGroup& objGroup)
{
    if (pView->m_strId == objGroup.m_strGroupName)
    {
        objGroup.m_listPresentViews.push_back(pView);
    }
    for (xViewIterator itView = pView->xViewTree::child_head(); itView.hasData(); ++itView)
    {
        fetchPresentViewsToGroup(itView.operator ->(), objGroup);
    }
}

void xValueHandler::fetchPresentViews()
{
    chASSERT(m_pHostView != NULL);

    typePresentGroupList::iterator it = m_listPresentGroup.begin();

    for (; it != m_listPresentGroup.end(); ++it)
    {
        fetchPresentViewsToGroup(m_pHostView, *it);
    }
}

void xValueHandler::asyncUpdateValueToViews()
{
    if (!m_bAsyncUpdate)
    {
        m_bAsyncUpdate = true;

        g_WorkStation.postThreadMsgEx(VIEW_EVENT_NONAME, 0, 0,
                                      MSG_METHOD(this, xValueHandler::onUpdateValueToViews));
    }
}

BOOL xValueHandler::onUpdateValueToViews(msgObject& /*objMessage*/)
{
    m_bAsyncUpdate = false;

    if (0 == m_listPresentGroup.size())
    {
        if (NULL != m_pHostView)
        {
            m_pHostView->showValue(m_strValue);
        }
    }
    else
    {
        typePresentGroupList::iterator itGroup = m_listPresentGroup.begin();

        for (; itGroup != m_listPresentGroup.end(); ++itGroup)
        {
            xPresentGroup& objGroup = *itGroup;
            yl::string strPresent;

            if (!objGroup.GetPresentByValue(m_strValue, strPresent))
            {
                return FALSE;
            }

            LIST_VIEW::iterator itView = objGroup.m_listPresentViews.begin();

            for (; itView != objGroup.m_listPresentViews.end(); ++itView)
            {
                xView* pView = *itView;

                pView->showValue(strPresent);
            }
        }
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
EXPORT_VIEW_HANDLER_CLASS(xComboBoxHandler);
xComboBoxHandler::xComboBoxHandler(int nType/* = VALUE_HANDLE_TYPE_COMBO_BOX*/)
    : xValueHandler(nType)
{
}

xComboBoxHandler::~xComboBoxHandler()
{
}

xComboBoxHandler* xComboBoxHandler::GetHandle(xViewHandler* pHandle)
{
    if (NULL != pHandle && VALUE_HANDLE_TYPE_COMBO_BOX == (VALUE_HANDLE_TYPE_MASK & pHandle->m_nType))
    {
        return static_cast<xComboBoxHandler*>(pHandle);
    }

    return NULL;
}

bool xComboBoxHandler::SwitchValue(xComboBox* pComboBox, bool bNext)
{
    if (NULL == pComboBox)
    {
        return false;
    }

    typePresentGroupList::iterator itGroup = m_listPresentGroup.begin();

    for (; itGroup != m_listPresentGroup.end(); ++itGroup)
    {
        xPresentGroup& objGroup = *itGroup;

        if (pComboBox->m_strId != objGroup.m_strGroupName)
        {
            continue;
        }

        if (bNext)
        {
            return SwitchToNextValue(objGroup.m_listValuePresent);
        }
        else
        {
            return SwitchToPreValue(objGroup.m_listValuePresent);
        }
    }

    return false;
}

bool xComboBoxHandler::SwitchToNextValue(LIST_VALUE_PRESENT& listValuePresent)
{
    if (0 == listValuePresent.size())
    {
        return false;
    }

    LIST_VALUE_PRESENT::iterator it = listValuePresent.begin();

    for (; it != listValuePresent.end(); ++it)
    {
        ST_VALUE_PRESENT& stValuePresent = *it;

        if (stValuePresent.strValue == m_strValue)
        {
            break;
        }
    }

    if (it != listValuePresent.end())
    {
        ++it;
    }

    if (it == listValuePresent.end())
    {
        ST_VALUE_PRESENT& stValuePresent = listValuePresent.front();

        setValue(stValuePresent.strValue);
        return true;
    }

    ST_VALUE_PRESENT& stValuePresent = *it;

    setValue(stValuePresent.strValue);
    return true;
}

bool xComboBoxHandler::SwitchToPreValue(LIST_VALUE_PRESENT& listValuePresent)
{
    if (0 == listValuePresent.size())
    {
        return false;
    }

    LIST_VALUE_PRESENT::iterator it = listValuePresent.begin();

    for (; it != listValuePresent.end(); ++it)
    {
        ST_VALUE_PRESENT& stValuePresent = *it;

        if (stValuePresent.strValue == m_strValue)
        {
            break;
        }
    }

    if (it == listValuePresent.begin() || it == listValuePresent.end())
    {
        ST_VALUE_PRESENT& stValuePresent = listValuePresent.back();

        setValue(stValuePresent.strValue);
        return true;
    }

    --it;

    ST_VALUE_PRESENT& stValuePresent = *it;

    setValue(stValuePresent.strValue);
    return true;
}
