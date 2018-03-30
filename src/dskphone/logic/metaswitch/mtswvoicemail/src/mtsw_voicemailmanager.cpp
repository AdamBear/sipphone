#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "mtsw_voicemailmanager.h"
#include "service_ipvp.h"
#include "voice/include/modvoice.h"

#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#include "contacts/directory/include/moddirinterface.h"

#define kszTempFile TEMP_PATH "mtswdata"
#define kszVoiceMailFile TEMP_PATH "voicemail.wav"

#define kszVoiceMailWav "/voicemail.wav?id="
#define kszVoiceMailList "/data?data=Meta_Subscriber_MetaSphere_VoicemailMessages"

#define kszData "data"
#define kszVMID "Id"
#define kszVMRead "Read"
#define kszVMUrgent "Urgent"
#define kszVMDisplayName "Name"
#define kszVMNumber "From"
#define kszVMDate "Received"
#define kszVMDuration "Duration"

#define kszSplite "_"

#define MAX_LIST_COUNT  1000

IMPLEMENT_GETINSTANCE(CMTSWVoiceMailManager)

CMTSWVoiceMailManager::CMTSWVoiceMailManager()
{
}

CMTSWVoiceMailManager::~CMTSWVoiceMailManager()
{
    MyFinal();
}

bool CompareTime(VoicemailItemData * lData, VoicemailItemData * rData)
{
    if (lData == NULL || rData == NULL)
    {
        return false;
    }

    return strcmp(lData->m_strTime.c_str(), rData->m_strTime.c_str()) > 0;
}

void CMTSWVoiceMailManager::Init()
{
    m_iItemIndex = 0;
    m_nTotalUnread = 0;
    m_nIndexFrom = 0;
    m_nTotalCount = 0;
    m_bDownLoading = true;
    m_bPlayLoading = false;
    m_ePlayStatus = STATUS_STOP;
    m_fileData.m_iID = -1;
    m_fileData.m_eDownloadStatus = FILE_DOWNLOAD_STATUS_NONE;
}


void CMTSWVoiceMailManager::MyFinal()
{
    remove(kszVoiceMailFile);
    // 释放链表
    ItemList::iterator itB = m_listItem.begin();
    for (; itB != m_listItem.end(); ++itB)
    {
        DEL_OBJ((*itB));
    }
    m_listItem.clear();

    m_iItemIndex = 0;
    m_nTotalUnread = 0;
    m_nIndexFrom = 0;
    m_nTotalCount = 0;
}

BOOL CMTSWVoiceMailManager::GetMessageList(VoiceMsgSimpListData * pData, int & nTotal)
{
    // 参数检查.
    if (pData == NULL)
    {
        return FALSE;
    }

    // 获取下标起始值.
    int nIndexFrom = pData->m_nIndexFrom;
    int nIndexTo = 0;
    if (pData->m_nTotalCount == 0)
    {
        nIndexTo = pData->m_nIndexFrom + m_listItem.size();
    }
    else
    {
        nIndexTo = pData->m_nIndexFrom + pData->m_nCountToGet;
    }

    // 遍历并返回指定下标的信息列表.
    VoiceMsgItemSimpData stTMISD;
    int i = -1;
    ItemList::iterator citer = m_listItem.begin();
    for (; citer != m_listItem.end(); ++citer)
    {
        VoicemailItemData * pItemData = (*citer);
        if (pItemData != NULL)
        {
            ++i;
            if (i >= nIndexFrom
                    && i < nIndexTo
                    && MyID2VMSD(&stTMISD, *pItemData))
            {
                pData->m_listMessage.push_back(stTMISD);
            }
        }
    }  // for.

    // 获取结果总数.
    pData->m_nTotalCount = i + 1;
    nTotal = pData->m_nTotalCount;

    m_nTotalCount = pData->m_nTotalCount;

    return TRUE;
}

int CMTSWVoiceMailManager::MyInsertItem(const VoicemailItemData & refItem)
{
    VoicemailItemData * pNew = new VoicemailItemData();
    if (pNew == NULL)
    {
        return -1;
    }

    *pNew = refItem;
    m_listItem.push_back(pNew);
    m_nTotalCount++;
    if (pNew->m_eStatus == STATUS_UNREAD)
    {
        m_nTotalUnread++;
    }
    return m_nTotalCount;
}

