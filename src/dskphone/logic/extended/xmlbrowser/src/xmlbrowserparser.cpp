#include "logic_inc.h"
#include "xmlbrowserparser.h"
#include "globalmodel.h"
#include <devicelib/networkserver.h>
#include "service_system.h"
#include "commonunits/t2xpath.h"
#if IF_FEATURE_START2START_ACD
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "broadsoft/callcenter/include/callcenter_data_define.h"
#endif

#define T2X_ICON_PATH_FWD     "CallForward.dob"
#define T2X_ICON_PATH_DND     "DND.dob"
#define T2X_ICON_PATH_MSG     "SMS.dob"
#define T2X_ICON_PATH_NOTE    ""
#define T2X_ICON_PATH_WAR     ""
#define T4X_ICON_PATH_FWD     "FWD-Icon.png"
#define T4X_ICON_PATH_DND     "DND.png"
#define T4X_ICON_PATH_MSG     "TextMessage.png"
#define T4X_ICON_PATH_NOTE    "note.png"
#define T4X_ICON_PATH_WAR     "warning.png"

#ifdef IF_FEATURE_XMLBROWSER

// 空白符
const char kszWhiteSpace[] = " \t\r\n";

// 删除对象
#define DELETE_XOBJECT(p)  \
{  \
    if ((p) != NULL)  \
    {  \
    delete (p);  \
    (p) = NULL;  \
    }  \
}

//
void ReleaseParseData(XMLParseData * pParseData)
{
    if (pParseData == NULL)
    {
        return;
    }
    // 释放Parse结构
    if (pParseData->pData == NULL)
    {
        delete pParseData;
        return;
    }
    // 根据类型释放
    switch (pParseData->eType)
    {
    case XMT_MENU:
        {
            XBTextMenuData * pData = (XBTextMenuData *)pParseData->pData;
            Delete_Dynamic_List(List_Menu_Item, pData->listItem);
            DELETE_XOBJECT(pData);
        }
        break;
    case XMT_DIRECTORY:
        {
            XBDirectoryData * pData = (XBDirectoryData *)pParseData->pData;
            Delete_Dynamic_List(List_Dir_Menu_Item, pData->listItem);
            DELETE_XOBJECT(pData);
        }
        break;
    case XMT_FORMATTEDTEXTSCREEN:
        {
            XBFormattedTextData * pData = (XBFormattedTextData *)pParseData->pData;
            Delete_Dynamic_List(List_Header_Zone, pData->listHeader);
            Delete_Dynamic_List(List_Scroll_Zone, pData->listScroll);
            Delete_Dynamic_List(List_Footer_Zone, pData->listFooter);
            DELETE_XOBJECT(pData);
        }
        break;
    case XMT_INPUT:
        {
            XBInputData * pData = (XBInputData *)pParseData->pData;
            Delete_Dynamic_List(List_Input_Field, pData->listItem);
            DELETE_XOBJECT(pData);
        }
        break;
    case XMT_IMAGEMENU:
    case XMT_IMAGESCREEN:
        {
            XBImageScreenData * pData = (XBImageScreenData *)pParseData->pData;
            Delete_Dynamic_List(List_Image_Item, pData->listImage);
            Delete_Dynamic_List(List_URI_Item, pData->stURLList.listItem);
            DELETE_XOBJECT(pData);
        }
        break;
    case XMT_TEXTSCREEN:
        {
            // no need list
            XBTextScreenData * pData = (XBTextScreenData *)pParseData->pData;
            DELETE_XOBJECT(pData);
        }
        break;
    case XMT_STATUS:
        {
            ReleaseStatusData((XBStatusData *)pParseData->pData);
        }
        break;
    case XMT_EXECUTE:
        {
            XBExecuteData * pData = (XBExecuteData *)pParseData->pData;
            Delete_Dynamic_List(List_Execute_Item, pData->listItem);
            DELETE_XOBJECT(pData);
        }
        break;
    case XMT_CONFIGURATION:
        {
            XBConfigData * pData = (XBConfigData *)pParseData->pData;
            Delete_Dynamic_List(List_Config_Item, pData->listItem);
            DELETE_XOBJECT(pData);
        }
        break;
#if IF_FEATURE_XMLB_SCREENSAVER
    case XMT_SCREENSAVER:
        {
            XBScreenSaverData * pData = (XBScreenSaverData *)pParseData->pData;
            Delete_Dynamic_List(List_Image_Item, pData->listImage);
            Delete_Dynamic_List(List_Line_Zone, pData->listLine);
            DELETE_XOBJECT(pData);
        }
        break;
#endif
    default:
        DELETE_XOBJECT(pParseData->pData);
        break;
    }
    DELETE_XOBJECT(pParseData);
}
//
void ReleaseStatusData(XBStatusData * pStatusData)
{
    if (pStatusData == NULL)
    {
        return;
    }
    Delete_Dynamic_List(List_Status_Item, pStatusData->listItem);
    DELETE_XOBJECT(pStatusData);
}

//////////////////////////////////////////////////////////////////////////
bool CXMLParseAction::Exec()
{

    m_iErrCode = ERR_OK;
    // 文件不存在则给出错误提示
    if (!pathFileExist(m_strXmlFilePath.c_str()))
    {
        m_iErrCode = ERR_EXIST;
        //_LANG("File do not exist!");
        return false;
    }

    // 加载xml文件
    xml_document docXmlFileXmlBrowser;
    if (!docXmlFileXmlBrowser.load_file(m_strXmlFilePath.c_str()))
    {
        m_iErrCode = ERR_OPEN;
        //_LANG("File load failed!");
        return false;
    }

    // 根据XML文件获取Element类型
    XML_ELEMENT_TYPE eType = XMT_UNKNOWN;
    xml_node xmlTypeData = docXmlFileXmlBrowser.first_child();
    while (!xmlTypeData.empty())
    {
        yl::string strBaseName = xmlTypeData.name();
        eType = GetTypeByName(strBaseName);
        if (eType != XMT_UNKNOWN)
        {
            break;
        }

        xmlTypeData = xmlTypeData.next_sibling();
    }

    // 如果未找到正确的Element类型则提示
    if (eType == XMT_UNKNOWN)
    {
        m_iErrCode = ERR_ROOT_NODE;
        return false;
    }

    // 根据类型解析数据
    return ParseData(eType, xmlTypeData);
}

// 根据名称获取Element类型
XML_ELEMENT_TYPE CXMLParseAction::GetTypeByName(const yl::string & strName)
{
    static const unsigned int uTypeLen = 32;
    static const char scszYealinkPrefix[] = "YealinkIPPhone";

    // partial struct define
    struct xmlElemObjType
    {
        char strUsedType[uTypeLen];
        XML_ELEMENT_TYPE eElemType;
    };
    // map
    static const struct xmlElemObjType s_mapElemType[] =
    {
        {"TextMenu",            XMT_MENU},
        {"FormattedTextScreen", XMT_FORMATTEDTEXTSCREEN},
        {"TextScreen",          XMT_TEXTSCREEN},
        {"InputScreen",         XMT_INPUT},
        {"Directory",           XMT_DIRECTORY},
        {"Execute",             XMT_EXECUTE},
        {"Status",              XMT_STATUS},
        {"Configuration",       XMT_CONFIGURATION},
        {"ImageMenu",           XMT_IMAGEMENU},
        {"ImageScreen",         XMT_IMAGESCREEN},
#ifdef IF_FEATURE_GENBAND
        {"EditingInterface",    XMT_INTERFACE},
#endif
#if IF_FEATURE_XMLB_SCREENSAVER
        {"ScreenSaver", XMT_SCREENSAVER},
#endif
        {"",                    XMT_UNKNOWN},
    };

    // validate prefix
    if (0 != strncmp(strName.c_str(), scszYealinkPrefix, strlen(scszYealinkPrefix)))
    {
        XMLBROWSER_WARN("Warning! Receive other provider object [%s]!", strName.c_str());
        // return XMT_UNKNOWN;
    }

    // locate at the object type name
    // yl::string strObjName = strName.substr(strlen(scszYealinkPrefix));

    for (int i = 0; s_mapElemType[i].eElemType != XMT_UNKNOWN; i++)
    {
        int iBeginPos = strName.length() - strlen(s_mapElemType[i].strUsedType);
        if (iBeginPos < 0)
        {
            continue;
        }
        yl::string strObjName = strName.substr(iBeginPos);
        if (strObjName == s_mapElemType[i].strUsedType)
        {
            return s_mapElemType[i].eElemType;
        }
    }

    return XMT_UNKNOWN;
}

bool CXMLParseAction::ParseData(XML_ELEMENT_TYPE eType, xml_node xmlTypeData)
{
    if (eType == XMT_UNKNOWN || xmlTypeData.empty())
    {
        m_iErrCode = ERR_ROOT_NODE;
        return false;
    }
    // 释放旧数据
    ReleaseData();

    m_pParseData = new XMLParseData();
    CHECK_NULL(m_pParseData, false);

    XB_TIP_TYPE eErrorType = XB_TIP_UNKOWN;
    bool bResult = false;
    // 还有待优化，属性的赋值可统一
    switch (eType)
    {
    case XMT_MENU:
        bResult = ParseTextMenu(xmlTypeData, eErrorType);
        break;
    case XMT_TEXTSCREEN:
        bResult = ParseTextScreen(xmlTypeData, eErrorType);
        break;
    case XMT_INPUT:
        {
            bResult = ParseInputScreen(xmlTypeData, eErrorType);
        }
        break;
    case XMT_DIRECTORY:
        bResult = ParseDirectory(xmlTypeData, eErrorType);
        break;
    case XMT_EXECUTE:
        bResult = ParseExecute(xmlTypeData, eErrorType);
        break;
    case XMT_STATUS:
        bResult = ParseStatus(xmlTypeData, eErrorType);
        break;
    case XMT_CONFIGURATION:
        bResult = ParseConfiguration(xmlTypeData, eErrorType);
        break;
    case XMT_IMAGEMENU:
        bResult = ParseImageMenu(xmlTypeData, eErrorType);
        break;
    case XMT_IMAGESCREEN:
        bResult = ParseImageScreen(xmlTypeData, eErrorType);
        break;
    case XMT_FORMATTEDTEXTSCREEN:
        bResult = ParseFormattedTextScreen(xmlTypeData, eErrorType);
        break;
#ifdef IF_FEATURE_GENBAND
    case XMT_INTERFACE:
        bResult = ParseInterface(xmlTypeData, eErrorType);
        break;
#endif
#if IF_FEATURE_XMLB_SCREENSAVER
    case XMT_SCREENSAVER:
        bResult = ParseScreenSaver(xmlTypeData, eErrorType);
        break;
#endif
    default:
        break;
    }
    // 判断结果
    if (bResult)
    {
        m_pParseData->eType = eType;
    }
    else
    {
        // 出错，释放可能分配的内存
        ReleaseData();
        if (m_iErrCode == ERR_OK)
        {
            m_iErrCode = ERR_UNKNOWN;
        }
    }
    return bResult;
}

// 检查属性,存在就获取,不存在就返回false
bool CXMLParseAction::DetectAttr(xml_node xmlNode, const char * lpszAttr, yl::string & strTemp)
{
    if (xmlNode.empty())
    {
        return false;
    }

    strTemp = xmlNode.attribute(lpszAttr).value();

    if (strTemp.empty())
    {
        return false;
    }

    return true;
}

// 获取节点内容,节点不存在返回false
bool CXMLParseAction::GetItemValue(xml_node xmlFatherNode, const char * lpszItem,
                                   yl::string & strValue)
{
    if (xmlFatherNode.empty())
    {
        return false;
    }

    const char * pValue = xmlFatherNode.child_value(lpszItem);
    if (pValue == NULL)
    {
        return false;
    }

    strValue = pValue;
    return true;
}

