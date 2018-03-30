#include "edkuimanager.h"
#include "settingui/include/modsettingui.h"
#include <algorithm>
#include <cctype>
#include "uimanager/moduimanager.h"
#include "feature/include/modfeature.h"
#include "setting/include/modmenu.h"
#include "edkmapdefine.h"
#include "directoryui/include/directorycommon.h"
#include "directoryui/include/moddirectoryui.h"
#include "baseui/modbaseui.h"
#include "setting/include/modzero.h"
#include "mainwnd/mainwnd.h"
#include "uicommon/qtcommon/qmisc.h"
#include "edk/include/modedk.h"
#include "baseui/t4xpicpath.h"
#include "dsskey/include/moddsskey.h"
#include "uicommon/uimanager/moduimanager.h"
#include "uicommon/uikernel/qsoftkeybar.h"
#include "ETLLib/_chMisc.h"
#include "xmlparser/xmlparser.hpp"
#include "ylstl/ylstringalgorithm.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "presence/include/presence_inc.h"
#include "edkui/modedkui.h"
#include "uicommon/globalinputhook/modinputhook.h"
#include "setting/include/authorityiddefine.h"
#include "resource/modresource.h"
#include "commonunits/t2xpath.h"

#define  EDK_ARRAY_SIZE     8
#define  EDK_PROGRAMKEY_SIZE    4

CEdkUIManager & GetEDKUIManagerInstance()
{
    static CEdkUIManager s_EDKUIManager;
    return s_EDKUIManager;
}

CEdkUIAction::CEdkUIAction()
{
    Init();
}

CEdkUIAction::~CEdkUIAction()
{

}

void CEdkUIAction::Init()
{
    m_mapOpenUI.clear();
    m_mapEnterDirectoryUI.clear();
    //加载数据
    LoadXMLMapByNodeName(res_getPathByName(RESOURCE_PATH_EDKCONFIG), EDK_MAP_UI_NODE);
    LoadXMLMapByNodeName(res_getPathByName(RESOURCE_PATH_EDKCONFIG), EDK_MAP_DIRECTORY_NODE);
}

bool CEdkUIAction::OpenUI(yl::string strUIAction, int nIndex/* = 0*/)
{
    bool bResult = false;

    //特殊判断几个界面的打开
    if (SpecialUIOpen(strUIAction))
    {
        return true;
    }

    if (strUIAction.find("OpenSubPage", 0) != yl::string::npos
            || strUIAction.find("OpenPage", 0) != yl::string::npos)
    {
        EDK_INFO("DoAction OpenSubPage");
        bResult = SettingUI_OpenPage(strUIAction, nIndex);
        return bResult;
    }

    //http://10.2.1.199/Bug.php?BugID=88325 由于之前接口存在问题 故在EDK此修改调用接口
    if (strUIAction.find("OpenDirectoryPage", 0) != yl::string::npos \
            || strUIAction.find("OpenDynamic", 0) != yl::string::npos \
            || strUIAction.find("OpenMessagePage", 0) != yl::string::npos)
    {
        //http://10.2.1.199/Bug.php?BugID=88319 blacklist无法获取焦点的特殊处理
        if (strUIAction == "OpenDirectoryPage(blacklist_list.xml)")
        {
            EDK_INFO("DoAction OpenDirectoryPage(blacklist_list.xml)");
            bResult = DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId);
            return bResult;
        }
        else
        {
            EDK_INFO("DoAction DirectoryPage Or OpenDynamic Or OpenMessagePage");
            bResult = Menu_DoAction(strUIAction);
            return bResult;
        }
    }

    if (strUIAction == "OpenMenu(menu_list.xml)")
    {
        EDK_INFO("DoAction BaseUI_CreateMenu");
        bResult = BaseUI_CreateMenu("OpenMenu(menu_list.xml)");
        return bResult;
    }
    else if (strUIAction == "zero_enter")
    {
        EDK_INFO("DoAction zero_Enter");
        zero_Enter();
        bResult = true;
        return bResult;
    }

    if (strUIAction.find(".xml", 0) != yl::string::npos
            && strUIAction.find("OpenMenu", 0) != yl::string::npos)
    {
        EDK_INFO("DoAction OpenSubPage");
        bResult = SettingUI_OpenPage(strUIAction, nIndex);
        return bResult;
    }

    std::map<yl::string, int>::iterator it = m_mapEnterDirectoryUI.find(strUIAction);
    if (it != m_mapEnterDirectoryUI.end())
    {
        EDK_INFO("DoAction DM_EnterDirectoryUI");
        bResult = EnterDirectoryUI(it->second);
        return bResult;
    }

    EDK_INFO("DoAction Fail:Not match any operation");
    return bResult;
}

