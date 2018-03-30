#include "remotecontactmanager.h"
#include "contactremoteaction.h"
#include "basemethod.h"
#include "taskaction/modtaskaction.h"
#include "directorymessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "ylalgorithm.h"


// 远程地址文件下载到本地的保存名.
#define kszFileSavedName  "RemoteDirectory.%010d.xml"
// 远程联系人配置文件名.
#define kszRemoteConfigName  "remote_phonebook_list.xml"
// 最大buffer长度.
#define knMaxBufferLen  260
// 最多可配置的远程地址薄的数量.
#define knMaxCofigurableItem  5
// 配置文件中的总显示名.
#define kszAllDisplayName  "display_name"
// 以下为xml文件定义.
// 根节点.
#define kszRemoteNodeRoot  "root_remote_phonebook"
// 各个item结点.
#define kszNodeItem  "item"
// name属性.
#define kszAttrNameName  "name"
// url属性.
#define kszAttrNameUrl  "url"
// 用户名属性
#define kszAtterNameUserName   "username"
// 密码属性
#define kszAtterNamePassWord   "password"
// 树的根节点名.
#define kszTreeRoot  "<URL>All"
// 远程地址薄定时更新最短时间间隔(second).
#define knMinUpdateTimeInterval  3600
//by yuyt
#define knMaxUpdateTimeInterval 1296000
// 定时更新的定时器Id.
#define knTimerId  91110
// 个人联系人最大数(1000).
#define knMaxUnitCount  1000
// 组联系人最大级树(4).
#define knMaxMenuLevel  4

enum REMOTE_DIR_REQUEST_RESULT
{
    RD_SUCC = 0,            //请求成功
    RD_FAILED = 1,          //请求失败
    RD_AUTH_FAILED = 401,   //鉴权失败
};

CRemoteContactManger * CRemoteContactManger::GetInstance()
{
    static CRemoteContactManger instance;
    return &instance;
}

CRemoteContactManger::CRemoteContactManger()
{
    m_eDirType = REMOTE_DIR;
    m_strDisplayName.clear();
    m_iInitTimer = 5 * 1000;
}

CRemoteContactManger::~CRemoteContactManger()
{

}

bool CRemoteContactManger::Init()
{
    return MyInit();
}

bool CRemoteContactManger::MyInit()
{
    //必须先初始化基类初始化函数
    CBaseContactManger::MyInit();
    // 从Xml配置文件中加载数据.
    MyLoadDataFromConfigXml(kszRemoteConfigName);

    //保存组信息到临时文件，供网页使用
    SaveGroupInfoToTempFile("RemoteDirGroupInfo.xml", "RemoteDirGroup");

    SetAutoUpdateTimer();

    if (IsNetworkConnected())
    {
        Update();
    }
    else
    {
        timerSetThreadTimer((UINT)&m_iInitTimer, m_iInitTimer);
    }

    return true;
}

bool CRemoteContactManger::ReqLoadContact(int nGroupId, const yl::string & strSearchKey/* = ""*/)
{
    DIR_INFO("Remote Contact ReqLoadContact GroupId[%d] Key[%s]", nGroupId, strSearchKey.c_str());
    ContactRemoteMenu * pGroup = Base2RemoteMenu(GetMenuDataById(nGroupId));

    if (pGroup == NULL)
    {
        return false;
    }

    if (pGroup->m_eStatus == FILE_DOWNLOADING)
    {
        return true;
    }

    ClearContactByGroupID(nGroupId);

    pGroup->m_eStatus = FILE_DOWNLOAD_READY;

    DIR_INFO("CRemoteContactManger::ReqLoadContact ....");

    if (pGroup->m_bSearchRootGroup)
    {
        if (strSearchKey.empty())
        {
            return false;
        }

        pGroup->m_strSearchKey = strSearchKey;
        pGroup->m_nNextPageId = knInvalidRId;
        pGroup->m_nprevPageId = knInvalidRId;
        pGroup->m_nParentId = kszRootId;
    }

    return MyExecDownload(nGroupId) == FILE_DOWNLOADING;
}

FileDownloadStatus CRemoteContactManger::GetHotKeyStatus(const yl::string & strKey, int nGroupId,
        int & nReHotkeyId)
{
    FileDownloadStatus reStatus = FILE_DOWNLOAD_STATUS_NONE;
    ContactRemoteMenu * pMenu = Base2RemoteMenu(GetDataById(nGroupId));

    if (pMenu == NULL)
    {
        return reStatus;
    }

    for (YLList<int>::iterator iter = pMenu->m_listHotKey.begin(); iter != pMenu->m_listHotKey.end();
            iter++)
    {
        ContactRemoteHotKey * pHotKey = Base2HotKey(GetDataById(*iter));

        if (pHotKey != NULL && pHotKey->m_strKey.compare(strKey) == 0)
        {
            reStatus = pHotKey->m_eStatus;
            nReHotkeyId = pHotKey->m_nId;
            break;
        }
    }

    return reStatus;
}