#if IF_BUG_32431
bool CXMLParseAction::ParseRefreshAttr(xml_node xmlTypeData)
{
    yl::string strTemp = "";
    if (DetectAttr(xmlTypeData, "refresh", strTemp))
    {
        m_pParseData->sRefreshData.nTimeOut = atoi(strTemp.c_str());
    }

    if (DetectAttr(xmlTypeData, "url", strTemp))
    {
        m_pParseData->sRefreshData.strURL = strTemp;
    }

    return (m_pParseData->sRefreshData.nTimeOut > 0 && !m_pParseData->sRefreshData.strURL.empty());
}
#endif

bool CXMLParseAction::ParseBeep(xml_node xmlTypeData, bool & bBeep)
{
    yl::string strTemp = "";
    if (DetectAttr(xmlTypeData, "Beep", strTemp))
    {
        bBeep = (strcmp(strTemp.c_str(), "no"));
    }
    else
    {
        bBeep = configParser_GetConfigInt(kszXMLBrowserBeep) == 1;
    }
    return true;
}

bool CXMLParseAction::ParseTextMenu(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    if (xmlTypeData.empty())
    {
        m_iErrCode = ERR_ROOT_NODE;
        return false;
    }

    CHECK_NULL(m_pParseData, false);

    XBTextMenuData * pMenuData = new XBTextMenuData;
    if (pMenuData == NULL)
    {
        m_iErrCode = ERR_MEM;
        return false;
    }
    if (m_pParseData->pData != NULL)
    {
        XMLBROWSER_INFO("data already exist !");
        m_iErrCode = ERR_MEM;
        delete pMenuData;
        pMenuData = NULL;
        return false;
    }
    m_pParseData->pData = pMenuData;

    // init error status
    m_iErrCode = ERR_OK;

    // 保存属性
    TextMenuAttr  & dataAttr = pMenuData->stAttr;
    // 临时变量
    yl::string strTemp = "";

    // 保存defaultIndex
    if (DetectAttr(xmlTypeData, "defaultIndex", strTemp))
    {
        // 配置文件中index从1开始，代码中index从0开始
        dataAttr.iDefaultIndex = atoi(strTemp.c_str()) - 1;
    }

    // 保存style
    if (DetectAttr(xmlTypeData, "style", strTemp))
    {
        dataAttr.eStyle = GetMenuStyle(strTemp);
    }

    ParseBeep(xmlTypeData, dataAttr.bBeep);

    // 保存cancelAction
    if (DetectAttr(xmlTypeData, "cancelAction", strTemp))
    {
        dataAttr.strCancelAction = strTemp;
    }

    // 保存wrapList
    if (DetectAttr(xmlTypeData, "wrapList", strTemp))
    {
        dataAttr.bWrapList = (strcmp(strTemp.c_str(), "no"));
    }

    // 保存Timeout
    if (DetectAttr(xmlTypeData, "Timeout", strTemp))
    {
        dataAttr.iTimeOut = atoi(strTemp.c_str());

        // 如果时间小于等于0，就设为默认值
        if (dataAttr.iTimeOut < 0)
        {
            dataAttr.iTimeOut = DEFAULT_TIMEOUT;
        }
    }

    // 保存LockIn
    if (DetectAttr(xmlTypeData, "LockIn", strTemp))
    {
        dataAttr.bLockIn = (!strcmp(strTemp.c_str(), "yes"));
    }

    // 保存destroyOnExit
    if (DetectAttr(xmlTypeData, "destroyOnExit", strTemp))
    {
        dataAttr.bDestroyOnExit = (!strcmp(strTemp.c_str(), "yes"));
        XMLBROWSER_INFO("dataAttr.bDestroyOnExit %d strTemp:%s.", dataAttr.bDestroyOnExit, strTemp.c_str());
    }

#if IF_BUG_32431
    ParseRefreshAttr(xmlTypeData);
#endif

    // 查找Title结点
    xml_node xmlTitleNode = xmlTypeData.child("Title");
    if (!xmlTitleNode.empty())
    {
        // 保存wraptitle
        if (DetectAttr(xmlTitleNode, "wrap", strTemp))
        {
            dataAttr.bWrapTitle = (strcmp(strTemp.c_str(), "no"));
        }

        // 保存Title内容
        dataAttr.strTitle = xmlTitleNode.child_value();
        if (dataAttr.strTitle.length() > MAX_TITLE_CHARS)
        {
            dataAttr.strTitle = dataAttr.strTitle.substr(0, MAX_TITLE_CHARS);
        }
    }

    // 保存ItemList
    List_Menu_Item & refListItem = pMenuData->listItem;

    if (!xmlTypeData.empty())
    {
        yl::string strValue = "";

        xml_node xmlItemNode = xmlTypeData.child("MenuItem");
        while (!xmlItemNode.empty())
        {
            // 检查节点是否正确
            if (strcmp("MenuItem", xmlItemNode.name()) != 0)
            {
                xmlItemNode = xmlItemNode.next_sibling();
                continue;
            }

            // 对Prompt要进行合法性判断
            // Prompt不存在或内容为空即为不合法
            if (!GetItemValue(xmlItemNode, "Prompt", strValue) || strValue.empty())
            {
                xmlItemNode = xmlItemNode.next_sibling();
                continue;
            }

            MenuItem * pMenuItem = new MenuItem();
            // 保存Prompt
            pMenuItem->strPrompt = strValue;

            // 保存URI
            if (GetItemValue(xmlItemNode, "URI", strValue))
            {
                pMenuItem->strURI = strValue;
            }

            // 保存Dial
            if (GetItemValue(xmlItemNode, "Dial", strValue))
            {
                pMenuItem->strDial = strValue;
            }

            // 保存Selection
            if (GetItemValue(xmlItemNode, "Selection", strValue))
            {
                pMenuItem->strSelection = strValue;
            }

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
            ParseSoftkey(xmlItemNode, pMenuItem->vecSoftkey);
#endif

            // 把合法的MenuItem加到菜单项链表中
            refListItem.push_back(pMenuItem);
            xmlItemNode = xmlItemNode.next_sibling();
        }
    }

    // MenuItem个数检查
    int iItemNum = refListItem.size();
    if (iItemNum < 1)
    {
        m_iErrCode = ERR_ROOT_NODE;
    }
    else if (iItemNum > MAX_MENU_ITEM)
    {
        m_iErrCode = ERR_ITEM_LIMIT;
    }
    // 解析
    ParseSoftkey(xmlTypeData, pMenuData->vecSoftkey);

    // 如果默认焦点超过菜单项个数，将焦点置为0
    if (dataAttr.iDefaultIndex > iItemNum - 1)
    {
        dataAttr.iDefaultIndex = 0;
    }

    return (m_iErrCode == ERR_OK);
}

bool CXMLParseAction::ParseTextScreen(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    if (xmlTypeData.empty())
    {
        m_iErrCode = ERR_ROOT_NODE;
        return false;
    }
    // 指针判断
    CHECK_NULL(m_pParseData, false);

    XBTextScreenData * pData = new XBTextScreenData;
    CHECK_NULL(pData, false);
    m_pParseData->pData = pData;

    // 保存属性
    TextScreenAttr  &  dataAttr = pData->stAttr;
    // 临时变量
    yl::string strTemp = "";

    ParseBeep(xmlTypeData, dataAttr.bBeep);

    // 保存cancelAction
    if (DetectAttr(xmlTypeData, "cancelAction", strTemp))
    {
        dataAttr.strCancelAction = strTemp;
    }

    // 保存doneAction
    if (DetectAttr(xmlTypeData, "doneAction", strTemp))
    {
        dataAttr.strDoneAction = strTemp;
    }

    // 保存wrapList
    if (DetectAttr(xmlTypeData, "wrapList", strTemp))
    {
        dataAttr.bWrapList = (strcmp(strTemp.c_str(), "no"));
    }

    // 保存Timeout
    if (DetectAttr(xmlTypeData, "Timeout", strTemp))
    {
        dataAttr.iTimeOut = atoi(strTemp.c_str());

        // 如果时间小于等于0，就设为默认值
        if (dataAttr.iTimeOut < 0)
        {
            dataAttr.iTimeOut = DEFAULT_TIMEOUT;
        }
    }

    // 保存LockIn
    if (DetectAttr(xmlTypeData, "LockIn", strTemp))
    {
        dataAttr.bLockIn = (!strcmp(strTemp.c_str(), "yes"));
    }

    // 保存destroyOnExit
    if (DetectAttr(xmlTypeData, "destroyOnExit", strTemp))
    {
        dataAttr.bDestroyOnExit = (!strcmp(strTemp.c_str(), "yes"));
    }

#if IF_BUG_32431
    ParseRefreshAttr(xmlTypeData);
#endif

    // 查找Title结点
    xml_node xmlTitleNode = xmlTypeData.child("Title");
    if (!xmlTitleNode.empty())
    {
        // 保存wraptitle
        if (DetectAttr(xmlTitleNode, "wrap", strTemp))
        {
            dataAttr.bWrapTitle = (strcmp(strTemp.c_str(), "no"));
        }

        // 保存Title内容
        dataAttr.strTitle = xmlTitleNode.child_value();
    }

    // text的内容
    yl::string & strText = pData->strText;

    chString strValue;
    // 找到Text结点
    xml_node xmlTextNode = xmlTypeData.child("Text");
    if (xmlTextNode.empty())
    {
        m_iErrCode = ERR_NODE;
        return false;
    }

    // 检测Text长度是否合法
    strText = xmlTextNode.child_value();
    if (strText.empty())
    {
        m_iErrCode = ERR_STR_LIMIT;
        return false;
    }
    else if (strText.length() > MAX_TEXT_LENGTH)
    {
        m_iErrCode = ERR_ITEM_LIMIT;
        return false;
    }

    ParseSoftkey(xmlTypeData, pData->vecSoftkey);

    return true;
}

bool CXMLParseAction::ParseInputScreen(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    if (xmlTypeData.empty())
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }

    CHECK_NULL(m_pParseData, false);
    XBInputData * pData = new XBInputData;

    m_pParseData->pData = pData;

    // 保存属性
    InputAttr  &  dataAttr = pData->stAttr;
    // 临时变量
    yl::string strTemp = "";

    // 保存defaultIndex
    if (DetectAttr(xmlTypeData, "defaultIndex", strTemp))
    {
        dataAttr.iDefaultIndex = atoi(strTemp.c_str()) - 1;
    }

    // 保存type
    if (DetectAttr(xmlTypeData, "type", strTemp))
    {
        dataAttr.eType = GetInputType(strTemp);
    }

    ParseBeep(xmlTypeData, dataAttr.bBeep);

    // 保存cancelAction
    if (DetectAttr(xmlTypeData, "cancelAction", strTemp))
    {
        dataAttr.strCancelAction = strTemp;
    }

    // 保存Password
    if (DetectAttr(xmlTypeData, "Password", strTemp))
    {
        dataAttr.bPassword = (!strcmp(strTemp.c_str(), "yes"));
    }

    // 保存Timeout
    if (DetectAttr(xmlTypeData, "Timeout", strTemp))
    {
        dataAttr.iTimeOut = atoi(strTemp.c_str());

        // 如果时间小于等于0，就设为默认值
        if (dataAttr.iTimeOut < 0)
        {
            dataAttr.iTimeOut = DEFAULT_TIMEOUT;
        }
    }

    // 保存LockIn
    if (DetectAttr(xmlTypeData, "LockIn", strTemp))
    {
        dataAttr.bLockIn = (!strcmp(strTemp.c_str(), "yes"));
    }

    // 保存inputLanguage
    if (DetectAttr(xmlTypeData, "inputLanguage", strTemp))
    {
        dataAttr.eLanguage = GetInputLanguage(strTemp);
    }

    // 保存displayMode
    if (DetectAttr(xmlTypeData, "displayMode", strTemp))
    {
        dataAttr.eDisplayMode = GetInputDisplayMode(strTemp);
    }

    // 保存destroyOnExit
    if (DetectAttr(xmlTypeData, "destroyOnExit", strTemp))
    {
        dataAttr.bDestroyOnExit = (!strcmp(strTemp.c_str(), "yes"));
    }

