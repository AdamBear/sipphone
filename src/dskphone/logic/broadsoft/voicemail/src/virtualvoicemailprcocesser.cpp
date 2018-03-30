#include "virtualvoicemailprcocesser.h"
#include "xmlparser/xmlparser.hpp"
#include "broadsoft/xsiactionhelper/include/xsiaction.h"
#include "dsklog/log.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "commonunits/modfileoperate.h"
#include "commonunits/modcommonunits.h"
#include "voice/include/modvoice.h"
#include "commonapi/stringhelperapi.h"
#include "devicelib/phonedevice.h"
#include <service_ipvp.h>

static int VirtualMailMaxGetBufferSize()
{
    return (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType()) ? 1024 * 1024 * 3 : 1024 * 1024 * 1;
}

static int VirtualMailMaxGetFileSize()
{
    return (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType()) ? 1024 * 1024 * 3 : 1024 * 1024 * 1;
}


int TranTimeString2Int(yl::string strTime)
{
    if (strTime.size() > 10)
    {
        strTime = strTime.substr(0, 10);
    }

    return atoi(strTime.c_str());
}

bool CompareTime(sVoiceMailData & lData, sVoiceMailData & rData)
{
    return TranTimeString2Int(lData.m_strTime) > TranTimeString2Int(rData.m_strTime);
}

CVirtualVoiceMailPrcocesser * CVirtualVoiceMailPrcocesser::GetInstance()
{
    static CVirtualVoiceMailPrcocesser s_Inst;

    return &s_Inst;
}

CVirtualVoiceMailPrcocesser::CVirtualVoiceMailPrcocesser()
    : m_bIsLoadingList(false)
    , m_ePlayStatus(PLAYSTATUS_None)
    , m_nPlayIndex(-1)
{
    XsiAction::InsertMap(XSI_VOICEMESG_LIST_GET, "voicemessagingmessages");
    XsiAction::InsertMap(XSI_VOICEMESG_DETAIL_GET, "voicemessagingmessages/");
    XsiAction::InsertMap(XSI_VOICEMESG_DELETE, "voicemessagingmessages/");
    XsiAction::InsertMap(XSI_VOICEMESG_MARKALL_READ_PUT, "voicemessagingmessages/markAllAsRead");
    XsiAction::InsertMap(XSI_VOICEMESG_MARKALL_UNREAD_PUT, "voicemessagingmessages/markAllAsUnread");
    XsiAction::InsertMap(XSI_VOICEMESG_MARK_READ_PUT, "voicemessagingmessages/");
    XsiAction::InsertMap(XSI_VOICEMESG_MARK_UNREAD_PUT, "voicemessagingmessages/");
}


CVirtualVoiceMailPrcocesser::~CVirtualVoiceMailPrcocesser()
{
}

int CVirtualVoiceMailPrcocesser::getMaxDuration(eMediaType eType/* = MEDIA_audio*/)
{
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    int nDuration = 0;

    if (eType == MEDIA_video)
    {
        switch (ePhoneType)
        {
        case PT_T49:
            {
                nDuration = 30 * 1000;
            }
            break;
        default:
            break;
        }

        return nDuration;
    }

    switch (ePhoneType)
    {
    case PT_T49:
        {
            nDuration = 2 * 60 * 1000;
        }
        break;
    case PT_T29:
    case PT_T46:
    case PT_T69:
    case PT_T48:
    case PT_T48S:
    case PT_T46S:
    case PT_T69S:
    case PT_T54S:
    case PT_T52:

        {
            nDuration = 60 * 1000;
        }
        break;
    case PT_T27:
    case PT_T41:
    case PT_T42:
    case PT_T43P:
    case PT_T43S:
    case PT_T27S:
    case PT_CP860:
    case PT_CP920:
        {
            nDuration = 25 * 1000;
        }
        break;
    default:
        break;
    }

    return nDuration;
}

