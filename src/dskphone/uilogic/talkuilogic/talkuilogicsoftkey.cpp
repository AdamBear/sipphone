#include "talkuilogicsoftkey.h"
//录制相关模块
#include "record/include/modrecord.h"

#include "baselibs/include/ylstl/ylstringalgorithm.h"

#include "talkuilogicenum.h"
#include "talkuilogicactiondefine.h"

SoftKey_TYPE GetTypeOfKey(const yl::string & strType)
{
    if (strType == "Send")
    {
        return ST_SEND;
    }
    else if (strType == "Send_SMS")
    {
        return ST_SEND_SMS;
    }
    else if (strType == "IME")
    {
        return ST_IMECHANGE;
    }
    else if (strType == "Delete")
    {
        return ST_DELETE;
    }
    else if (strType == "Line")
    {
        return ST_LINE;
    }
    else if (strType == "History")
    {
        return ST_HISTORY;
    }
    else if (strType == "Directory")
    {
        return ST_POOL;
    }
    else if (strType == "Cancel")
    {
        return ST_CANCEL;
    }
    else if (strType == "End Call")
    {
        return ST_ENDCALL;
    }
    else if (strType == "Answer")
    {
        return ST_ANSWER;
    }
    else if (strType == "Forward")
    {
        return ST_CALLINFORWARD;
    }
    else if (strType == "Silence")
    {
        return ST_SILENCE;
    }
    else if (strType == "Reject")
    {
        return ST_REJECT;
    }
    else if (strType == "Decline")
    {
        return ST_DECLINE;
    }
    else if (strType == "Transfer")
    {
        return ST_TRANSFER;
    }
    else if (strType == "B Transfer")
    {
        return ST_TRANSFER;
    }
    else if (strType == "NewCall")
    {
        return ST_NEWCALL;
    }
    else if (strType == "Ok")
    {
        return ST_OK;
    }
    else if (strType == "Switch")
    {
        return ST_SWITCH;
    }
    else if (strType == "CC")
    {
        return ST_CCOMPLETION;
    }
    else if (strType == "Hold")
    {
        return ST_HOLD;
    }
    else if (strType == "SWAP")
    {
        return ST_SWAP;
    }
    else if (strType == "Conference")
    {
        return ST_CONFERENCE;
    }
    else if (strType == "Resume")
    {
        return ST_RESUME;
    }
    else if (strType == "Split")
    {
        return ST_SPLIT;
    }
    else if (strType == "Mute")
    {
        return ST_MUTE;
    }
    else if (strType == "Add")
    {
        return ST_ADD;
    }
    else if (strType == "Remove")
    {
        return ST_REMOVE;
    }
    else if (strType == "PriChat")
    {
        return ST_PRI_CHAT;
    }
    else if (strType == "ExitPri")
    {
        return ST_EXIT_PRICHAT;
    }
    //  else if (strType == "Pool")         return ST_POOL;
    else if (strType == "Directories")
    {
        return ST_POOL;
    }
    else if (strType == "Favorite")
    {
        return ST_POOL;
    }
//  else if (strType == "Favorite")     return ST_FAVORITE;
    else if (strType == "PriHold")
    {
        return ST_PRIVATEHOLD;
    }
    else if (strType == "Back")
    {
        return ST_BACK;
    }
    else if (strType == "Enter")
    {
        return ST_ENTER;
    }

    else if (strType == "Select")
    {
        return ST_SELECT;
    }

    else if (strType == "Exit")
    {
        return ST_EXIT;
    }
    else if (strType == "Connect")
    {
        return ST_CONNECT;
    }
    else if (strType == "View")
    {
        return ST_VIEW;
    }
    else if (strType == "Confirm")
    {
        return ST_CONFIRM;
    }
    else if (strType == "Save")
    {
        return ST_SAVE;
    }
    else if (strType == "DPickup")
    {
        return ST_DPICKUP;
    }
    else if (strType == "GPickup")
    {
        return ST_GPICKUP;
    }
#if IF_BUG_27796
    else if (strType == "ZPickup")
    {
        return ST_Z_PICKUP;
    }
#endif
    else if (strType == "Dial")
    {
        return ST_DIAL;
    }
    else if (strType == "Barge In")
    {
        return ST_BARGEIN;
    }
    else if (strType == "CallPull")
    {
        return ST_CALLPULL;
    }
    else if (strType == "Block")
    {
        return ST_BLOCK;
    }
    else if (strType == "Pickup")
    {
        return ST_PICKUP;
    }
    else if (strType == "Park")
    {
        return ST_PARK;
    }
    else if (strType == "GPark")
    {
        return ST_GROUP_PARK;
    }
//  else if (strType == "Retrieve")     return ST_RETRIEVE;
    else if (strType == "Retrieve")
    {
        return ST_RETRIEVE_PARK;
    }
    else if (strType == "Delete All")
    {
        return ST_DELETEALL;
    }
    else if (strType == "Login")
    {
        return ST_ACD_LOGIN;
    }
    else if (strType == "Logout")
    {
        return ST_ACD_LOGOUT;
    }
    else if (strType == "Avail")
    {
        return ST_ACD_AVAILABLE;
    }
    else if (strType == "Unavail")
    {
        return ST_ACD_UNAVAILABLE;
    }
    else if (strType == "GuestIn")
    {
        return ST_HOTELING_GUESTIN;
    }
    else if (strType == "GuestOut")
    {
        return ST_HOTELING_GUESTOUT;
    }
    else if (strType == "Disp Code")
    {
        return ST_ACD_DISPCODE;
    }
    else if (strType == "Trace")
    {
        return ST_ACD_TRACE;
    }
    else if (strType == "Call Info")
    {
        return ST_ACD_CALLINFO;
    }
    else if (strType == "Emergcy")
    {
        return ST_ACD_EMERGENCY;
    }
    else if (strType == "Favorite")
    {
        return ST_FAVORITE;
    }
    else if (strType == "Manager")
    {
        return ST_CONF_MANAGE;
    }
    else if (strType == "Paging_softkey")
    {
        return ST_PAGING;
    }
    else if (strType == "RTP Status")
    {
        return ST_RTP_STATUS;
    }
    else if (strType == "Security")
    {
        return ST_SECURITY;
    }
#if IF_BUG_23548
    else if (strType == "Call Back")
    {
        return ST_CALLCOMPLETIONCALLBACK;
    }
#endif

    //t49 新增
    //录音相关
#if IF_TALKUI_USB_ENABLE
    // http://10.2.1.199/Bug.php?BugID=78332
    //softkey layout 添加 录音和截图的softkey
    //http://10.2.1.199/Bug.php?BugID=76011
    //根据76011，record的softkey改为不受softkey layout控制
//  else if (strType == "Record")       return GetRecordSoftkey();      //开始、停止 录制
    else if (strType == "Screenshot")
    {
        return ST_SCREENSHOT;    //截图
    }

#endif

#ifndef _WIN32
#warning 会议优化
#endif
    /*
        else if (strType == "Join")     return ST_JOIN;
        else if (strType == "Conf Remove")      return ST_CONF_REMOVE;
        else if (strType == "SplitAll")     return ST_SPLIT_ALL;
        else if (strType == "Conf Far Mute")        return ST_CONF_FAR_MUTE;

        else if (strType == "Direct")       return ST_DIRECT;
        else if (strType == "Consult")      return ST_CONSULT;
    */
    else if (strType == "FarHold")
    {
        return ST_FAR_HOLD;
    }

    else
    {
        return ST_EMPTY;
    }
}