#if IF_BUG_32431
    ParseRefreshAttr(xmlTypeData);
#endif

    // 查找Title结点
    xml_node xmlTitleNode = xmlTypeData.child("Title");
    if (!xmlTitleNode.empty())
    {
        // 保存wraptitle
        if (DetectAttr(xmlTitleNode, "wrap", strTemp))
        {
            dataAttr.bWrapTitle = (strcmp(strTemp.c_str(), "no"));
        }

        // 保存Title内容
        dataAttr.strTitle = xmlTitleNode.child_value();
    }

    // 保存ItemList
    List_Input_Field & ListInput = pData->listItem;
    // 临时变量
    yl::string strValue = "";

    // 保存URL,必须存在
    xml_node xmlURLNode = xmlTypeData.child("URL");
    if (xmlURLNode.empty())
    {
        eErrorType = XB_TIP_PARSE_FAIL;
    }
    else
    {
        strValue = xmlURLNode.child_value();
        if (strValue.empty())
        {
            eErrorType = XB_TIP_NO_SUBMIT_URI;
        }

        dataAttr.strURI = commonAPI_TrimString(strValue, kszWhiteSpace);

//      m_strURL = strValue;
    }

    if (eErrorType != XB_TIP_UNKOWN)
    {
        return false;
    }

    // 向链表中存入数据
    // 找到InputField结点
    xml_node xmlItemNode = xmlTypeData.child("InputField");
    if (xmlItemNode.empty())
    {
        // http://192.168.1.99/Bug.php?BugID=17578
        // compatible single field
        InputField * pInputField = new InputField();
        if (ParserInputField(xmlTypeData, pInputField))
        {
            pInputField->bPassword = dataAttr.bPassword;
            pInputField->eType = dataAttr.eType;

            // 把合法的数据加到链表中
            ListInput.push_back(pInputField);
        }
        else
        {
            delete pInputField;
            pInputField = NULL;
        }
    }
    else
    {
        for (; !xmlItemNode.empty(); xmlItemNode = xmlItemNode.next_sibling("InputField"))
        {
            InputField * pInputField = new InputField();

            // 保存type
            if (DetectAttr(xmlItemNode, "type", strValue))
            {
                pInputField->eType = GetInputType(strValue);
            }
            else
            {
                // 值为空或者未配置该项就使用页面的默认值
                pInputField->eType = dataAttr.eType;
            }

            // 保存password
            if (DetectAttr(xmlItemNode, "password", strValue))
            {
                // 不为yes也不为no就使用页面的默认值
                if (strcmp(strValue.c_str(), "yes") && strcmp(strValue.c_str(), "no"))
                {
                    pInputField->bPassword = dataAttr.bPassword;
                }
                else
                {
                    pInputField->bPassword = (!strcmp(strValue.c_str(), "yes"));
                }
            }
            else
            {
                // 值为空或者未配置该项就使用页面的默认值
                pInputField->bPassword = dataAttr.bPassword;
            }

            // 保存editable
            if (DetectAttr(xmlItemNode, "editable", strValue))
            {
                pInputField->bEditable = (strcmp(strValue.c_str(), "no"));
            }

            if (!ParserInputField(xmlItemNode, pInputField))
            {
                delete pInputField;
                pInputField = NULL;
                continue;
            }
            // 把合法的数据加到链表中
            ListInput.push_back(pInputField);
        }
    }

    // Input Screen个数检查
    int iItemNum = ListInput.size();
    if (iItemNum < 1)
    {
        eErrorType = XB_TIP_PARSE_FAIL;
    }
    else if (iItemNum > MAX_INPUT_NUM)
    {
        eErrorType = XB_TIP_ITEMS_OUT_OF_LIMIT;
    }
    if (eErrorType != XB_TIP_UNKOWN)
    {
        return false;
    }

    // 判断是否全部为空
    int iIndex = 0;
    // 得到第一个不为empty的输入框的index
    if (!GetFirstUnEmptyIndex(iIndex, ListInput))
    {
        eErrorType = XB_TIP_PARSE_FAIL;
    }
    if (eErrorType != XB_TIP_UNKOWN)
    {
        return false;
    }

    // 如果默认焦点超过菜单项个数，将焦点置为第一个非空的输入框焦点
    if (dataAttr.iDefaultIndex > iItemNum - 1)
    {
        dataAttr.iDefaultIndex = iIndex;
    }

    // 解析
    ParseSoftkey(xmlTypeData, pData->vecSoftkey);

    return true;
}

bool CXMLParseAction::ParseDirectory(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    if (xmlTypeData.empty())
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }

    CHECK_NULL(m_pParseData, false);
    XBDirectoryData * pData = new XBDirectoryData;
    m_pParseData->pData = pData;

    // 保存属性
    DirectoryAttr  & dataAttr = pData->stAttr;
    // 临时变量
    yl::string strTemp = "";

    // 保存next
    if (DetectAttr(xmlTypeData, "next", strTemp))
    {
        dataAttr.strNext = strTemp;
    }

    // 保存previous
    if (DetectAttr(xmlTypeData, "previous", strTemp))
    {
        dataAttr.strPrevious = strTemp;
    }

    // 保存defaultIndex
    if (DetectAttr(xmlTypeData, "defaultIndex", strTemp))
    {
        dataAttr.iDefaultIndex = atoi(strTemp.c_str()) - 1;
    }

    ParseBeep(xmlTypeData, dataAttr.bBeep);

    // 保存cancelAction
    if (DetectAttr(xmlTypeData, "cancelAction", strTemp))
    {
        dataAttr.strCancelAction = strTemp;
    }

    // 保存Timeout
    if (DetectAttr(xmlTypeData, "Timeout", strTemp))
    {
        dataAttr.iTimeOut = atoi(strTemp.c_str());

        // 如果时间小于等于0，就设为默认值
        if (dataAttr.iTimeOut < 0)
        {
            dataAttr.iTimeOut = DEFAULT_TIMEOUT;
        }
    }

    // 保存LockIn
    if (DetectAttr(xmlTypeData, "LockIn", strTemp))
    {
        dataAttr.bLockIn = (!strcmp(strTemp.c_str(), "yes"));
    }

    // 保存destroyOnExit
    if (DetectAttr(xmlTypeData, "destroyOnExit", strTemp))
    {
        dataAttr.bDestroyOnExit = (!strcmp(strTemp.c_str(), "yes"));
    }

#if IF_BUG_32431
    ParseRefreshAttr(xmlTypeData);
#endif

    // 查找Title结点
    xml_node xmlTitleNode = xmlTypeData.child("Title");
    if (!xmlTitleNode.empty())
    {
        // 保存wraptitle
        if (DetectAttr(xmlTitleNode, "wrap", strTemp))
        {
            dataAttr.bWrapTitle = (strcmp(strTemp.c_str(), "no"));
        }

        // 保存Title内容
        dataAttr.strTitle = xmlTitleNode.child_value();
    }

#if IF_FEATURE_XMLDIRECTORY
    // 查找URL结点
    xml_node xmlUrlNode = xmlTypeData.child("URL");
    if (!xmlUrlNode.empty())
    {
        // 保存Url内容
        dataAttr.strUrl = xmlUrlNode.child_value();
    }

    // 查找InputField结点
    xml_node xmlInputNode = xmlTypeData.child("InputField");
    if (!xmlInputNode.empty())
    {
        // 保存type
        if (DetectAttr(xmlInputNode, "type", strTemp))
        {
            pData->stInputField.eType = GetInputType(strTemp);
        }

        if (GetItemValue(xmlInputNode, "Prompt", strTemp))
        {
            pData->stInputField.strPrompt = strTemp;
        }

        if (GetItemValue(xmlInputNode, "Parameter", strTemp))
        {
            pData->stInputField.strParam = strTemp;
        }

        if (GetItemValue(xmlInputNode, "preKey", strTemp))
        {
            pData->stInputField.strDefault = strTemp;
        }

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
        ParseSoftkey(xmlInputNode, pData->stInputField.vecSoftkey);
#endif
    }
#endif

    // 保存ItemList
    List_Dir_Menu_Item & ListItem = pData->listItem;
    yl::string strValue = "";
    xml_node xmlItemNode = xmlTypeData.child("MenuItem");
    for (; xmlItemNode != NULL; xmlItemNode = xmlItemNode.next_sibling("MenuItem"))
    {
        // 对Prompt要进行合法性判断
        // Prompt不存在或内容为空即为不合法
        if (!GetItemValue(xmlItemNode, "Prompt", strValue) || strValue.empty())
        {
            continue;
        }

        DirMenuItem * pDirMenuItem = new DirMenuItem();
        // 保存Prompt
        pDirMenuItem->strPrompt = strValue;

        // 保存URI
        if (GetItemValue(xmlItemNode, "URI", strValue))
        {
            pDirMenuItem->strURI = strValue;
        }

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
        ParseSoftkey(xmlItemNode, pDirMenuItem->vecSoftkey);
#endif
        // 把合法的DirMenuItem加到菜单项链表中
        ListItem.push_back(pDirMenuItem);
    }

    // DirMenuItem个数检查
    int iItemNum = ListItem.size();
#if IF_FEATURE_XMLDIRECTORY
    if (pData->stInputField.strParam.empty())
#endif
    {
        if (iItemNum < 1)
        {
            eErrorType = XB_TIP_PARSE_FAIL;
            return false;
        }
        else if (iItemNum > MAX_DIRECTORY_NUM)
        {
            eErrorType = XB_TIP_ITEMS_OUT_OF_LIMIT;
            return false;
        }
    }

    // 如果默认焦点超过菜单项个数，将焦点置为0
    if (dataAttr.iDefaultIndex > iItemNum - 1)
    {
        dataAttr.iDefaultIndex = 0;
    }

    ParseSoftkey(xmlTypeData, pData->vecSoftkey);

    return true;
}

bool CXMLParseAction::ParseExecute(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    if (xmlTypeData.empty())
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }

    CHECK_NULL(m_pParseData, false);
    XBExecuteData * pData = new XBExecuteData;
    CHECK_NULL(pData, false);
    m_pParseData->pData = pData;

    // 保存属性
    ExecuteAttr & dataAttr = pData->stAttr;
    // 临时变量
    yl::string strTemp = "";

    ParseBeep(xmlTypeData, dataAttr.bBeep);

#if IF_BUG_32431
    ParseRefreshAttr(xmlTypeData);
#endif

    // 保存ItemList
    List_Execute_Item & ListItem = pData->listItem;
    yl::string strValue = "";

    xml_node xmlItemNode = xmlTypeData.child("ExecuteItem");
    while (!xmlItemNode.empty())
    {
        // 检查节点是否正确
        if (strcmp("ExecuteItem", xmlItemNode.name()) != 0)
        {
            xmlItemNode = xmlItemNode.next_sibling();
            continue;
        }

        ExecuteItem * pExecuteItem = new ExecuteItem();

        // 保存URI
        if (DetectAttr(xmlItemNode, "URI", strValue))
        {
            pExecuteItem->strURI = strValue;
        }

        // 把合法的ExecuteItem加到菜单项链表中
        ListItem.push_back(pExecuteItem);
        xmlItemNode = xmlItemNode.next_sibling();
    }

    // ExecuteItem个数检查
    if (ListItem.size() > MAX_EXECUTE_ITEM)
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }

    return true;
}