BOOL CMTSWVoiceMailManager::MyID2VMSD(VoiceMsgItemSimpData * pData,
                                      const VoicemailItemData & refItem)
{
    if (pData == NULL)
    {
        return FALSE;
    }

    // 拷贝名字.
    yl::string strDisplay = "";  // 显示名.

    yl::string strTemp = GetNameFromContact(refItem.m_strFromUserName.c_str());

    if (strTemp.empty())
    {
        if (!refItem.m_strFromName.empty())
        {
            strTemp = refItem.m_strFromName;
        }
        else
        {
            strTemp = refItem.m_strFromUserName;
        }
    }

    if (refItem.m_bUrgent)
    {
        strDisplay.append(" ! ");
    }

    strDisplay.append(strTemp.c_str());

    // 从账号中查找账号名.
    pData->m_strName = strDisplay;

    // 拷贝时间.
    pData->m_tTime = TransformTime2Time_t(refItem.m_strTime.c_str(), true);

    pData->m_eStatus = refItem.m_eStatus;

    return TRUE;
}

yl::string CMTSWVoiceMailManager::GetNameFromContact(const char * lpszNumber)
{
    if (lpszNumber == NULL
            || strlen(lpszNumber) == 0)
    {
        return "";
    }

    yl::string strNumber = lpszNumber;

    yl::string strName;

    // 先从Metaswitch联系人查找
#if IF_FEATURE_METASWITCH_DIRECTORY
    if (configParser_GetConfigInt(kszMTSWDirEnable) != 0
            && configParser_GetConfigInt(kszMetaSwitchActive) != 0)
    {
        Dir_GetCotactNameByNumber(strNumber, strName, MTSW_DIR);
    }
#endif

    if (strName.empty())
    {
        Dir_GetCotactNameByNumber(strNumber, strName, LOCAL_DIR);
    }

    return strName.empty() ? "" : strName;
}

BOOL CMTSWVoiceMailManager::RemoveAllMessage()
{
    for (ItemList::iterator iter = m_listItem.begin();
            iter != m_listItem.end();)
    {
        VoicemailItemData * pItemData = (*iter);
        if (pItemData == NULL)
        {
            continue;
        }

        delete (*iter);
        (*iter) = NULL;
        iter = m_listItem.erase(iter);
    }

    m_nTotalCount = 0;
    return TRUE;
}

BOOL CMTSWVoiceMailManager::RemoveMessage()
{
    // 遍历信息列表, 从中删除指定组中下标对应的项.
    int i = -1;  // 记录成员在组中的下标.
    for (ItemList::iterator iter = m_listItem.begin();
            iter != m_listItem.end();
            ++iter)
    {
        VoicemailItemData * pItemData = (*iter);
        if (pItemData != NULL)
        {
            ++i;
            if (i == m_iItemIndex)
            {
                if (pItemData->m_eStatus == STATUS_UNREAD)
                {
                    m_nTotalUnread--;
                }

                delete (*iter);
                (*iter) = NULL;
                m_listItem.erase(iter);
                m_nTotalCount--;
                return TRUE;
            }
        }
    }  // for.

    return FALSE;
}

VoicemailItemData * CMTSWVoiceMailManager::MyFindItem(int nIndex)
{
    // 参数检查.
    if (nIndex < 0)
    {
        return NULL;
    }

    // 遍历查找到信息.
    int i = -1;
    for (ItemList::iterator iter = m_listItem.begin();
            iter != m_listItem.end();
            ++iter)
    {
        VoicemailItemData * pItemData = (*iter);
        if (pItemData != NULL)
        {
            ++i;
            if (i == nIndex)
            {
                return pItemData;
            }
        }
    }  // for.

    return NULL;
}


VoicemailItemData * CMTSWVoiceMailManager::MyFindNextUnReadItem(int nIndex)
{
    // 参数检查.
    if (nIndex < 0
            || GetTotalUnRead() < 1)
    {
        return NULL;
    }

    // 遍历查找到信息.
    int i = -1;
    for (ItemList::iterator iter = m_listItem.begin();
            iter != m_listItem.end();
            ++iter)
    {
        VoicemailItemData * pItemData = (*iter);
        if (pItemData != NULL)
        {
            ++i;
            if (nIndex >= i)
            {
                continue;
            }

            if (pItemData->m_eStatus == STATUS_UNREAD)
            {
                m_iItemIndex = i;
                return pItemData;
            }
        }
    }  // for.

    //搜索到最后一条数据依然不是未读邮件，则从第一条再次搜索
    i = -1;
    for (ItemList::iterator iter = m_listItem.begin();
            iter != m_listItem.end();
            ++iter)
    {
        VoicemailItemData * pItemData = (*iter);
        if (pItemData != NULL)
        {
            ++i;

            if (pItemData->m_eStatus == STATUS_UNREAD)
            {
                m_iItemIndex = i;
                return pItemData;
            }
        }
    }  // for.

    return NULL;
}

