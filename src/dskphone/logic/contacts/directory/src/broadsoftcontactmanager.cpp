#include "broadsoftcontactmanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "basemethod.h"
#include "bsftdirectoryaction.h"
#include "taskaction/modtaskaction.h"
#include "threadtask/include/modthreadtask.h"
#include "directorystructure.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "directorymessagedefine.h"
#include "dircommonfunc.h"
#include "ylalgorithm.h"

#include "commonunits/modcommonunits.h"
#include "localcontactmanager.h"
#include "contacts/directory/include/moddirinterface.h"

#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"

// BroadSoft文件下载到本地的保存名.
#define kszFileSavedName  "BroadSoftDirectory_%d.xml"

//BroadSoft联系人配置绑定账号id
#define knAccountId 0

#define MAX_FILE_ID 20

#define kszPersonalFile  "Personal.xml"

#define kszRootNode  "Personal"

bool CompareBsftContact(int nLeftId, int nRightId)
{
    return g_pBroadSoftDirManager->GetCompResult(nLeftId, nRightId) < 0;
}

CBroadSoftContactManager * CBroadSoftContactManager::GetInstance()
{
    static CBroadSoftContactManager instance;
    return &instance;
}

CBroadSoftContactManager::CBroadSoftContactManager()
{
    m_eDirType = BSFT_DIR;

    m_iInitTimer = 5 * 1000;
}

CBroadSoftContactManager::~CBroadSoftContactManager()
{

}

bool CBroadSoftContactManager::MyInit()
{
    CBaseContactManger::MyInit();

    m_refLastSearchData.Reset();

    if (MyCreatGroupsFromConfig())
    {
        //保存组信息到临时文件，供网页使用
        SaveGroupInfoToTempFile("BroadsoftGroupInfo.xml", "BroadsoftGroup");

        if (BsftDir_UpdateEnable())
        {
            SetAutoUpdateTimer();
        }
        else
        {
            // 停止定时器
            StopAutoUpdateTimer();
        }

        //创建All组
        ContactBroadSoftMenu * pServerSearch = new ContactBroadSoftMenu;
        if (pServerSearch != NULL)
        {
            pServerSearch->m_strDisplayName = "~";
            //状态始终为OK
            pServerSearch->m_eStatus = FILE_DOWNLOADED_OK;

            InsertMap(pServerSearch, kszServerSearchGroupId);
        }
        return true;
    }
    return false;
}

bool CBroadSoftContactManager::PreLoad(int nGroupId/* = knInvalidRId*/, bool bLoad /* = true */)
{
    DIR_INFO("load bsft contact group[%d] load[%d]", nGroupId, bLoad);
    MyFinal();

    if (MyInit())
    {
        if (!BsftDir_UpdateEnable()
                && (!bLoad || nGroupId == knInvalidRId))
        {
            return true;
        }
        if (bLoad && IsDirAvailable())
        {
            if (IsNetworkConnected())
            {
                //默认预加载第一个非All组
                if (nGroupId == knInvalidRId || nGroupId == kszAllContactId)
                {
                    nGroupId = m_vGroupId[1];
                }

                if (FILE_DOWNLOAD_FAIL == MyExecDownload(nGroupId, false, true))
                {
                    return false;
                }
            }
            else
            {
                timerSetThreadTimer((UINT)&m_iInitTimer, m_iInitTimer);
            }

        }
        return true;
    }

    return false;
}

void CBroadSoftContactManager::SetAutoUpdateTimer()
{
    if (IsDirAvailable())
    {
        // 读取定时器时间
        using namespace yl;
        int iUpdateInterval = max(configParser_GetConfigInt(kszBstDirUpdateInterVal), 60);
        if (iUpdateInterval > 43200 || iUpdateInterval < 0)
        {
            iUpdateInterval = 60;
        }

        m_iTimerID = iUpdateInterval * 60 * 1000;

        timerSetThreadTimer((UINT)&m_iTimerID, m_iTimerID);
    }
}


void CBroadSoftContactManager::StopAutoUpdateTimer()
{
    if (m_iTimerID > 0)
    {
        timerKillThreadTimer((UINT)&m_iTimerID);
        m_iTimerID = 0;
    }
}

DIRECTORY_RET_CODE CBroadSoftContactManager::Update(int nGroupId /* = kszAllContactId */)
{
    DIR_INFO("NetworkDir Update (nGroupId = %d)", nGroupId);
    if (!IsNetworkConnected())
    {
        DIR_INFO("network is unuseable!");
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    if (nGroupId != kszAllContactId)
    {
        ReqLoadContact(nGroupId);
    }
    else
    {
        if (!IsDirAvailable())
        {
            DIR_INFO("Dir Not Config!");

            return DIR_RET_GROUP_UNEXISTED;
        }

        bool isPreLoadNotOver = false;

        for (int i = 0; i < m_vGroupId.size(); i++)
        {
            ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(m_vGroupId[i]));

            //已在下载中不在进行下载
            if (m_vGroupId[i] == kszAllContactId || pMenu == NULL)
            {
                continue;
            }

            if (pMenu->m_eStatus == FILE_DOWNLOADING || pMenu->m_bReqNext)
            {
                isPreLoadNotOver = true;
                continue;
            }

            ClearContactByGroupID(m_vGroupId[i]);

            pMenu->m_mapNameToId.clear();
            pMenu->m_eStatus = FILE_DOWNLOAD_READY;

            MyExecDownload(m_vGroupId[i]);
        }

        if (!isPreLoadNotOver)
        {
            //容错排序错乱问题。
            ClearChildIDListByGroupID(kszAllContactId);
        }
    }
    return DIR_RET_SUCC;
}

bool CBroadSoftContactManager::MyCreatGroupsFromConfig()
{
    //重置
    m_bIsOnlySearchEnterPrise = false;

    if (configParser_GetConfigInt(kszBroadsoftActive) != 1
            || configParser_GetConfigInt(kszBsftEnableDirectory) == 0
            || configParser_GetConfigInt(kszXSISwitch) == 0)
    {
        DIR_WARN("disable contact");
        return false;
    }
    //sip XSI 鉴权

    static bool s_bBootFirst = true; // 是否开机第一次
    if (!s_bBootFirst)
    {
        // 第一次初始化加载的时候不需要写custom配置
        ResetCustomListStatus();
    }
    else
    {
        s_bBootFirst = false;
    }


    bool bIsAllSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);

    yl::string strXSIUserId = configParser_GetCfgItemStringValue(kszAccountXSIUserID, knAccountId);

    if (bIsAllSipAuth)
    {
        //userId为sip UserName  passwordc存的是userID：sip passward
        m_strUser = configParser_GetCfgItemStringValue(kszAccountRegisterName, knAccountId);
        m_strPassword = strXSIUserId;
        m_strPassword.append(":").append(configParser_GetCfgItemStringValue(kszAccountPassword,
                                         knAccountId).c_str());
    }
    else
    {
        m_strUser = strXSIUserId;
        m_strPassword = configParser_GetCfgItemStringValue(kszAccountXSIPassword, knAccountId);
    }

    m_strServer = configParser_GetCfgItemStringValue(kszAccountXSIHostAddr, knAccountId);
    m_strPort = configParser_GetCfgItemStringValue(kszAccountXSIPort, knAccountId);
    yl::string strServerType = configParser_GetCfgItemStringValue(kszAccountXSIServerType, knAccountId);
    yl::string strVirsion = configParser_GetConfigString(kszBsftVersion);
    if (strVirsion.empty())
    {
        strVirsion = "v2.0";
    }

    yl::string strCustomURL = configParser_GetCfgItemStringValue(kszAccountXSICustomURL, knAccountId);

    if (strCustomURL.empty())
    {
        strCustomURL = BRROADWORKS_DEFAULT_URL;
    }

    if (!m_strServer.empty() && !strXSIUserId.empty() && !strServerType.empty()
            && (!bIsAllSipAuth || !m_strUser.empty()))
    {
        //m_strServer = commonAPI_FormatString(BRROADWORKS_CONTACT_CMD_PREFEX, strServerType.c_str(), m_strServer.c_str(),m_strPort.c_str(),strVirsion.c_str(),strXSIUserId.c_str());
        m_strServer = commonAPI_FormatString(BRROADWORKS_CONTACT_NEW_CMD_PREFEX, strServerType.c_str(),
                                             m_strServer.c_str(), m_strPort.c_str(), strCustomURL.c_str(), strVirsion.c_str(),
                                             strXSIUserId.c_str());

        if (configParser_GetConfigInt(kszBsftGroup) == 1)
        {
            CreatConfigGroup(kszGroupId, kszBsftGroupName, "Group", m_strServer);
        }

        if (configParser_GetConfigInt(kszBsftEnterprise) == 1)
        {
            CreatConfigGroup(kszEnterPriseId, kszBsftEnterpriseName, "Enterprise", m_strServer);
            m_bIsOnlySearchEnterPrise = true;
        }

        if (configParser_GetConfigInt(kszBsftGroupCommon) == 1)
        {
            CreatConfigGroup(kszGroupCommonId, kszBsftGCommonName, "GroupCommon", m_strServer, "G-Common");
        }

        if (configParser_GetConfigInt(kszBsftEnterpriseCommon) == 1)
        {
            CreatConfigGroup(kszEnterPriseCommonId, kszBsftECommonName, "EnterpriseCommon", m_strServer,
                             "E-Common");
        }

        if (configParser_GetConfigInt(kszBsftPersonal) == 1)
        {
            CreatConfigGroup(kszPersonalId, kszBsftPersonalName, "Personal", m_strServer);
        }

        if (configParser_GetConfigInt(kszBsftCustom) == 1)
        {
            CreatConfigGroup(kszCustomId, kszBsftCustomName, "CustomContact", m_strServer);
        }

        return true;
    }
    return false;
}