bool CXMLParseAction::ParseStatus(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    if (xmlTypeData.empty())
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }
    CHECK_NULL(m_pParseData, false);
    XBStatusData * pData = new XBStatusData;
    CHECK_NULL(pData, false);
    m_pParseData->pData = pData;

    // 保存属性
    StatusAttr & dataAttr = pData->stAttr;
    // 临时变量
    yl::string strTemp = "";

    ParseBeep(xmlTypeData, dataAttr.bBeep);

    // 保存wraplist
    if (DetectAttr(xmlTypeData, "wrapList", strTemp))
    {
        dataAttr.bWrapList = (strTemp != "no");
    }
    else
    {
        dataAttr.bWrapList = true;
    }

    // 保存Timeout
    if (DetectAttr(xmlTypeData, "Timeout", strTemp))
    {
        dataAttr.iTimeOut = atoi(strTemp.c_str());

        // 如果时间非法，就设为默认值
        // 字符串为非数字时解析出来是0，就设为默认值
        if (dataAttr.iTimeOut < 0 || dataAttr.iTimeOut > 3600
                || (dataAttr.iTimeOut == 0 && strTemp != "0"))
        {
            dataAttr.iTimeOut = 30;
        }
    }
    else
    {
        dataAttr.iTimeOut = 30;
    }

    // 保存SessionID
    if (DetectAttr(xmlTypeData, "SessionID", strTemp))
    {
        dataAttr.strSessionID = strTemp;
    }
    else
    {
        // 如果取不到SessionID，则设为空
        dataAttr.strSessionID = "";
    }

#if IF_FEATURE_START2START_ACD
    // ACD状态
    if (DetectAttr(xmlTypeData, "CallCenterStatus", strTemp))
    {
        int iType = atoi(strTemp.c_str());

        if ((ACD_STATUS)iType >= AS_IDLE && (ACD_STATUS)iType < AS_WRAPUP)
        {
            //ACD_SetAcdStatus中有刷新linekey，但Qt不允许异步线程绘图
            etl_NotifyApp(false,
                          ACD_MESSAGE_S2S_STATUS_CHANGE, 0, (ACD_STATUS)iType);

//          ACD_SetAcdStatus(  );

            ACD_SetSessionID(dataAttr.strSessionID);
        }

        if (DetectAttr(xmlTypeData, "RefreshTimeout", strTemp))
        {
            long iTimer = atol(strTemp.c_str());

            CallCenter_SetAutoRefreshTime(iTimer);
        }
    }
#endif

#if IF_BUG_32431
    ParseRefreshAttr(xmlTypeData);
#endif

    // 保存ItemList
    List_Status_Item & ListItem = pData->listItem;

    // 临时变量
    yl::string strValue = "";

    xml_node xmlItemNode = xmlTypeData.first_child();
    while (!xmlItemNode.empty())
    {
        STATUS_TYPE eType;
        // 检查节点是否正确
        if (strcmp("Message", xmlItemNode.name()) == 0)
        {
            eType = ST_MESSAGE_NORMAL;
        }
        else if (strcmp("MessageInDsskey", xmlItemNode.name()) == 0)
        {
            eType = ST_MESSAGE_DSSKEY;
        }
        else
        {
            xmlItemNode = xmlItemNode.next_sibling();
            continue;
        }

        // Message内容为空，表示要清空相应SessionID的信息
        strValue = xmlItemNode.child_value();

        StatusItem * pStatusItem = new StatusItem();

        // 保存Message
        pStatusItem->strMsg = TrimString(strValue);

        // 保存SessionID
        pStatusItem->strSessionID = dataAttr.strSessionID;

        pStatusItem->eType = eType;

        if (pStatusItem->eType == ST_MESSAGE_DSSKEY)
        {
#if IF_BUG_31595
            // 解析Dsskey数据
            if (strValue.empty())
            {
                ParseDssKeyData(xmlItemNode, pStatusItem);
            }
#endif
            ListItem.push_back(pStatusItem);
            xmlItemNode = xmlItemNode.next_sibling();
            continue;
        }

        // 保存Size
        if (DetectAttr(xmlItemNode, "Size", strValue))
        {
            pStatusItem->eSize = GetLineSize(strValue);
        }

        // 保存Align
        if (DetectAttr(xmlItemNode, "Align", strValue))
        {
            pStatusItem->iAlign = GetLineAlign(strValue);
        }

        // 保存Color
        if (DetectAttr(xmlItemNode, "Color", strValue))
        {
            pStatusItem->eColor = GetLineColor(strValue);
        }

        // 保存账号
        if (DetectAttr(xmlItemNode, "Account", strValue))
        {
            pStatusItem->strAccount = strValue;
        }

        // 保存IconPath
        if (DetectAttr(xmlItemNode, "Icon", strValue))
        {
            // 如果消息内容为空，则表示不显示该条信息，同时也不能显示图标
            if (!pStatusItem->strMsg.empty())
            {
                pStatusItem->strIconPath = GetIconPath(strValue);
            }
        }

        // 如果message为空则不加到链表中
        if (!pStatusItem->strMsg.empty())
        {
            // 追加
            ListItem.push_back(pStatusItem);
        }
        else
        {
            delete pStatusItem;
            pStatusItem = NULL;
        }

        // 只显示前面的n条记录
        if (ListItem.size() >= MAX_STATUS_NUM)
        {
            break;
        }

        xmlItemNode = xmlItemNode.next_sibling();
    }

    // Status个数检查
    int iStatusNum = ListItem.size();
    if (iStatusNum > MAX_STATUS_NUM)
    {
        eErrorType = XB_TIP_ITEMS_OUT_OF_LIMIT;
        return false;
    }

    return true;
}

bool CXMLParseAction::ParseConfiguration(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    if (xmlTypeData.empty())
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }

    CHECK_NULL(m_pParseData, false);
    XBConfigData * pData = new XBConfigData;
    CHECK_NULL(pData, false);
    m_pParseData->pData = pData;

#if IF_BUG_32431
    ParseRefreshAttr(xmlTypeData);
#endif

    yl::string strFirstChild = xmlTypeData.first_child().name();
    if (strFirstChild == "Item")
    {
        ParseConfigV7(pData, xmlTypeData, eErrorType);
        pData->eCfgModel = XCM_M7;
    }
    else if (strFirstChild == "ConfigurationItem")
    {
        // T2X V71 之前配置模式
        ParseConfigV1(pData, xmlTypeData, eErrorType);
        pData->eCfgModel = XCM_M1;
    }
    else if (strFirstChild == "Path")
    {
        // T3X VPX V71 之前的配置模式
        ParseConfigV2(pData, xmlTypeData, eErrorType);
        // M2 会先转为M7
        pData->eCfgModel = XCM_M7;
    }
    else
    {
        return false;
    }

    // ConfigItem个数检查
    if (pData->listItem.size() > MAX_CONFIG_ITEM)
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }
    return true;
}

bool CXMLParseAction::ParseImageMenu(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    // 数据结构共用
    return ParseImageScreen(xmlTypeData, eErrorType);
}

bool CXMLParseAction::ParseImageScreen(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    if (xmlTypeData.empty())
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }

    CHECK_NULL(m_pParseData, false);
    XBImageScreenData * pData = new XBImageScreenData;
    m_pParseData->pData = pData;

    // 保存属性
    ImageAttr  & dataAttr = pData->stAttr;
    // 临时变量
    yl::string strTemp = "";
    // 保存cancelAction
    if (DetectAttr(xmlTypeData, "cancelAction", strTemp))
    {
        dataAttr.strCancelAction = strTemp;
    }

    // 保存doneAction
    if (DetectAttr(xmlTypeData, "doneAction", strTemp))
    {
        dataAttr.strDoneAction = strTemp;
    }

    ParseBeep(xmlTypeData, dataAttr.bBeep);

    // 保存Timeout
    if (DetectAttr(xmlTypeData, "Timeout", strTemp))
    {
        dataAttr.iTimeOut = atoi(strTemp.c_str());

        // 如果时间小于等于0，就设为默认值
        if (dataAttr.iTimeOut < 0)
        {
            dataAttr.iTimeOut = DEFAULT_TIMEOUT;
        }
    }

    // 保存LockIn
    if (DetectAttr(xmlTypeData, "LockIn", strTemp))
    {
        dataAttr.bLockIn = (strTemp == "yes");
    }

    // 保存mode
    if (DetectAttr(xmlTypeData, "mode", strTemp))
    {
        dataAttr.eMode = GetImageMode(strTemp);
    }

    // 保存destroyOnExit
    if (DetectAttr(xmlTypeData, "destroyOnExit", strTemp))
    {
        dataAttr.bDestroyOnExit = (!strcmp(strTemp.c_str(), "yes"));
    }

#if IF_BUG_32431
    ParseRefreshAttr(xmlTypeData);
#endif

    xml_node xmlImageNode = xmlTypeData.child("Image");
    // 检查第一个节点是否为空
    if (xmlImageNode.empty())
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }

    // 保存ImageList
    List_Image_Item & ListImage = pData->listImage;
    // 遍历结点
    while (xmlImageNode != NULL)
    {
        // 如果是Image结点就取数据
        if (strcmp("Image", xmlImageNode.name()) == 0)
        {
            // 获取该Line节点的数据
            ImageItem * pImageItem = GetImageItem(xmlImageNode);
            // 将数据加到Image链表中
            ListImage.push_back(pImageItem);
        }
        // 下一结点
        // xmlImageNode = xmlImageNode.next_sibling("Image");
        // 目前只支持一个image
        break;
    }

    xml_node xmlUriListNode = xmlTypeData.child("URIList");
    if (!xmlUriListNode.empty())
    {
        XURIList & refURIList = pData->stURLList;
        // 保存属性
        DetectAttr(xmlUriListNode, "base", refURIList.strBase);

        yl::string strKeyMask = "0123456789*#";
        // 获取URI
        for (xml_node xmlURINode = xmlUriListNode.child("URI");
                !xmlURINode.empty();
                xmlURINode = xmlURINode.next_sibling("URI"))
        {
            yl::string strKeyName = "";
            // 保存属性
            DetectAttr(xmlURINode, "key", strKeyName);
            if (strKeyName.length() != 1)
            {
                continue;
            }
            int nPos = strKeyMask.find(strKeyName);
            if (nPos == yl::string::npos)
            {
                continue;
            }
            // 删除已经找到的Key，以便不能重复
            strKeyMask.erase(nPos, 1);

            yl::string strURI = xmlURINode.child_value();
            if (!strURI.empty())
            {
                XURIItem * pItem = new XURIItem;
                CHECK_NULL(pItem, false);
                pItem->strKey = strKeyName;
                pItem->strURI = strURI;

                refURIList.listItem.push_back(pItem);
            }

        }
    }

    ParseSoftkey(xmlTypeData, pData->vecSoftkey);

    return true;
}