ReadStatus CMTSWVoiceMailManager::GetMessageStatus(int nIndex /*= -1*/)
{
    if (nIndex == -1)
    {
        nIndex = m_iItemIndex;
    }

    // 查找租中指定序号的信息数据.
    VoicemailItemData * pItemData = MyFindItem(nIndex);
    if (NULL == pItemData)
    {
        return STATUS_TYPE_NONE;
    }

    return pItemData->m_eStatus;
}

BOOL CMTSWVoiceMailManager::GetMessageDetail(VoiceMsgItemDetailData * pData, int nIndex)
{
    // 查找指定序号的信息数据.
    VoicemailItemData * pItemData = MyFindItem(nIndex);
    if (pItemData != NULL)
    {
        // 转换数据.
        MyID2VMIDD(pData, *pItemData);
        return TRUE;
    }

    return FALSE;
}

// BOOL CMTSWVoiceMailManager::GetNextUnReadMessage( VoiceMsgItemDetailData* pData, int nIndex )
// {
//  ItemData* pItemData = MyFindNextUnReadItem(nIndex);
//  if (pItemData != NULL)
//  {
//      // 转换数据.
//      MyID2VMIDD(pData, *pItemData);
//      return TRUE;
//  }
//
//  return FALSE;
// }

BOOL CMTSWVoiceMailManager::MyID2VMIDD(VoiceMsgItemDetailData * pData,
                                       const VoicemailItemData & refItem)
{
    if (pData != NULL)
    {
        yl::string strTemp = GetNameFromContact(refItem.m_strFromUserName.c_str());

        if (strTemp.empty())
        {
            if (!refItem.m_strFromName.empty())
            {
                strTemp = refItem.m_strFromName;
            }
            else
            {
                strTemp = refItem.m_strFromUserName;
            }
        }

        // 拷贝From.
        pData->m_strName = strTemp;
        pData->m_strNumber = refItem.m_strFromUserName;

        // 拷贝时间.
        pData->m_tTime = TransformTime2Time_t(refItem.m_strTime.c_str(), true);

        pData->m_strDuration = refItem.m_strDuration;

        pData->m_bUrgent = refItem.m_bUrgent;

        pData->m_eStatus = refItem.m_eStatus;

        return true;
    }

    return false;
}

yl::string CMTSWVoiceMailManager::FormatDuration(int nDuration)
{
    char szBuffer[32] = {0};

    int nHour = nDuration / 3600;
    int nMinSec = nDuration % 3600;
    int nMin = nMinSec / 60;
    int nSec = nMinSec % 60;

    sprintf(szBuffer, "%02d:%02d:%02d", nHour, nMin, nSec);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    return szBuffer;
}


time_t CMTSWVoiceMailManager::TransformTime2Time_t(LPCSTR strDate, BOOL bHaveSecond)
{
    //2014.12.02 19:44:13
    int iYear = 0;
    int iMonth = 0;
    int iDay = 0;
    int iHour = 0;
    int iMinute = 0;
    int iSecond = 0;
    int iZoneHour = 0;
    int iZoneMinute = 0;
    sscanf((char *)strDate, "%d.%d.%d %d:%d:%d", &iYear, &iMonth, &iDay, &iHour, &iMinute, &iSecond);
    yl::string strLogTime = (yl::string)(strDate);
    yl::string strTimeZone = "";
    int nLogTimeLen = strLogTime.length();

    if (nLogTimeLen >= 6)
    {
        strTimeZone = strLogTime.substr(nLogTimeLen - 6, 6);
    }

    sscanf(strTimeZone.c_str(), "%d:%d", &iZoneHour, &iZoneMinute);

    yl::string  szRetVal = "";

    //将时间转化成time_t
    tm tmWhen;
    tmWhen.tm_year = iYear - 1900;
    if (iMonth < 0)
    {
        iMonth = 0;
    }
    tmWhen.tm_mon = iMonth - 1;
    tmWhen.tm_mday = iDay;
    tmWhen.tm_hour = iHour ;
    tmWhen.tm_min = iMinute;
    if (bHaveSecond)
    {
        tmWhen.tm_sec = iSecond;
    }

    time_t tTime   =   mktime(&tmWhen);
    return tTime;
}