bool CVirtualVoiceMailPrcocesser::ReqVoicemessageList()
{
    APP_INFO("CVirtualVoiceMailPrcocesser::ReqVoicemessageList() m_bIsLoadingList[%d] m_nLineID[%d]",
             m_bIsLoadingList, m_nLineID);
    m_vecMailData.clear();

    if (m_bIsLoadingList)
    {
        return true;
    }

    if (ExeXsiAction(m_nLineID, XSI_VOICEMESG_LIST_GET) != NULL)
    {
        m_nPlayIndex = -1;
        m_bIsLoadingList = true;

        return true;
    }

    return false;
}

bool CVirtualVoiceMailPrcocesser::ReqPlayContentByIndex(int index)
{
    APP_INFO("CVirtualVoiceMailPrcocesser::ReqPlayContentByIndex[%d] m_nPlayIndex[%d]", index,
             m_nPlayIndex);
    //已经下载完则不需重新下载
    if (m_nPlayIndex == index)
    {
        etl_NotifyApp(false, XSI_MESSAGE_REQ_FINISH, XSI_VOICEMESG_DETAIL_GET, m_nLineID);
        return true;
    }

    if (index >= m_vecMailData.size()
            || index < 0)
    {
        return false;
    }

    sVoiceMailData & refData = m_vecMailData[index];

    //目前不支持视频播放
    if (refData.m_eMediaType == MEDIA_video
            && !IsSupportVideoMail())
    {
        SXsiExtenInfo xsiExtenInfo;
        xsiExtenInfo.m_nError = TA_FILE_OPERATION_NOT_SUPPORTED;
        xsiExtenInfo.m_strErrInfo = "Video File Not Supported";
        etl_NotifyAppEx(false, XSI_MESSAGE_REQ_FAIL, XSI_VOICEMESG_DETAIL_GET,  m_nLineID,
                        sizeof(SXsiExtenInfo), &xsiExtenInfo);
        return true;
    }

    if (refData.m_strMessgId.empty())
    {
        return false;
    }

    int nMaxDuration = getMaxDuration(refData.m_eMediaType);
    if (refData.m_nDuration > nMaxDuration)
    {
        SXsiExtenInfo xsiExtenInfo;
        xsiExtenInfo.m_nError = TA_FILE_EXCEED_MAX_SIZE;
        xsiExtenInfo.m_strErrInfo = "Request File To Large";
        etl_NotifyAppEx(false, XSI_MESSAGE_REQ_FAIL, XSI_VOICEMESG_DETAIL_GET,  m_nLineID,
                        sizeof(SXsiExtenInfo), &xsiExtenInfo);

        APP_INFO("voice mail is too large[%d] limit[%d]", refData.m_nDuration, nMaxDuration);
        return true;
    }

    XsiAction * pAction = ExeXsiAction(m_nLineID, XSI_VOICEMESG_DETAIL_GET, refData.m_strMessgId);
    if (pAction != NULL)
    {
        pAction->SetExtraParam(index);
        return true;
    }

    return false;
}

bool CVirtualVoiceMailPrcocesser::ReqDeleteMessageByIndex(int index)
{
    if (index >= m_vecMailData.size()
            || index < 0)
    {
        return false;
    }

    sVoiceMailData & refData = m_vecMailData[index];

    XsiAction * pAction = ExeXsiAction(m_nLineID, XSI_VOICEMESG_DELETE, refData.m_strMessgId);
    if (pAction != NULL)
    {
        pAction->SetExtraParam(index);
        return true;
    }

    return false;
}

bool CVirtualVoiceMailPrcocesser::ReqMarkAllRead()
{
    return ExeXsiAction(m_nLineID, XSI_VOICEMESG_MARKALL_READ_PUT) != NULL;
}

bool CVirtualVoiceMailPrcocesser::ReqMarkAllUnRead()
{
    return ExeXsiAction(m_nLineID, XSI_VOICEMESG_MARKALL_UNREAD_PUT) != NULL;
}