bool CXMLParseAction::ParseFormattedTextScreen(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    if (xmlTypeData.empty())
    {
        return false;
    }

    CHECK_NULL(m_pParseData, false);
    XBFormattedTextData * pData = new XBFormattedTextData;
    m_pParseData->pData = pData;

    // 保存属性
    FormattedTextAttr & dataAttr = pData->stAttr;
    // 临时变量
    yl::string strTemp = "";
    // 保存cancelAction
    if (DetectAttr(xmlTypeData, "cancelAction", strTemp))
    {
        dataAttr.strCancelAction = strTemp;
    }

    // 保存doneAction
    if (DetectAttr(xmlTypeData, "doneAction", strTemp))
    {
        dataAttr.strDoneAction = strTemp;
    }

    ParseBeep(xmlTypeData, dataAttr.bBeep);

    // 保存Timeout
    if (DetectAttr(xmlTypeData, "Timeout", strTemp))
    {
        dataAttr.iTimeOut = atoi(strTemp.c_str());

        // 如果时间小于等于0，就设为默认值
        if (dataAttr.iTimeOut < 0)
        {
            dataAttr.iTimeOut = DEFAULT_TIMEOUT;
        }
    }

    // 保存LockIn
    if (DetectAttr(xmlTypeData, "LockIn", strTemp))
    {
        dataAttr.bLockIn = (!strcmp(strTemp.c_str(), "yes"));
    }

    // 保存destroyOnExit
    if (DetectAttr(xmlTypeData, "destroyOnExit", strTemp))
    {
        dataAttr.bDestroyOnExit = (!strcmp(strTemp.c_str(), "yes"));
    }

#if IF_BUG_32431
    ParseRefreshAttr(xmlTypeData);
#endif

    // 保存HeaderList
    List_Header_Zone  &  ListHeader = pData->listHeader;
    // 保存ScrollList
    List_Scroll_Zone  &  ListScroll = pData->listScroll;
    // 保存FooterList
    List_Footer_Zone  &  ListFooter = pData->listFooter;

    xml_node xmlItemNode = xmlTypeData.first_child();
    // 检查第一个节点是否为空
    if (xmlItemNode.empty())
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }

    // 对Header Line的处理
    // 检查节点是否为Line，是就依此遍历结点，直到Line结点结束
    while (!xmlItemNode.empty() && strcmp("Line", xmlItemNode.name()) == 0)
    {
        // 获取该Line节点的数据
        LineData * pLineData = GetLineData(xmlItemNode);
        // 将数据加到Header链表中
        ListHeader.push_back(pLineData);
        // 下一结点
        xmlItemNode = xmlItemNode.next_sibling();
    }

    // 对Scroll Line的处理
    // 检查节点是否为Scroll，是就依此遍历其子结点，直到Scroll结点结束
    while (!xmlItemNode.empty() && strcmp("Scroll", xmlItemNode.name()) == 0)
    {
        // 找到Scroll下的第一个子结点
        xml_node xmlSubItem = xmlItemNode.first_child();

        while (!xmlSubItem.empty() && strcmp("Line", xmlSubItem.name()) == 0)
        {
            // 获取该Line节点的数据
            LineData * pLineData = GetLineData(xmlSubItem);
            // 将数据加到Header链表中
            ListScroll.push_back(pLineData);
            // 下一结点
            xmlSubItem = xmlSubItem.next_sibling();
        }

        // 下一结点
        xmlItemNode = xmlItemNode.next_sibling();
    }

    // 对Footer Line的处理
    // 检查节点是否为Line，是就依此遍历结点，直到Line结点结束
    while (!xmlItemNode.empty() && strcmp("Line", xmlItemNode.name()) == 0)
    {
        // 获取该Line节点的数据
        LineData * pLineData = GetLineData(xmlItemNode);
        // 将数据加到Header链表中
        ListFooter.push_back(pLineData);
        // 下一结点
        xmlItemNode = xmlItemNode.next_sibling();
    }

    ParseSoftkey(xmlTypeData, pData->vecSoftkey);

    return true;
}

static XB_SOFTKEY_TYPE GetKeyType(const char * cszURI)
{
    struct XSoftkeyPair
    {
        char szURI[32];
        XB_SOFTKEY_TYPE eXType;
    };

#define SOFTKEY_URI_PRE "SoftKey:"

    // 类型和URI对应的数组
    static const XSoftkeyPair s_arrayURI[] =
    {
        {SOFTKEY_URI_PRE"Exit",        XB_ST_EXIT  },
        {SOFTKEY_URI_PRE"Select",      XB_ST_SELECT},
        {SOFTKEY_URI_PRE"Dial",        XB_ST_DIAL },
        {SOFTKEY_URI_PRE"Submit",      XB_ST_SUBMIT},
        {SOFTKEY_URI_PRE"Previous",    XB_ST_PREVIOUS},
        {SOFTKEY_URI_PRE"Next",        XB_ST_NEXT},
        {SOFTKEY_URI_PRE"More",        XB_ST_MORE},
        {SOFTKEY_URI_PRE"Dot",         XB_ST_DOT},
        {SOFTKEY_URI_PRE"BackSpace",   XB_ST_BACKSPACE},
        {SOFTKEY_URI_PRE"NextSpace",   XB_ST_NEXTSPACE},
        {SOFTKEY_URI_PRE"ChangeMode",  XB_ST_CHANGEMODE},
        {SOFTKEY_URI_PRE"Search",      XB_ST_SEARCH},
        {"",   XB_ST_EMPTY},
        {"",   XB_ST_UNKNOWN},
    };

    for (int i = 0; s_arrayURI[i].eXType != XB_ST_UNKNOWN; i++)
    {
        if (strcmp(s_arrayURI[i].szURI, cszURI) == 0)
        {
            return s_arrayURI[i].eXType;
        }
    }
    return XB_ST_FUNCTION;
}

// 解析Softkey
bool CXMLParseAction::ParseSoftkey(xml_node xmlTypeData, VecSoftkeyParam & vecSoftkey)
{
    if (xmlTypeData.empty())
    {
        return false;
    }

    xml_node xmlSoftkey = xmlTypeData.child("SoftKey");
    for (; !xmlSoftkey.empty(); xmlSoftkey = xmlSoftkey.next_sibling("SoftKey"))
    {
        yl::string strTmp;
        if (!DetectAttr(xmlSoftkey, "index", strTmp))
        {
            continue;
        }
        int iIndex = atoi(strTmp.c_str());
        // http://10.2.1.199/Bug.php?BugID=40017
        if (iIndex <= 0 || iIndex > MAX_SOFTKEY_NUM)
        {
            continue;
        }

        while (iIndex > vecSoftkey.size())
        {
            vecSoftkey.push_back(SoftkeyParam());
        }
        SoftkeyParam & refParam = vecSoftkey[iIndex - 1];

        GetItemValue(xmlSoftkey, "Label", refParam.strName);
        GetItemValue(xmlSoftkey, "URI", refParam.strURI);

        // 容错，Label为空时，用URI显示
        if (refParam.strName.empty() && !refParam.strURI.empty())
        {
            refParam.strName = refParam.strURI.substr(yl::string("SoftKey:").length(),
                               refParam.strURI.length() - yl::string("SoftKey:").length());
        }

        refParam.eType = GetKeyType(refParam.strURI.c_str());
    }

    return true;
}

// 配置的第三个版本，M7模板。定版
bool CXMLParseAction::ParseConfigV7(XBConfigData * pData, xml_node xmlTypeData,
                                    XB_TIP_TYPE & eErrorType)
{
    CHECK_NULL(pData, false);

    if (xmlTypeData.empty())
    {
        return false;
    }

    // 保存属性
    ConfigAttr & dataAttr = pData->stAttr;
    // 临时变量
    yl::string strTemp = "";

    ParseBeep(xmlTypeData, dataAttr.bBeep);

#if IF_FEATURE_XMLBROWSER_HOTELING
    // 保存Message
    if (DetectAttr(xmlTypeData, "Message", strTemp))
    {
        dataAttr.strMessage = strTemp;
    }

    // 保存MessageTime
    if (DetectAttr(xmlTypeData, "MessageTime", strTemp))
    {
        dataAttr.iMessageTime = atoi(strTemp.c_str());
        if (dataAttr.iMessageTime <= 0)
        {
            dataAttr.iMessageTime = 3;
        }
    }

    // 保存MessageTime
    if (DetectAttr(xmlTypeData, "setType", strTemp))
    {
        if (strcmp(strTemp.c_str(), "hoteling") == 0)
        {
            dataAttr.eType = SET_HOTELING;
        }
    }
#endif

    // 保存ItemList
    List_Config_Item & ListPathItem = pData->listItem;

    yl::string strConfig = "";

    // 查找Path结点
    for (xml_node xmlPathNode = xmlTypeData.child("Item");
            !xmlPathNode.empty();
            xmlPathNode = xmlPathNode.next_sibling("Item"))
    {
        ConfigItem * pItem = new ConfigItem;
        if (pItem == NULL)
        {
            m_iErrCode = ERR_MEM;
            return false;
        }
        pItem->strValue = xmlPathNode.child_value();

#if IF_FEATURE_XMLBROWSER_HOTELING
        if (dataAttr.eType == SET_HOTELING)
        {
            strTemp = pItem->strValue;
            if (strTemp.find("hoteling.user_status") != yl::string::npos)
            {
                strConfig = strTemp;
            }
        }
#endif
        // 保存解析好的数据
        pData->listItem.push_back(pItem);
    }

#if IF_FEATURE_XMLBROWSER_HOTELING
    //Hoteling 没有成功登陆或登出则不给autop发消息
    if (dataAttr.eType == SET_HOTELING)
    {
        if (!strConfig.empty())
        {
            yl::string::size_type nPos = strConfig.find_first_of('=');
            if (nPos != yl::string::npos)
            {
                yl::string strValue = strConfig.substr(nPos + 1);
                strValue = commonAPI_TrimString(strValue, " ");
                int iValue = atoi(strValue.c_str());
                if ((iValue == 1 && _XMLBrowserManager.IsHotelingLogin())
                        || (iValue == 0 && !_XMLBrowserManager.IsHotelingLogin()))
                {
                    return (pData->listItem.size() > 0);
                }
            }
        }
        Delete_Dynamic_List(List_Config_Item, pData->listItem);
        return true;
    }
#endif

    return (pData->listItem.size() > 0);
}

// 配置的第一个版本，类似M1的autop配置
bool CXMLParseAction::ParseConfigV1(XBConfigData * pData, xml_node xmlTypeData,
                                    XB_TIP_TYPE & eErrorType)
{
    /*
    <ConfigurationItem>
    <Path>/yealink/config/Setting/Setting.cfg</Path>
    <Session>PhoneSetting</Session>
    <Parameter>ProductName</Parameter>
    <Value>SIP-T28P</Value>
    </ConfigurationItem>
    */
    CHECK_NULL(pData, false);

    if (xmlTypeData.empty())
    {
        return false;
    }

    // 保存属性
    ConfigAttr & dataAttr = pData->stAttr;
    // 临时变量
    yl::string strTemp = "";

    ParseBeep(xmlTypeData, dataAttr.bBeep);

    // 保存ItemList
    List_Config_Item & ListPathItem = pData->listItem;

    yl::string strConfig = "";

    // 查找Path结点
    for (xml_node xmlCfgNode = xmlTypeData.child("ConfigurationItem");
            !xmlCfgNode.empty();
            xmlCfgNode = xmlCfgNode.next_sibling("ConfigurationItem"))
    {
        ConfigItem * pItem = new ConfigItem;
        if (pItem == NULL)
        {
            m_iErrCode = ERR_MEM;
            return false;
        }

        pItem->strPath = xmlCfgNode.child("Path").child_value();
        pItem->strSection = xmlCfgNode.child("Session").child_value();
        pItem->strParam = xmlCfgNode.child("Parameter").child_value();
        pItem->strValue = xmlCfgNode.child("Value").child_value();

        // 保存解析好的数据
        pData->listItem.push_back(pItem);
    }

    return (pData->listItem.size() > 0);
}

//
const char * TransToM7Param(const yl::string & refParam)
{
    if (refParam == "DKtype")
    {
        return M7_DSSKEY_TYPE;
    }
    else if (refParam == "Line")
    {
        return M7_DSSKEY_LINE;
    }
    else if (refParam == "Value")
    {
        return M7_DSSKEY_VALUE;
    }
    else if (refParam == "Label")
    {
        return M7_DSSKEY_LABEL;
    }
    else if (refParam == "PickupValue")
    {
        return M7_DSSKEY_PK_VAL;
    }
    return "";
}