//打开UI操作
bool CEdkUIAction::OpenUIAction(yl::string strUIAction, int nIndex/* = 0*/)
{
    //判断是否允许进入，根据权限，配置等情况判断
    if (!EDKUI_GetIsAllowEnter(strUIAction))
    {
        EDK_INFO("%s is not find in map or is not allow enter", strUIAction.c_str());
        return false;
    }

    //判断映射中是否存在
    std::map<yl::string, yl::string>::iterator it = m_mapOpenUI.find(strUIAction);
    if (it == m_mapOpenUI.end())
    {
        EDK_INFO("%s is not find in map", strUIAction.c_str());
        return false;
    }

    //三级权限控制是否允许打开相应UI
    if (feature_UserModeIsOpen())
    {
        if (!feature_UserModeConfigItemShow(Menu_GetWebItemKeyByClickAction(it->second)))
        {
            EDK_INFO("%s is not allow in feature_user_mode", strUIAction.c_str());
            return false;
        }
    }

    return OpenUI(it->second, nIndex);
}


CEdkSoftkeyAction::CEdkSoftkeyAction()
{
    Init();
}

CEdkSoftkeyAction::~CEdkSoftkeyAction()
{

}

//初始化映射
void CEdkSoftkeyAction::Init()
{
    m_mapSoftkeyAction.clear();
    m_mapDefaultLabel.clear();
    //加载数据
    LoadXMLMapByNodeName(res_getPathByName(RESOURCE_PATH_EDKCONFIG), EDK_MAP_SF_NODE);
    LoadXMLMapByNodeName(res_getPathByName(RESOURCE_PATH_EDKCONFIG), EDK_MAP_DEFAULT_LABEL);
}

//插入当前UI的EDKsoftkey
void CEdkSoftkeyAction::InsertEDKSoftkeyData(CArraySoftKeyBarData & arrSoftKeyBarData)
{
    if (!Edk_IsEnable() || arrSoftKeyBarData.empty())
    {
        return;
    }

    YLList<SoftKey_Data> listSoftKey;
    Edk_GetEdkSoftKeyList(listSoftKey);

    if (listSoftKey.size() == 0)
    {
        return;
    }

    YLList<SoftKey_Data>::ListIterator iter = listSoftKey.begin();
    for (; iter != listSoftKey.end(); ++iter)
    {
        SoftKey_Data & dataSoftKey = *iter;

        CSoftKeyBarData SoftkeyData;
        char szEDK[EDK_ARRAY_SIZE] = {0};
        sprintf(szEDK, "%d", dataSoftKey.eType);
        SoftkeyData.m_strSoftkeyAction += SOFTKEY_EDK_ACTION;
        SoftkeyData.m_strSoftkeyAction += szEDK;
        SoftkeyData.m_strSoftkeyTitle = toQString(Edk_GetEdkSoftKeyLabel(dataSoftKey.eType));
        SoftkeyData.m_strIconPath = PIC_SOFTKEY_EDK;

        if (dataSoftKey.iPos >= arrSoftKeyBarData.size())
        {
            arrSoftKeyBarData.push_back(SoftkeyData);
        }
        else
        {
            arrSoftKeyBarData.insert(arrSoftKeyBarData.begin() + dataSoftKey.iPos, SoftkeyData);
        }
    }
}

//判断softkey在当前UI是否存在
bool CEdkSoftkeyAction::IsActionExist(yl::string strHint, yl::string & strAction)
{
    //获取顶层窗口
    CBaseDialog * pDlg =  UIManager_GetTopWindow();
    if (pDlg == NULL)
    {
        return false;
    }

    CArraySoftKeyBarData objSoftkeyBarData;
    pDlg->GetSoftkey(objSoftkeyBarData);

    bool bShowSoftkey = pDlg->IsShowSoftkey();
    if (!bShowSoftkey)
    {
        return false;
    }

    if (Edk_IsEnable())
    {
        InsertEDKSoftkeyData(objSoftkeyBarData);
    }

    yl::to_lower(strHint);

    //判断当前顶层窗口是否存在该softkey
    CArraySoftKeyBarData::iterator it = objSoftkeyBarData.begin();
    for (; it != objSoftkeyBarData.end(); it++)
    {
        yl::string strTmpAction = it->m_strSoftkeyAction;
        yl::to_lower(strTmpAction);
        if (strTmpAction == strHint)
        {
            strAction = it->m_strSoftkeyAction;
            break;
        }
    }

    return (it == objSoftkeyBarData.end() ? false : true);
}