BOOL CMTSWVoiceMailManager::MarkReadStatus(ReadStatus eStatus)
{
    // 查找指定序号的信息数据.
    VoicemailItemData * pItemData = MyFindItem(m_iItemIndex);
    if (NULL == pItemData)
    {
        return FALSE;
    }

    if (pItemData->m_eStatus == eStatus)
    {
        return FALSE;
    }

    yl::string strId = pItemData->m_strId;

    yl::string strUrl = "/data?";

    yl::string strData = "";

    if (eStatus == STATUS_READ)
    {
        strData = "\r\nContent-Disposition: form-data; name=\"Meta_Subscriber_MetaSphere_VoicemailMessagesToMarkAsRead\"\r\n";
    }
    else if (eStatus == STATUS_UNREAD)
    {
        strData = "\r\nContent-Disposition: form-data; name=\"Meta_Subscriber_MetaSphere_VoicemailMessagesToMarkAsUnread\"\r\n";
    }

    strData.append("\r\n");
    strData.append("[{\"_\":\"");
    strData.append(strId.c_str());
    strData.append("\"}]\r\n");

    MTSW_ExecPost(this, strUrl, strData, ACTIONTYPE_MARK);

    pItemData->m_eStatus = eStatus;

    if (eStatus == STATUS_READ)
    {
        m_nTotalUnread--;
    }
    else if (eStatus == STATUS_UNREAD)
    {
        m_nTotalUnread++;
    }

    return TRUE;
}

BOOL CMTSWVoiceMailManager::Delete()
{
    VoicemailItemData * pItemData = MyFindItem(m_iItemIndex);
    if (NULL == pItemData)
    {
        return FALSE;
    }

    yl::string strId = pItemData->m_strId;

    yl::string strUrl = "/data?";

    yl::string strData =
        "\r\nContent-Disposition: form-data; name=\"Meta_Subscriber_MetaSphere_VoicemailMessagesToDelete\"\r\n";

    strData.append("\r\n");
    strData.append("[{\"_\":\"");
    strData.append(strId.c_str());
    strData.append("\"}]\r\n");

    MTSW_ExecPost(this, strUrl, strData, ACTIONTYPE_DELETE);

    return RemoveMessage();
}

BOOL CMTSWVoiceMailManager::DeleteAll()
{
    yl::string strUrl = "/data?";
    yl::string strID = "";
    yl::string strData =
        "\r\nContent-Disposition: form-data; name=\"Meta_Subscriber_MetaSphere_AllMessagesToDelete\"\r\n";
    strData.append("\r\n");
    strData.append("[");

    ItemList::iterator citer = m_listItem.begin();
    for (; citer != m_listItem.end(); ++citer)
    {
        VoicemailItemData * pItemData = (*citer);
        if (pItemData != NULL)
        {
            strID = pItemData->m_strId;

            strData.append("{\"_\":\"");
            strData.append(strID.c_str());
            strData.append("\"}");

            if (citer != m_listItem.end())
            {
                strData.append(",");
            }
        }
    }

    strData.append("]\r\n");

    MTSW_ExecPost(this, strUrl, strData, ACTIONTYPE_DELETE);

    return RemoveAllMessage();
}

BOOL CMTSWVoiceMailManager::DownloadVoiceMail()
{
    VoicemailItemData * pItemData = MyFindItem(m_iItemIndex);
    if (NULL == pItemData)
    {
        return FALSE;
    }

    yl::string strId = pItemData->m_strId;

    //下载的为同一个Voicemail,而且之前已经下载成功则直接发送消息播放语音
    if (atoi(strId.c_str()) == m_fileData.m_iID)
    {
        if (m_fileData.m_eDownloadStatus == FILE_DOWNLOADED_OK)
        {
            SETTING_INFO("[CMTSWVoiceMailManager] DownloadVoiceMail same ID and FILE_DOWNLOADED_OK");
            etl_NotifyApp(false, MTSW_MESSAGE_DOWNLOADED, m_iItemIndex, 0);
            return TRUE;
        }
        else if (m_fileData.m_eDownloadStatus == FILE_DOWNLOAD_READY)
        {
            SETTING_INFO("[CMTSWVoiceMailManager] DownloadVoiceMail same ID and FILE_DOWNLOADING");
            return TRUE;
        }
    }

    m_fileData.m_iID = atoi(strId.c_str());
    m_fileData.m_eDownloadStatus = FILE_DOWNLOAD_READY;

    yl::string strUrl = kszVoiceMailWav;
    strUrl.append(strId.c_str());

    MTSW_ExecDownLoad(this, strUrl, ACTIONTYPE_VMFILE, kszVoiceMailFile, m_fileData.m_iID);

    return TRUE;
}