// 配置的第二个版本，原T3X和VPX所用
bool CXMLParseAction::ParseConfigV2(XBConfigData * pData, xml_node xmlTypeData,
                                    XB_TIP_TYPE & eErrorType)
{
    /*
    <Path config="LineKey">
    <Item>linekey3.DKtype=XMLBrowser</Item>
    <Item>linekey3.Label=testMenu</Item>
    <Item>linekey3.Value=http://10.2.6.2:8000/workspace/JxzTestmenu.xml</Item>
    </Path>
    */
    CHECK_NULL(pData, false);

    // 保存属性
    ConfigAttr & dataAttr = pData->stAttr;
    // 临时变量
    yl::string strTemp = "";

    ParseBeep(xmlTypeData, dataAttr.bBeep);

    // 保存ItemList
    List_Config_Item & ListPathItem = pData->listItem;

    yl::string strConfig = "";

    // 查找Path结点
    xml_node xmlPathNode = xmlTypeData.child("Path");
    while (!xmlPathNode.empty())
    {
        // 获取Path
        DetectAttr(xmlPathNode, "config", strConfig);

        // 如果path为空则找下一结点
        if (strConfig.empty())
        {
            // 查找下一个Path
            xmlPathNode = xmlPathNode.next_sibling();
            continue;
        }
        //
        for (xml_node xmlItemNode = xmlPathNode.child("Item");
                xmlItemNode != NULL;
                xmlItemNode = xmlItemNode.next_sibling("Item"))
        {
            ConfigItem * pConfigItem = new ConfigItem();

            // 解析配置信息
            if (!ConfigParser(pConfigItem, xmlItemNode))
            {
                // 解析失败就查找下一Item结点
                delete pConfigItem;
                pConfigItem = NULL;
                continue;
            }

            yl::string strNewValue = "";
            int  nIndex = -1;
            int  nExpIndex = -1;
            // memorykey.x.xxx = value
            if (sscanf(pConfigItem->strSection.c_str(), "memory%d", &nIndex) == 1)
            {
                strNewValue = "memorykey";
            }
            else if (sscanf(pConfigItem->strSection.c_str(), "linekey%d", &nIndex) == 1)
            {
                strNewValue = "linekey";
            }
            // expansion_module.x.key.xxx = value
            else if (sscanf(pConfigItem->strSection.c_str(), "expkey%d", &nIndex) == 1
                     && sscanf(strConfig.c_str(), "EXP%d", &nExpIndex) == 1)
            {
                strNewValue = "expansion_module";
                strNewValue.append(".");
                char szExpIndex[32] = "";
                sprintf(szExpIndex, "%d", nExpIndex);
                strNewValue.append(szExpIndex);

                strNewValue.append(".key");
            }
            else
            {
                XMLBROWSER_INFO("Unknown section name [%s]", pConfigItem->strSection.c_str());
                // 解析失败就查找下一Item结点
                delete pConfigItem;
                pConfigItem = NULL;
                continue;
            }

            pConfigItem->strParam = TransToM7Param(pConfigItem->strParam);
            if (pConfigItem->strParam == M7_DSSKEY_TYPE)
            {
                // 是从旧的T3X/VPX格式转过来的，要进行名称到值的转换
                int nDssKeyType = TransformDsskeyType(pConfigItem->strValue.c_str());
                if (nDssKeyType > -1)
                {
                    char szType[16] = "";
                    sprintf(szType, "%d", nDssKeyType);
                    pConfigItem->strValue = szType;
                }
            }

            char szIndex[32] = "";
            sprintf(szIndex, ".%d", nIndex);
            strNewValue.append(szIndex);
            strNewValue.append(".");
            strNewValue.append(pConfigItem->strParam);
            strNewValue.append("=");
            strNewValue.append(pConfigItem->strValue);

            // Value会在对象处理时作为M7的值使用
            pConfigItem->strValue = strNewValue;
            // 保存解析好的数据
            pData->listItem.push_back(pConfigItem);
        }

        // 查找下一个Path
        xmlPathNode = xmlPathNode.next_sibling();
    }

    return true;
}

// 获取style
MENU_ITEM_STYLE CXMLParseAction::GetMenuStyle(const yl::string & strTemp)
{
    if (!strcmp(strTemp.c_str(), "none"))
    {
        return STYLE_NONE;
    }
    else if (!strcmp(strTemp.c_str(), "radio"))
    {
        return STYLE_RADIO;
    }
    else
    {
        return STYLE_NUMBERED;
    }
}

// 获取type
INPUT_TYPE CXMLParseAction::GetInputType(const yl::string & strTemp)
{
    if (!strcmp(strTemp.c_str(), "IP"))
    {
        return INPUT_IP;
    }
    else if (!strcmp(strTemp.c_str(), "string"))
    {
        return INPUT_STRING;
    }
    else if (!strcmp(strTemp.c_str(), "number"))
    {
        return INPUT_NUMBER;
    }
    else if (!strcmp(strTemp.c_str(), "timeUS"))
    {
        return INPUT_TIME_US;
    }
    else if (!strcmp(strTemp.c_str(), "timeInt"))
    {
        return INPUT_TIME_INT;
    }
    else if (!strcmp(strTemp.c_str(), "dateUS"))
    {
        return INPUT_DATE_US;
    }
    else if (!strcmp(strTemp.c_str(), "dateInt"))
    {
        return INPUT_DATE_INT;
    }
    else if (!strcmp(strTemp.c_str(), "empty"))
    {
        return INPUT_EMPTY;
    }
    else
    {
        // 默认使用页面的属性值
        return INPUT_STRING;
    }
}

// 获取输入语言类型
INPUT_LANGUAGE CXMLParseAction::GetInputLanguage(const yl::string & strTemp)
{
    if (!strcmp(strTemp.c_str(), "English"))
    {
        return INPUT_LANG_ENGLISH;
    }
    else if (!strcmp(strTemp.c_str(), "French"))
    {
        return INPUT_LANG_FRENCH;
    }
    else if (!strcmp(strTemp.c_str(), "German"))
    {
        return INPUT_LANG_GERMAN;
    }
    else if (!strcmp(strTemp.c_str(), "Italian"))
    {
        return INPUT_LANG_ITALIAN;
    }
    else if (!strcmp(strTemp.c_str(), "Spanish"))
    {
        return INPUT_LANG_SPANISH;
    }
    else
    {
        return INPUT_LANG_ENGLISH;
    }
}

// 获取DisplayMode
INPUT_DISPLAY_MODE CXMLParseAction::GetInputDisplayMode(const yl::string & strTemp)
{
    if (!strcmp(strTemp.c_str(), "normal"))
    {
        return MODE_NORMAL;
    }
    else if (!strcmp(strTemp.c_str(), "condensed"))
    {
        return MODE_CONDENSED;
    }
    else
    {
        return MODE_NORMAL;
    }
}

// 得到第一个不为empty的输入框的index
bool CXMLParseAction::GetFirstUnEmptyIndex(int & iIndex, List_Input_Field ListInput)
{
    List_Input_Field::iterator iter = ListInput.begin();
    for (int i = 0; iter != ListInput.end(); iter++, ++i)
    {
        if ((*iter)->eType != INPUT_EMPTY)
        {
            iIndex = i;
            return true;
        }
    }

    return false;
}

// 解析某个输入项的节点
bool CXMLParseAction::ParserInputField(xml_node xmlInputNode, InputField * pInputData)
{
    if (xmlInputNode.empty()
            || pInputData == NULL)
    {
        return false;
    }

    yl::string strValue = "";

    // 保存Prompt
    if (GetItemValue(xmlInputNode, "Prompt", strValue))
    {
        pInputData->strPrompt = strValue;
    }

    // 保存Parameter
    if (GetItemValue(xmlInputNode, "Parameter", strValue))
    {
        // Parameter不能为空
        if (strValue.empty())
        {
            return false;
        }

        pInputData->strParam = strValue;
    }

    // 保存Selection
    if (GetItemValue(xmlInputNode, "Selection", strValue))
    {
        pInputData->strSelection = strValue;
    }

    // 保存Default
    if (GetItemValue(xmlInputNode, "Default", strValue))
    {
        pInputData->strDefault = strValue;
    }
    GetInputValue(pInputData->eType, pInputData->strDefault);
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    ParseSoftkey(xmlInputNode, pInputData->vecSoftkey);
#endif
    return true;
}

// 获取输入框的默认值
bool CXMLParseAction::GetInputValue(INPUT_TYPE eInputType, yl::string & strValue)
{
    if (!strValue.empty())
    {
        return false;
    }

    switch (eInputType)
    {
    case INPUT_TIME_US:
        strValue = "12:00:00 AM";
        break;
    case INPUT_TIME_INT:
        strValue = "00:00:00";
        break;
    case INPUT_DATE_US:
    case INPUT_DATE_INT:
        {
            // dateUS   MM/DD/YYYY 其中MM:1-12,DD:1-31,YYYY:0000-9999 12/31/2009
            time_t tCurrent = netGetLocalTime();
            struct tm * tmLocal;
            tmLocal = localtime(&tCurrent);

            char szBuf[512] = {0};
            if (eInputType == INPUT_DATE_US)
            {
                sprintf(szBuf, "%d/%d/%d", tmLocal->tm_mon + 1, tmLocal->tm_mday, tmLocal->tm_year + 1900);
            }
            else
            {
                sprintf(szBuf, "%d/%d/%d", tmLocal->tm_mday, tmLocal->tm_mon + 1, tmLocal->tm_year + 1900);
            }
            strValue = szBuf;
        }
        break;
    default:
        return false;
    }
    return true;
}
// 获取一个Line的数据
LineData * CXMLParseAction::GetLineData(xml_node xmlItemNode)
{
    yl::string strValue = "";
    chString strTemp = "";

    LineData * pLineData = new LineData();
    // 保存size
    if (DetectAttr(xmlItemNode, "Size", strValue))
    {
        pLineData->eFont = GetLineSize(strValue);
    }

    // 保存align
    if (DetectAttr(xmlItemNode, "Align", strValue))
    {
        pLineData->iAlign = GetLineAlign(strValue);
    }

    // 保存color
    if (DetectAttr(xmlItemNode, "Color", strValue))
    {
        if (IsRGB(strValue))
        {
            pLineData->strRGB = strValue;
        }
        else
        {
            pLineData->eColor = GetLineColor(strValue);
        }
    }

    // 保存文本内容
    pLineData->strText = xmlItemNode.child_value();

    return pLineData;
}

// 获取字体大小
X_FONT_TYPE CXMLParseAction::GetLineSize(const yl::string & strTemp)
{
    if (!strcmp(strTemp.c_str(), "small"))
    {
        return X_FONT_SMALL;
    }
    else if (!strcmp(strTemp.c_str(), "double"))
    {
        return X_FONT_DOUBLE;
    }
    else if (!strcmp(strTemp.c_str(), "large"))
    {
        return X_FONT_LARGE;
    }
    else
    {
        return X_FONT_NORMAL;   // 默认返回normal
    }
}

// 获取对齐方式
int CXMLParseAction::GetLineAlign(const yl::string & strTemp)
{
    if (!strcmp(strTemp.c_str(), "center"))
    {
        return DT_CENTER | DT_VCENTER;
    }
    else if (!strcmp(strTemp.c_str(), "right"))
    {
        return DT_RIGHT | DT_VCENTER;
    }
    else
    {
        return DT_LEFT | DT_VCENTER;        // 默认返回left
    }
}

