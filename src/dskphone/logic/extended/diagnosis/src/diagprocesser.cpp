#ifdef IF_FEATURE_DIAGNOSE
#include "../include/diagdef.h"
#include "diagprocesser.h"
#include "notify/include/modnotify.h"
#include "commondefine/service_dsk_interface.h"
#include <devicelib/networkserver.h>
#include "dsklog/log.h"
#include <configparser/modconfigparser.h>
#include <service_config.h>
#include "path_define.h"
#include "commonunits/modcommonunits.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "talk/talklogic/include/modtalklogic.h"

#define FeedBackAataPath    CONFIG_DATA_PATH "feedbackappdata.xml"
#define FeedBackAataTmpPath CONFIG_DATA_PATH "feedbackappdata.xml.tmp"


#ifdef IF_FEATURE_USER_DATA
LRESULT DiagTimeOutMegProcess(msgObject & refObj)
{
    g_objDiagManager.OnTime(refObj.wParam);
    return TRUE;
}
#endif // #ifdef IF_FEATURE_USER_DATA

CDiagProcesser::CDiagProcesser()
    : CEventBase(EMT_DIAG)
#ifdef IF_FEATURE_USER_DATA
    , m_mapData(true)
#endif //#ifdef IF_FEATURE_USER_DATA
{
#ifdef IF_FEATURE_USER_DATA
    m_nMinMem = 0xffffffff;
#endif //#ifdef IF_FEATURE_USER_DATA
}

CDiagProcesser::~CDiagProcesser()
{

}

// 由于要将gui当做service程序，因此对外发送消息扩展为多个
UINT CDiagProcesser::MapEvent2Msg( int eEvent )
{
    switch (eEvent)
    {
    // 只有dmservice需求的事件，才单独定义消息；内部上报的默认使用default消息即可
    case DSK_DIAG_EVENT_TYPE_TRAN_FAILED:   return DSK_MSG_TALK_INFO_TRAN_FAILED;
    case DSK_DIAG_EVENT_TYPE_TALK_ESTABLISHED: return DSK_MSG_TALK_INFO_ESTABLISHED;
    case DSK_DIAG_EVENT_TYPE_TALK_TERMINATED: return DSK_MSG_TALK_INFO_TERMINATED;
    case DSK_DIAG_EVENT_TYPE_USB_RECORDED:  return DSK_MSG_MENU_INFO_USB_RECORDED;
    case DSK_DIAG_EVENT_TYPE_APP:           return DSK_MSG_APP_INFO_DEFAULT;
    case DSK_DIAG_EVENT_TYPE_TALK:          return DSK_MSG_TALK_INFO_DEFAULT;
    case DSK_DIAG_EVENT_TYPE_CONTACT:       return DSK_MSG_CONTACT_INFO_DEFAULT;
    case DSK_DIAG_EVENT_TYPE_MENU:          return DSK_MSG_MENU_INFO_DEFAULT;
    case DSK_DIAG_EVENT_TYPE_DIAG_COMMON:   return DSK_MSG_MENU_INFO_DEFAULT;
    default:
        return DSK_MSG_MENU_INFO_DEFAULT;
    }

    return DSK_MSG_MENU_INFO_DEFAULT;
}

CDiagProcesser & CDiagProcesser::GetInstace()
{
    static CDiagProcesser m_sInstace;

    return m_sInstace;
}

void CDiagProcesser::Init()
{
    // 注册类型
    RegisterModule();

#ifdef IF_FEATURE_USER_DATA

    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, DiagTimeOutMegProcess);

    if (pathFileExist(FeedBackAataTmpPath))
    {
        ACTION_WARN("back feedback app data");
        // 保存上次统计的信息
        rename(FeedBackAataTmpPath, FeedBackAataPath);
    }
#endif // IF_FEATURE_USER_DATA

    return;
}

// 发送诊断事件
bool CDiagProcesser::SendEvent(EventDataBase * pData)
{
    if (NULL == pData)
    {
        return false;
    }

#if 1
    ACTION_INFO("Send Diag Event[%d]", pData->m_eEventType);
#endif

    int iReportType = pData->MapReportType();
    if (iReportType != DSK_DIAG_EVENT_TYPE_DIAG_NONE)
    {
        bool bHandled = true;
        DiagAlertInfo tEventInfo;
        memset(&tEventInfo, 0, sizeof(DiagAlertInfo));
        tEventInfo.time = time(NULL);
        tEventInfo.data_type = DMS_DATA_TYPE_JSON;

        bHandled = pData->TransferReportData(tEventInfo);
        if (bHandled)
        {
#if 0
            ACTION_INFO("Send diag Event[%d] [%s] [%x]", pData->m_eEventType, tEventInfo.data, MapEvent2Msg(iReportType));
#endif
            // 发送notify给诊断程序:消息扩展为多个
            notify_Notify2ReqAppEx(FALSE, MapEvent2Msg(iReportType), iReportType, 1, sizeof(DiagAlertInfo),
                                   &tEventInfo);
        }
    }

#ifdef IF_FEATURE_USER_DATA
    SaveEvent(pData);
#endif //#ifdef IF_FEATURE_USER_DATA

    return true;
}