bool CVirtualVoiceMailPrcocesser::ReqMarkByIndex(int index, bool bRead)
{
    if (index >= m_vecMailData.size()
            || index < 0)
    {
        return false;
    }

    sVoiceMailData & refData = m_vecMailData[index];

    if (bRead == refData.m_bRead)
    {
        return true;
    }

    eXsiActionType eType = XSI_VOICEMESG_MARK_READ_PUT;
    yl::string strExtUrl = refData.m_strMessgId;
    if (!bRead)
    {
        eType = XSI_VOICEMESG_MARK_UNREAD_PUT;
        strExtUrl.append(kszReqMarkAsUnRead);
    }
    else
    {
        strExtUrl.append(kszReqMarkAsRead);
    }

    XsiAction * pAction = ExeXsiAction(m_nLineID, eType, strExtUrl);
    if (pAction != NULL)
    {
        pAction->SetExtraParam(index);
        return true;
    }

    return false;
}

bool CVirtualVoiceMailPrcocesser::ProcessByType(XsiAction * pAction)
{
    if (pAction == NULL)
    {
        return false;
    }
    bool suc = true;
    APP_INFO("CVirtualVoiceMailPrcocesser::ProcessByType[%d]", pAction->GetXsiType());
    switch (pAction->GetXsiType())
    {
    case XSI_VOICEMESG_LIST_GET:
        {
            suc = PraseMesgList(pAction->GetFilePath());
            m_bIsLoadingList = false;
        }
        break;

    case XSI_VOICEMESG_DETAIL_GET:
        {
            suc = PraseMesgDetail(pAction->GetFilePath(), pAction->GetExtraParam());
        }
        break;

    case XSI_VOICEMESG_MARKALL_READ_PUT:
        {
            suc = ProcessMarkAll(true);
        }
        break;

    case XSI_VOICEMESG_MARKALL_UNREAD_PUT:
        {
            suc = ProcessMarkAll(false);
        }
        break;

    case XSI_VOICEMESG_MARK_READ_PUT:
        {
            suc = ProcessMark(pAction->GetExtraParam(), true);
        }
        break;

    case XSI_VOICEMESG_MARK_UNREAD_PUT:
        {
            suc = ProcessMark(pAction->GetExtraParam(), false);
        }
        break;
    case XSI_VOICEMESG_DELETE:
        {
            suc = ProcessDelete(pAction->GetExtraParam());
        }
        break;
    default:
        break;
    }

    return suc;
}

bool CVirtualVoiceMailPrcocesser::ProcessDelete(int index)
{
    if (index >= m_vecMailData.size()
            || index < 0)
    {
        return false;
    }

    m_vecMailData.removeAt(index);
    return true;
}

bool CVirtualVoiceMailPrcocesser::ProcessMarkAll(bool bRead)
{
    for (int i = 0; i < m_vecMailData.size(); i++)
    {
        sVoiceMailData & refData = m_vecMailData[i];

        refData.m_bRead = bRead;
    }

    return true;
}

bool CVirtualVoiceMailPrcocesser::ProcessMark(int index, bool bRead)
{
    APP_INFO("CVirtualVoiceMailPrcocesser::ProcessMark(index %d, bRead %d)", index, bRead);
    if (index >= m_vecMailData.size()
            || index < 0)
    {
        return false;
    }

    sVoiceMailData & refData = m_vecMailData[index];

    refData.m_bRead = bRead;
    return true;
}

