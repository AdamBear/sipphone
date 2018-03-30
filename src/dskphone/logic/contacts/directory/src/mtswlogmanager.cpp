#if IF_FEATURE_METASWITCH_CALLLOG
#include "mtswlogmanager.h"
#include "configiddefine.h"
#include "basemethod.h"
#include "directorymessagedefine.h"
#include "moddirinterface.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include "locallogmanager.h"
#include "metaswitch/mtswcontrol/include/mtswaction.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"

#define kszCallLogUrl "/data?data=Meta_Subscriber_CallLists"

//解析时数据
#define kszData "data"
#define kszCall "Call"
#define kszMissedCalls "MissedCalls"
#define kszAnsweredCalls "AnsweredCalls"
#define kszDialedCalls "DialedCalls"
#define kszSplite "_"


CMTSWLogManager * CMTSWLogManager::GetInstance()
{
    static CMTSWLogManager instance;
    return &instance;
}

CMTSWLogManager::CMTSWLogManager()
{
    m_eDirType = MTSW_LOG;
    m_eStatus = FILE_DOWNLOAD_STATUS_NONE;
    m_bIsNeedAddToAllGroup = true;
    MyInit();
}

CMTSWLogManager::~CMTSWLogManager()
{
}

bool CMTSWLogManager::MyInit()
{
    return MyCreatGroupsFromConfig();
}

bool CMTSWLogManager::MyCreatGroupsFromConfig()
{
    if (configParser_GetConfigInt(kszMTSWCallListEnable) == 0
            || configParser_GetConfigInt(kszMetaSwitchActive) != 1)
    {
        return false;
    }

    // 创建组
    CreatConfigGroup(kszAllContactId, TRID_ALL_CALLLOG);
    CreatConfigGroup(kszPlacedLogId, TRID_PLACED_CALL_LOG);
    CreatConfigGroup(kszMissLogId, TRID_MISSED_CALLLOG);
    CreatConfigGroup(kszReceivedLogId, TRID_RECEIVED_CALL_LOG);

    return true;
}

void CMTSWLogManager::CreatConfigGroup(int nGroupId, yl::string strName)
{
    MTSWLogGroupRecord * pGroup = new MTSWLogGroupRecord();

    if (pGroup != NULL)
    {
        pGroup->m_strDisplayName = strName;

        InsertMap(pGroup, nGroupId);

        AddIdToMenuShowList(nGroupId);
    }
}

bool CMTSWLogManager::ClearCallLog()
{
    //清空所有历史记录
    ClearAllContactFromMap();

    ReSetId();
    return true;
}

void CMTSWLogManager::PreLoad()
{
    MyFinal();
    if (!MyInit())
    {
        return;
    }

    if (MTSW_IsLogined())
    {
        ReqLoadCallLog();
    }
}

bool CMTSWLogManager::ReqLoadCallLog()
{
    if (m_eStatus == FILE_DOWNLOADING)
    {
        DIR_INFO("-=-=-=-=CallList DownLoading=-=-=-=-\n");
        return true;
    }
    //清空所有历史记录
    ClearCallLog();

    return MyExecDownload();
}

bool CMTSWLogManager::MyExecDownload()
{
    yl::string strUrl = kszCallLogUrl;

    if (MTSW_ExecDownLoad(this, strUrl.c_str(), ACTION_CALLLOGLIST))
    {
        m_eStatus = FILE_DOWNLOADING;

        return true;
    }

    // 其他情况, 则下载失败.
    m_eStatus = FILE_DOWNLOAD_FAIL;
    return false;
}

bool CMTSWLogManager::ParseDownloadedFile(cJSON * pObject)
{
    bool bResult = false;

    if (pObject == NULL)
    {
        return false;
    }

    cJSON * pData = cJSON_GetObjectItem(pObject, "data");

    if (pData == NULL)
    {
        return false;
    }

    cJSON * pMissCall = cJSON_GetObjectItem(pData, kszMissedCalls);
    bResult |= ParseJsonData(pMissCall, kszMissLogId, CT_MISSCALL);
    cJSON * pAnsweredCall = cJSON_GetObjectItem(pData, kszAnsweredCalls);
    bResult |=  ParseJsonData(pAnsweredCall, kszReceivedLogId, CT_INCOMING);
    cJSON * pDialedCall = cJSON_GetObjectItem(pData, kszDialedCalls);
    bResult |=  ParseJsonData(pDialedCall, kszPlacedLogId, CT_OUTGOING);

    return bResult;
}

// 解析组
bool CMTSWLogManager::ParseJsonData(cJSON * objJson, int nGroupId, CALL_TYPE eLogType)
{
    bool bResult = true;

    if (objJson == NULL)
    {
        return false;
    }

    cJSON * pCall = cJSON_GetObjectItem(objJson, kszCall);

    if (pCall != NULL)
    {
        for (cJSON * pData = pCall->child; pData != NULL; pData = pData->next)
        {
            MTSWCallLogRecord * pNew = new MTSWCallLogRecord();

            if (pNew->PraseData(pData))
            {
                pNew->m_eLogType = eLogType;
                pNew->m_nParentId = nGroupId;

                AddContact(pNew, nGroupId, true, false);
            }
            else
            {
                delete pNew;
                pNew = NULL;
            }
        }

        return true;
    }

    return false;
}

bool CMTSWLogManager::IsNeedShowAllGroup()
{
    return false;
}

const yl::string & CMTSWLogManager::GetSortKeyById(int nId)
{
    MTSWCallLogRecord * pLog = Base2MTSWLog(GetDataById(nId));

    if (pLog != NULL)
    {
        return pLog->m_strBornTick;
    }

    return kszEmptyString;
}

void CMTSWLogManager::OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType, int nId)
{
    DIR_INFO("-=-=-=-=OnRequestError:%d=-=-=-=-=-\n", eErrCode);
    m_eStatus = FILE_DOWNLOAD_FAIL;

    if (eErrCode != TA_SESSION_EXPIRED)
    {
        etl_NotifyApp(
            FALSE,
            METASWITCH_CALLLOG_DOWNLOADED_RESULT,
            0,
            eErrCode);
    }
}

bool CMTSWLogManager::OnLoadSucProcess(CMTSWAction * pAction)
{

    DIR_INFO("CMTSWLogManager::OnLoadSucProcess");

    BOOL bResult = false;


    bResult = ParseDownloadedFile(pAction->GetJsonObject());


    m_eStatus = bResult ? FILE_DOWNLOADED_OK : FILE_DOWNLOAD_FAIL;

    //清空miss notify
    if (bResult)
    {
        g_pLocalLogManager->ClearLogs(kszMissNotifId);
    }

    etl_NotifyApp(
        FALSE,
        METASWITCH_CALLLOG_DOWNLOADED_RESULT,
        0,
        bResult);

    return bResult;
}

#endif