void CRemoteContactManger::ReloadContact()
{
    MyFinal();
    MyInit();
}

DIRECTORY_RET_CODE CRemoteContactManger::Update(int nGroupId/* = -1*/)
{
    DIR_INFO("RemoteDir Update (nGroupId = %d)", nGroupId);
    if (!IsNetworkConnected())
    {
        DIR_INFO("network is unuseable!");
        return DIR_RET_RERROR_NETWORK_ERROR;
    }

    if (nGroupId != -1)
    {
        if (!ReqLoadContact(nGroupId))
        {
            return DIR_RET_FAIL;
        }
    }
    else
    {
        ClearContactByGroupID(kszAllContactId);

        for (int i = 0; i < m_vGroupId.size(); i++)
        {
            ContactRemoteMenu * pMenu = Base2RemoteMenu(GetDataById(m_vGroupId[i]));

            //已在下载中不在进行下载
            if (pMenu == NULL || pMenu->m_eStatus == FILE_DOWNLOADING || pMenu->m_bSearchRootGroup)
            {
                continue;
            }

            pMenu->m_listHotKey.clear();
            pMenu->m_eStatus = FILE_DOWNLOAD_READY;

            MyExecDownload(m_vGroupId[i]);
        }
    }
    return DIR_RET_SUCC;
}

int CRemoteContactManger::MyExecDownload(int nGroupId)
{
    static int s_nFileId = 0;  // 本地文件保存id名.

    ContactRemoteMenu * pGroup = Base2RemoteMenu(GetMenuDataById(nGroupId));

    if (pGroup != NULL && pGroup->m_eStatus != FILE_DOWNLOADING)
    {
        //级数限制
        int nLevel = GetLevelById(nGroupId);

        if (nLevel > knMaxMenuLevel)
        {
            pGroup->m_eStatus = FILE_DOWNLOAD_MENU_UPPER_LIMIT_REACHED;

            return FILE_DOWNLOAD_MENU_UPPER_LIMIT_REACHED;
        }

        yl::string strUrl =  pGroup->GetUrl();

        yl::string strUserName = pGroup->m_strUserName;

        yl::string strPassWord = pGroup->m_strPassWord;

        // 生成本地保存路径.
        yl::string strFormat(g_strTempPath);
        strFormat.append(kszFileSavedName);

        char szbuffer[218] = {0};
        sprintf(szbuffer, strFormat.c_str(), s_nFileId++);
        yl::string strFullPath = szbuffer;

        // 开始下载.
        CContactRemoteAction * pAction = CreateContactRemoteAction(
                                             strFullPath.c_str(), strUrl.c_str());

        if (pAction != NULL)
        {
            pGroup->m_eStatus = FILE_DOWNLOADING;
            pAction->SetRead(true);  // 读取文件.

            pAction->SetCallback(this);  // 回调函数.
            pAction->SetGroupId(nGroupId);
            pAction->SetUsername(strUserName);
            pAction->SetPassword(strPassWord);

            if (PT_T42 != devicelib_GetPhoneType()
                    && PT_T41 != devicelib_GetPhoneType())
            {
                pAction->SetMaxDownloadSize(GetDirMaxDownloadSize());
            }

            pAction->SetSync(false);
            return TaskAction_ExecAction(pAction)
                   ? FILE_DOWNLOADING : FILE_DOWNLOAD_FAIL;
        }
    }
    // 其他情况, 则下载失败.
    return FILE_DOWNLOAD_FAIL;
}