bool CVirtualVoiceMailPrcocesser::PraseMesgList(const yl::string & strFile)
{
    xml_document doc;

    if (!doc.load_file(strFile.c_str()))
    {
        return false;
    }

    xml_node InfoListNode = doc.child(kszNodeVoiceMessagingMessages).child(kszNodemessageInfoList);

    if (InfoListNode.empty())
    {
        return false;
    }

    for (xml_node InfoNode = InfoListNode.child(kszNodemessageInfo); !InfoNode.empty();
            InfoNode = InfoNode.next_sibling(kszNodemessageInfo))
    {
        sVoiceMailData NewData;

        NewData.m_nLine = m_nLineID;

        xml_node UserNode = InfoNode.child(kszNodecallingPartyInfo);

        if (!UserNode.empty())
        {
            xml_node PrivacyNode = UserNode.child(kszNodeprivacy);
            if (!PrivacyNode.empty())
            {
                NewData.m_strName = "";
                NewData.m_strNumber = "";
                NewData.m_bPrivacy = true;
            }
            else
            {
                xml_node NameNode = UserNode.child(kszNodename);
                xml_node NumberNode = UserNode.child(kszNodeaddress);

                if (!NumberNode.empty())
                {
                    NewData.m_strNumber = NumberNode.child_value();

                    int indexRight = NewData.m_strNumber.find_first_of("@");
                    if (indexRight != yl::string::npos)
                    {
                        NewData.m_strNumber = NewData.m_strNumber.substr(0, indexRight);
                    }

                    int indexLeft = NewData.m_strNumber.find_first_of(":");
                    if (indexLeft != yl::string::npos)
                    {
                        NewData.m_strNumber = NewData.m_strNumber.substr(indexLeft + 1);
                    }
                }

                if (!NameNode.empty())
                {
                    NewData.m_strName = NameNode.child_value();
                }
                else
                {
                    NewData.m_strName = NewData.m_strNumber;
                }
            }
        }

        xml_node durationNode = InfoNode.child(kszNodeduration);

        if (!durationNode.empty())
        {
            NewData.m_nDuration = atoi(durationNode.child_value());
        }

        xml_node TimeNode = InfoNode.child(kszNodetime);

        if (!TimeNode.empty())
        {
            NewData.m_strTime = TimeNode.child_value();
            CorrectTimeZone(NewData.m_strTime);
        }

        xml_node videoNode = InfoNode.child(kszNodevideo);

        if (!videoNode.empty())
        {
            if (configParser_GetConfigInt(kszBroadsoftShowVideoMail) != 1)
            {
                continue;
            }
            NewData.m_eMediaType = MEDIA_video;
        }

        xml_node readNode = InfoNode.child(kszNoderead);

        if (!readNode.empty())
        {
            NewData.m_bRead = true;
        }

        xml_node urgentNode = InfoNode.child(kszNodeurgent);

        if (!urgentNode.empty())
        {
            NewData.m_bUrgent = true;
        }

        xml_node confidentialNode = InfoNode.child(kszNodeconfidential);

        if (!confidentialNode.empty())
        {
            NewData.m_bConfidential = true;
        }

        xml_node MessageIdNode = InfoNode.child(kszNodemessageId);

        if (!MessageIdNode.empty())
        {
            yl::string strMessageId = MessageIdNode.child_value();
            int index = strMessageId.find_last_of("/");

            if (index != yl::string::npos)
            {
                NewData.m_strMessgId = strMessageId.substr(index + 1);
            }
            else
            {
                NewData.m_strMessgId = strMessageId;
            }
        }
        //APP_INFO("\nm_strName[%s]  m_strNumber[%s] m_strMessgId[%s]   \n   m_nDuration[%d] m_nTime[%s]  m_bRead[%d] m_bUrgent[%d] m_bConfidential[%d]\n"
        //       , NewData.m_strName.c_str(), NewData.m_strNumber.c_str(), NewData.m_strMessgId.c_str(), NewData.m_nDuration, NewData.m_strTime.c_str(), NewData.m_bRead, NewData.m_bUrgent, NewData.m_bConfidential);
        m_vecMailData.push_back(NewData);
    }

    Sort(m_vecMailData, CompareTime);

    return true;
}