#ifdef IF_FEATURE_USER_DATA
bool CDiagProcesser::OnTime(UINT uTimerID)
{
    if (uTimerID == (UINT)&m_mapData)
    {
        if (talklogic_SessionExist())
        {
            return true;
        }

        ACTION_INFO("save stats data timeover");
        timerKillThreadTimer(uTimerID);

        SaveStaticData();
        return true;
    }

    return false;
}

bool CDiagProcesser::SaveEvent(EventDataBase * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    if (!pData->IsSaveStatsInfo())
    {
        return false;
    }


    IterMapDiagInfo iter = m_mapData.find(pData->m_eEventType);
    if (iter == m_mapData.end())
    {
        EventStatsDataBase * pInfo = new EventStatsDataBase();
        if (NULL == pInfo)
        {
            return false;
        }

        pInfo->TransferStatsData(pData);

        m_mapData.put(pInfo->m_eEventType, pInfo);
    }
    else
    {
        EventStatsDataBase * pStats = iter->second;

        if (!pStats->IsSupportSecondKey())
        {
            pStats->TransferStatsData(pData);
        }
        else
        {
            int nCount = m_mapData.count(pStats->m_eEventType);

            if (nCount > 100)
            {
                // 单个事件记录不超过100个，防止外部随意调用，造成问题
                ACTION_WARN("more event");
                return false;
            }

            EventStatsDataBase * pInfo = new EventStatsDataBase();
            if (NULL == pInfo)
            {
                return false;
            }

            pInfo->TransferStatsData(pData);

            bool bFind = false;

            for (int i = 0; i < nCount; ++i)
            {
                pStats = iter->second;

                if (pStats->CompareSecondKey(pInfo))
                {
                    bFind = true;
                    pStats->TransferStatsData(pData);
                    break;
                }

                ++iter;
            }

            if (!bFind)
            {
                m_mapData.insert(pInfo->m_eEventType, pInfo);
            }
            else
            {
                delete pInfo;
            }
        }
    }

    timerSetThreadTimer((UINT)&m_mapData, 30 * 1000);

    ACTION_INFO("save event[%d] w[%d] l[%d] [%s] [%d]", pData->m_eEventType, pData->m_wParam,
                pData->m_lParam, pData->m_strExt.c_str(),
                m_mapData.size());
    return true;
}

bool CDiagProcesser::SaveStaticData()
{
    ACTION_INFO("save static data");
    xml_document docTmp;

    xml_node nodeDecl = docTmp.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("app", "1.0");

    xml_node nodeRoot = docTmp.append_child("sys");
    AppendSysData(nodeRoot);

    nodeRoot = docTmp.append_child("data");

    IterMapDiagInfo iter = m_mapData.begin();
    while (iter != m_mapData.end())
    {
        EventStatsDataBase * pInfo = iter->second;

        pInfo->FormatXmlData(nodeRoot);

        ++iter;
    }

    //保存到文件
    docTmp.save_file(FeedBackAataTmpPath);

    ACTION_INFO("save static data end");
    return true;
}

void CDiagProcesser::AppendSysData(xml_node & node)
{
    xml_node nodeData = node.append_child("sys");

    char szFirmware[32] = { 0 };
    sys_get_sw_version(szFirmware, 32);
    nodeData.append_attribute("version", szFirmware);

    //1.0 系统时间 /proc/uptime
    yl::string strValue = commonUnits_GetUpTime();

    nodeData.append_attribute("uptime", strValue.c_str());

    int nTotal, nFree = 0, nBuff = 0, nCache = 0;
    commonUnits_GetMemInfo(nTotal, nFree, nBuff, nCache);

    strValue = commonAPI_FormatString("%d", nFree);
    nodeData.append_attribute("free", strValue.c_str());

    strValue = commonAPI_FormatString("%d", nBuff);
    nodeData.append_attribute("buff", strValue.c_str());

    strValue = commonAPI_FormatString("%d", nCache);
    nodeData.append_attribute("cache", strValue.c_str());

    if (m_nMinMem > nFree + nBuff + nCache)
    {
        m_nMinMem = nFree + nBuff + nCache;
    }

    // 内存极小值
    strValue = commonAPI_FormatString("%d", m_nMinMem);
    nodeData.append_attribute("minmem", strValue.c_str());

    return;
}

EventStatsDataBase * CDiagProcesser::CreateStatsData(EventType eType)
{
    EventStatsDataBase * pData = NULL;
    switch (eType)
    {
    case EVENT_ENTER_PAGE:
    case EVENT_EXIT_PAGE:
//  {
//
//  }
//  break;
    default:
        pData = new EventStatsDataBase(eType);
    }

    return pData;
}
#endif // IF_FEATURE_USER_DATA

#endif //#ifdef IF_FEATURE_DIAGNOSE