bool CRemoteContactManger::MyLoadDataFromConfigXml(
    const yl::string & strFileName)
{
    // 获取配置文件全路径.
    yl::string strFullXmlFilePath = g_strConfigPath;//(CONTACT_PATH);
    strFullXmlFilePath.append(strFileName);

    DIR_INFO("Load remote config file [%s]", strFullXmlFilePath.c_str());

    xml_document doc;
    doc.load_file(strFullXmlFilePath.c_str());

    xml_node nodeRootNode = doc.child(kszRemoteNodeRoot);
    if (!nodeRootNode)
    {
        return false;
    }

    xml_attribute attrTmp = nodeRootNode.attribute(kszAllDisplayName);
    if (attrTmp)
    {
        m_strDisplayName = attrTmp.value();
    }
    else
    {
        m_strDisplayName = "";
    }

    int nGroupId = kszAllContactId + 1;

    for (xml_node node = nodeRootNode.child(kszNodeItem); node; node = node.next_sibling(kszNodeItem))
    {
        yl::string strName = "";

        yl::string strUrl = "";

        yl::string strUserName = "";

        yl::string strPassWord = "";

        // 读取名称属性.
        attrTmp = node.attribute(kszAttrNameName);
        if (attrTmp)
        {
            strName = attrTmp.value();
        }

        attrTmp = node.attribute(kszAttrNameUrl);
        if (attrTmp)
        {
            strUrl = attrTmp.value();
        }

        attrTmp = node.attribute(kszAtterNameUserName);
        if (attrTmp)
        {
            strUserName = attrTmp.value();
        }

        attrTmp = node.attribute(kszAtterNamePassWord);
        if (attrTmp)
        {
            char pBuffer[160] = "";
            char pBufferData[160] = "";

            strPassWord = attrTmp.value();
            if (!strPassWord.empty())
            {
                strncpy(pBuffer, strPassWord.c_str(), chMIN(160, strPassWord.size()));
                pw_data_decrypt((const unsigned char*)pBuffer, (unsigned char*)pBufferData);
            }

            strPassWord = pBufferData;
        }

        if (!strName.empty())
        {
            ContactRemoteMenu * pMenu = new ContactRemoteMenu();

            if (pMenu == NULL)
            {
                return false;
            }

            pMenu->m_strDisplayName = strName;
            pMenu->m_strURL = strUrl;
            pMenu->m_strUserName = strUserName;
            pMenu->m_strPassWord = strPassWord;

            if (strUrl.find("#SEARCH") != yl::string::npos)
            {
                pMenu->m_bSearchRootGroup = true;
                pMenu->m_strTitle = strName;
            }

            int nId = InsertMap(pMenu, nGroupId++);

            AddIdToMenuShowList(nId);
        }

        if (m_vGroupId.size() >= knMaxCofigurableItem)
        {
            break;
        }
    }

    return true;
}

bool CRemoteContactManger::IsNeedShowAllGroup()
{
    return false;
}

CContactRemoteAction * CRemoteContactManger::CreateContactRemoteAction(
    const char * pszLocalFilePath, const char * pszRemoteFilePath)
{
#if (20 != PHONE_TYPE)
    CContactRemoteAction * pAction =
        new CContactRemoteAction(pszLocalFilePath, pszRemoteFilePath);
    return pAction;
#else
    return NULL;
#endif
}

void CRemoteContactManger::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    if (pTaskAction == NULL)
    {
        return;
    }

    if (pTaskAction->GetReqType() != NS_TA::TA_REQUEST_NETWORK_FILE)
    {
        CBaseContactManger::OnTaskActionCallback(pTaskAction);
        return;
    }

    DIR_INFO("Remote contact model: begin to deal with the download result.");

    // 返回结果的类型为请求网络文件.
    CContactRemoteAction * pRemoteAction = static_cast<CContactRemoteAction *>(pTaskAction);
    int iReqResult = RD_FAILED;//远程联系人请求结果
    if (pRemoteAction != NULL)
    {
        FileDownloadStatus eStatusNew = FILE_DOWNLOAD_FAIL;

        if (pRemoteAction->IsOperationSucceed())
        {
            eStatusNew = FILE_DOWNLOADED_OK;
        }

        if (eStatusNew == FILE_DOWNLOADED_OK)
        {
            iReqResult = RD_SUCC;
        }

        // 设置下载状态, 如果用户已取消则不能更改状态.
        ContactRemoteMenu * pMenu = Base2RemoteMenu(GetMenuDataById(pRemoteAction->GetGroupId()));

        if (pMenu == NULL)
        {
            iReqResult = RD_FAILED;
        }
        else if (pMenu->m_eStatus != FILE_DOWNLOADING)
        {
            return;
        }
        else
        {
            if (pRemoteAction->GetTaskActionErrorType() == NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED)
            {
                iReqResult = RD_AUTH_FAILED;
            }

            pRemoteAction->SetNeedReleaseData(false);

            pMenu->m_eStatus = eStatusNew;

            if (pRemoteAction->GetNextMenuPageGroup() != NULL)
            {
                pRemoteAction->GetNextMenuPageGroup()->m_nId = GetId();

                InsertMap(pRemoteAction->GetNextMenuPageGroup(), pRemoteAction->GetNextMenuPageGroup()->m_nId);

                pMenu->m_nNextPageId = pRemoteAction->GetNextMenuPageGroup()->m_nId;

                pRemoteAction->GetNextMenuPageGroup()->m_nParentId = pMenu->m_nParentId;
            }

            ListData & refListData = pRemoteAction->GetListData();

            if (!pRemoteAction->GetTitle().empty())
            {
                pMenu->m_strTitle = pRemoteAction->GetTitle();
            }

            int iAddCount = 0;
            for (ListData::iterator iter = refListData.begin(); iter != refListData.end(); iter++)
            {
                BaseDirDataRecord * pBase = *iter;

                if (NULL == pBase)
                {
                    continue;
                }

                if (pBase->m_nId == knInvalidRId)
                {
                    pBase->m_nId = GetId();
                }

                InsertMap(pBase, pBase->m_nId);

                if (pBase->m_eType == DIR_TYPE_REMOTE_HOTKEY)
                {
                    pMenu->m_listHotKey.push_back(pBase->m_nId);
                    MyExecDownload(pBase->m_nId);
                }
                else
                {
                    //异步线程已排序
                    AddIdToGroup(pBase->m_nId, pBase->m_nParentId, false);

                    if (pBase->m_eType == DIR_TYPE_REMOTE_UNIT && pMenu->m_eType != DIR_TYPE_REMOTE_HOTKEY
                            || DIR_TYPE_LOCAL_CONTACT == pBase->m_eType)
                    {
                        //单元联系人都加入到All组，不排序,超级搜索才用到，到时候结果再排序效率较好
                        AddIdToGroup(pBase->m_nId, kszAllContactId, false);
                    }
                    else if (pBase->m_eType == DIR_TYPE_REMOTE_MENU)
                    {
                        DirGroupRecord * pGroup = GetMenuDataById(pBase->m_nId);
                        //混合解析不需要再下载
                        if (pGroup != NULL && pGroup->m_eStatus != FILE_DOWNLOADED_OK)
                        {
                            MyExecDownload(pBase->m_nId);
                        }
                    }
                }
            }
        }


        etl_NotifyApp(false,
                      REMOTE_PHONEBOOK_ALL_DOWNLOADED_FINISHED,
                      pRemoteAction->GetGroupId(),
                      iReqResult);
    }
}