BOOL CMTSWVoiceMailManager::Play()
{
    SETTING_INFO("[MTSWVoiceMailManager] Play voicemail PlayStatus[%d] Channel[%d]", GetPlayStatus(), voice_GetCurrentChannel());
    if (GetPlayStatus() != STATUS_STOP)
    {
        Stop(false);
    }
    SetPlayStatus(STATUS_PLAY);

    if (voice_GetCurrentChannel() == CHANNEL_IDLE)
    {
        voice_SwitchChannel(CHANNEL_SELECT);
    }

    int iRet = ipvp_media_play2(mkGetHandle(), kszVoiceMailFile, 0);
    return (iRet == 0);
}

BOOL CMTSWVoiceMailManager::Pause()
{
    SETTING_INFO("[MTSWVoiceMailManager] Pause voicemail");
    if (GetPlayStatus() != STATUS_PLAY)
    {
        return FALSE;
    }
    SetPlayStatus(STATUS_MTSW_PAUSE);

    int iRet = ipvp_media_pause(mkGetHandle(), 0);
    return (iRet == 0);
}

BOOL CMTSWVoiceMailManager::Resume()
{
    SETTING_INFO("[MTSWVoiceMailManager] Resume voicemail");
    if (GetPlayStatus() != STATUS_MTSW_PAUSE)
    {
        return FALSE;
    }
    SetPlayStatus(STATUS_PLAY);

    int iRet = ipvp_media_resume(mkGetHandle(), 0);
    return (iRet == 0);
}

BOOL CMTSWVoiceMailManager::Stop(bool bReset/* = true*/)
{
    SETTING_INFO("[MTSWVoiceMailManager] Stop voicemail bReset[%d]", bReset);
    if (GetPlayStatus() == STATUS_STOP)
    {
        if (bReset)
        {
            voice_SwitchChannel(CHANNEL_AUTO);
        }
        return FALSE;
    }
    SetPlayStatus(STATUS_STOP);

    // 若正在播放铃声, GUI不能停止, VPM会自动停止, 但状态需要更新
    if (voice_IsPlayingRing() || (ipvp_media_stop(mkGetHandle(), 0) == 0))
    {
        if (bReset)
        {
            // 声道切回
            voice_SwitchChannel(CHANNEL_AUTO);
        }
    }

    return TRUE;
}


BOOL CMTSWVoiceMailManager::DownloadNextVoiceMail()
{
    VoicemailItemData * pItemData = MyFindNextUnReadItem(m_iItemIndex);
    if (pItemData != NULL)
    {
        DownloadVoiceMail();
        return TRUE;
    }
    return FALSE;
}

int CMTSWVoiceMailManager::GetDuration()
{
    // 查找租中指定序号的信息数据.
    VoicemailItemData * pItemData = MyFindItem(m_iItemIndex);
    if (NULL == pItemData)
    {
        return 0;
    }

    return pItemData->m_iDuration;
}

BOOL CMTSWVoiceMailManager::LoadVoiceMailList()
{
    MyFinal();
    yl::string strUrl = kszVoiceMailList;
    m_fileData.m_iID = -1;
    if (m_fileData.m_eDownloadStatus != FILE_DOWNLOADING)
    {
        if (MTSW_ExecDownLoad(this, strUrl.c_str(), ACTIONTYPE_VMLIST, "", TRUE))
        {
            m_fileData.m_eDownloadStatus = FILE_DOWNLOADING;
            return TRUE;
        }
        else
        {
            SETTING_INFO("[CMTSWVoiceMailManager] MTSW_ExecDownLoad Fail!");
            m_fileData.m_eDownloadStatus = FILE_DOWNLOAD_FAIL;
            etl_NotifyApp(false, MTSW_MESSAGE_VMLIST_RESULT, 0, 0);
            return FALSE;
        }
    }
    SETTING_INFO("[CMTSWVoiceMailManager] LoadVoiceMailList Fail Already In Downloading");
    return FALSE;
}