bool CEdkSoftkeyAction::DoEdkSoftkeyAction(yl::string strHint)
{
    yl::string strAction = "";
    if (!IsActionExist(strHint, strAction))
    {
        EDK_INFO("%s is not exist in current UI", strHint.c_str());
        return false;
    }

    CBaseDialog * pDlg =  _UIManager.GetTopWnd();
    if (pDlg == NULL)
    {
        return false;
    }

    if (strAction.find(SOFTKEY_EDK_ACTION, 0) != yl::string::npos)
    {
        if (Edk_IsEnable())
        {
            //取出第四个到最后的字符，并转换位数字
            return true;
        }
        else
        {
            EDK_INFO("edk is not enable");
            return false;
        }
    }

    return pDlg->DoAction(toQString(strAction));
}

//执行softkey
bool CEdkSoftkeyAction::DoSoftkeyAction(yl::string strHint)
{
    //判断当前是否有弹出框，有则不操作softkey
    if (MessageBox_GetMessageBoxCount() > 0)
    {
        EDK_INFO("The current scene exist messagebox, return false");
        return false;
    }

    //直接响应Home,Back按钮
    if (strHint == EDK_SF_BACK)
    {
        CBaseDialog * pTopWnd = UIManager_GetTopWindow();
        EDK_INFO("Deal Back softkey.");
        if (pTopWnd != NULL)
        {
            pTopWnd->OnBackButtonClick();
        }

        return true;
    }
    else if (strHint == EDK_SF_HOME)
    {
        CBaseDialog * pTopWnd = UIManager_GetTopWindow();
        EDK_INFO("Deal Home softkey.");
        if (pTopWnd != NULL)
        {
            pTopWnd->OnHomeButtonClick();
        }

        return true;
    }

    //判断是否是EDKkey
    int nEDKType = Edk_GetEdkSoftKeyType(strHint);
    EDK_INFO("Do SoftkeyAction:%s", strHint.c_str());
    if (nEDKType != EDK_SOFTKEY_NON)
    {
        char szEDK[EDK_ARRAY_SIZE] = {0};
        sprintf(szEDK, "%d", nEDKType);
        yl::string strTmpHint = "";
        strTmpHint += SOFTKEY_EDK_ACTION;
        strTmpHint += szEDK;
        EDK_INFO("Do Customkey:%s", strTmpHint.c_str());
        return DoEdkSoftkeyAction(strTmpHint);
    }

    //判断是否是programkey
    if (UIManager_IsInIdle())
    {
        for (int nIndex = 0; nIndex < EDK_PROGRAMKEY_SIZE; nIndex++)
        {
            yl::string strDefaultLabel = GetProgramKeyDefaultLabel(nIndex);
            yl::to_lower(strDefaultLabel);
            if (strHint == strDefaultLabel)
            {
                EDK_INFO("Do Programablekey:%s", strHint.c_str());
                char szPROGRAM_ID[EDK_ARRAY_SIZE] = {0};
                sprintf(szPROGRAM_ID, "%d", nIndex);
                yl::string strTmpIndex = "";
                strTmpIndex += EDK_SOFTKEY_PROGRAM;
                strTmpIndex += szPROGRAM_ID;
                EDK_INFO("Do Programablekey:%s", strTmpIndex.c_str());
                return DoEdkSoftkeyAction(strTmpIndex);
            }
        }
    }

    std::map<yl::string, yl::string>::iterator it = m_mapSoftkeyAction.find(strHint);
    if (it == m_mapSoftkeyAction.end())
    {
        EDK_INFO("%s is not find in map", strHint.c_str());
        return false;
    }

    return DoEdkSoftkeyAction(it->second);
}

//通过key查找value m_mapSoftkeyAction
yl::string CEdkSoftkeyAction::FindValueByKey(yl::string strKey)
{
    std::map<yl::string, yl::string>::iterator it = m_mapSoftkeyAction.find(strKey);
    return it == m_mapSoftkeyAction.end() ? "" : it->second;
}