void CBroadSoftContactManager::CreatConfigGroup(int nGroupId, const char * lpszConfigName,
        yl::string strName, const yl::string & strServer, const yl::string & strDefaultName/* = ""*/)
{
    ContactBroadSoftMenu * pNew = new ContactBroadSoftMenu();

    pNew->m_strFullUrl = strServer + strName;

    pNew->m_strDisplayName = configParser_GetConfigString(lpszConfigName);

    //空值有UI去进行取默认值和进行翻译
//  if (pNew->m_strDisplayName.empty())
//  {
//      pNew->m_strDisplayName = strDefaultName.empty() ? strName : strDefaultName;
//  }

    DIR_INFO("Add Group: %d %s %s", nGroupId, pNew->m_strDisplayName.c_str(),
             pNew->m_strFullUrl.c_str());

    InsertMap(pNew, nGroupId);

    AddIdToMenuShowList(nGroupId);
}

bool CBroadSoftContactManager::IsNeedShowAllGroup()
{
    return BsftDir_UpdateEnable();
}

int CBroadSoftContactManager::MyExecDownload(int GroupId, bool isSearch /* = false */,
        bool isPreLoad /* = false */,
        const yl::string & strStartIndex /* =  */,
        const yl::string & strCountPerPage /* = */)
{
    yl::string strUrl;
    // 生成本地保存路径.
    yl::string strFullPath(g_strTempPath);

    if (++m_nFileIndex == MAX_FILE_ID)
    {
        m_nFileIndex = 0;
    }

    //Refresh需同时下载多个，一个文件会出现覆盖写的问题。
    char szTemp[100];
    memset(szTemp, 0, 100);
    sprintf(szTemp, kszFileSavedName, m_nFileIndex);
    strFullPath.append(szTemp);

    ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetDataById(GroupId));

    if (pGroup == NULL)
    {
        DIR_WARN("get group fail");
        return FILE_DOWNLOAD_FAIL;
    }
    else if (pGroup->m_eStatus == FILE_DOWNLOADING)
    {
        return FILE_DOWNLOADING;
    }

    strUrl =  isSearch ? pGroup->m_strSearchUrl : pGroup->m_strFullUrl;

    if (!BsftDir_UpdateEnable()
            && (GroupId == kszGroupId
                || GroupId == kszEnterPriseId))
    {
        //排序方式为firstname
        AddUrlParameter(strUrl, "sortColumn", "firstName", false);
    }

    if (!strStartIndex.empty() && !strCountPerPage.empty())
    {
        AddUrlParameter(strUrl, "start", strStartIndex, false);
        AddUrlParameter(strUrl, "results", strCountPerPage, false);
    }

    DIR_INFO("CBSFTDirProcessor::MyExecDownload url[%s]", strUrl.c_str());

    if (!strUrl.empty())
    {
        CBSFTDirectoryAction * pAction = new CBSFTDirectoryAction(strFullPath.c_str(), strUrl.c_str());
        if (pAction != NULL)
        {
            pAction->SetBsftSearch(isSearch);
            pAction->SetExtraParam(GroupId);//保存Id

            if (!isSearch)
            {
                pGroup->m_eStatus = FILE_DOWNLOADING;
            }

            pAction->SetRead(true);  // 读取文件.
            pAction->SetUsername(m_strUser);
            pAction->SetPassword(m_strPassword);
            pAction->SetCallback(this);  // 回调函数.
            pAction->SetPreLoad(isPreLoad);
            bool bIsAllSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);

            if (bIsAllSipAuth)
            {
                pAction->SetDownLoadMode(MODE_SIP_XSI_AUTH);
            }
            else if (CBaseHelper::IsXsiAvailable(0))
            {
                pAction->SetDownLoadMode(MODE_BASIC_XSI_AUTH);
            }

            pAction->SetSync(false);  // 异步.
            return TaskAction_ExecAction(pAction)
                   ? FILE_DOWNLOADING : FILE_DOWNLOAD_FAIL;
        }
    }
    // 其他情况, 则下载失败.
    return FILE_DOWNLOAD_FAIL;
}

bool CBroadSoftContactManager::MyExecDelete(int nId)
{
    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(kszPersonalId));
    if (pMenu == NULL)
    {
        return false;
    }
    yl::string strFullUrl = pMenu->m_strFullUrl;

    yl::string strDispalyName = GetDisplayNameById(nId);

    if (strDispalyName.empty())
    {
        return false;
    }
    strFullUrl.append("/").append(strDispalyName);

    yl::string strLocalFile = "xxx";//不为空即可

    DIR_INFO("CContactBroadSoftManager::MyExecDelete,url = %s", strFullUrl.c_str());
    CBSFTDirectoryAction * pAction = new CBSFTDirectoryAction(strLocalFile.c_str(), strFullUrl.c_str());

    if (pAction != NULL)
    {
        pAction->SetExtraParam(nId);
        pAction->SetRead(false);        // post文件.
        pAction->SetDelete(true);       // Delete方法
        pAction->SetSync(false);        // 异步.
        pAction->SetUsername(m_strUser);
        pAction->SetPassword(m_strPassword);
        pAction->SetCallback(this);  // 回调函数.
        bool bIsAllSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);

        if (bIsAllSipAuth)
        {
            pAction->SetDownLoadMode(MODE_SIP_XSI_AUTH);
        }
        else if (CBaseHelper::IsXsiAvailable(0))
        {
            pAction->SetDownLoadMode(MODE_BASIC_XSI_AUTH);
        }

        return TaskAction_ExecAction(pAction);
    }
    return false;
}

bool CBroadSoftContactManager::MyExecPost(const yl::string & strUrl)
{
    DIR_INFO("CContactBroadSoftManager::MyExecPost url = %s", strUrl.c_str());
    yl::string strFullPath(g_strConfigPath);
    strFullPath.append(kszPersonalFile);

    CBSFTDirectoryAction * pAction = new CBSFTDirectoryAction(strFullPath.c_str(), strUrl.c_str());

    if (pAction != NULL)
    {
        pAction->SetRead(false);  // post文件.
        pAction->SetSync(false);  // 异步.
        pAction->SetCallback(this);  // 回调函数.
        pAction->SetUsername(m_strUser);
        pAction->SetPassword(m_strPassword);
        bool bIsAllSipAuth = (configParser_GetConfigInt(kszAllowSipAuth) == 1);
        if (bIsAllSipAuth)
        {
            pAction->SetDownLoadMode(MODE_SIP_XSI_AUTH);
        }
        else if (CBaseHelper::IsXsiAvailable(0))
        {
            pAction->SetDownLoadMode(MODE_BASIC_XSI_AUTH);
        }

        TaskAction_ExecAction(pAction);
        return true;
    }
    return false;
}

bool CBroadSoftContactManager::AddParameters(int GroupId, ContactBroadSoftItemData & refSearchData)
{
    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(GroupId));

    if (pMenu != NULL)
    {
        pMenu->m_strSearchUrl = pMenu->m_strFullUrl;
        //Name
        if (!refSearchData.m_strDisplayName.empty())
        {
            int index = refSearchData.m_strDisplayName.find(",");
            yl::string strFirstName;
            yl::string strLastName;
            if (index != yl::string::npos)
            {
                strFirstName = refSearchData.m_strDisplayName.substr(0, index);
                strLastName = refSearchData.m_strDisplayName.substr(index + 1);
            }

            // 都为空，取原本名字
            if (!BsftDir_UpdateEnable()
                    && strFirstName.empty()
                    && strLastName.empty())
            {
                strFirstName = refSearchData.m_strDisplayName;
                strLastName = refSearchData.m_strDisplayName;
            }
            if (IsPersonal(GroupId) || IsCommonDir(GroupId))
            {
                yl::string strName = commonAPI_TrimString(refSearchData.m_strDisplayName, ",");
                AddUrlParameter(pMenu->m_strSearchUrl, kszNodeName, strName, true, false);
            }
            else
            {
                if (!strFirstName.empty())
                {
                    AddUrlParameter(pMenu->m_strSearchUrl, kszNodeFirstName, strFirstName, true, false);
                }
                if (!strLastName.empty())
                {
                    AddUrlParameter(pMenu->m_strSearchUrl, kszNodeLastName, strLastName, true, false);
                }
            }
        }
        //Number
        if (!refSearchData.m_strNumber.empty())
        {
            AddUrlParameter(pMenu->m_strSearchUrl, kszNodeNumber, refSearchData.m_strNumber, true, false);
        }
        //Mobile
        if (!refSearchData.m_strMobile.empty())
        {
            AddUrlParameter(pMenu->m_strSearchUrl, kszNodeMobile, refSearchData.m_strMobile, false, false);
        }
        //Extension
        if (!refSearchData.m_strExtension.empty())
        {
            AddUrlParameter(pMenu->m_strSearchUrl, kszNodeExtension, refSearchData.m_strExtension, true, false);
        }
        //Department
        if (!refSearchData.m_strDepartment.empty())
        {
            AddUrlParameter(pMenu->m_strSearchUrl, kszNodeDepartment, refSearchData.m_strDepartment, false);
        }
        //Email
        if (!refSearchData.m_strEmail.empty())
        {
            AddUrlParameter(pMenu->m_strSearchUrl, kszNodeEmail, refSearchData.m_strEmail, true, false);
        }

        SetSearchCriteriaModeOr(pMenu->m_strSearchUrl, refSearchData.m_bCriteriaMode);

        return true;
    }

    return false;
}

