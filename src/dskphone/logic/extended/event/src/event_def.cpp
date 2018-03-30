#include "event_def.h"
#include "devicelib/networkserver.h"
#include "xmlparser/xmlparser.hpp"
#include "commonapi/stringhelperapi.h"
#include "commonunits/modcommonunits.h"
#include "contacts/directory/include/directoryenumtypes.h"
#include "service_dsk_define.h"

void EventStatsDataBase::FormatXmlData(xml_node & node)
{
    xml_node nodeEven = node.append_child("event");
    nodeEven.append_attribute("name", GetEvenName(m_eEventType));

    yl::string strValue = commonUnits_Time2String(m_nTime);
    nodeEven.append_attribute("time", strValue.c_str());

    if (!m_strExt.empty())
    {
        nodeEven.append_attribute("ext", m_strExt.c_str());
    }

    nodeEven.append_attribute("cnt", m_nCnt);

    switch (m_eEventType)
    {
    case EVENT_CONTACT_MODIFY:
        {
            switch (m_wParam)
            {
            case LOCAL_DIR:
                strValue = "local";
                break;
            case LOCAL_LOG:
                strValue = "log";
                break;
            case BSFT_LOG:
                strValue = "bsfb-log";
                break;
            case BSFT_DIR:
                strValue = "bsft";
                break;
            case REMOTE_DIR:
                strValue = "remote";
                break;
            case BLUETOOTH_DIR:
                strValue = "blue-dir";
                break;
            case PLCM_DIR:
                strValue = "plcm";
                break;
            case GB_DIR:
                strValue = "gb";
                break;
            case MTSW_LOG:
                strValue = "mtsw-log";
                break;
            case MTSW_DIR:
                strValue = "mtsw-dir";
                break;
            case LDAP_DIR:
                strValue = "ldap";
                break;
            case UC_DIR:
                strValue = "uc";
                break;
            case CLOUD_DIR:
                strValue = "cloud";
                break;
            default:
                strValue = "";
            }

            if (!strValue.empty())
            {
                nodeEven.append_attribute("type", m_wParam);
            }

            if (m_lParam > 0)
            {
                nodeEven.append_attribute("size", m_lParam);
            }
        }
        break;
    case EVENT_ESTABLISHED:
        {
            nodeEven.append_attribute("max", m_lParam);
        }
        break;
    default:
        nodeEven.append_attribute("w", m_wParam);
        nodeEven.append_attribute("l", m_lParam);
        break;
    }
    return;
}

void EventStatsDataBase::TransferStatsData(EventDataBase * pEventData)
{
    m_eEventType = pEventData->m_eEventType;
    m_nCnt += 1;
    m_nTime = netGetLocalTime();
    m_wParam = pEventData->m_wParam;
    m_lParam = pEventData->m_lParam;
    m_strExt = pEventData->m_strExt;

    // 其他数据
}