bool CVirtualVoiceMailPrcocesser::PraseMesgDetail(const yl::string & strFile, int index)
{
#if NEW_BASE64_DECODE
    if (index >= m_vecMailData.size()
            || index < 0)
    {
        return false;
    }

    int iFileSize = pathFileSize(strFile.c_str());
    if (iFileSize > VirtualMailMaxGetFileSize())
    {
        APP_WARN("voicefile data is too large [%u] limit[%d]!", iFileSize, VirtualMailMaxGetBufferSize());
        return false;
    }

    yl::string strVoiceFile(kszVoiceMailFilePath);
    remove(strVoiceFile.c_str());

    FILE * pVoiceFile = fopen(strVoiceFile.c_str(), "a");
    FILE * pFile = fopen(strFile.c_str(), "r");
    if (pFile == NULL || pVoiceFile == NULL)
    {
        return false;
    }

    char szCode[4096] = {0};
    if (fread(szCode, 1, sizeof(szCode), pFile) <= 0)
    {
        fclose(pFile);
        pFile = NULL;

        fclose(pVoiceFile);
        pVoiceFile = NULL;
        return false;
    }

    yl::string strContent = szCode;
    //定位base64内容起始位置
    int iIndex = strContent.find("<content>");
    if (iIndex == yl::string::npos)
    {
        APP_WARN("XML Content Error!");
        fclose(pFile);
        pFile = NULL;

        fclose(pVoiceFile);
        pVoiceFile = NULL;
        return false;
    }
    fseek(pFile, iIndex + 9, SEEK_SET);

    while (!feof(pFile))
    {
        memset(szCode, 0, 4096);
        if (fread(szCode, 1, sizeof(szCode), pFile) <= 0)
        {
            continue;
        }

        strContent = szCode;
        iIndex = strContent.find("</content>");
        if (iIndex == yl::string::npos)
        {
            iIndex = sizeof(szCode);
        }

        unsigned char tmp[4096] = {0};

        int iDecodeLen = base64_decode(szCode, iIndex, tmp);
        if (iDecodeLen < 0)
        {
            continue;
        }

        fwrite((char *)tmp, iDecodeLen, 1, pVoiceFile);
    }

    //关闭文件
    fclose(pFile);
    fclose(pVoiceFile);
    pFile = NULL;
    pVoiceFile = NULL;

    m_nPlayIndex = index;

    return true;
#else
    xml_document doc;

    if (!doc.load_file(strFile.c_str()))
    {
        return false;
    }

    if (index >= m_vecMailData.size()
            || index < 0)
    {
        return false;
    }

    sVoiceMailData & refData = m_vecMailData[index];

    xml_node mesgContent = doc.child(kszNodeVoiceMessage).child(kszNodemessageMediaContent);

    if (mesgContent.empty())
    {
        return false;
    }

    xml_node typeNode = mesgContent.child(kszNodemediaType);

    yl::string strType;

    if (!typeNode.empty())
    {
        strType = typeNode.child_value();
    }

    xml_node contentNode = mesgContent.child(kszNodecontent);

    if (!contentNode.empty())
    {
        yl::string strBasr64Data = contentNode.child_value();
        yl::string strFile(kszVoiceMailFilePath);

        if (!strBasr64Data.empty() && SaveVoiceFile(strBasr64Data, strFile))
        {
            m_nPlayIndex = index;
            return true;
        }
    }

    return false;
#endif
}

#if !NEW_BASE64_DECODE
bool CVirtualVoiceMailPrcocesser::SaveVoiceFile(yl::string & strBase64Data,
        yl::string & strFileName)
{
    int iMaxBufferSize = VirtualMailMaxGetBufferSize();
    if (strBase64Data.length() > iMaxBufferSize)
    {
        APP_WARN("voicefile data is too large [%u] limit[%d]!", strBase64Data.length(), iMaxBufferSize);
        return false;
    }

    // transform base64 to binary
    char        b64[iMaxBufferSize];
    b64[0] = '\0';
    unsigned    b64_len = 0;
    unsigned    bin_len = 0;

    strncpy(b64, strBase64Data.c_str(), strBase64Data.length());
    b64[iMaxBufferSize - 1] = '\0';

    // the length of b64 characters
    b64_len = strBase64Data.length();
    b64_len = remove_newline(b64, b64_len);

    int nLength = base64_decoded_len(b64, b64_len);

    if (nLength > iMaxBufferSize)
    {
        APP_WARN("out buffer too small, need expaneded");
        return false;
    }
    unsigned char   bin[iMaxBufferSize];
    bin[0] = '\0';
    memset(bin, 0, iMaxBufferSize);
    bin_len = base64_decode(b64, b64_len, bin);

    file_write(strFileName.c_str(), (char *)bin, bin_len);

    return true;
}
#endif