void CBroadSoftContactManager::AddUrlParameter(yl::string & strUrl, const yl::string & strName,
        const yl::string & strValue, bool isPreMatch, bool isCaseSensitive/* = true*/)
{
    if (strUrl.empty())
    {
        return;
    }
    if (strUrl.find("?") == yl::string::npos)
    {
        strUrl.append("?");
    }
    else
    {
        strUrl.append("&");
    }

    strUrl.append(UrlEncode(strName));
    strUrl.append("=");
    if (isPreMatch)
    {
        strUrl.append("*");
    }
    strUrl.append(UrlEncode(strValue));
    if (isPreMatch)
    {
        strUrl.append("*");
    }

    if (!isCaseSensitive)
    {
        //不区分大小写
        strUrl.append("/i");
    }
}

yl::string CBroadSoftContactManager::UrlEncode(const yl::string & strName)
{
    static const yl::string ENCODE_CHARS = "+ /?%#&=";
    static char szBuffer[16] = { 0 };
    yl::string result;
    result.reserve(strName.size() * 3);
    for (int i = 0; i < strName.size(); ++i)
    {
        char ch = strName[i];
        if (ch <= 0 || (ENCODE_CHARS.find(strName[i]) != yl::string::npos)) //需要转义，转义后push
        {
            sprintf(szBuffer, "%%%X", strName[i]);
            result.append(szBuffer);
        }
        else //不需要转义，直接push
        {
            result.push_back(strName[i]);
        }
    }
    return result;
}


void CBroadSoftContactManager::SetSearchCriteriaModeOr(yl::string & strUrl, bool isOn)
{
    if (isOn)
    {
        AddUrlParameter(strUrl, yl::string("searchCriteriaModeOr"), yl::string("True"));
    }
    else
    {
        //默认为false
        //AddUrlParameter(strUrl,"searchCriteriaModeOr","False");
    }
}

void CBroadSoftContactManager::ResetCustomListStatus()
{
    configParser_SetConfigString(kszBsftCustomListFailReason, "");
    configParser_SetConfigInt(kszBsftCustomListStatus, 0);
}

bool CBroadSoftContactManager::ReqLoadContact(int nGroupId)
{
    ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetMenuDataById(nGroupId));

    if (pGroup == NULL)
    {
        return false;
    }

    if (pGroup->m_eStatus == FILE_DOWNLOADING || pGroup->m_bReqNext)
    {
        return true;
    }

    if (BsftDir_UpdateEnable())
    {
        pGroup->m_mapNameToId.clear();
        ClearContactByGroupID(nGroupId);
    }
    else
    {
        ClearContactInfo(nGroupId);
    }

    pGroup->m_eStatus = FILE_DOWNLOAD_READY;

    return MyExecDownload(nGroupId) == FILE_DOWNLOADING;
}

bool CBroadSoftContactManager::ReqNextPageContact(int nGroupId, bool bSearch /*= false*/)
{
    DIR_INFO("CBroadSoftContactManager::ReqNextPageContact nGroup[%d] isSearch[%d]",
             nGroupId, bSearch);
    if (!IsNetworkConnected())
    {
        return false;
    }

    if (BsftDir_UpdateEnable())
    {
        return false;
    }

    if (bSearch)
    {
        ContactBroadSoftMenu* pSearchGroup = Base2BsftMenu(GetDataById(kszServerSearchGroupId));

        if (pSearchGroup == NULL)
        {
            return false;
        }

        if (kszAllContactId == nGroupId)
        {
            for (int i = 0; i < m_vGroupId.size(); i++)
            {
                if ((m_vGroupId[i] == kszGroupId && IsOnlySearchEnterprise())
                        || !PreSearchProcess(m_vGroupId[i], m_refLastSearchData))
                {
                    continue;
                }
                ReqNextPage(m_vGroupId[i], bSearch);
            }

            DirGroupRecord * pMenu = GetMenuDataById(kszCustomId);

            if (pMenu != NULL)
            {
                for (int i = 0; i < pMenu->m_vChildrenId.size(); i++)
                {
                    if (PreSearchProcess(pMenu->m_vChildrenId[i], m_refLastSearchData))
                    {
                        ReqNextPage(pMenu->m_vChildrenId[i], bSearch);
                    }
                }
            }
            return true;
        }
        else
        {
            if (nGroupId == kszCustomId)
            {
                DirGroupRecord * pMenu = GetMenuDataById(kszCustomId);

                if (pMenu != NULL)
                {
                    for (int i = 0; i < pMenu->m_vChildrenId.size(); i++)
                    {
                        if (PreSearchProcess(pMenu->m_vChildrenId[i], m_refLastSearchData))
                        {
                            ReqNextPage(pMenu->m_vChildrenId[i], bSearch);
                        }
                    }
                }
                return true;
            }
            if (!PreSearchProcess(nGroupId, m_refLastSearchData))
            {
                return false;
            }
        }


    }
    else
    {
        ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetMenuDataById(nGroupId));

        if (pGroup == NULL)
        {
            return false;
        }

        if (pGroup->m_eStatus == FILE_DOWNLOADING)
        {
            return true;
        }
    }

    return ReqNextPage(nGroupId, bSearch);
}

DIRECTORY_RET_CODE CBroadSoftContactManager::ReqSearchContact(int nGroupId,
        ContactBroadSoftItemData & refSearchData)
{
    bool suc = false;

    DirGroupRecord * pSearchGroup = GetMenuDataById(kszServerSearchGroupId);

    if (pSearchGroup == NULL)
    {
        return DIR_RET_GROUP_UNEXISTED;
    }

    pSearchGroup->m_vChildrenId.clear();

    m_refLastSearchData.Reset();

    if (!BsftDir_UpdateEnable())
    {
        ContactBroadSoftMenu* pSearchMenu = Base2BsftMenu(pSearchGroup);
        if (pSearchMenu)
        {
            pSearchMenu->m_nSearchStartIndex = 0;
            pSearchMenu->m_nCountPerPage = 0;
            pSearchMenu->m_nServerTotalCount = 0;
            // custom 组设置为custom搜索
            pSearchMenu->m_bCustomSearch = (nGroupId == kszCustomId);
        }
    }

    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    if (nGroupId != kszAllContactId)
    {
        if (nGroupId == kszCustomId)
        {
            DirGroupRecord * pMenu = GetMenuDataById(kszCustomId);

            if (pMenu != NULL)
            {
                for (int i = 0; i < pMenu->m_vChildrenId.size(); i++)
                {
                    if (PreSearchProcess(pMenu->m_vChildrenId[i], refSearchData))
                    {
                        suc = MyExecDownload(pMenu->m_vChildrenId[i], true) == FILE_DOWNLOADING;
                    }
                }
            }
        }
        else if (PreSearchProcess(nGroupId, refSearchData))
        {
            suc = MyExecDownload(nGroupId, true) == FILE_DOWNLOADING;
        }
    }
    else
    {
        for (int i = 0; i < m_vGroupId.size(); i++)
        {
            if ((m_vGroupId[i] == kszGroupId && IsOnlySearchEnterprise())
                    || !PreSearchProcess(m_vGroupId[i], refSearchData))
            {
                continue;
            }
            suc = MyExecDownload(m_vGroupId[i], true) == FILE_DOWNLOADING;
        }

        DirGroupRecord * pMenu = GetMenuDataById(kszCustomId);

        if (pMenu != NULL)
        {
            for (int i = 0; i < pMenu->m_vChildrenId.size(); i++)
            {
                if (PreSearchProcess(pMenu->m_vChildrenId[i], refSearchData))
                {
                    suc = MyExecDownload(pMenu->m_vChildrenId[i], true) == FILE_DOWNLOADING;
                }
            }
        }
    }

    if (suc)
    {
        m_refLastSearchData = refSearchData;
        return GET_LIST_DOWNLOADING;
    }
    else
    {
        return DIR_RET_SEARCH_PARM_ERROR;
    }
}

