#include "plcmcontactmanager.h"
#include "contactremoteaction.h"
#include "basemethod.h"
#include "taskaction/modtaskaction.h"
#include "directorymessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "ylalgorithm.h"
#include "devicelib/networkserver.h"

#if IF_BUG_PLCMDIR

#define  kszFileSavedName "plcmdir.xml"
#define knPlcmTimerId  91111


CPLCMContactManger * CPLCMContactManger::GetInstance()
{
    static CPLCMContactManger instance;
    return &instance;
}

CPLCMContactManger::CPLCMContactManger()
{
    m_eDirType = PLCM_DIR;
    m_iInitTimer = 5 * 1000;
}

CPLCMContactManger::~CPLCMContactManger()
{

}

bool CPLCMContactManger::Init()
{
    return MyInit();
}

bool CPLCMContactManger::MyInit()
{
    //必须先初始化基类初始化函数
    CBaseContactManger::MyInit();

    if (configParser_GetConfigInt(kszPlcmPhoneEnable) != 1)
    {
        timerKillThreadTimer(knPlcmTimerId);
        return false;
    }

    ContactRemoteMenu * pMenu = new ContactRemoteMenu();

    //pMenu->m_strDisplayName = kszPlcmPhoneBook;

    if (!GetRemoteUrl(pMenu->m_strURL))
    {
        delete pMenu;

        return false;
    }

    InsertMap(pMenu, kszPlcmDirGroupId);

    AddIdToMenuShowList(kszPlcmDirGroupId);

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

bool CPLCMContactManger::GetRemoteUrl(yl::string & strUrl)
{
    strUrl.clear();

    strUrl = configParser_GetConfigString(kszAutoProvisionUrl);

    //if (strUrl.empty())
    //{
    //  return false;
    //}


    yl::string strMac = netGetMACText('\0');

    strMac.to_lower();

    strUrl.append("/").append(strMac).append("-directory.xml");

    DIR_INFO("Plcm DIR url[%s]", strUrl.c_str());

    return true;
}

bool CPLCMContactManger::ReqLoadContact(int nGroupId)
{
    DIR_INFO("CPLCMContactManger::ReqLoadContact Begin");
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

    DIR_INFO("CPLCMContactManger::ReqLoadContact ....");


    return MyExecDownload(nGroupId) == FILE_DOWNLOADING;
}



void CPLCMContactManger::ReloadContact()
{
    MyFinal();
    MyInit();
}

DIRECTORY_RET_CODE CPLCMContactManger::Update()
{
    if (!IsNetworkConnected())
    {
        DIR_INFO("network is unuseable!");
        return DIR_RET_RERROR_NETWORK_ERROR;
    }


    if (!ReqLoadContact(kszPlcmDirGroupId))
    {
        return DIR_RET_FAIL;
    }


    return DIR_RET_SUCC;
}

int CPLCMContactManger::MyExecDownload(int nGroupId)
{
    ContactRemoteMenu * pGroup = Base2RemoteMenu(GetMenuDataById(nGroupId));

    if (pGroup != NULL && pGroup->m_eStatus != FILE_DOWNLOADING)
    {
        // 生成本地保存路径.
        yl::string strFullPath(g_strTempPath);
        strFullPath.append(kszFileSavedName);

        // 开始下载.
        CContactRemoteAction * pAction = CreateContactRemoteAction(
                                             strFullPath.c_str(), pGroup->GetUrl().c_str());

        DIR_INFO("CPLCMContactManger::MyExecDownload [%s]", pGroup->GetUrl().c_str());

        if (pAction != NULL)
        {
            pGroup->m_eStatus = FILE_DOWNLOADING;
            pAction->SetRead(true);  // 读取文件.

            pAction->SetCallback(this);  // 回调函数.
            pAction->SetGroupId(nGroupId);

            pAction->SetSync(false);
            return TaskAction_ExecAction(pAction)
                   ? FILE_DOWNLOADING : FILE_DOWNLOAD_FAIL;
        }
    }
    // 其他情况, 则下载失败.
    return FILE_DOWNLOAD_FAIL;
}



bool CPLCMContactManger::IsNeedShowAllGroup()
{
    return false;
}

CContactRemoteAction * CPLCMContactManger::CreateContactRemoteAction(
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

void CPLCMContactManger::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    CBaseContactManger::OnTaskActionCallback(pTaskAction);

    if (pTaskAction == NULL
            || pTaskAction->GetReqType() != NS_TA::TA_REQUEST_NETWORK_FILE)
    {
        return;
    }

    DIR_INFO("PLCM contact model: begin to deal with the download result.");

    // 返回结果的类型为请求网络文件.
    CContactRemoteAction * pRemoteAction = static_cast<CContactRemoteAction *>(pTaskAction);
    bool isFailed = true;
    if (pRemoteAction != NULL)
    {
        FileDownloadStatus eStatusNew = FILE_DOWNLOAD_FAIL;

        if (pRemoteAction->IsOperationSucceed())
        {
            eStatusNew = FILE_DOWNLOADED_OK;
        }

        if (eStatusNew == FILE_DOWNLOADED_OK)
        {
            isFailed = false;
        }

        // 设置下载状态, 如果用户已取消则不能更改状态.

        ContactRemoteMenu * pMenu = Base2RemoteMenu(GetMenuDataById(pRemoteAction->GetGroupId()));

        if (pMenu == NULL)
        {
            isFailed = true;
        }
        else if (pMenu->m_eStatus != FILE_DOWNLOADING)
        {
            return;
        }
        else
        {
            pRemoteAction->SetNeedReleaseData(false);

            pMenu->m_eStatus = eStatusNew;

            ListData & refListData = pRemoteAction->GetListData();

            for (ListData::iterator iter = refListData.begin(); iter != refListData.end(); iter++)
            {
                BaseDirDataRecord * pBase = *iter;

                if (pBase->m_nId == knInvalidRId)
                {
                    pBase->m_nId = GetId();
                }

                InsertMap(pBase, pBase->m_nId);

                //异步线程已排序
                AddIdToGroup(pBase->m_nId, pBase->m_nParentId, false);

                AddIdToGroup(pBase->m_nId, kszAllContactId, false);
            }
        }

        etl_NotifyApp(false,
                      PLCM_PHONEBOOK_ALL_DOWNLOADED_FINISHED,
                      pRemoteAction->GetGroupId(),
                      isFailed);

        DIR_INFO("Send messg to UI PLCM_PHONEBOOK_ALL_DOWNLOADED_FINISHED,GroupId[%d],bFail[%d]",
                 pRemoteAction->GetGroupId(), isFailed);
    }
}

void CPLCMContactManger::SetAutoUpdateTimer()
{
    timerKillThreadTimer(knPlcmTimerId);

    // 读取定时器时间.
    int nInterVal = configParser_GetConfigInt(kszPlcmPhoneUpdateInterval);

    if (nInterVal < 60)
    {
        nInterVal = 60;
    }

    using namespace yl;
    nInterVal = min(nInterVal, 86400) * 60 * 1000;

    // 启动定时更新的计时器.
    timerSetThreadTimer(knPlcmTimerId, nInterVal);
}

bool CPLCMContactManger::OnTimeout(unsigned int uTimerID, bool bInTalk)
{
    if (bInTalk)
    {
        // 通话中不去更新
        return false;
    }

    if (uTimerID == knPlcmTimerId)
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


#endif