//通过value查找key m_mapSoftkeyAction
yl::string CEdkSoftkeyAction::FindKeyByValue(yl::string strValue)
{
    std::map<yl::string, yl::string>::iterator it = m_mapSoftkeyAction.begin();
    for (; it != m_mapSoftkeyAction.end(); it++)
    {
        if (it->second == strValue)
        {
            break;
        }
    }

    return it == m_mapSoftkeyAction.end() ? "" : it->first;
}

CEdkUIManager::CEdkUIManager()
{
    m_pedkSoftkeyAction = new CEdkSoftkeyAction();
    m_pedkUIAction = new CEdkUIAction();
}

CEdkUIManager::~CEdkUIManager()
{
    if (m_pedkSoftkeyAction != NULL)
    {
        delete m_pedkSoftkeyAction;
        m_pedkSoftkeyAction = NULL;
    }

    if (m_pedkUIAction != NULL)
    {
        delete m_pedkUIAction;
        m_pedkUIAction = NULL;
    }

}

//刷新softkey和title
void CEdkUIManager::RefreshUI2Hint(CBaseDialog * pBaseDlg)
{
    RereshSoftHint(pBaseDlg);
    RereshTitleHint(pBaseDlg);
}

void CEdkUIManager::RereshSoftHint(CBaseDialog * pBaseDlg)
{
    if (pBaseDlg == NULL)
    {
        return;
    }

    if (!pBaseDlg->IsShowSoftkey())
    {
        return;
    }

    _MainWND.DrawSoftkey(pBaseDlg);
}

void CEdkUIManager::RereshTitleHint(CBaseDialog * pBaseDlg)
{
    if (pBaseDlg == NULL)
    {
        return;
    }

    TitleLayoutInfo objInfo;
    if (!pBaseDlg->IsShowTitle(objInfo))
    {
        return;
    }

    UIManager_UpdateWnd(pBaseDlg);
}

bool CEdkUIManager::DoUIAction(yl::string strUIAction, int nIndex/* = 0*/)
{
    if (m_pedkUIAction == NULL)
    {
        return false;
    }

    return m_pedkUIAction->OpenUIAction(strUIAction, nIndex);
}

bool CEdkUIManager::DoSoftkeyAction(yl::string strHint)
{
    if (m_pedkSoftkeyAction == NULL)
    {
        return false;
    }

    return m_pedkSoftkeyAction->DoSoftkeyAction(strHint);
}

//通过programablekey的index查找映射中的提示模式值 m_mapDefaultLabel
yl::string CEdkSoftkeyAction::GetProgramKeyDefaultLabel(int nProgramIndex)
{
    if (nProgramIndex < 0 || nProgramIndex > 3)
    {
        return "";
    }

    int iDsskeyID = dsskey_GetDsskeyID(DMT_PROGRAMEKEY_MODULE, 0, nProgramIndex);
    DssKeyLogicData DKlogicData;
    dsskey_GetData(iDsskeyID, DKlogicData);

    int nDKType = DKlogicData.eDKType;
    yl::string strName = GetDefaultLabelByType(commonAPI_FormatString("%d", nDKType));
    yl::to_lower(strName);
    return strName;
}

yl::string CEdkUIManager::GetHintNameBySoftkeyAction(yl::string strAction)
{
    if (m_pedkSoftkeyAction == NULL)
    {
        return "";
    }

    //判断是否是programablekey
    if (strAction.find(EDK_SOFTKEY_PROGRAM, 0) != yl::string::npos)
    {
        int nProgramIndex = atoi(strAction.substr(EDK_SOFTKEY_PROGRAM_LENGTH).c_str());
        return m_pedkSoftkeyAction->GetProgramKeyDefaultLabel(nProgramIndex);
    }

    //判断是否是edkkey
    if (strAction.find(SOFTKEY_EDK_ACTION, 0) != yl::string::npos)
    {
        int nEDKType = atoi(strAction.substr(SOFTKEY_EDK_ACTION_STRING_LENGTH).c_str());
        return Edk_GetEdkSoftKeyID((EDKSOFTKEYTYPE)nEDKType);
    }

    yl::to_lower(strAction);
    yl::string strKey = m_pedkSoftkeyAction->FindKeyByValue(strAction);

    return strKey;
}

//通过key查找value m_mapOpenUI
yl::string CEdkUIAction::FindValueByKey(yl::string strKey)
{
    std::map<yl::string, yl::string>::iterator it = m_mapOpenUI.find(strKey);
    return it == m_mapOpenUI.end() ? "" : it->second;
}

