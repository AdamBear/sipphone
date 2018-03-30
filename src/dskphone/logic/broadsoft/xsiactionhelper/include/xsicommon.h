#ifndef __XSI_COMMON_H__
#define __XSI_COMMON_H__

#include <ylstring.h>
#include "innermsgdefine.h"

//按方法归类
enum eXsiActionType
{
    //get 类
    XSI_GET_BEGIN,
    //anywhere
    XSI_ANYWHERE_GET_LOCATION_LIST,
    XSI_ANYWHERE_GET_LOCATION_DETAIL,
    //anonymouscall
    XSI_ANONYOUSCALL_GET_STATE,
    XSI_ANONYMOUNSCALL_REJECT_GET_STATE,
    //CallForwardNotReachable
    XSI_CALLFORWARD_NOTREACHABLE_GET,
    //GroupNightForward
    XSI_GROUPNIGHT_FORWARD_GET,
    //RemoteOffice
    XSI_REMOTEOFFICE_GET,
    //Mobility
    XSI_MOBILITY_GET,
    //SimultaneousRingPersonal
    XSI_SIMULTANEOUSRING_GET,

    XSI_CALLID_GET,

    XSI_CALLID_DETAILS_GET,

    //MusicOnHold
    XSI_MUSICONHOLD_GET,

    //call waiting
    XSI_CALL_WAITING_GET,

    XSI_CALL_RECORD_MODE_GET,

    XSI_EXECUTIVE_ASSISTANTLIST_GET,

    XSI_EXECUTIVE_FILTER_GET,

    XSI_EXECUTIVE_SCREENING_GET,

    XSI_EXECUTIVE_ALTER_GET,

    XSI_ASSISTANT_EXECUTIVELIST_GET,

    XSI_VOICEMESG_LIST_GET,

    XSI_VOICEMESG_DETAIL_GET,

    XSI_FORWARD_SELECTIVE_GET,

    XSI_SILENT_ALERTING_GET,

    XSI_GET_END,

    //delete 类
    XSI_DELETE_BEGIN,
    //anywhere
    XSI_ANYWHERE_DELETE_LOCATION,

    XSI_VOICEMESG_DELETE,

    XSI_DELETE_END,

    //put 类
    XSI_PUT_BEGIN,
    //anywhere
    XSI_ANYWHERE_MODIFY_LOCATION,
    XSI_ANYWHERE_MODIFY_LOCATIONS,
    //anonymouscall
    XSI_ANONYOUSCALL_PUT_STATE,
    XSI_ANONYMOUNSCALL_REJECT_PUT_STATE,
    //CallForwardNotReachable
    XSI_CALLFORWARD_NOTREACHABLE_PUT,
    //RemoteOffice
    XSI_REMOTEOFFICE_PUT,
    //Mobility
    XSI_MOBILITY_PUT,
    //SimultaneousRingPersonal
    XSI_SIMULTANEOUSRING_PUT,

    XSI_CALLPARK,

    XSI_GROUPPARK,

    //MusicOnHold
    XSI_MUSICONHOLD_PUT,

    //call waiting
    XSI_CALL_WAITING_PUT,

    XSI_CALL_RECORD_START,
    XSI_CALL_RECORD_STOP,
    XSI_CALL_RECORD_PAUSE,
    XSI_CALL_RECORD_RESUME,
    XSI_CALL_RECORD_MODE_PUT,

    XSI_EXECUTIVE_ASSISTANT_CALLPUSH, // call push

    XSI_EXECUTIVE_ASSISTANTLIST_PUT,

    XSI_EXECUTIVE_FILTER_PUT,

    XSI_EXECUTIVE_SCREENING_PUT,

    XSI_EXECUTIVE_ALTER_PUT,

    XSI_ASSISTANT_EXECUTIVELIST_PUT,

    XSI_VOICEMESG_MARKALL_READ_PUT,

    XSI_VOICEMESG_MARKALL_UNREAD_PUT,

    XSI_VOICEMESG_MARK_READ_PUT,

    XSI_VOICEMESG_MARK_UNREAD_PUT,

    XSI_FORWARD_SELECTIVE_PUT,

    XSI_SILENT_ALERTING_PUT,

    XSI_PUT_END,

    //post 类
    XSI_POST_BEGIN,
    //anywhere
    XSI_ANYWHERE_ADD_LOCATION,

    //callpark
    XSI_PARK_RETRIVE,

    XSI_EXECUTIVE_ASSISTANT_CALLINITIATE, // Initiate Call

    XSI_POST_END,
};
// XSI消息定义.
enum XsiMessage
{
    XSI_MESSAGE_BEGIN = INNER_MSG_XSI_MESSAGE_BEGIN,

    //wparm:xsi type lparm:line id ext:error info
    XSI_MESSAGE_REQ_FAIL,

    //wparm:xsi type lparm:lineId
    XSI_MESSAGE_REQ_FINISH,

    //wparm:exe action type lparm:lineId
    XSI_MESSAGE_FINISH_ALL,

    XSI_MESSAGE_END = INNER_MSG_XSI_MESSAGE_END
};

//执行xsi cmd时的错误信息
struct SXsiExtenInfo
{
    int             m_nError; //error 编号
    yl::string      m_strErrInfo; //error具体说明

    int m_nEntenParm;

    void Reset()
    {
        m_nError = -1;
        m_strErrInfo.clear();
        m_nEntenParm = -1;
    }

    SXsiExtenInfo()
    {
        Reset();
    }
};

#endif