bool CVirtualVoiceMailPrcocesser::GetVoiceMailListData(VoiceMailListData * pData)
{
    if (pData == NULL || pData->m_nIndexFrom < 0)
    {
        return false;
    }

    pData->m_nTotalCount = m_vecMailData.size();

    APP_INFO("GetVoiceMailListData m_nTotalCount[%d]  From[%d]  Count[%d]", pData->m_nTotalCount,
             pData->m_nIndexFrom, pData->m_nCountToGet);
    int nCount = pData->m_nCountToGet;
    if (pData->m_nIndexFrom == 0 && pData->m_nCountToGet == 0)
    {
        nCount = pData->m_nTotalCount;
    }

    for (int i = pData->m_nIndexFrom; i < m_vecMailData.size(); i++)
    {
        if (nCount <= 0)
        {
            break;
        }

        pData->m_listData.push_back(m_vecMailData[i]);
        nCount--;
    }

    return true;
}

bool CVirtualVoiceMailPrcocesser::GetDetailByIndex(sVoiceMailData & refData, int index)
{
    if (index >= m_vecMailData.size()
            || index < 0)
    {
        return false;
    }

    refData = m_vecMailData[index];

    return true;
}

void CVirtualVoiceMailPrcocesser::ClearListData()
{
    APP_INFO("CVirtualVoiceMailPrcocesser::ClearListData()");
    m_vecMailData.clear();
    m_nPlayIndex = -1;
    m_ePlayStatus = PLAYSTATUS_None;
    m_bIsLoadingList = false;
    remove(kszVoiceMailFilePath);
}

void CVirtualVoiceMailPrcocesser::FailProcess(XsiAction * pAction)
{
    APP_INFO("CVirtualVoiceMailPrcocesser::FailProcess");
    XsiBaseProcesser::FailProcess(pAction);

    if (pAction == NULL)
    {
        return;
    }

    switch (pAction->GetXsiType())
    {
    case XSI_VOICEMESG_LIST_GET:
        {
            m_bIsLoadingList = false;
        }
        break;
    case XSI_VOICEMESG_DETAIL_GET:
        {
            m_nPlayIndex = -1;
        }
        break;
    default:
        break;
    }
}

bool CVirtualVoiceMailPrcocesser::Play()
{
    APP_INFO("CVirtualVoiceMailPrcocesser::Play() m_ePlayStatus[%d]", m_ePlayStatus);
    if (m_ePlayStatus == PLAYSTATUS_Play
            || m_nPlayIndex == -1)
    {
        return false;
    }

    if (m_ePlayStatus == PLAYSTATUS_Pause
            || m_ePlayStatus == PLAYSTATUS_Stop)
    {
        Stop();
    }

    voice_SwitchChannel(CHANNEL_SELECT);

    int iRet = ipvp_media_play2(mkGetHandle(), kszVoiceMailFilePath, 0);

    ////播放时将标记为已读
    //if (!m_vecMailData[m_nPlayIndex].m_bRead)
    //{
    //  ReqMarkByIndex(m_nPlayIndex, true);
    //}

    m_ePlayStatus = PLAYSTATUS_Play;
    return (iRet == 0);
}

bool CVirtualVoiceMailPrcocesser::Pause()
{
    if (m_ePlayStatus != PLAYSTATUS_Play)
    {
        return false;
    }

    int iRet = ipvp_media_pause(mkGetHandle(), 0);
    APP_INFO("CVirtualVoiceMailPrcocesser::Pause()");
    m_ePlayStatus = PLAYSTATUS_Pause;
    return (iRet == 0);
}