//获取录制的softkey
SoftKey_TYPE GetRecordSoftkey()
{
#if IF_TALKUI_USB_ENABLE
    //当前如果是处于 视频或音频录制 那么添加 停止录制的softkey
    if (modRecord_IsRecording(RECORD_AUDIO) || modRecord_IsRecording(RECORD_VIDEO))
    {
        return ST_STOP_RECORD;
    }

    return ST_START_RECORD;
#endif

    return ST_EMPTY;
}

yl::string ToMakeLower(yl::string str)
{
    yl::to_lower(str);
    return str;
}


//通过softkeytype 获取 softkey 的action
yl::string GetSoftkeyTitleBySoftkeyType(SoftKey_TYPE softkeyType)
{
    switch (softkeyType)
    {
    case ST_EMPTY:
        {
            return "";
        }
        break;
    case ST_SEND:
        {
            return TRID_SEND;
        }
        break;
    case ST_IMECHANGE:
        {
            return TRID_INPUT_METHOD_SHORT;
        }
        break;
    case ST_DELETE:
        {
            return TRID_DELETE;
        }
        break;
    case ST_LINE:
        {
            return TRID_LINE;
        }
        break;
    case ST_HISTORY:
        {
            return TRID_HISTORY;
        }
        break;
    case ST_DIRECTORY:
        {
            return TRID_DIRECTORY;
        }
        break;
    case ST_CANCEL:
        {
            return TRID_CANCEL;
        }
        break;
    case ST_ANSWER:
        {
            return TRID_ANSWER;
        }
        break;

#ifndef _WIN32
#warning 会议优化
#endif
    /*
        case ST_JOIN:
            {
                return TRID_JOIN;
            }
            break;
        case ST_DIRECT:
            {
                return TRID_DIRECT;
            }
            break;
        case ST_CONSULT:
            {
                return TRID_CONSULT;
            }
            break;
    */
    case ST_FAR_HOLD:
        {
            return TRID_FARHOLD;
        }
        break;
    case ST_FAR_RESUME:
        {
            return TRID_FARRESUME;
        }
        break;

    case ST_CALLINFORWARD:
        {
            return TRID_FORWARD;
        }
        break;
    case ST_SILENCE:
        {
            return TRID_SILENCE;
        }
        break;
    case ST_REJECT:
        {
            return TRID_REJECT;
        }
        break;
    case ST_TRANSFER:
        {
            return TRID_TRANSFER;
        }
        break;
    case ST_NEWCALL:
        {
            return TRID_NEW_CALL;
        }
        break;
    case ST_OK:
        {
            return TRID_OK;
        }
        break;
    case ST_SWITCH:
        {
            return TRID_SWITCH;
        }
        break;
    //case ST_CCOMPLETION:
    //  {
    //      return "";
    //  }
    //  break;
    case ST_HOLD:
        {
            return TRID_HOLD;
        }
        break;
    case ST_SWAP:
        {
            return TRID_SWAP;
        }
        break;
    case ST_CONFERENCE:
        {
            return TRID_CONF;
        }
        break;
    case ST_RESUME:
        {
            return TRID_RESUME;
        }
        break;
    case ST_SPLIT:
        {
            return TRID_SPLIT;
        }
        break;

#ifndef _WIN32
#warning 会议优化
#endif
    /*
        case ST_SPLIT_ALL:
            {
                return TRID_SPLIT_ALL;
            }
            break;
    */
    case ST_MUTE:
        {
            return TRID_MUTE;
        }
        break;
    //case ST_MORE:
    //  {
    //      return "";
    //  }
    //  break;
    //case ST_ADD:
    //  {
    //      return "";
    //  }
    //  break;
    case ST_REMOVE:
        {
            return TRID_REMOVE;
        }
        break;
    //case ST_PRI_CHAT:     //与对方私聊
    //  {
    //      return "";
    //  }
    //  break;
    //case ST_EXIT_PRICHAT: //取消与对方私聊
    //  {
    //      return "";
    //  }
    //  break;
    case ST_POOL:
        {
            return TRID_FAVOURITE_DIRECTORIES;
        }
        break;
    case ST_PRIVATEHOLD:
        {
            return TRID_PRI_HOLD;
        }
        break;
    case ST_BACK:       //POOL界面返回
        {
            return TRID_BACK;
        }
        break;
    case ST_ENTER:  //POOL界面的进入
        {
            return TRID_ENTER;
        }
        break;

    case ST_SELECT:         //账号选择界面
        {
            return TRID_SELECT;
        }
        break;

    case ST_EXIT:               //IDLE界面的提示
        {
            return TRID_EXIT;
        }
        break;
    case ST_DPICKUP:
        {
            return TRID_DIRECT_PICKUP;
        }
        break;
    case ST_GPICKUP:
        {
            return TRID_GROUP_CALL_PICKUP;
        }
        break;
#if IF_BUG_27796
    case ST_Z_PICKUP:
        {
            return TRID_Z_PICKUP;
        }
        break;
#endif
    case ST_PARK:
        {
            return TRID_PARK_CALL;
        }
        break;
    case ST_GROUP_PARK:
        {
            return TRID_GROUP_CALL_PARK;
        }
        break;
    case ST_RETRIEVE_PARK:
        {
            return TRID_RETRIEVE;
        }
        break;
    case ST_ENDCALL:
        {
            return TRID_END_CALL;
        }
        break;
    //此处省略
    case ST_ACD_DISPCODE:
        {
            return TRID_ACD_DISPOSITION_CODE_SHORT;
        }
        break;
    case ST_ACD_TRACE:
        {
            return TRID_ACD_TRACE;
        }
        break;
    case ST_ACD_CALLINFO:
        {
            return TRID_CALL_INFO;
        }
        break;
    case ST_ACD_EMERGENCY:
        {
            return TRID_EMERGENCY;
        }
        break;
    case ST_CONF_MANAGE:
        {
            return TRID_MANAGE;
        }
        break;
    case ST_CONF_FAR_MUTE:
        {
            return TRID_FAR_MUTE;
        }
        break;
    case ST_CONF_REMOVE:
        {
            return TRID_REMOVE;
        }
        break;
    case ST_CONF_UNMUTE:
        {
            return TRID_UNMUTE;
        }
        break;
    case ST_RTP_STATUS:
        {
            return TRID_RTP_STATUS;
        }
        break;
    case ST_RTP_SWITCH:
        {
            return TRID_SWITCH;
        }
        break;
    //pickupmenu界面
    case ST_PICKUP:
        {
            return TRID_PICK_UP;
        }
        break;
    case ST_DIAL:
        {
            return TRID_DIAL;
        }
        break;
    //scamenu界面使用
    case ST_BARGEIN:
        {
            return TRID_BARGE_IN;
        }
        break;
    case ST_CALLPULL:
        {
            return TRID_CALL_PULL;
        }
        break;
    case ST_SCAMENU_RETRIEVE:
        {
            return TRID_RETRIEVE;
        }
        break;

#ifdef IF_FEATURE_ENTRANCE_GUARD
    case ST_OPENDOOR:
        {
            return TRID_OPENDOOR;
        }
        break;
#endif

#if IF_BUG_23548
    case ST_CALLCOMPLETIONCALLBACK:
        {
            return TRID_CALL_BACK;
        }
        break;
#endif
    case ST_SECURITY:
        {
            return TRID_SECURITY;
        }
        break;
#if IF_TALKUI_USB_ENABLE
    //录制相关
    //开始录音
    case ST_START_RECORD:
        {
            return TRID_START_RECORD;
        }
        break;
    //停止录音
    case ST_STOP_RECORD:
        {
            return TRID_STOP_RECORD;
        }
        break;
    //暂停录音
    case ST_PAUSE_RECORD:
        {
            return TRID_PAUSE_RECORD;
        }
        break;
    //恢复录音
    case ST_RESUME_RECORD:
        {
            return TRID_RESUME_RECORD;
        }
        break;

    //截图
    case ST_SCREENSHOT:
        {
            return TRID_SCREENSHOT;
        }
        break;
#endif

    case ST_IGNORE:
        {
            return TRID_IGNORE;
        }
        break;
    case ST_DECLINE:
        {
            return TRID_DECLINE;
        }
        break;

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    case ST_RECORD_START:
        {
            return TRID_RECORD_START;
        }
        break;
    case ST_RECORD_STOP:
        {
            return TRID_RECORD_STOP;
        }
        break;
    case ST_RECORD_PAUSE:
        {
            return TRID_RECORD_PAUSE;
        }
        break;
    case ST_RECORD_RESUME:
        {
            return TRID_RECORD_RESUME;
        }
        break;
#endif

#if IF_TALKUI_PSTN
    case ST_FLASH:
        {
            return TRID_FLASH;
        }
        break;
#endif

#if IF_TALKUI_EXECUTIVE
    case ST_CALLPUSH:
        {
            return TRID_DSSKEY_CALL_PUSH;
        }
        break;
    case ST_CALLINITIATION:
        {
            return TRID_DSSKEY_CALL_INITIATION;
        }
        break;
#ifndef _WIN32
#warning ST_SILENTBARGEIN
#endif
        /*
            case ST_SILENTBARGEIN:
                {
                    return TRID_SILENT_BARGIN;
                }
                break;
        */
#endif

    default:
        {
            return "";
        }
        break;
    }

    return "";
}