bool CBroadSoftContactManager::PreSearchProcess(int nId, ContactBroadSoftItemData & refSearchData)
{
    //如配置项都没有下载成功则不再进行该项的搜索
    if (nId < kszPersonalId || nId == kszCustomId)
    {
        return false;
    }

    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(nId));

    if (pMenu == NULL)
    {
        return false;
    }

    // 分页下载始终可以搜索
    if (pMenu->m_eStatus != FILE_DOWNLOADED_OK
            && BsftDir_UpdateEnable())
    {
        return false;
    }

    if (refSearchData.m_strDisplayName == ",")
    {
        refSearchData.m_strDisplayName = "";
    }

    //personal只支持2种搜索
    if ((IsCommonDir(nId) || IsPersonal(nId))
            && refSearchData.m_strDisplayName.empty()
            && refSearchData.m_strNumber.empty())
    {
        //返回false不在继续处理
        return false;
    }

    //Custom只支持4中搜索
    if (IsCustomSubDir(nId)
            && refSearchData.m_strDisplayName.empty()
            && refSearchData.m_strExtension.empty()
            && refSearchData.m_strNumber.empty()
            && refSearchData.m_strMobile.empty())
    {
        return false;
    }

    //添加搜索参数
    return AddParameters(nId, refSearchData);
}

void CBroadSoftContactManager::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    if (pTaskAction == NULL)
    {
        return;
    }

    DIR_INFO("CContactBroadSoftModel::OnTaskActionCallback begin");
    if (pTaskAction->GetReqType() != NS_TA::TA_REQUEST_NETWORK_FILE)
    {
        CBaseContactManger::OnTaskActionCallback(pTaskAction);
        return;
    }

    // 返回结果的类型为请求网络文件.
    CBSFTDirectoryAction * pNetworkAction =
        dynamic_cast<CBSFTDirectoryAction *>(pTaskAction);
    if (pNetworkAction == NULL)
    {
        return;
    }

    CBaseHelper::ProcessCallback(pTaskAction, 0, XSI_DIR_SEND);

    //Delete
    if (pNetworkAction->IsDelete())
    {
        int nOldId = pNetworkAction->GetExtraParam();
        //處理Edit刪除返回的情況

        EditMap::iterator citer = m_editMap.find(nOldId);
        if (citer != m_editMap.end())
        {
            //编辑保持原有Id不变
            int nID = AddContact2Personal(citer->second, nOldId);
            m_editMap.erase(citer);

            if (nID > 0)
            {
                // 编辑完成通知UI刷新界面
                etl_NotifyApp(false, BROADSOFT_PHONEBOOK_POST_RESULT, nID, 1);
            }
        }

        return;
    }

    //Post
    if (!pNetworkAction->IsRead())
    {
        return;
    }

    int nGroupId = pNetworkAction->GetExtraParam();

    // 是否存在下载项.
    ContactBroadSoftMenu * pContactGroup = Base2BsftMenu(GetDataById(nGroupId));

    if (pContactGroup == NULL)
    {
        DIR_INFO("pContactGroup = NULL!");
        return;
    }

    int messg = BROADSOFT_PHONEBOOK_SERVERSEARCH_RESULT;
    // 只有状态为下载中, 才可以复制数据. 因为用户
    // 可能会在下载过程中取消下载.
    if (!pNetworkAction->IsBsftSearch())
    {
        if ((pContactGroup->m_eStatus != FILE_DOWNLOADING
                && pContactGroup->m_nServerTotalCount <= pContactGroup->m_vChildrenId.size())
                || pContactGroup->m_eStatus == FILE_DOWNLOAD_CANCELED)
        {
            return;
        }
        messg = BROADSOFT_PHONEBOOK_DOWNLOADED_RESULT;
    }

    if (!pNetworkAction->IsOperationSucceed())
    {
        int result = 0;

        if (pNetworkAction->GetTaskActionErrorType() == NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED)
        {
            result = 401;
        }

        // 发送消息.
        DIR_INFO("Send Message to UI! result = %d", result);


        etl_NotifyApp(false, messg, nGroupId, result);

        if (nGroupId == kszCustomId || pNetworkAction->IsPreLoad())
        {
            configParser_SetConfigString(kszBsftCustomListFailReason,
                                         result == 401 ? "auth error" : "Load Fail");

            configParser_SetConfigInt(kszBsftCustomListStatus, FILE_DOWNLOAD_FAIL);
        }

        if (!pNetworkAction->IsBsftSearch())
        {
            pContactGroup->m_eStatus = FILE_DOWNLOAD_FAIL;
        }

        pContactGroup->m_bReqNext = false;
    }
    else
    {
        bool suc = false;

        bool bReqNext = false;

        //Custom List
        if (nGroupId == kszCustomId)
        {
            yl::string strCustomList;
            if (MyLoadCustomListFromFile(strCustomList, pNetworkAction->GetFilePath()))
            {
                configParser_SetConfigInt(kszBsftCustomListStatus, FILE_DOWNLOADED_OK);
                configParser_SetConfigString(kszBsftCustomListFailReason, "");
                MyInsertSubCustomDir(strCustomList);

                //保存组信息到临时文件，供网页使用
                SaveGroupInfoToTempFile("BroadsoftGroupInfo.xml", "BroadsoftGroup");
                suc = true;
            }
            else
            {
                configParser_SetConfigInt(kszBsftCustomListStatus, FILE_DOWNLOAD_FAIL);
                configParser_SetConfigString(kszBsftCustomListFailReason, "Parse Fail");
            }
        }
        else
        {
            suc = MyLoadDataFromFile(nGroupId, pNetworkAction->GetFilePath(), pNetworkAction->IsBsftSearch(),
                                     bReqNext);
        }

        if (!pNetworkAction->IsBsftSearch())
        {
            pContactGroup->m_eStatus = suc ? FILE_DOWNLOADED_OK : FILE_DOWNLOAD_FAIL;
        }

        DIR_INFO("Send Message to UI! suc = %d GroupId[%d],bReqNext[%d]", suc, nGroupId, bReqNext);

        etl_NotifyAppEx(false, messg, nGroupId, suc, sizeof(bool), &bReqNext);

        if (pNetworkAction->IsPreLoad() && BsftDir_UpdateEnable())
        {
            ProcessPreLoadSuc();
        }
    }

    remove(pNetworkAction->GetFilePath().c_str());
}

void CBroadSoftContactManager::ProcessPreLoadSuc()
{
    for (int i = 0; i < m_vGroupId.size(); i++)
    {
        int nGroupId = m_vGroupId[i];
        if (nGroupId == kszAllContactId)
        {
            continue;
        }

        ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(nGroupId));
        if (pMenu != NULL && pMenu->m_eStatus != FILE_DOWNLOADED_OK)
        {
            MyExecDownload(nGroupId);
        }
    }
}

int CBroadSoftContactManager::AddContact2Personal(ContactBroadSoftPersonal & refData,
        int nId/* = knInvalidRId*/)
{
    //先判断网络是否可用
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    if (refData.m_strDisplayName.empty()
            || refData.m_listNumber.size() == 0
            || refData.GetNumberByIndex(0).empty())
    {
        return DIR_RET_EMPTY;
    }

    ContactBroadSoftMenu * pGroupPersonal = Base2BsftMenu(GetDataById(kszPersonalId));

    if (pGroupPersonal == NULL)
    {
        return DIR_RET_GROUP_UNEXISTED;
    }

    bool bFail = false;
    if (BsftDir_UpdateEnable())
    {
        bFail = pGroupPersonal->m_eStatus != FILE_DOWNLOADED_OK;
    }
    else
    {
        // XSI 无法使用
        bFail = XSI_SUCCESS != CBaseHelper::GetXSIStatus(0);
    }

    if (bFail)
    {
        return DIR_RET_FAIL;
    }

    if (pGroupPersonal->IsContactNameExist(refData.m_strDisplayName) != -1)
    {
        return DIR_RET_CONTACT_EXIST;
    }

    int Index = DIR_RET_FAIL;

    ContactBroadSoftPersonal * pNew = new ContactBroadSoftPersonal();

    *pNew = refData;

    Index = AddContact(pNew, kszPersonalId, true, true, nId);
    if (Index > 0)
    {
        pGroupPersonal->m_mapNameToId.put(refData.m_strDisplayName, Index);
        SaveToFile(refData);
        yl::string strFileName = "Personal";
        MyExecPost(pGroupPersonal->m_strFullUrl);
        ResetSearchKey();
    }

    return Index;
}

// 将BSFT联系人添加到本地的ALL 组中
int CBroadSoftContactManager::AddContact2AllContact(ContactGeneralItemData & refData)
{
    return g_pLocalDirManager->AddLocalContact(refData, kszAllContactId);
}