bool CVirtualVoiceMailPrcocesser::Resume()
{
    if (m_ePlayStatus != PLAYSTATUS_Pause)
    {
        return false;
    }

    int iRet = ipvp_media_resume(mkGetHandle(), 0);
    APP_INFO("CVirtualVoiceMailPrcocesser::Resume()");
    m_ePlayStatus = PLAYSTATUS_Play;
    return (iRet == 0);
}

bool CVirtualVoiceMailPrcocesser::Seek(unsigned int locate)
{
    if (m_ePlayStatus == PLAYSTATUS_None
            || m_ePlayStatus == PLAYSTATUS_Stop)
    {
        return false;
    }

    int iRet = ipvp_media_seek(mkGetHandle(), locate, 0);
    APP_INFO("CVirtualVoiceMailPrcocesser::Seek(%d)", locate);
    return (iRet == 0);
}

bool CVirtualVoiceMailPrcocesser::Stop()
{
    APP_INFO("CVirtualVoiceMailPrcocesser::Stop()");
    if (m_ePlayStatus == PLAYSTATUS_None
            || m_ePlayStatus == PLAYSTATUS_Stop)
    {
        return false;
    }

    // 若正在播放铃声, GUI不能停止, VPM会自动停止, 但状态需要更新
    if (voice_IsPlayingRing() || (ipvp_media_stop(mkGetHandle(), 0) == 0))
    {
        // 声道切回
        voice_SwitchChannel(CHANNEL_AUTO);
    }


    m_ePlayStatus = PLAYSTATUS_Stop;
    return true;
}

void CVirtualVoiceMailPrcocesser::CorrectTimeZone(yl::string & strTime)
{
    if (strTime.empty())
    {
        return;
    }

    int iTime = TranTimeString2Int(strTime);
    /*
    const int klSecPerHour = 60 * 60;
    const int klSecPerMin = 60;
    int iTimeZoneHour;
    int iTimeZoneMin;
    GetLocalTimeZone(iTimeZoneHour, iTimeZoneMin);
    iTime = iTime + iTimeZoneHour * klSecPerHour + iTimeZoneMin * klSecPerMin;
    */
    strTime = commonAPI_FormatString("%d", iTime);
}
/*
void CVirtualVoiceMailPrcocesser::GetLocalTimeZone(int & iTimeZoneHour, int & iTimeZoneMin)
{
    iTimeZoneHour = 0;
    iTimeZoneMin = 0;
    yl::string strZoneTemp = configParser_GetConfigString(kszCurrentTimeZoneTime).c_str();
    if (!strZoneTemp.empty()
            && strZoneTemp.compare("0") != 0)
    {
        if (strZoneTemp.find("-") == yl::string::npos)
        {
            if (strZoneTemp.find(":") == yl::string::npos)
            {
                sscanf(strZoneTemp.c_str(), "+%d", &iTimeZoneHour);
            }
            else
            {
                sscanf(strZoneTemp.c_str(), "+%d:%d", &iTimeZoneHour, &iTimeZoneMin);
            }
        }
        else
        {
            if (strZoneTemp.find(":") == yl::string::npos)
            {
                sscanf(strZoneTemp.c_str(), "-%d", &iTimeZoneHour);
            }
            else
            {
                sscanf(strZoneTemp.c_str(), "-%d:%d", &iTimeZoneHour, &iTimeZoneMin);
            }
            iTimeZoneHour = -iTimeZoneHour;
            iTimeZoneMin = -iTimeZoneMin;
        }
    }
}
*/
bool CVirtualVoiceMailPrcocesser::IsSupportVideoMail()
{
    // 2016.11.17暂定不支持 Mov格式有版权问题，底层ipvp不支持，V82确定
//  PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
//  if (ePhoneType == PT_T49)
//  {
//      return true;
//  }

    return false;
}