const char * EventStatsDataBase::GetEvenName(int nEvent)
{
    switch (nEvent)
    {
    case EVENT_SETUP_COMPLETED:
        return "appstart";

    case EVENT_LOGIN_ON:
        return "login";
    case EVENT_LOGIN_OFF:
        return "logout";
    case EVENT_REGISTER_FAILED:
        return "regist";

    case EVENT_OFFHOOK:
        return "offhook";
    case EVENT_ONHOOK:
        return "onhook";
    case EVENT_INCOMING_CALL:
        return "callin";
    case EVENT_REJECT_INCOMING_CALL:
        return "reject";
    case EVENT_FORWARD_INCOMING_CALL:
        return "fwd";
    case EVENT_CALL_REMOTE_CANCELED:
        return "realse";
    case EVENT_ANSWER_NEW_IN_CALL:
        return "answer2";
    case EVENT_REJECT_NEW_IN_CALL:
        return "reject2";
    case EVENT_CALLOUT:
        return "callout";
    case EVENT_CANCEL_CALLOUT:
        return "cancelcall";
    case EVENT_REMOTE_BUSY:
        return "busy";
    case EVENT_ESTABLISHED:
        return "talking";
    case EVENT_TERMINATED:
        return "terminated";


    case EVENT_OPEN_DND:
        return "dnd-1";
    case EVENT_CLOSE_DND:
        return "dnd-0";
    case EVENT_OPEN_ALWAYS_FORWARD:
        return "forward-1";
    case EVENT_CLOSE_ALWAYS_FORWARD:
        return "forward-0";
    case EVENT_OPEN_BUSY_FORWARD:
        return "busyfwd-1";
    case EVENT_CLOSE_BUSY_FORWARD:
        return "busyfwd-0";
    case EVENT_OPEN_NO_ANSWER_FORWARD:
        return "answerfwd-1";
    case EVENT_CLOSE_NO_ANSWER_FORWARD:
        return "answerfwd-0";

    case EVENT_TRANSFER_CALL:
        return "transf";
    case EVENT_BLIND_TRANSFER:
        return "b-transf";
    case EVENT_ATTENDED_TRANSFER:
        return "a-transf";
    case EVENT_TRANSFER_FINISHED:
        return "trans-end";
    case EVENT_TRANSFER_FAILED:
        return "trans-fail";
    case EVENT_HOLD:
        return "hold";
    case EVENT_UNHOLD:
        return "unhold";
    case EVENT_MUTE:
        return "mute";
    case EVENT_UNMUTE:
        return "unmute";

    case EVENT_MISSED_CALL:
        return "miss-call";
    case EVENT_BUSY_TO_IDLE:
        return "toidle";
    case EVENT_IDLE_TO_BUSY:
        return "tobusy";

    case EVENT_IP_CHANGED:
        return "ip-change";
    case EVENT_REMOTE_FORWARD:
        return "befwd";
    case EVENT_REMOTE_HOLD:
        return "held";
    case EVENT_REMOTE_UNHOLD:
        return "unheld";
    case EVENT_AUTOP_FINISH:
        return "autopend";
    case EVENT_HANDFREE:
        return "handfree";
    case EVENT_HEADSET:
        return "headset";
    case EVENT_OPEN_CALLWAIT:
        return "callwait-1";
    case EVENT_CLOSE_CALLWAIT:
        return "callwait-0";


    case EVENT_ENTER_DIAL:
        return "enterdial";
    case EVENT_END_DIAL:
        return "exitdial";
    case EVENT_RINGBACK:
        return "ringback";
    case EVENT_CONFERENCE:
        return "conf";
    case EVENT_DTMF:
        return "dtmf";
    case EVENT_CALL_FAIL:
        return "callfail";
    case EVENT_BLIND_TRANSFER_FAILED:
        return "b-transfail";
    case EVENT_ATTENDED_TRANSFER_FAILED:
        return "a-transfail";
    case EVENT_CALL_REMOTE_NO_ANSWER:
        return "remotenoanswer";

    // autop
    case EVENT_AUTOP_START:
        return "autopstart";
    case EVENT_AUTOP_STATUS:
        return "autopstus";
    case EVENT_AUTOP_END:
        return "autopend";
    case EVENT_AUTOP_ZERO_TOUCH_START:
        return "zerostart";
    case EVENT_AUTOP_ZERO_TOUCH_STATUS:
        return "zerostus";
    case EVENT_AUTOP_ZERO_TOUCH_END:
        return "zeroend";
    case EVENT_AUTOP_RPS_START:
        return "rps-start";
    case EVENT_AUTOP_RPS_STATUS:
        return "rps-stus";
    case EVENT_AUTOP_RPS_END:
        return "rps-end";

    case EVENT_ENTER_PAGE:
        return "enter";
    case EVENT_EXIT_PAGE:
        return "exit";

    case EVENT_CONTACT_SEARCH:
        return "search";
    case EVENT_CONTACT_MODIFY:
        return "dir-modify";

    case EVENT_HOTPLUG_ADD:
        return "usb-add";
    case EVENT_HOTPLUG_REMOVE:
        return "usb-remove";
    case EVENT_SUPER_SEARCH:
        return "super-search";
    default:
        return "";
    }

    return "";
}