void CBroadSoftContactManager::SaveToFile(ContactBroadSoftPersonal & refData)
{
    xml_document doc;
    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "UTF-8");

    xml_node nodeRootNode = doc.append_child(kszRootNode);
    nodeRootNode.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");

    xml_node nodeTmp = nodeRootNode.append_child("startIndex");
    nodeTmp.text().set("1");

    nodeTmp = nodeRootNode.append_child("numberOfRecords");
    nodeTmp.text().set("1");

    nodeTmp = nodeRootNode.append_child("totalAvailableRecords");
    nodeTmp.text().set("1");

    xml_node nodeEntry = nodeRootNode.append_child("entry");
    xml_node nodeTmpEntry = nodeEntry.append_child("name");
    nodeTmpEntry.text().set(refData.m_strDisplayName.c_str());

    yl::string strNumber;
    refData.GetNumberByIndex(0, strNumber);

    nodeTmpEntry = nodeEntry.append_child("number");
    nodeTmpEntry.text().set(strNumber.c_str());

    yl::string strFileFullPath(g_strConfigPath);
    strFileFullPath.append(kszPersonalFile);

    doc.save_file(strFileFullPath.c_str());
}

bool CBroadSoftContactManager::MyLoadDataFromFile(int GroupId, const yl::string & strFilePath,
        bool isSearch, bool & bReqNest)
{
    DIR_INFO("load downloaded broadsoft contact file [%s]", strFilePath.c_str());

    // 个人联系人根节点名.
#define kszNodePersonal "Personal"
    // 组联系人根节点.
#define kszNodeGroup "Group"
#define kszNodeEnterPriseCommon "EnterpriseCommon"
#define kszNodeGroupCommon "GroupCommon"
#define kszNodeEnterPrise "Enterprise"
#define kszNodeCustom "Directory"
#define kszNodeTotalCount "totalAvailableRecords"
#define kszNodeCountPerPage "numberOfRecords"
#define kszNodestartIndex "startIndex"

    if (strFilePath.empty())
    {
        return false;
    }

    // 打开Xml文件.
    if (!pathFileExist(strFilePath.c_str()))
    {
        return false;
    }

    bool succ = false;

    xml_document doc;
    OpenBSFTXML(strFilePath.c_str(), doc);

    yl::string strNodeName;

    xml_node node = doc.first_child();

    for (; node; node = node.next_sibling())
    {
        strNodeName = node.name();
        if (strNodeName.compare(kszNodeGroup) == 0)
        {
            succ = MyLoadGroupXml(GroupId, &node, isSearch);
            break;
        }
        else if (strNodeName.compare(kszNodePersonal) == 0)
        {
            succ = MyLoadPersonalXml(GroupId, &node, isSearch);
            break;
        }
        else if (strNodeName.compare(kszNodeEnterPrise) == 0)
        {
            succ = MyLoadEnterpriseXml(GroupId, &node, isSearch);
            break;
        }
        else if (strNodeName.compare(kszNodeGroupCommon) == 0
                 || strNodeName.compare(kszNodeEnterPriseCommon) == 0)
        {
            succ = MyLoadCommonXml(GroupId, &node, isSearch);
            break;
        }
        else if (strNodeName.compare(kszNodeCustom) == 0)
        {
            succ = MyLoadCustomXml(GroupId, &node, isSearch);
            break;
        }
    }

    // 自动更新才需要下载下一页
    if (BsftDir_UpdateEnable())
    {
        //分页请求
        bReqNest = ReqNextPage(GroupId, node, isSearch);
    }
    else
    {
        ParsePageInfo(GroupId, node, isSearch);
        bReqNest = false;
    }

    return succ;
}

bool CBroadSoftContactManager::ReqNextPage(int GroupId, const xml_node & node, bool isSearch)
{
    DIR_INFO("CBSFTDirProcessor::ReqNextPage");
    if (node.empty())
    {
        return false;
    }

    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(GroupId));

    if (pMenu == NULL)
    {
        return false;
    }

    yl::string strTotalCount;
    yl::string strCountPerPage;

    int nStartIndex = 0;

    xml_node NodeTotalCount = node.child(kszNodeTotalCount);

    xml_node NodeCountPerPage = node.child(kszNodeCountPerPage);

    xml_node NodeStartIndex = node.child(kszNodestartIndex);

    if (NodeTotalCount.empty() || NodeCountPerPage.empty()/* || NodeStartIndex.empty()*/)
    {
        pMenu->m_bReqNext = false;
        return false;
    }

    strCountPerPage = NodeCountPerPage.child_value();
    pMenu->m_nCountPerPage = atoi(strCountPerPage.c_str());

    strTotalCount = NodeTotalCount.child_value();
    pMenu->m_nServerTotalCount = atoi(strTotalCount.c_str());

    if (NodeStartIndex.empty())
    {
        if (isSearch)
        {
            ContactBroadSoftMenu* pSearchMenu = Base2BsftMenu(GetDataById(kszServerSearchGroupId));
            if (pSearchMenu != NULL)
            {
                nStartIndex = pSearchMenu->m_vChildrenId.size() - pMenu->m_nCountPerPage + 1;
            }
        }
        else
        {
            nStartIndex = pMenu->m_vChildrenId.size() - pMenu->m_nCountPerPage + 1;
        }
    }
    else
    {
        nStartIndex = atoi(NodeStartIndex.child_value());
    }

    DIR_INFO("pData->m_nCountPerPage = %d,pData->m_nServerTotalCount = %d,now count[%d]",
             pMenu->m_nCountPerPage, pMenu->m_nServerTotalCount, pMenu->m_vChildrenId.size());

    //int nNowCount = pMenu->m_vChildrenId.size()
    if (nStartIndex + pMenu->m_nCountPerPage <= pMenu->m_nServerTotalCount
            && !strCountPerPage.empty()
            && !strTotalCount.empty())
    {
        char szBuf[32] = {0};
        sprintf(szBuf, "%d", nStartIndex + pMenu->m_nCountPerPage);
        yl::string strStartIndex = szBuf;

        DIR_INFO("CBSFTDirProcessor::ReqNextPage MyExecDownload");
        pMenu->m_eStatus = FILE_DOWNLOAD_READY;
        MyExecDownload(GroupId, isSearch, false, strStartIndex, strCountPerPage);

        //搜索不要影响组本身状态，返回之前状态
        if (isSearch)
        {
            pMenu->m_eStatus = FILE_DOWNLOADED_OK;
        }

        pMenu->m_bReqNext = true;

        return true;
    }

    pMenu->m_bReqNext = false;

    return false;
}


bool CBroadSoftContactManager::ReqNextPage(int nGroupID, bool isSearch /*= false*/)
{
    DIR_INFO("CBSFTDirProcessor::ReqNextPage Group [%d] Search [%d]", nGroupID, isSearch);

    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(nGroupID));

    if (pMenu == NULL)
    {
        DIR_INFO("ReqNextPage Group Null");
        return false;
    }

    pMenu->m_bReqNext = false;

    int nStartIndex = isSearch ? pMenu->m_nSearchStartIndex : pMenu->m_nNextStartIndex;

    DIR_INFO("m_nNextStartIndex = %d m_nSearchStartIndex = %d m_nCountPerPage = %d, m_nServerTotalCount = %d,now count[%d]",
             pMenu->m_nNextStartIndex, pMenu->m_nSearchStartIndex, pMenu->m_nCountPerPage, pMenu->m_nServerTotalCount, pMenu->m_vChildrenId.size());

    if (nStartIndex <= pMenu->m_nServerTotalCount)
    {
        DIR_INFO("CBSFTDirProcessor::ReqNextPage MyExecDownload");
        pMenu->m_eStatus = FILE_DOWNLOAD_READY;
        //搜索判断普通的next index 非搜索判断 search index
        if ((pMenu->m_nNextStartIndex == 0 && !isSearch)
                || (pMenu->m_nSearchStartIndex == 0 && isSearch))
        {
            MyExecDownload(nGroupID, isSearch);
        }
        else
        {
            char szBuf[32] = { 0 };
            if (isSearch)
            {
                sprintf(szBuf, "%d", pMenu->m_nSearchStartIndex);
            }
            else
            {
                sprintf(szBuf, "%d", pMenu->m_nNextStartIndex);
            }
            yl::string strStartIndex = szBuf;
            sprintf(szBuf, "%d", pMenu->m_nCountPerPage);
            yl::string strCountPerPage = szBuf;

            MyExecDownload(nGroupID, isSearch, false, strStartIndex, strCountPerPage);
        }

        return true;
    }

    return false;
}