// 获取字体颜色
TEXT_FONT_COLOR CXMLParseAction::GetLineColor(const yl::string & strTemp)
{
    if (!strcmp(strTemp.c_str(), "black"))
    {
        return TEXT_FONT_COLOR_BLACK;
    }
    else if (!strcmp(strTemp.c_str(), "red"))
    {
        return TEXT_FONT_COLOR_RED;
    }
    else if (!strcmp(strTemp.c_str(), "green"))
    {
        return TEXT_FONT_COLOR_GREEN;
    }
    else if (!strcmp(strTemp.c_str(), "brown"))
    {
        return TEXT_FONT_COLOR_BROWN;
    }
    else if (!strcmp(strTemp.c_str(), "blue"))
    {
        return TEXT_FONT_COLOR_BLUE;
    }
    else if (!strcmp(strTemp.c_str(), "magenta"))
    {
        return TEXT_FONT_COLOR_MAGENTA;
    }
    else if (!strcmp(strTemp.c_str(), "cyan"))
    {
        return TEXT_FONT_COLOR_CYAN;
    }
    else if (!strcmp(strTemp.c_str(), "lightgray"))
    {
        return TEXT_FONT_COLOR_LIGHTGRAY;
    }
    else if (!strcmp(strTemp.c_str(), "darkgray"))
    {
        return TEXT_FONT_COLOR_DARKGRAY;
    }
    else if (!strcmp(strTemp.c_str(), "lightred"))
    {
        return TEXT_FONT_COLOR_LIGHTRED;
    }
    else if (!strcmp(strTemp.c_str(), "lightgreen"))
    {
        return TEXT_FONT_COLOR_LIGHTGREEN;
    }
    else if (!strcmp(strTemp.c_str(), "yellow"))
    {
        return TEXT_FONT_COLOR_YELLOW;
    }
    else if (!strcmp(strTemp.c_str(), "lightblue"))
    {
        return TEXT_FONT_COLOR_LIGHTBLUE;
    }
    else if (!strcmp(strTemp.c_str(), "lightmagenta"))
    {
        return TEXT_FONT_COLOR_LIGHTMAGENTA;
    }
    else if (!strcmp(strTemp.c_str(), "lightcyan"))
    {
        return TEXT_FONT_COLOR_LIGHTCYAN;
    }
    else
    {
        return TEXT_FONT_COLOR_WHITE;   // 默认返回white
    }
}

ImageItem * CXMLParseAction::GetImageItem(xml_node xmlItemNode)
{
    yl::string strValue = "";
    chString strTemp = "";

    ImageItem * pImageItem = new ImageItem();
    // 保存类型
    pImageItem->bUrlData = true;
    if (DetectAttr(xmlItemNode, "type", strValue))
    {
        pImageItem->bUrlData = (strValue != "bin");
    }

    // 保存verticalAlign
    if (DetectAttr(xmlItemNode, "verticalAlign", strValue))
    {
        pImageItem->eVertical = GetVerticalAlign(strValue);
    }

    // 保存horizontalAlign
    if (DetectAttr(xmlItemNode, "horizontalAlign", strValue))
    {
        pImageItem->eHorizontal = GetHorizontalAlign(strValue);
    }

    // 保存horizontalAlign
    if (DetectAttr(xmlItemNode, "height", strValue))
    {
        int iHeight = atoi(strValue.c_str());
        pImageItem->uHeight = iHeight < 0 ? 0 : iHeight;
    }
    // 保存horizontalAlign
    if (DetectAttr(xmlItemNode, "width", strValue))
    {
        int iWidth = atoi(strValue.c_str());
        pImageItem->uWidth = iWidth < 0 ? 0 : iWidth;
    }

    // 保存图片characters或URL
    pImageItem->strImageURL = xmlItemNode.child_value();

    // 保存图片文件名
    pImageItem->strImagePath = GetImagePathByURL(pImageItem->strImageURL);

    return pImageItem;
}

// 获取下载图片的文件名
yl::string CXMLParseAction::GetImagePathByURL(yl::string strImageURL)
{
    int iPosition = strImageURL.find_last_of("/");
    yl::string strPath = TEMP_PATH + strImageURL.substr(iPosition + 1);
    return strPath;
}

// 获取垂直对齐方式
IMAGE_VERTICAL_ALIGN CXMLParseAction::GetVerticalAlign(const yl::string & strTemp)
{
    if (strTemp == "top")
    {
        return IMAGE_VERTICAL_ALIGN_TOP;
    }
    else if (strTemp == "bottom")
    {
        return IMAGE_VERTICAL_ALIGN_BOTTOM;
    }
    else
    {
        return IMAGE_VERTICAL_ALIGN_CENTER; // 默认返回middle
    }
}
// 获取水平对齐方式
IMAGE_HORIZONTAL_ALIGN CXMLParseAction::GetHorizontalAlign(const yl::string & strTemp)
{
    if (strTemp == "left")
    {
        return IMAGE_HORIZONTAL_ALIGN_LEFT;
    }
    else if (strTemp == "right")
    {
        return IMAGE_HORIZONTAL_ALIGN_RIGHT;
    }
    else
    {
        return IMAGE_HORIZONTAL_ALIGN_CENTER;   // 默认返回middle
    }
}

// 获取显示方式
IMAGE_MODE CXMLParseAction::GetImageMode(const yl::string & strTemp)
{
    if (strTemp == "fullscreen")
    {
        return IMAGE_MODE_FULLSCREEN;
    }
    else
    {
        return IMAGE_MODE_REGULAR;  // 默认返回regular
    }
}


// 获取IconPath
yl::string CXMLParseAction::GetIconPath(const yl::string & strTemp)
{
    bool bColor = false;
    yl::string strDir = "";

    if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
    {
        bColor = true;
        strDir = res_getPathByName(RESOURCE_PATH_XMLPIC);
    }


    yl::string strIcon = "";
    if (strTemp == "Forward")
    {
        strIcon = bColor ? T4X_ICON_PATH_FWD : T2X_ICON_PATH_FWD;
    }
    else if (strTemp == "DND")
    {
        strIcon = bColor ? T4X_ICON_PATH_DND : T2X_ICON_PATH_DND;
    }
    else if (strTemp == "Message")
    {
        strIcon = bColor ? T4X_ICON_PATH_MSG : T2X_ICON_PATH_MSG;
    }
    else if (strTemp == "Note")
    {
        strIcon = bColor ? T4X_ICON_PATH_NOTE : T2X_ICON_PATH_NOTE;
    }
    else if (strTemp == "Warning")
    {
        strIcon = bColor ? T4X_ICON_PATH_WAR : T2X_ICON_PATH_WAR;
    }
    else if (strTemp == "Conference")
    {
        strIcon = "";
    }
    else
    {
        strIcon = "";
    }


    return strDir.append(strIcon);
}

// 根据path获得一个可用的PathItem
PathItem * CXMLParseAction::GetPathItemByPath(const char * lpszPath, List_Path_Item ListPathItem)
{
    if (lpszPath == "")
    {
        return NULL;
    }

    List_Path_Item::iterator iter = ListPathItem.begin();

    // 遍历链表
    for (; iter != ListPathItem.end(); iter++)
    {
        PathItem * pPathItem = *iter;
        // 如果有相同路径存在就返回其所在的PathItem
        if (pPathItem->strPath == lpszPath)
        {
            return pPathItem;
        }
    }

    // 没有相同路径存在就新建一个PathItem加入链表并返回
    PathItem * pNewPathItem = new PathItem();

    // 保存path
    pNewPathItem->strPath = lpszPath;
    ListPathItem.push_back(pNewPathItem);

    return pNewPathItem;
}

// 解析配置信息
bool CXMLParseAction::ConfigParser(ConfigItem * pConfigItem, xml_node xmlItemNode)
{
    // 检查数据合法性
    if (pConfigItem == NULL || xmlItemNode.empty())
    {
        return false;
    }

    // 获取配置信息
    yl::string strTemp = xmlItemNode.child_value();
    if (strTemp.empty())
    {
        return false;
    }

    // 解析配置信息
    ScanConfig(pConfigItem->strSection, pConfigItem->strParam, pConfigItem->strValue, strTemp);

    // 去掉首尾的空格
    pConfigItem->strSection = TrimString(pConfigItem->strSection);
    pConfigItem->strParam = TrimString(pConfigItem->strParam);
    pConfigItem->strValue = TrimString(pConfigItem->strValue);

    XMLBROWSER_INFO("before szSection[%s], szParam[%s], szValue[%s]",
                    pConfigItem->strSection.c_str(), pConfigItem->strParam.c_str(), pConfigItem->strValue.c_str());

    return true;
}

// 去掉首尾的空格
yl::string CXMLParseAction::TrimString(const yl::string & strTemp)
{
    if (strTemp.empty())
    {
        return "";
    }

    // 获得第一个不是空格的位置
    int iBegin = strTemp.find_first_not_of(" ");
    // 获得最后一个不是空格的位置
    int iEnd = strTemp.find_last_not_of(" ");

    // 只有空格的处理
    if (iBegin == yl::string::npos || iEnd == yl::string::npos || iEnd < iBegin)
    {
        return "";
    }
    // 取出子串
    yl::string strResult = strTemp.substr(iBegin, (iEnd - iBegin + 1));
    return strResult;
}

// 解析配置信息
void CXMLParseAction::ScanConfig(yl::string & strSection, yl::string & strParam,
                                 yl::string & strValue, const yl::string & strTemp)
{
    // 记录起始位置
    int iBegin = strTemp.find_first_of(".");
    int iEnd = strTemp.find_first_of("=");

    // 解析Section
    strSection = strTemp.substr(0, iBegin);
    // 解析Param
    strParam = strTemp.substr(iBegin + 1, iEnd - iBegin - 1);
    // 解析Value
    strValue = strTemp.substr(iEnd + 1);
}

// 整形转为字符串
yl::string CXMLParseAction::Myitoa(int i)
{
    char szBuffer[256] = "";
    sprintf(szBuffer, "%d", i);
    return szBuffer;
}

// Dsskey的类型转换
int CXMLParseAction::TransformDsskeyType(const char * szValue)
{
    return dsskey_GetDssKeyType(szValue);
}

#ifdef IF_FEATURE_GENBAND
//解析开发接口数据
bool CXMLParseAction::ParseInterface(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    if (xmlTypeData.empty())
    {
        return false;
    }

    CHECK_NULL(m_pParseData, false);
    InterfaceData * pData = new InterfaceData;

    m_pParseData->pData = pData;

    // 临时变量
    yl::string strTemp = "";

    ParseBeep(xmlTypeData, dataAttr.bBeep);

#if IF_BUG_32431
    ParseRefreshAttr(xmlTypeData);
#endif

    xml_node xmlItemNode = xmlTypeData.first_child();
    // 检查第一个节点是否为空
    if (xmlItemNode.empty())
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }

    // 获取该Label节点的数据, 将数据加到strLabel中
    GetItemValue(xmlTypeData, "Label", pData->strLabel);

    // 获取该Method节点的数据, 将数据加到strMethod中
    GetItemValue(xmlTypeData, "Method", pData->strMethod);

    // 获取该URL节点的数据, 将数据加到strUrl中
    GetItemValue(xmlTypeData, "URL", pData->strUrl);

    // get phone type from cfg file
    char bufProductName[512] = {0};
    sys_replace_wildcards((char *)configParser_GetConfigString(kszProductName).c_str(), 0,
                          bufProductName, sizeof(bufProductName) / sizeof(char));
    yl::string strPhoneType = bufProductName;

    // get firmware from cfg file
    // cfgserver 不能使用configParser获取这个配置，统一使用sys服务获取
    char szFirmware[64] = { 0 };
    yl::string strFirmware = "";
    if (0 == sys_get_sw_version(szFirmware, 64))
    {
        strFirmware = szFirmware;
    }

    // get mac
    yl::string strMAC = netGetMACText('\0');

    yl::string strIP;

    netGetWanIP(strIP);

    string_replace(pData->strUrl, VAR_MAC, strMAC);
    string_replace(pData->strUrl, VAR_IP, strIP);
    string_replace(pData->strUrl, VAR_PHONE_TYPE, strPhoneType);
    string_replace(pData->strUrl, VAR_FIRMWARE, strFirmware);

    XMLBROWSER_INFO("url[%s]", pData->strUrl.c_str());

    // 获取该RequestData节点的数据, 将数据加到strRequestData中
    GetItemValue(xmlTypeData, "RequestData", pData->strRequestData);
    if (!Base64Dec(pData->strRequestData))
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }

    // 获取该HTTPHeaders节点的数据, 将数据加到strHttpHeaders中
    GetItemValue(xmlTypeData, "HTTPHeaders", pData->strHttpHeaders);
    if (!Base64Dec(pData->strHttpHeaders))
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }
    else
    {
        // 解析转化客户提出特殊格式
        ChangeHeadFormat(pData->strHttpHeaders);
        // 如果HTTP Headers非空，则添加前
        pData->strHttpHeaders = "HTTPHeaders:" + pData->strHttpHeaders + "\r\n";
    }

    //to do -syl
    //usleep(5000*1000);

    return true;
}

