#include "logic_inc.h"

#ifdef IF_FEATURE_XMLBROWSER

// XMLBrowser的配置更新路径，交给AUTOP处理
#define XML_BROWSER_CONFIG_PATH   TEMP_PATH"xml_browser_config.cfg"
#define XML_PRE_CONFIG_FILE     "xml_browser_config"
// M7的文件前缀版本
#define AUTOP_M7_VERSION          "#!version:1.0.0.1\n\n"

#if IF_FEATURE_XMLBROWSER_HOTELING
#include "configserver/configserverinc.h"
#endif

#include "autopcontrol/include/modautopcontrol.h"

//////////////////////////////////////////////////////////////////////////
yl::string xGetUniqueConfigName()
{
    static int s_iTempFileId = 0;
    if (s_iTempFileId >= MAX_TEMP_FILE)
    {
        s_iTempFileId = 0;
    }
    // 生成本地保存路径.
    yl::string strFormat(TEMP_PATH);
    strFormat.append(XML_PRE_CONFIG_FILE);
    char szFileId[8] = {0};
    sprintf(szFileId, "%d", ++s_iTempFileId);
    strFormat.append(szFileId);
    strFormat.append(".cfg");

    XMLBROWSER_INFO("GetConfigFileName(%s)", strFormat.c_str());
    return strFormat;
}

#if IF_BUG_25845
yl::string CElementConfig::m_strAutopUrl;
#endif

CElementConfig::CElementConfig()
{
}

CElementConfig::~CElementConfig()
{

}

// 创建之后的处理
bool CElementConfig::AfterCreate()
{

    // 取消提示框
    _XMLBrowserManager.ExitMessageBox();
    // 播放提示音
    if (IsNeedPlayBeep())
    {
        _XMLBrowserManager.PlayBeep();
    }

#if IF_FEATURE_XMLBROWSER_HOTELING
    //弹提示信息
    ShowTips();
#endif

    // 更改配置文件
    ModifyConfig();

    return true;
}

// 销毁之前的处理
void CElementConfig::BeforeDestroy()
{

}

// 是否播放提示音
bool CElementConfig::IsNeedPlayBeep()
{
    return _GetXData().stAttr.bBeep;
}