bool EventStatsDataBase::IsSupportSecondKey()
{
    switch (m_eEventType)
    {
    case EVENT_ENTER_PAGE:
        //case EVENT_EXIT_PAGE:
        {
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

bool EventStatsDataBase::CompareSecondKey(EventStatsDataBase * pData)
{
//  printf("----%s --- %s", m_strExt.c_str(), pData->m_strExt.c_str());
    switch (m_eEventType)
    {
    case EVENT_ENTER_PAGE:
    case EVENT_EXIT_PAGE:
        {
            return m_strExt == pData->m_strExt;
        }
        break;
    default:
        break;
    }
    return false;
}

EventStatsDataBase::EventStatsDataBase()
{
    m_eEventType = EVENT_TYPE_NONE;
    m_nTime = 0;
    m_nCnt = 0;
}

EventStatsDataBase::EventStatsDataBase(EventType eType)
{
    m_eEventType = eType;
    m_nTime = 0;
    m_nCnt = 0;
}

bool EventDataBase::TransferReportData(DiagAlertInfo & tEventInfo)
{
    // 转换成上报数据：目前中性未开发

    bool bHandled = false;

	yl::string strData;

    struct DiagInfo
    {
        EventType  eEventType;
        const char * lpszAlertName;
        const char * lpszDescription;
        const char * lpszSeverity;
    };

    static DiagInfo sDiagInfoMap[] =
    {
        { EVENT_SETUP_COMPLETED, "app start", "gui app start.", "Minor"},
        { EVENT_TRANSFER_FAILED, "Transfer failure", "This Alarm is activated when failing to transfer the call.", "Minor" },
		{ EVENT_ESTABLISHED, "Talk establish", "This info is activated when talk established.", "Minor" },
		{ EVENT_MEDIA_VOICE_STOP, "Talk terminate", "This info is activated when talk terminated.", "Minor" },
		{ EVENT_CALL_FAIL, "Call fail", "This info is activated when call failed.", "Minor" },


		{ EVENT_OPEN_DND, "DND opened", "This info is activated when dnd opened.", "Minor" },
		{ EVENT_CLOSE_DND, "DND close", "This info is activated when dnd closed", "Minor" },

		{ EVENT_OPEN_ALWAYS_FORWARD, "Always Forward", "This info is activated when always forward opened.", "Minor" },
		{ EVENT_CLOSE_ALWAYS_FORWARD, "Always Forward", "This info is activated when always forward closed", "Minor" },
		{ EVENT_OPEN_BUSY_FORWARD, "Busy Forward", "This info is activated when busy forward opened.", "Minor" },
		{ EVENT_CLOSE_BUSY_FORWARD, "Busy Forward", "This info is activated when busy forward closed.", "Minor" },
		{ EVENT_OPEN_NO_ANSWER_FORWARD, "No Answer Forward", "This info is activated when no answer forward opened.", "Minor" },
		{ EVENT_CLOSE_NO_ANSWER_FORWARD, "No Answer Forward", "This info is activated when no answer forward closed.", "Minor" },
		
		{ EVENT_HOTPLUG_USB_RECORDED, "USB record", "This info is activated when record ended.", "Minor" },


		{ EVENT_TYPE_MAX, "", "", "" }
    };

    int i = 0;
    while (sDiagInfoMap[i].eEventType != EVENT_TYPE_MAX)
    {
        if (sDiagInfoMap[i].eEventType == m_eEventType)
        {
            strncpy(tEventInfo.name, sDiagInfoMap[i].lpszAlertName,
                    strlen(sDiagInfoMap[i].lpszAlertName) > sizeof(tEventInfo.name)
                    ? sizeof(tEventInfo.name) : strlen(sDiagInfoMap[i].lpszAlertName));

            strncpy(tEventInfo.description, sDiagInfoMap[i].lpszDescription,
                    strlen(sDiagInfoMap[i].lpszDescription) > sizeof(tEventInfo.description)
                    ? sizeof(tEventInfo.description) : strlen(sDiagInfoMap[i].lpszDescription));

            strncpy(tEventInfo.severity, sDiagInfoMap[i].lpszSeverity,
                    strlen(sDiagInfoMap[i].lpszSeverity) > sizeof(tEventInfo.severity)
                    ? sizeof(tEventInfo.severity) : strlen(sDiagInfoMap[i].lpszSeverity));

            strcpy(tEventInfo.module, "UI");

            bHandled = true;
            break;
        }

        i++;
    }

	if (m_eEventType == EVENT_HOTPLUG_USB_RECORDED)
	{
		strData.append("{");

		strData = strData + commonAPI_FormatString("\"path\":\"%s\"", m_strExt.c_str());

		strData.append("}");

		strncpy(tEventInfo.data, strData.c_str(), chMIN(1024, strData.size()));
	}

    return bHandled;
}

int EventDataBase::MapReportType()
{
    switch (m_eEventType)
    {
//  case EVENT_SETUP_COMPLETED:
	case EVENT_TRANSFER_FAILED:		return DSK_DIAG_EVENT_TYPE_TRAN_FAILED;
	case EVENT_ESTABLISHED:			return DSK_DIAG_EVENT_TYPE_TALK_ESTABLISHED;
	case EVENT_MEDIA_VOICE_STOP:	return DSK_DIAG_EVENT_TYPE_TALK_TERMINATED;
	case EVENT_CALL_FAIL:			return DSK_DIAG_EVENT_TYPE_TALK;
	case EVENT_OPEN_DND:			return DSK_DIAG_EVENT_TYPE_MENU;
	case EVENT_OPEN_ALWAYS_FORWARD:		return DSK_DIAG_EVENT_TYPE_MENU;
	case EVENT_CLOSE_ALWAYS_FORWARD:		return DSK_DIAG_EVENT_TYPE_MENU;
	case EVENT_OPEN_BUSY_FORWARD:		return DSK_DIAG_EVENT_TYPE_MENU;
	case EVENT_CLOSE_BUSY_FORWARD:		return DSK_DIAG_EVENT_TYPE_MENU;
	case EVENT_OPEN_NO_ANSWER_FORWARD:		return DSK_DIAG_EVENT_TYPE_MENU;
	case EVENT_CLOSE_NO_ANSWER_FORWARD:		return DSK_DIAG_EVENT_TYPE_MENU;
	case EVENT_HOTPLUG_USB_RECORDED:		return DSK_DIAG_EVENT_TYPE_USB_RECORDED;
    default:
		return DSK_DIAG_EVENT_TYPE_DIAG_NONE;
    }

    ACTION_INFO("evevnt need't report[%d]", m_eEventType);
	return DSK_DIAG_EVENT_TYPE_DIAG_NONE;
}

bool EventDataBase::IsSaveStatsInfo()
{
    switch (m_eEventType)
    {
    case EVENT_AUTOP_FINISH:
        {
            ACTION_INFO("evevnt need't save[%d]", m_eEventType);
            return false;
        }
        break;
    default:
        break;
    }

    if (m_eEventType > EVENT_TYPE_NONE
            && m_eEventType < EVENT_TYPE_MAX)
    {
        return true;
    }

    return false;
}

bool TalkEventData::TransferReportData(DiagAlertInfo & tEventInfo)
{
	EventDataBase::TransferReportData(tEventInfo);

	struct timespec runningtime = { 0, 0 };
	clock_gettime(CLOCK_MONOTONIC, &runningtime);

	yl::string strData = commonAPI_FormatString(
		"{\"lineId\":%d, \"callId\":%d, \"calltype\":%d, \"duration\":%d, \"time\":%d, \"runningtimesec\":%d, \"runningtimensec\":%d}",
		m_wParam, m_vecCallID.size() > 0 ? m_vecCallID[0] : 0, m_nCallType, m_nDuration,
		m_tTime, runningtime.tv_sec, runningtime.tv_nsec);

	strncpy(tEventInfo.data, strData.c_str(), chMIN(1024, strData.size()));

	return true;
}