bool CBroadSoftContactManager::MyLoadCustomListFromFile(yl::string & strCustomList,
        const yl::string & strFilePath)
{
#define kszNodeDirectory "directory"
    //#define kszNodeName "name"

#define kszCustomListMAX 5

    xml_document doc;
    doc.load_file(strFilePath.c_str(), parse_default | parse_declaration);

    xml_node nodeDeclaration = doc.child("xml");

    //http://10.2.1.199/Bug.php?BugID=34025
    if (!nodeDeclaration.empty())
    {
        xml_attribute AttrEncoding =  nodeDeclaration.attribute("encoding");
        if (!AttrEncoding.empty())
        {
            yl::string strValue;
            strValue.assign(AttrEncoding.value(), strlen(AttrEncoding.value()));
            if (strValue.compare("ISO-8859-1") == 0)
            {
                doc.load_file(strFilePath.c_str(), parse_default | parse_declaration, encoding_latin1);
            }
        }
    }

    xml_node node = doc.child("CustomContact");
    if (node.empty())
    {
        return false;
    }
    int index = 0;
    yl::string strUseList = configParser_GetConfigString(kszBsftCustomListUse);

    DIR_INFO("strUseList = %s", strUseList.c_str());

    for (node = node.child(kszNodeDirectory); node; node = node.next_sibling(kszNodeDirectory))
    {
        xml_node childNode = node.child(kszNodeName);
        if (!childNode.empty())
        {
            yl::string strDirectory = childNode.child_value();
#if 0
            DIR_INFO("-----------------------strDirectory = %s", strDirectory.c_str());
#endif
            if (strDirectory.empty())
            {
                continue;
            }
            if (!strCustomList.empty())
            {
                strCustomList.append(",");
            }
            strCustomList.append(strDirectory);
            if (++index >= kszCustomListMAX && strUseList.empty())
            {
                strUseList = strCustomList;
            }
        }
    }
    if (strUseList.empty())
    {
        strUseList = strCustomList;
    }

    if (strCustomList.find(strUseList) == yl::string::npos)
    {
        YLList<yl::string> ListCustomUse, ListCustomAll;

        ParseStringBySeparator(strUseList, ',', ListCustomUse);

        ParseStringBySeparator(strCustomList, ',', ListCustomAll);

        strUseList.clear();
        DIR_INFO("strUseList = %s", strUseList.c_str());

        for (YLList<yl::string>::iterator iter = ListCustomUse.begin(); iter != ListCustomUse.end();
                iter++)
        {
            yl::string & strDir = *iter;

            for (YLList<yl::string>::iterator cite = ListCustomAll.begin(); cite != ListCustomAll.end();
                    cite++)
            {
                yl::string & strDirall = *cite;
                if (strDir.compare(strDirall) == 0)
                {
                    if (!strUseList.empty())
                    {
                        strUseList.append(",");

                    }
                    strUseList.append(strDir);
                    DIR_INFO("strUseList = %s", strUseList.c_str());
                }
            }
        }
    }
    configParser_SetConfigString(kszBsftCustomListAll, strCustomList.c_str());
    configParser_SetConfigString(kszBsftCustomListUse, strUseList.c_str());
    DIR_INFO("----------------------strCustomList = %s,strUseList= %s", strCustomList.c_str(),
             strUseList.c_str());

    strCustomList = strUseList;
    return true;
}

void CBroadSoftContactManager::InsertOneContact(BaseContactRecord * pAdd,
        ContactBroadSoftMenu * pGroup, bool isSearch)
{
    if (pAdd != NULL && pGroup != NULL)
    {
        if (isSearch && BsftDir_UpdateEnable())
        {
            int nExistId = pGroup->IsContactNameExist(pAdd->m_strDisplayName);

            if (nExistId != -1)
            {
                AddIdToGroup(nExistId, kszServerSearchGroupId);
                delete pAdd;
                return;
            }
        }

        if (pAdd->m_strNameIndex.empty())
        {
            pAdd->m_strNameIndex = commonAPI_transT9String(pAdd->GetDisplayName());
        }

        int nId = InsertMap(pAdd);
        pAdd->m_nParentId = pGroup->m_nId;
        pGroup->m_mapNameToId.put(pAdd->m_strDisplayName, pAdd->m_nId);

        if (BsftDir_UpdateEnable()
                || !isSearch)
        {
            //bsft联系人统一按照显示名排序--t49新需求按配置是否排序
            AddIdToGroup(nId, pGroup->m_nId, configParser_GetConfigInt(kszNotLocalDirSortEnable) == 1);
        }

        // 自动更新开关关闭也不加入all 组，加入all组没意义，之后也要清空
        if ((pGroup->m_nId != kszGroupId || !IsOnlySearchEnterprise())
                && BsftDir_UpdateEnable())
        {
            AddIdToGroup(nId, kszAllContactId);
        }


        if (isSearch)
        {
            AddIdToGroup(nId, kszServerSearchGroupId);
        }
    }
}

bool CBroadSoftContactManager::MyLoadPersonalXml(int GroupId,
        xml_node * pElem, bool isSearch)
{
    if (GroupId != kszPersonalId || pElem == NULL)
    {
        return false;
    }

    ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetDataById(GroupId));

    if (pGroup == NULL)
    {
        return false;
    }

    // 读取各个节点的值.
    xml_node nodeEntry = pElem->child(kszNodeEntry);
    for (; nodeEntry; nodeEntry = nodeEntry.next_sibling(kszNodeEntry))
    {
        ContactBroadSoftPersonal * pNew = new ContactBroadSoftPersonal();
        if (pNew != NULL)
        {
            pNew->ParseFromXMLNode(nodeEntry);//解析联系人
            InsertOneContact(pNew, pGroup, isSearch);
        }
    }

    return true;
}

bool CBroadSoftContactManager::MyLoadEnterpriseXml(int GroupId,
        xml_node * pElem, bool isSearch)
{
    if (GroupId != kszEnterPriseId || pElem == NULL)
    {
        return false;
    }

    ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetDataById(GroupId));

    if (pGroup == NULL)
    {
        return false;
    }

#define kszNodeEnterPriseDirectory "enterpriseDirectory"
#define kszNodeDirectoryDetails "directoryDetails"

    // 读取各个节点的值.
    xml_node nodeDirectoryDetails = pElem->child(kszNodeEntry);
    bool isV1 = true;

    if (!nodeDirectoryDetails)
    {
        isV1 = false;
        xml_node nodeGroupDirectory = pElem->child(kszNodeEnterPriseDirectory);
        nodeDirectoryDetails = nodeGroupDirectory.child(kszNodeDirectoryDetails);
    }

    for (; nodeDirectoryDetails;
            nodeDirectoryDetails = nodeDirectoryDetails.next_sibling(isV1 ? kszNodeEntry :
                                   kszNodeDirectoryDetails))
    {
        ContactBroadSoftEnterprise * pNew = new ContactBroadSoftEnterprise();
        if (pNew != NULL)
        {
            pNew->ParseFromXMLNode(nodeDirectoryDetails);//解析联系人
            InsertOneContact(pNew, pGroup, isSearch);
        }
    }  // Outer for.

    return true;
}

bool CBroadSoftContactManager::MyLoadGroupXml(int GroupId,
        xml_node * pElem, bool isSearch)
{
    if (GroupId != kszGroupId || pElem == NULL)
    {
        return false;
    }

    ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetDataById(GroupId));

    if (pGroup == NULL)
    {
        return false;
    }

#define kszNodeGroupDirectory "groupDirectory"
#define kszNodeDirectoryDetails "directoryDetails"

    // 读取各个节点的值.
    xml_node nodeDirectoryDetails = pElem->child(kszNodeEntry);
    bool isV1 = true;

    if (!nodeDirectoryDetails)
    {
        isV1 = false;
        xml_node nodeGroupDirectory = pElem->child(kszNodeGroupDirectory);
        nodeDirectoryDetails = nodeGroupDirectory.child(kszNodeDirectoryDetails);
    }

    for (; nodeDirectoryDetails;
            nodeDirectoryDetails = nodeDirectoryDetails.next_sibling(isV1 ? kszNodeEntry :
                                   kszNodeDirectoryDetails))
    {
        ContactBroadSoftGroup * pNew = new ContactBroadSoftGroup();
        if (pNew != NULL)
        {
            pNew->ParseFromXMLNode(nodeDirectoryDetails);//解析联系人
            InsertOneContact(pNew, pGroup, isSearch);
        }
    }  // Outer for.

    return true;
}

bool CBroadSoftContactManager::MyLoadCustomXml(int GroupId,
        xml_node * pElem, bool isSearch)
{
    if (pElem == NULL)
    {
        return false;
    }

    ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetDataById(GroupId));

    if (pGroup == NULL)
    {
        return false;
    }

#define kszNodeContacts "contacts"
#define kszNodeContact "contact"

    // 读取各个节点的值.
    xml_node Node = pElem->child(kszNodeContacts);

    if (!Node)
    {
        return false;
    }
    Node = Node.child(kszNodeContact);

    for (; Node; Node = Node.next_sibling(kszNodeContact))
    {
        ContactBroadSoftCustom * pNew = new ContactBroadSoftCustom();
        if (pNew != NULL)
        {
            pNew->ParseFromXMLNode(Node);//解析联系人
            InsertOneContact(pNew, pGroup, isSearch);
        }
    }  // Outer for.

    return true;
}