// 修改配置
void CElementConfig::ModifyConfig()
{
    bool bModifySuccess = false;

    // 解决多条Excute执行config时，就只有最后那条生效的问题，每条命令一个文件
    // autop执行完，删除
    yl::string strConfigFile;
    FILE * hFile = NULL;
#if IF_BUG_25845
    if (m_strAutopUrl.empty())
    {
        strConfigFile = xGetUniqueConfigName();
        hFile = fopen(strConfigFile.c_str(), "w");
    }
    else
    {
        strConfigFile = m_strAutopUrl;
        hFile = fopen(strConfigFile.c_str(), "a");
    }
#endif

    if (hFile == NULL)
    {
        XMLBROWSER_WARN("open failed. [%s]", strConfigFile.c_str());
        return;
    }

    // 写文件
    if (_GetXData().eCfgModel == XCM_M7
#if IF_BUG_25845
            && m_strAutopUrl.empty()
#endif
       )
    {
        if (fwrite(AUTOP_M7_VERSION, strlen(AUTOP_M7_VERSION), 1, hFile) != 1)
        {
            fclose(hFile);
            hFile = NULL;
            XMLBROWSER_WARN("write value filed. %s", AUTOP_M7_VERSION);
            return;
        }
    }

    List_Config_Item & listConfigItem = _GetXData().listItem;

    yl::string strSectionName = "";
    // 遍历Path链表
    List_Config_Item::iterator iter = listConfigItem.begin();
    for (; iter != listConfigItem.end(); ++iter)
    {
        if ((*iter) == NULL)
        {
            continue;
        }
        ConfigItem * pItem = (*iter);
        if (pItem->strValue.empty())
        {
            continue;
        }

        // 写文件
        if (_GetXData().eCfgModel == XCM_M7)
        {
            if (IsDsskeyConfigChange(pItem->strValue))
            {
                if (fwrite(pItem->strValue.c_str(), pItem->strValue.length(), 1, hFile) != 1)
                {
                    XMLBROWSER_INFO("write value filed. %s", pItem->strValue.c_str());
                    continue;
                }
                fwrite("\n", 1, 1, hFile);
                bModifySuccess = true;
            }
        }
        else
        {
            yl::string strLineBuf = "";
            if (strSectionName != pItem->strSection)
            {
                // section不同，才新加section
                strSectionName = pItem->strSection;

                strLineBuf.append("[ ");
                strLineBuf.append(pItem->strSection);
                strLineBuf.append(" ]\n");
                fwrite(strLineBuf.c_str(), strLineBuf.length(), 1, hFile);

                strLineBuf = "path = ";
                strLineBuf.append(pItem->strPath);
                strLineBuf.append("\n");
                fwrite(strLineBuf.c_str(), strLineBuf.length(), 1, hFile);
            }

            strLineBuf = pItem->strParam;
            strLineBuf.append(" = ");
            strLineBuf.append(pItem->strValue);
            strLineBuf.append("\n");
            fwrite(strLineBuf.c_str(), strLineBuf.length(), 1, hFile);
            bModifySuccess = true;
        }
    }

    fclose(hFile);
    hFile = NULL;

    // 发消息通知Dsskey和SIP，配置改变
    if (bModifySuccess)
    {
#if IF_FEATURE_XMLBROWSER_HOTELING
        if (_GetXData().stAttr.eType == SET_HOTELING)
        {
            yl::string strAutopUrl = strConfigFile;
            XMLBROWSER_INFO("SET_HOTELING post message[%d] to autop for config [%s] updating ! ",
                            ATP_MSG_REQ_START_BY_URL, strConfigFile.c_str());
            if (_XMLBrowserManager.IsHotelingLogin())
            {
                //若已经登陆过了则不需要重新清空配置
                if (configParser_GetConfigInt(kszHotelingLoginStatus) != 1)
                {
                    msgSendMsgToThread(msgGetThreadByName(CONFIG_SERVER_THREAD_NAME), CSM_CLEAR_USERCFG, 5, 0);
                }

                timerSetThreadTimer(TIMER_ID(m_strAutopUrl), 3 * 1000);
                m_strAutopUrl = strConfigFile;

                //Autop_NotifyAutoServerEx(false, ATP_MSG_REQ_START_BY_URL, 0, 0,
                //  strAutopUrl.length() + 1, (LPCVOID)(strAutopUrl.c_str()));
            }
            else
            {
                Autop_NotifyAutoServer(false, ATP_MSG_REQ_UPLOAD_USERCFG, 514, 0);

                Autop_NotifyAutoServerEx(false, ATP_MSG_REQ_START_BY_URL, 0, 0,
                                         strAutopUrl.length() + 1, (LPCVOID)(strAutopUrl.c_str()));

                msgSendMsgToThread(msgGetThreadByName(CONFIG_SERVER_THREAD_NAME), CSM_CLEAR_USERCFG, 5, 0);
            }
            return;
        }
#endif

#if IF_BUG_25845
        if (talklogic_SessionExist())
        {
            timerSetThreadTimer(TIMER_ID(m_strAutopUrl), 300);
            m_strAutopUrl = strConfigFile;
        }
        else
#endif
        {
            XMLBROWSER_INFO("post message[%d] to autop for config [%s] updating ! ",
                            ATP_MSG_REQ_START_BY_URL, strConfigFile.c_str());

            yl::string strAutopUrl = strConfigFile;
            // yl::string strAutopUrl = "http://10.3.3.4:9/update/T2X/123.cfg";

            Autop_NotifyAutoServerEx(false, ATP_MSG_REQ_START_BY_URL, 0, 0,
                                     strAutopUrl.length() + 1, (LPCVOID)(strAutopUrl.c_str()));
        }
    }
    else
    {
        remove(strConfigFile.c_str());
    }
}

// 判断Dsskey配置是否已改变，如果改变才保存
bool CElementConfig::IsDsskeyConfigChange(const yl::string & strItem)
{
    // 解析配置信息：expansion_module.1.key.1.label=Keine Funktion， linekey.23.label=Keine Funktion
    DSSKEY_MODULE_TYPE eDsskeyModuleType = DMT_NON;
    int nExpIndex = -1;
    int nDsskeyIndex = -1;
    yl::string strKey, strValue;

    if (!ParseDsskeyItemData(strItem, eDsskeyModuleType, nExpIndex, nDsskeyIndex, strKey, strValue))
    {
        return true;
    }

    if (nExpIndex != -1)
    {
        nDsskeyIndex = dsskey_GetDsskeyID(eDsskeyModuleType, nExpIndex, nDsskeyIndex);
    }
    //获取本地Dsskey的属性
    DssKeyLogicData dsskeyData;
    dsskey_GetData(nDsskeyIndex, dsskeyData);

    bool bChange = false;
    //判断值是否有改变
    if (strKey == "value")
    {
        bChange = dsskeyData.strValue != strValue;
    }
    else if (strKey == "type")
    {
        //http://10.2.1.199/Bug.php?BugID=92494
        if (strValue.empty())
        {
            bChange = true;
        }
        else
        {
            int nType = atoi(strValue.c_str());
            bChange = dsskeyData.eDKType != (DssKey_Type)nType;
        }
    }
    else if (strKey == "label")
    {
        bChange = dsskeyData.strLabel != strValue;
    }
    else if (strKey == "line")
    {
        int nLineID = atoi(strValue.c_str());
        bool bAutoLine = configParser_GetConfigInt(kszAutoLine) == 0;
        if (bAutoLine && nLineID > 0)
        {
            nLineID -= 1;
        }
        bChange = dsskeyData.iLineID != nLineID;
    }
    else if (strKey == "extension" || strKey == "pickup_value")
    {
        bChange = dsskeyData.strExtension != strValue;
    }

    return bChange;
}