//通过value查找key m_mapOpenUI
yl::string CEdkUIAction::FindKeyByValue(yl::string strValue)
{
    std::map<yl::string, yl::string>::iterator it = m_mapOpenUI.begin();
    for (; it != m_mapOpenUI.end(); it++)
    {
        if (it->second == strValue)
        {
            break;
        }
    }

    return it == m_mapOpenUI.end() ? "" : it->first;
}

yl::string CEdkUIManager::GetHintNameByUIAction(yl::string strAction)
{
    if (m_pedkUIAction == NULL)
    {
        return "";
    }

    yl::string strKey = m_pedkUIAction->FindKeyByValue(strAction);

    return strKey;
}

QString CEdkUIManager::GetHintNameByUIAction(QString strAction)
{
    yl::string strKey = GetHintNameByUIAction(fromQString(strAction));

    return toQString(strKey);
}

//获取当前窗口的提示模式的值
yl::string CEdkUIManager::GetCurPageHint()
{
    CBaseDialog * pDlg =  UIManager_GetTopWindow();
    if (pDlg == NULL)
    {
        return "";
    }

    return fromQString(pDlg->GetWindowHint());
}

//设置当前窗口提示模式的值
void CEdkUIManager::SetCurPageHint(yl::string strHint, CBaseDialog * pDlg)
{
    if (pDlg == NULL)
    {
        return;
    }

    pDlg->SetHintTitle(toQString(strHint));
}

yl::string CEdkUIManager::GetHintByIndex(int nIndex)
{
    if (m_pedkUIAction == NULL)
    {
        return "";
    }

    return m_pedkUIAction->FindKeyByIndex(nIndex);
}

//通过value查找key m_mapEnterDirectoryUI
yl::string CEdkUIAction::FindKeyByIndex(int nIndex)
{
    yl::string strTmpMap = "";
    std::map<yl::string, int>::iterator it = m_mapEnterDirectoryUI.begin();
    for (; it != m_mapEnterDirectoryUI.end(); it++)
    {
        if (it->second == nIndex)
        {
            break;
        }
    }

    return it == m_mapEnterDirectoryUI.end() ? strTmpMap : it->first;
}

//从文件加载映射
void CEdkSoftkeyAction::LoadXMLMapByNodeName(const yl::string & strFile, const yl::string & strNode)
{
    yl::string strFullPath = strFile;

    EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, strFile = %s", strFile.c_str());

    if (!pathFileExist(strFullPath.c_str()))
    {
        return;
    }

    xml_document docXmlFile;
    if (!docXmlFile.load_file(strFullPath.c_str()))
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, Load File Fail, strFile = %s", strFile.c_str());
        return;
    }

    xml_node xmlRoot = docXmlFile.first_child();
    if (xmlRoot.empty())
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, First Child is Empty, strFile = %s", strFile.c_str());
        return;
    }

    //判断名字
    yl::string strBaseName = xmlRoot.name();
    if (0 != strBaseName.compare(EDK_MAP_ROOT_NODE))
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, First Child Name Error, strFile = %s, strBaseName = %s",
                 strFile.c_str(), strBaseName.c_str());
        return;
    }

    xml_node xmlChildData = xmlRoot.child(strNode.c_str());
    yl::string strChildName = xmlChildData.name();
    if (0 != strChildName.compare(strNode))
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, Child Name Error, strFile = %s, strChildName = %s",
                 strFile.c_str(), strChildName.c_str());
        return;
    }

    xml_node xmlItemData = xmlChildData.child(EDK_MAP_ITEM_NODE);
    while (!xmlItemData.empty())
    {
        if (strcmp(EDK_MAP_ITEM_NODE, xmlItemData.name()) != 0)
        {
            xmlItemData = xmlItemData.next_sibling();
            continue;
        }

        if (strNode == EDK_MAP_SF_NODE)
        {
            yl::string strTempHint = xmlItemData.attribute(EDK_MAP_HINT_ATTR).value();
            yl::string strTempAction = xmlItemData.attribute(EDK_MAP_ACTION_ATTR).value();

            m_mapSoftkeyAction.insert(std::pair<yl::string, yl::string>(strTempHint, strTempAction));
        }
        else if (strNode == EDK_MAP_DEFAULT_LABEL)
        {
            yl::string strTempType = xmlItemData.attribute(EDK_MAP_TYPE_ATTR).value();
            yl::string strTempSFId = xmlItemData.attribute(EDK_MAP_TYPE_SFID).value();

            m_mapDefaultLabel.insert(std::pair<yl::string, yl::string>(strTempType, strTempSFId));
        }
        else
        {
            break;
        }

        xmlItemData = xmlItemData.next_sibling();
    }
}