bool CBroadSoftContactManager::MyLoadCommonXml(int GroupId, xml_node * pElem, bool isSearch)
{
    if (pElem == NULL)
    {
        return false;
    }

    ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetDataById(GroupId));

    if (pGroup == NULL)
    {
        return false;
    }

#define kszNodeCommonEntry "commonPhoneEntry"

    // 读取各个节点的值.
    xml_node nodeCommonEntry = pElem->child(kszNodeCommonEntry);
    for (; nodeCommonEntry; nodeCommonEntry = nodeCommonEntry.next_sibling(kszNodeCommonEntry))
    {
        ContactBroadSoftCommon * pNew = new ContactBroadSoftCommon();
        if (pNew != NULL)
        {
            pNew->ParseFromXMLNode(nodeCommonEntry);//解析联系人
            InsertOneContact(pNew, pGroup, isSearch);
        }
    }
    return true;
}

void CBroadSoftContactManager::MyInsertSubCustomDir(const yl::string & strCustomList)
{
    if (strCustomList.empty())
    {
        return;
    }

    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(kszCustomId));

    if (pMenu == NULL)
    {
        return;
    }

    if (pMenu->m_vChildrenId.size() != 0)
    {
        ClearContactByGroupID(kszCustomId);
    }

    YLList<yl::string> ListCustom;

    ParseStringBySeparator(strCustomList, ',', ListCustom);

    int nCustomSubId = kszCustomId + 1;

    for (YLList<yl::string>::iterator iter = ListCustom.begin(); iter != ListCustom.end();
            iter++)
    {
        yl::string & strCustomDirectory = *iter;

        if (strCustomDirectory.empty())
        {
            continue;
        }

        ContactBroadSoftMenu * pNew = new ContactBroadSoftMenu();


        pNew->m_strFullUrl = pMenu->m_strFullUrl + "/";
        pNew->m_strFullUrl.append(strCustomDirectory);
        pNew->m_strDisplayName = strCustomDirectory;
        pNew->m_nParentId = kszCustomId;

        InsertMap(pNew, nCustomSubId);

        AddIdToGroup(nCustomSubId, kszCustomId, false);

        MyExecDownload(nCustomSubId);

        nCustomSubId++;
    }
}

void CBroadSoftContactManager::ClearCustomList(bool IsClearLocal)
{
    if (IsClearLocal)
    {
        configParser_SetConfigString(kszBsftCustomListUse, "");
    }
    //
    configParser_SetConfigString(kszBsftCustomListAll, "");
}

bool CBroadSoftContactManager::IsPersonal(int GroupId)
{
    return GroupId == kszPersonalId;
}

bool CBroadSoftContactManager::IsCommonDir(int GroupId)
{
    return (GroupId == kszEnterPriseCommonId || GroupId == kszGroupCommonId);
}

bool CBroadSoftContactManager::IsCustomSubDir(int GroupId)
{
    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(GroupId));
    if (pMenu == NULL)
    {
        return false;
    }

    if (pMenu->m_nParentId == kszCustomId)
    {
        return true;
    }
    return false;
}

bool CBroadSoftContactManager::IsOnlySearchEnterprise()
{
    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(kszEnterPriseId));

    if (pMenu != NULL && pMenu->m_eStatus != FILE_DOWNLOAD_FAIL && m_bIsOnlySearchEnterPrise)
    {
        return true;
    }
    return false;
}

DIRECTORY_RET_CODE CBroadSoftContactManager::DeletePersonalContact(VectorId & vecRemoveId,
        BSFT_SEARCH_TYPE eType/* = SEARCH_NONE*/)
{
    //先判断网络是否可用
    if (!IsNetworkConnected())
    {
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    ContactBroadSoftMenu * pPersonal = Base2BsftMenu(GetDataById(kszPersonalId));

    if (pPersonal == NULL)
    {
        return DIR_RET_GROUP_UNEXISTED;
    }

    for (int i = 0; i < vecRemoveId.size(); i++)
    {
        pPersonal->m_mapNameToId.erase(GetDisplayNameById(vecRemoveId[i]));
        MyExecDelete(vecRemoveId[i]);
    }

    RemoveRecord(vecRemoveId, kszPersonalId);

    // 自动更新关闭，没有本地搜索,本地搜索相当于服务器搜索，需要删除真实数据
    if (!BsftDir_UpdateEnable())
    {
        RemoveRecord(vecRemoveId, kszServerSearchGroupId);
    }

    //解决先进本地搜索后进入网络搜索，删除personal联系人还存在空的联系人问题
    //http://bugfree.yealink.com/Bug.php?BugID=115126
    RemoveIdFromGroup(vecRemoveId, kszSearchGroupId);
    RemoveIdFromGroup(vecRemoveId, kszServerSearchGroupId);

    //switch (eType)
    //{
    //case SEARCH_LOCAL:
    //   RemoveIdFromGroup(vecRemoveId, kszSearchGroupId);
    //    break;
    //case SEARCH_SERVER:
    //    RemoveIdFromGroup(vecRemoveId, kszServerSearchGroupId);
    //    break;
    //default:
    //    break;
    //}

    ResetSearchKey();
    return DIR_RET_SUCC;
}

DIRECTORY_RET_CODE CBroadSoftContactManager::EditPersonalContact(int nId,
        ContactBroadSoftPersonal & refData, BSFT_SEARCH_TYPE eType/* = SEARCH_NONE*/)
{
    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(kszPersonalId));

    if (pMenu == NULL)
    {
        return DIR_RET_GROUP_UNEXISTED;
    }

    ContactBroadSoftPersonal * pContact = Base2Personal(GetDataById(nId));

    if (pContact == NULL)
    {
        return DIR_RET_FAIL;
    }

    if (refData.m_strDisplayName.empty()
            || refData.m_listNumber.size() == 0
            || refData.GetNumberByIndex(0).empty())
    {
        return DIR_RET_EMPTY;
    }


    //联系人重复
    if (refData.m_strDisplayName.compare(pContact->m_strDisplayName) != 0
            && pMenu->IsContactNameExist(refData.m_strDisplayName) != -1)
    {
        return DIR_RET_CONTACT_EXIST;
    }

    VectorId rmVect;
    rmVect.push_back(nId);


    //删除记录
    DIRECTORY_RET_CODE Rec = DeletePersonalContact(rmVect, eType);

    //在map中添加需要新加入编辑后的记录
    if (Rec == DIR_RET_SUCC)
    {
        m_editMap[nId] = refData;
    }
    return Rec;
}

bool CBroadSoftContactManager::OnTimeout(unsigned int uTimerID, bool bInTalk)
{
    if (bInTalk)
    {
        // 通话中不去更新
        return false;
    }

    if (uTimerID == (UINT)&m_iTimerID)
    {

        if (CBaseHelper::GetXSIStatus() != XSI_SUCCESS)
        {
            DIR_INFO("CContactBroadSoftManager::OnTimeoutMessage not to Update!");
            return true;
        }
        ReSetId();
        Update();
        return true;
    }
    else if (uTimerID == (UINT)&m_iInitTimer)
    {
        if (IsNetworkConnected())
        {
            timerKillThreadTimer((UINT)&m_iInitTimer);

            if (IsDirAvailable())
            {
                MyExecDownload(m_vGroupId[1], false, true);
            }
        }
        return true;
    }

    return false;
}

bool CBroadSoftContactManager::GetContactNameByNumber(const yl::string & strNumber,
        yl::string & strName, bool bSync /*= true*/)
{
    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(kszPersonalId));

    //http://10.2.1.199/Bug.php?BugID=33640
    if (pMenu != NULL && pMenu->m_eStatus == FILE_DOWNLOADED_OK)
    {
        for (int i = 0; i < pMenu->m_vChildrenId.size(); i++)
        {
            ContactBroadSoftPersonal * pContact = Base2Personal(GetDataById(pMenu->m_vChildrenId[i]));

            if (pContact == NULL)
            {
                continue;
            }

            if (strNumber.compare(pContact->GetNumberByIndex(0)) == 0)
            {
                strName = pContact->GetDisplayName();
                return true;
            }
        }
    }

    return CBaseContactManger::GetContactNameByNumber(strNumber, strName);
}