bool CMTSWVoiceMailManager::ParseVoiceMailList(cJSON * pOjbParse)
{
    if (pOjbParse == NULL)
    {
        return FALSE;
    }

    VoicemailItemData itemData;

    cJSON * pData = cJSON_GetObjectItem(pOjbParse, kszData);

    if (pData == NULL)
    {
        return false;
    }

    char * pValue = NULL;

    for (cJSON * pMail = pData->child; pMail; pMail = pMail->next)
    {
        itemData.Reset();

        pValue = cJSON_GetFistChildValueByObjectName(pMail, kszVMID);

        if (pValue != NULL)
        {
            itemData.m_strId = pValue;
        }

        pValue = cJSON_GetFistChildValueByObjectName(pMail, kszVMDisplayName);
        if (pValue != NULL)
        {
            itemData.m_strFromName = pValue;

        }
        pValue = cJSON_GetFistChildValueByObjectName(pMail, kszVMNumber);
        if (pValue != NULL)
        {
            itemData.m_strFromUserName = pValue;

        }
        pValue = cJSON_GetFistChildValueByObjectName(pMail, kszVMDate);
        if (pValue != NULL)
        {
            itemData.m_strTime = pValue;
        }

        int nDuration = cJSON_GetFirstChildIntByObjectName(pMail, kszVMDuration);

        itemData.m_iDuration = nDuration / 1000;  //ms->s

        itemData.m_strDuration = FormatDuration(itemData.m_iDuration);

        bool bIsRead = cJSON_GetFirstChildBoolByObjectName(pMail, kszVMRead);

        itemData.m_eStatus = bIsRead ? STATUS_READ : STATUS_UNREAD;

        itemData.m_bUrgent = cJSON_GetFirstChildBoolByObjectName(pMail, kszVMUrgent);
        // PUSH到成员链表中去
        if (MyInsertItem(itemData) < 0)
        {
            return FALSE;
        }
    }

    Sort(m_listItem, CompareTime);

    return TRUE;
}

void CMTSWVoiceMailManager::OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType, int nId)
{
    m_fileData.m_eDownloadStatus = FILE_DOWNLOAD_FAIL;
    if (nType == ACTIONTYPE_VMFILE)
    {
        if (m_bPlayLoading
                && nId == m_fileData.m_iID)
        {
            etl_NotifyApp(false, MTSW_MESSAGE_DOWNLOADFAILED, eErrCode, 0);
        }
    }
    else if (nType == ACTIONTYPE_VMLIST)
    {
        if (eErrCode != NS_TA::TA_SESSION_EXPIRED)
        {
            etl_NotifyApp(false, MTSW_MESSAGE_VMLIST_RESULT, eErrCode, 0);
        }
    }
    return;
}

bool CMTSWVoiceMailManager::OnLoadSucProcess(CMTSWAction * pAction)
{
    if (m_fileData.m_eDownloadStatus == FILE_DOWNLOAD_CANCELED)
    {
        SETTING_INFO("CMTSWVoiceMailManager::FILE_DOWNLOAD_CANCELED");
        return false;
    }

    m_fileData.m_eDownloadStatus = FILE_DOWNLOADED_OK;
    if (pAction->GetMTSWActionType() == ACTIONTYPE_VMLIST)
    {
        bool bsuc = ParseVoiceMailList(pAction->GetJsonObject());

        etl_NotifyApp(false, MTSW_MESSAGE_VMLIST_RESULT, 0, bsuc);
    }
    else if (pAction->GetMTSWActionType() == ACTIONTYPE_VMFILE)
    {
        if (m_bPlayLoading
                && m_fileData.m_iID == pAction->GetExtraParam())
        {
            etl_NotifyApp(false, MTSW_MESSAGE_DOWNLOADED, m_iItemIndex, 0);
        }
    }
    return true;
}

bool CMTSWVoiceMailManager::IsMessagesListEnable()
{
    bool bIsEnable = (configParser_GetConfigInt(kszMetaSwitchActive) == 1
                      && configParser_GetConfigInt(kszMTSWCommportalEnable) == 1
                      && configParser_GetConfigInt(kszMTSWVMListEnable) == 1);
    return bIsEnable;
}
#endif