// base64解密是否正确
bool CXMLParseAction::Base64Dec(yl::string & strData)
{
    if (!strData.empty())
    {
        // 去除无效字符
        strData.trim_both("\r \n\t");
        unsigned char tmp1[4096] = {0};

        if (PW_Base64Dec(tmp1, (const unsigned char *)strData.c_str(), strData.size()) < 0)
        {
            return false;
        }

        strData = (char *)tmp1;

        return true;
    }

    return false;
}

// 转化{'Connection': 'keep-alive', 'Content-Type': 'application/json', 'Authorization': 'Basic XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX', 'Accept': 'application/json'}
void CXMLParseAction::ChangeHeadFormat(yl::string & strData)
{
    // 测试版转化
    if (!strData.empty())
    {
        strData.trim_both("{}");
        string_replace(strData, "','", "\r\n");
        string_replace(strData, "'", "");
    }
}
#endif

#if IF_BUG_31595
// 解析DSSkey数据
//数据格式
//<MessageInDsskey  id = "1", type="63">
//  <itemdata value="28" line="0" label="44904 44904" extension=""/>
//</MessageInDsskey>
bool CXMLParseAction::ParseDssKeyData(xml_node xmlDsskeyData, StatusItem * pStatusItem)
{
    xml_node xmlItemDataNode = xmlDsskeyData.child("itemdata");

    if (xmlItemDataNode.empty())
    {
        return false;
    }

    int iDsskeyIndex = -1;
    int iDsskeyID = -1;
    yl::string strId, strType, strLine;
    DetectAttr(xmlDsskeyData, "id", strId);
    DetectAttr(xmlDsskeyData, "type", strType);
    DetectAttr(xmlItemDataNode, "value", pStatusItem->dsskeyData.strValue);
    DetectAttr(xmlItemDataNode, "line", strLine);
    DetectAttr(xmlItemDataNode, "label", pStatusItem->dsskeyData.strLabel);
    DetectAttr(xmlItemDataNode, "extension", pStatusItem->dsskeyData.strExtension);

    if (!strId.empty())
    {
        iDsskeyIndex = atoi(strId.c_str());
        iDsskeyID = dsskey_GetDsskeyIDByKeyIndex(iDsskeyIndex);
        if (exp_IsEXPKey(iDsskeyID))
        {
            pStatusItem->dsskeyData.iDssKeyID = iDsskeyID;
        }
        else
        {
            pStatusItem->dsskeyData.iDssKeyID = iDsskeyIndex;
        }

    }

    if (!strType.empty())
    {
        pStatusItem->dsskeyData.eDKType = (DssKey_Type)atoi(strType.c_str());
    }

    if (!strLine.empty())
    {
        pStatusItem->dsskeyData.iLineIndex = atoi(strLine.c_str());
    }

    pStatusItem->dsskeyData.eType = dsskey_GetModuleType(pStatusItem->dsskeyData.iDssKeyID);

    XMLBROWSER_INFO("dsskey logic info: dsskeyID=[%d] type=[%d] line=[%d] value=[%s] lable=[%s] expType=[%d]"
                    , pStatusItem->dsskeyData.iDssKeyID
                    , pStatusItem->dsskeyData.eDKType
                    , pStatusItem->dsskeyData.iLineIndex
                    , pStatusItem->dsskeyData.strValue.c_str()
                    , pStatusItem->dsskeyData.strLabel.c_str()
                    , pStatusItem->dsskeyData.eType);

    return true;
}
#endif //IF_BUG_31595

#if IF_FEATURE_XMLB_SCREENSAVER
bool CXMLParseAction::ParseScreenSaver(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType)
{
    if (xmlTypeData.empty())
    {
        eErrorType = XB_TIP_PARSE_FAIL;
        return false;
    }

    CHECK_NULL(m_pParseData, false);
    XBScreenSaverData * pData = new XBScreenSaverData;
    m_pParseData->pData = pData;

    // 保存ImageList
    List_Image_Item & ListImage = pData->listImage;
    // 保存LineList
    List_Line_Zone & ListLine = pData->listLine;
    //保存系统时间
    SysTimeData & stSystemTime = pData->stSysTime;
    //保存状态图标
    StatusIconData & stStatusIcon = pData->stStatusIcon;
    // 保存属性
    ScreenSaverAttr & dataAttr = pData->stAttr;

    // 临时变量
    yl::string strTemp = "";

    // 保存LineSpacing
    if (DetectAttr(xmlTypeData, "LineSpacing", strTemp))
    {
        dataAttr.iLineSpacing = atoi(strTemp.c_str());
        // 如果小于等于0，就设为默认值
        if (dataAttr.iLineSpacing < 0)
        {
            dataAttr.iLineSpacing = 0;
        }
    }

    // 保存InsertImageLineNum
    if (DetectAttr(xmlTypeData, "InsertImageLineNum", strTemp))
    {
        dataAttr.iInserImageLineNum = atoi(strTemp.c_str());
        // 如果小于-1，就设为默认值
        if (dataAttr.iInserImageLineNum < 0)
        {
            dataAttr.iInserImageLineNum = 0;
        }
    }

    xml_node xmlImageNode = xmlTypeData.child("Image");

    // 遍历结点
    while (xmlImageNode != NULL)
    {
        // 如果是Image结点就取数据
        if (strcmp("Image", xmlImageNode.name()) == 0)
        {
            // 获取该Line节点的数据
            ImageItem * pImageItem = GetImageItem(xmlImageNode);
            // 将数据加到Image链表中
            ListImage.push_back(pImageItem);
        }
        // 下一结点
        // xmlImageNode = xmlImageNode.next_sibling("Image");
        // 目前只支持一个image
        break;
    }

    xml_node xmlItemNode = xmlTypeData.child("Line");
    int iLineNum = 0;
    int iLineMaxNum = GetMaxLineNum();
    // 检查节点是否为Line，是就依此遍历结点，个数不超过最大值，直到Line结点结束
    while (!xmlItemNode.empty()
            && strcmp("Line", xmlItemNode.name()) == 0
            && iLineNum < iLineMaxNum)
    {
        ++iLineNum;
        // 获取该Line节点的数据
        LineData * pLineData = GetLineData(xmlItemNode);
        // 将数据加到Header链表中
        ListLine.push_back(pLineData);
        // 下一结点
        xmlItemNode = xmlItemNode.next_sibling();
    }

    xml_node xmlSysTimeNode = xmlTypeData.child("SystemTime");
    //检查系统时间节点
    if (!xmlSysTimeNode.empty())
    {
        ParseSystemTimeData(xmlSysTimeNode, stSystemTime);
    }

    xml_node xmlStatusIconNode = xmlTypeData.child("StatusIcons");
    //检查状态图标节点
    if (!xmlStatusIconNode.empty())
    {
        ParseStatusIconData(xmlStatusIconNode, stStatusIcon);
    }

    return true;
}
int CXMLParseAction::GetMaxLineNum()
{
    //这边统一使用目前所有机型支持的最大显示行数
    //显示几行由UI来处理
    int iMaxLineNum = 15;


    return iMaxLineNum;
}

void CXMLParseAction::ParseStatusIconData(xml_node xmlStatusIconNode,
        StatusIconData & strStatusIconData)
{
    yl::string strValue = "";

    // 保存verticalAlign
    if (DetectAttr(xmlStatusIconNode, "verticalAlign", strValue))
    {
        if (!strValue.empty())
        {
            strStatusIconData.eVertical = GetVerticalAlign(strValue);
        }
        //为空则使用默认值IMAGE_VERTICAL_ALIGN_TOP
    }

    // 保存horizontalAlign
    if (DetectAttr(xmlStatusIconNode, "horizontalAlign", strValue))
    {
        if (!strValue.empty())
        {
            strStatusIconData.eHorizontal = GetHorizontalAlign(strValue);
        }
        //为空则使用默认值IMAGE_HORIZONTAL_ALIGN_CENTER
    }

    strStatusIconData.bShow = GetShowOfSysTimeorStatusIcon(xmlStatusIconNode.child_value());
}

void CXMLParseAction::ParseSystemTimeData(xml_node xmlSysTimeNode, SysTimeData & strSysTimeData)
{
    yl::string strValue = "";

    // 保存size
    if (DetectAttr(xmlSysTimeNode, "Size", strValue))
    {
        strSysTimeData.eFont = GetLineSize(strValue);
    }
    // 保存color
    if (DetectAttr(xmlSysTimeNode, "Color", strValue))
    {
        if (IsRGB(strValue))
        {
            strSysTimeData.strRGB = strValue;
        }
        else
        {
            strSysTimeData.eColor = GetLineColor(strValue);
        }
    }

    // 保存verticalAlign
    if (DetectAttr(xmlSysTimeNode, "verticalAlign", strValue))
    {
        if (!strValue.empty())
        {
            strSysTimeData.eVertical = GetVerticalAlign(strValue);
        }
        //为空则使用默认值IMAGE_VERTICAL_ALIGN_TOP
    }

    // 保存horizontalAlign
    if (DetectAttr(xmlSysTimeNode, "horizontalAlign", strValue))
    {
        if (!strValue.empty())
        {
            strSysTimeData.eHorizontal = GetHorizontalAlign(strValue);
        }
        //为空则使用默认值IMAGE_HORIZONTAL_ALIGN_RIGHT
    }

    strSysTimeData.bShow = GetShowOfSysTimeorStatusIcon(xmlSysTimeNode.child_value());
}

bool CXMLParseAction::GetShowOfSysTimeorStatusIcon(const yl::string & strTemp)
{
    if (strTemp == "hide")
    {
        return false;
    }
    else //等于show或者其他情况都为显示
    {
        return true;
    }
}

bool CXMLParseAction::IsRGB(yl::string strRGB)
{
    int iRed, iGreen, iBlue;
    strRGB.trim_both();
    strRGB.to_upper();

    if (sscanf(strRGB.c_str(), "%d,%d,%d", &iRed, &iGreen, &iBlue) == 3)
    {
        //直接后面判断三个值是否正确
    }
    else if (sscanf(strRGB.c_str(), "RGB(%d,%d,%d)", &iRed, &iGreen, &iBlue) == 3)
    {
        //直接后面判断三个值是否正确
    }
    else
    {
        return false;
    }

    if (iRed > 256 || iRed < 0)
    {
        return false;
    }
    if (iGreen > 256 || iGreen < 0)
    {
        return false;
    }
    if (iBlue > 256 || iBlue < 0)
    {
        return false;
    }
    return true;
}

#endif //#if IF_FEATURE_XMLB_SCREENSAVER


#endif // #ifdef IF_FEATURE_XMLBROWSER