bool CElementConfig::ParseDsskeyItemData(const yl::string & strItem,
        DSSKEY_MODULE_TYPE & eDsskeyModuleType,
        int & nExpIndex,
        int & nDsskeyIndex,
        yl::string & strKey,
        yl::string & strValue)
{
    yl::string strTemp = strItem;

    // 获取Dsseky module type， strItem = “expansion_module.1.key.1.label=Keine Funktion”
    eDsskeyModuleType = DMT_NON;
    yl::string::size_type nPos = strTemp.find_first_of('.');
    if (nPos != yl::string::npos)
    {
        yl::string strModule = strTemp.substr(0, nPos);
        strTemp = strTemp.substr(nPos + 1);
        if (strModule == "linekey")
        {
            eDsskeyModuleType = DMT_LINEKEY_MODULE;
        }
        else if (strModule == "expansion_module")
        {
            eDsskeyModuleType = (DSSKEY_MODULE_TYPE)exp_GetEXPType(); //此处去Dsskey模块获取exp类型
        }
    }

    if (eDsskeyModuleType == DMT_NON)
    {
        return false;
    }

    // 获取Dsskey序号， strItem = “1.key.1.label=Keine Funktion”
    if (eDsskeyModuleType == (DSSKEY_MODULE_TYPE)exp_GetEXPType())
    {
        nExpIndex = -1;
        nPos = strTemp.find_first_of('.');
        if (nPos != yl::string::npos)
        {
            yl::string strExpIndex = strTemp.substr(0, nPos);
            strTemp = strTemp.substr(nPos + 1);

            nExpIndex = atoi(strExpIndex.c_str()) - 1;
        }

        // 去掉key. 这几个字符
        nPos = strTemp.find_first_of('.');
        if (nPos != yl::string::npos)
        {
            strTemp = strTemp.substr(nPos + 1);
        }
    }

    // 获取Dsskey序号， strItem = “1.label=Keine Funktion”
    nDsskeyIndex = -1;
    nPos = strTemp.find_first_of('.');
    if (nPos != yl::string::npos)
    {
        yl::string strKeyIndex = strTemp.substr(0, nPos);
        strTemp = strTemp.substr(nPos + 1);

        nDsskeyIndex = atoi(strKeyIndex.c_str()) - 1;
    }

    // 获取配置项名和值， strItem = “label=Keine Funktion”
    nPos = strTemp.find_first_of('=');
    if (nPos != yl::string::npos)
    {
        strKey = strTemp.substr(0, nPos);
        strKey = commonAPI_TrimString(strKey, " ");
        strValue = strTemp.substr(nPos + 1);
        strValue = commonAPI_TrimString(strValue, " ");
    }

    if (strValue.compare("%NULL%") == 0
            || strValue.compare("!NULL!") == 0)
    {
        strValue = "";
    }

    return true;
}

#if IF_BUG_25845
bool CElementConfig::OnTimer(UINT uTimerID)
{
    if (uTimerID == TIMER_ID(m_strAutopUrl))
    {
        if (talklogic_SessionExist())
        {
            // 不杀定时器,等下次再检查
            return false;
        }

        timerKillThreadTimer(uTimerID);

        XMLBROWSER_INFO("post message to autop for config [%s] updating ! ", m_strAutopUrl.c_str());

        Autop_NotifyAutoServerEx(false, ATP_MSG_REQ_START_BY_URL, 0, 0,
                                 m_strAutopUrl.length() + 1, (LPCVOID)(m_strAutopUrl.c_str()));

        m_strAutopUrl.clear();

        return true;
    }

    return false;
}
#endif

#if IF_FEATURE_XMLBROWSER_HOTELING
void CElementConfig::ShowTips()
{
    yl::string strMessage = _GetXData().stAttr.strMessage;
    if (strMessage.empty())
    {
        return;
    }

    int iMessageTime = _GetXData().stAttr.iMessageTime;

    _XMLBrowserManager.ShowTips(strMessage, iMessageTime * 1000);
}
#endif

#endif // #ifdef IF_FEATURE_XMLBROWSER