void CRemoteContactManger::SetAutoUpdateTimer()
{
    //if (IsDirAvailable())
    {
        int nTime = configParser_GetConfigInt(kszSRemoteNameFlashTime);
        //为0不更新
        if (nTime != 0)
        {
            // 读取定时器时间
            using namespace yl;
            int nInterVal = max(configParser_GetConfigInt(kszSRemoteNameFlashTime),
                                knMinUpdateTimeInterval) * 1000;

            //可能越界变负数
            if (nInterVal < 0 || nInterVal > knMaxUpdateTimeInterval * 1000)
            {
                nInterVal = 3600 * 1000;
            }
            APP_INFO("RemoteDirAutoUpdateTime:%d", nInterVal);
            // 启动定时更新的计时器.
            timerSetThreadTimer(knTimerId, nInterVal);

        }
        else
        {
            timerKillThreadTimer(knTimerId);
        }
    }
}

bool CRemoteContactManger::OnTimeout(unsigned int uTimerID, bool bInTalk)
{
    if (bInTalk)
    {
        // 通话中不去更新
        return false;
    }

    if (uTimerID == knTimerId)
    {
        DIR_INFO("CRemoteContactManger::OnTimeout Update");
        //UI 要求发送此消息
        etl_NotifyApp(false, REMOTE_PHONEBOOK_BEGIN_UPDATE, 0, 0);
        ReSetId();
        Update();
        return true;
    }
    else if (uTimerID == (UINT)&m_iInitTimer)
    {
        if (IsNetworkConnected())
        {
            timerKillThreadTimer((UINT)&m_iInitTimer);
            Update();
        }
        return true;
    }
    return false;
}

int CRemoteContactManger::GetLevelById(int nId)
{
    int nLevel = 0;
    while (nId > 0 && nId != knInvalidRId)
    {
        BaseDirDataRecord * pData = GetDataById(nId);

        if (pData != NULL)
        {
            nId = pData->m_nParentId;
            nLevel++;
        }
        else
        {
            break;
        }
    }
    return nLevel;
}

const yl::string & CRemoteContactManger::GetRemoteDisplayName()
{
    return m_strDisplayName;
}

bool CRemoteContactManger::ClearContactByGroupID(int nGroupId)
{
    if (nGroupId != kszAllContactId)
    {
        CBaseContactManger::ClearContactByGroupID(nGroupId);
    }
    else
    {
        ClearChildIDListByGroupID(kszAllContactId);
        for (int i = 0; i < m_vGroupId.size(); i++)
        {
            ClearContactByGroupID(m_vGroupId[i]);
        }
    }

    return true;
}

int CRemoteContactManger::GetRootGroupId(int nId)
{
    ContactRemoteMenu * pMenu = Base2RemoteMenu(GetDataById(nId));

    if (pMenu == NULL)
    {
        return knInvalidRId;
    }

    if (pMenu->m_nprevPageId > 0)
    {
        return GetRootGroupId(pMenu->m_nprevPageId);
    }
    else if (pMenu->m_nParentId > 0)
    {
        return GetRootGroupId(pMenu->m_nParentId);
    }
    else
    {
        return pMenu->m_nId;
    }
}