void CBroadSoftContactManager::ResetDisplayName(int nGroupID)
{
    if (nGroupID != kszEnterPriseId
            && nGroupID != kszGroupId
            && nGroupID != kszCustomId)
    {
        return;
    }
    DirGroupRecord * pGroup = GetMenuDataById(nGroupID);
    if (pGroup != NULL)
    {
        if (nGroupID == kszGroupId)
        {
            for (int i = 0; i < pGroup->m_vChildrenId.size(); ++i)
            {
                ContactBroadSoftGroup * pContact = Base2Group(GetDataById(pGroup->m_vChildrenId[i]));
                if (pContact != NULL)
                {
                    if (configParser_GetConfigInt(kszBsftDirNameShowMode) == 0)
                    {
                        pContact->m_strDisplayName = pContact->m_strFirstName + " " + pContact->m_strLastName;
                    }
                    else
                    {
                        pContact->m_strDisplayName = pContact->m_strLastName + "," + pContact->m_strFirstName;
                    }
                }
            }
        }
        else if (nGroupID == kszEnterPriseId)
        {
            for (int i = 0; i < pGroup->m_vChildrenId.size(); ++i)
            {
                ContactBroadSoftEnterprise * pContact = Base2Enterprise(GetDataById(pGroup->m_vChildrenId[i]));
                if (pContact != NULL)
                {
                    if (configParser_GetConfigInt(kszBsftDirNameShowMode) == 0)
                    {
                        pContact->m_strDisplayName = pContact->m_strFirstName + " " + pContact->m_strLastName;
                    }
                    else
                    {
                        pContact->m_strDisplayName = pContact->m_strLastName + "," + pContact->m_strFirstName;
                    }
                }
            }
        }
        else if (nGroupID == kszCustomId)
        {
            for (int i = 0; i < pGroup->m_vChildrenId.size(); ++i)
            {
                ContactBroadSoftCustom * pContact = Base2Custom(GetDataById(pGroup->m_vChildrenId[i]));
                if (pContact != NULL)
                {
                    if (configParser_GetConfigInt(kszBsftDirNameShowMode) == 0)
                    {
                        pContact->m_strDisplayName = pContact->m_strFirstName + " " + pContact->m_strLastName;
                    }
                    else
                    {
                        pContact->m_strDisplayName = pContact->m_strLastName + "," + pContact->m_strFirstName;
                    }
                }
            }
        }
    }
}

void CBroadSoftContactManager::MySort(int nGroupId)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup != NULL)
    {
        yl::stable_sort(pGroup->m_vChildrenId.begin(), pGroup->m_vChildrenId.end(), CompareBsftContact);
    }
}

void CBroadSoftContactManager::SortAllData()
{
    DIR_INFO("CBroadSoftContactManager::SortAllData [%d]", m_vGroupId.size());
    //组内联系人排序
    for (int i = 0; i < m_vGroupId.size(); i++)
    {
        if (m_vGroupId[i] != kszEnterPriseId
                && m_vGroupId[i] != kszGroupId
                && m_vGroupId[i] != kszCustomId
                && m_vGroupId[i] != kszAllContactId)
        {
            continue;
        }
        ResetDisplayName(m_vGroupId[i]);
        MySort(m_vGroupId[i]);
    }

    DIR_INFO("CBroadSoftContactManager::SortAllData End");
}

void CBroadSoftContactManager::ClearContactInfo(int nGroupID)
{
    // 不是分段下载不要删除
    if (BsftDir_UpdateEnable())
    {
        return;
    }

    DIR_INFO("CBroadSoftContactManager::ClearContactInfo [%d]", nGroupID);
    ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetMenuDataById(nGroupID));

    if (pGroup == NULL)
    {
        return ;
    }

    pGroup->m_mapNameToId.clear();
    pGroup->m_nNextStartIndex = 0;
    pGroup->m_nSearchStartIndex = 0;
    pGroup->m_nServerTotalCount = 0;
    pGroup->m_nCountPerPage = 0;
    pGroup->m_eStatus = FILE_DOWNLOAD_STATUS_NONE;

    ClearContactByGroupID(nGroupID);
}


bool CBroadSoftContactManager::HasNextPage(int nGroupID)
{
    // 不是分段下载
    if (BsftDir_UpdateEnable())
    {
        return false;
    }

    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(nGroupID));

    if (pMenu == NULL)
    {
        return false;
    }

    // 搜索不分页
    if (nGroupID == kszSearchGroupId)
    {
        return false;
    }

    DIR_INFO("CBroadSoftContactManager::HasNextPage [%d] [%d] [%d] [%d]",
             nGroupID, pMenu->m_nSearchStartIndex, pMenu->m_nNextStartIndex, pMenu->m_nServerTotalCount);

    if (pMenu->m_nServerTotalCount <= 0)
    {
        return false;
    }

    if (pMenu->m_nNextStartIndex == 0 && pMenu->m_nServerTotalCount == 0)
    {
        return true;
    }

    bool bHasNextPage = false;

    if (nGroupID == kszServerSearchGroupId
            && pMenu->m_bCustomSearch)
    {
        DirGroupRecord * pCustomMenu = GetMenuDataById(kszCustomId);

        if (pCustomMenu != NULL)
        {
            for (int i = 0; i < pCustomMenu->m_vChildrenId.size(); i++)
            {
                ContactBroadSoftMenu * pGroup = Base2BsftMenu(GetDataById(pCustomMenu->m_vChildrenId[i]));
                if (pGroup)
                {
                    bHasNextPage = (pGroup->m_nSearchStartIndex < pGroup->m_nServerTotalCount) || bHasNextPage;
                }
            }
        }
    }
    else
    {
        int nStartIndex = nGroupID == kszServerSearchGroupId ? pMenu->m_nSearchStartIndex : pMenu->m_nNextStartIndex;

        bHasNextPage = nStartIndex < pMenu->m_nServerTotalCount;
    }


    DIR_INFO("----------------has next page [%d]", bHasNextPage);
    return bHasNextPage;

}

bool CBroadSoftContactManager::ParsePageInfo(int GroupId, const xml_node& node, bool bIsSearch /*= false*/)
{
    DIR_INFO("CBSFTDirProcessor::ParsePageInfo [%d]", GroupId);
    if (node.empty())
    {
        return false;
    }

    ContactBroadSoftMenu * pMenu = Base2BsftMenu(GetDataById(GroupId));

    if (pMenu == NULL)
    {
        return false;
    }

    VectorId vecRemoveId;
    vecRemoveId.push_back(kszViewMoreId);
    RemoveIdFromGroup(vecRemoveId, GroupId);

    if (bIsSearch)
    {
        RemoveIdFromGroup(vecRemoveId, kszServerSearchGroupId);
    }

    yl::string strTotalCount;
    yl::string strCountPerPage;

    int nStartIndex = 0;

    xml_node NodeTotalCount = node.child(kszNodeTotalCount);

    xml_node NodeCountPerPage = node.child(kszNodeCountPerPage);

    xml_node NodeStartIndex = node.child(kszNodestartIndex);

    if (NodeTotalCount.empty() || NodeCountPerPage.empty()/* || NodeStartIndex.empty()*/)
    {
        pMenu->m_bReqNext = false;
        return false;
    }

    strCountPerPage = NodeCountPerPage.child_value();
    pMenu->m_nCountPerPage = atoi(strCountPerPage.c_str());

    strTotalCount = NodeTotalCount.child_value();
    pMenu->m_nServerTotalCount = atoi(strTotalCount.c_str());

    if (NodeStartIndex.empty())
    {
        if (bIsSearch)
        {
            ContactBroadSoftMenu* pSearchMenu = Base2BsftMenu(GetDataById(kszServerSearchGroupId));
            if (pSearchMenu != NULL)
            {
                // 应该拿刚解析出来的每页ID来判断
                nStartIndex = pSearchMenu->m_vChildrenId.size() - pMenu->m_nCountPerPage + 1;
            }
        }
        else
        {
            nStartIndex = pMenu->m_vChildrenId.size() - pMenu->m_nCountPerPage + 1;
        }
    }
    else
    {
        nStartIndex = atoi(NodeStartIndex.child_value());
    }

    // 每页个数小于0
    if (nStartIndex < 0)
    {
        nStartIndex = 0;
    }

    if (bIsSearch)
    {
        pMenu->m_nSearchStartIndex = nStartIndex + pMenu->m_nCountPerPage;
        ContactBroadSoftMenu* pSearchMenu = Base2BsftMenu(GetDataById(kszServerSearchGroupId));
        if (pSearchMenu != NULL)
        {
            if (pMenu->m_nParentId == kszCustomId)
            {
                pSearchMenu->m_nCountPerPage = yl::max(pSearchMenu->m_nCountPerPage, pMenu->m_nCountPerPage);
                pSearchMenu->m_nServerTotalCount = yl::max(pSearchMenu->m_nServerTotalCount, pMenu->m_nServerTotalCount);
            }
            else
            {
                pSearchMenu->m_nCountPerPage = pMenu->m_nCountPerPage;
                pSearchMenu->m_nServerTotalCount = pMenu->m_nServerTotalCount;
            }
            pSearchMenu->m_nSearchStartIndex = pMenu->m_nSearchStartIndex;

        }

    }
    else
    {
        pMenu->m_nNextStartIndex = nStartIndex + pMenu->m_nCountPerPage;
    }

    if (HasNextPage(GroupId))
    {
        pMenu->m_vChildrenId.push_back(kszViewMoreId);
    }

    if (bIsSearch && HasNextPage(kszServerSearchGroupId))
    {
        AddIdToGroup(kszViewMoreId, kszServerSearchGroupId, false);
    }

    pMenu->m_bReqNext = false;

    return false;
}