//从文件加载映射
void CEdkUIAction::LoadXMLMapByNodeName(const yl::string & strFile, const yl::string & strNode)
{
    yl::string strFullPath = strFile;

    EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, strFile = %s", strFile.c_str());

    if (!pathFileExist(strFullPath.c_str()))
    {
        return;
    }

    xml_document docXmlFile;
    if (!docXmlFile.load_file(strFullPath.c_str()))
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, Load File Fail, strFile = %s", strFile.c_str());
        return;
    }

    xml_node xmlRoot = docXmlFile.first_child();
    if (xmlRoot.empty())
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, First Child is Empty, strFile = %s", strFile.c_str());
        return;
    }

    //判断名字
    yl::string strBaseName = xmlRoot.name();
    if (0 != strBaseName.compare(EDK_MAP_ROOT_NODE))
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, First Child Name Error, strFile = %s, strBaseName = %s",
                 strFile.c_str(), strBaseName.c_str());
        return;
    }

    xml_node xmlChildData = xmlRoot.child(strNode.c_str());
    yl::string strChildName = xmlChildData.name();
    if (0 != strChildName.compare(strNode))
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, Child Name Error, strFile = %s, strChildName = %s",
                 strFile.c_str(), strChildName.c_str());
        return;
    }

    xml_node xmlItemData = xmlChildData.child(EDK_MAP_ITEM_NODE);
    while (!xmlItemData.empty())
    {
        if (strcmp(EDK_MAP_ITEM_NODE, xmlItemData.name()) != 0)
        {
            xmlItemData = xmlItemData.next_sibling();
            continue;
        }

        if (strNode == EDK_MAP_UI_NODE)
        {
            yl::string strTempHint = xmlItemData.attribute(EDK_MAP_HINT_ATTR).value();
            yl::string strTempAction = xmlItemData.attribute(EDK_MAP_ACTION_ATTR).value();

            m_mapOpenUI.insert(std::pair<yl::string, yl::string>(strTempHint, strTempAction));
        }
        else if (strNode == EDK_MAP_DIRECTORY_NODE)
        {
            yl::string strTempHint = xmlItemData.attribute(EDK_MAP_HINT_ATTR).value();
            yl::string strTempAction = xmlItemData.attribute(EDK_MAP_ACTION_ATTR).value();

            m_mapEnterDirectoryUI.insert(std::pair<yl::string, int>(strTempAction, atoi(strTempHint.c_str())));
        }
        else
        {
            break;
        }

        xmlItemData = xmlItemData.next_sibling();
    }
}

//通过softkey的type获取提示模式值
yl::string CEdkSoftkeyAction::GetDefaultLabelByType(yl::string strType)
{
    std::map<yl::string, yl::string>::iterator it = m_mapDefaultLabel.find(strType);
    return (it == m_mapDefaultLabel.end() ? "" : it->second);
}

//根据规则进入联系人界面
bool CEdkUIAction::EnterDirectoryUI(int nType)
{
    bool nResult = false;

    switch (nType)
    {
    case DIRECTORY_TYPE_BTUC:
        nResult = DM_EnterDirectoryUI(nType, ALL_BUDDIES_ID);
        break;
    case DIRECTORY_TYPE_BLUETOOTH:
        nResult = DM_EnterDirectoryUI(nType, ALL_BUDDIES_ID);
        break;
    default:
        nResult = DM_EnterDirectoryUI(nType);
        break;
    }

    return nResult;
}

bool CEdkUIAction::SpecialUIOpen(yl::string strUIAction)
{
    if (strUIAction == EDK_IPV4)
    {
        SubMenuData objData(EDK_UI_ACTION_NETWORK_WANPORT, 0);
        int nType = 0;
        objData.pBeforeSetSubpageData = (void *)&nType;
        return SettingUI_OpenPage(objData);
    }
    else if (strUIAction == EDK_IPV6)
    {
        SubMenuData objData(EDK_UI_ACTION_NETWORK_WANPORT, 0);
        int nType = 1;
        objData.pBeforeSetSubpageData = (void *)&nType;
        return SettingUI_OpenPage(objData);
    }

    return false;
}
