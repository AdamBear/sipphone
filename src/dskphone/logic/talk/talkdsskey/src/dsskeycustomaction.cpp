#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"


static const yl::string CUSTOM_DTMF_KEY = "custom_dtmf_key";
static const yl::string CUSTOM_STATUS_KEY = "custom_static_key";
//DTMF COMMAND
static const char COMMAND_START_TAG = '<';
static const char COMMAND_END_TAG = '>';
static const char COMMAND_PAUSE_FMT[] = "<%d>";/*"<pause%*[^0-9]%d%*[^a-zA-Z]sec>";*/

static const yl::string CUSTOM_NONE = "none";
static const yl::string CUSTOM_COLORRING = "clorring";
static const yl::string CUSTOM_TALKING = "talking";
static const yl::string CUSTOM_INTRAN = "intran";
static const yl::string CUSTOM_INCONF = "inconf";
static const yl::string CUSTOM_TRANED = "traned";
static const yl::string CUSTOM_CONFED = "confed";
static const yl::string CUSTOM_HOLD = "hold";
static const yl::string CUSTOM_HELD = "held";

static const int DTMF_TIMER_INTERVAL = 180;


YLHashMap<int, int> CDsskeyCustomAction::mapDtmfIndex;

CDsskeyCustomAction::CDsskeyCustomAction()
    : CDssTalkBaseAciton(DT_CUSTOM)
{

}

bool CDsskeyCustomAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                     void* pExtraData)
{
    // 是否存在通话
    if (!talklogic_SessionExist())
    {
        return false;
    }

    // Value是否为空
    if (logicData.strValue.empty())
    {
        return false;
    }

    // 退出界面
    AdapterBox_ExitDsskeyDlg();

    // 获取Custum状态
    dsskey_SetDssekeyExtFiled(dsskeyDataID, CUSTOM_STATUS_KEY, GetCustomStatus());
    if (GetCustomStatus() != CUSTOM_NONE)
    {
        // 设置定时器
        mapDtmfIndex[dsskeyDataID] = 0;
        dsskey_SetTimer(dsskeyDataID, DTMF_TIMER_INTERVAL);
        return true;
    }

    return false;
}

bool CDsskeyCustomAction::OnTimer(int dsskeyDataID, UINT timerID, const DssKeyLogicData& logicData,
                                  void* pExtraData)
{

    if (timerID == dsskey_GetRelatedTimerID(dsskeyDataID))
    {
        //先取消定时器
        dsskey_KillTimer(dsskeyDataID);

        //检查有效性
        const yl::string& strDtmf = logicData.strValue;
        yl::string lastCustom;
        dsskey_GetDssekeyExtFiled(dsskeyDataID, CUSTOM_STATUS_KEY, lastCustom);
        // 状态转换或者DTMF已发送完成就停止
        if (GetCustomStatus() != lastCustom
                || mapDtmfIndex[dsskeyDataID] >= strDtmf.size())
        {
            mapDtmfIndex.erase(dsskeyDataID);
        }
        else
        {
            // 获取焦点Session
            if (!talklogic_SessionExist())
            {
                // 无焦点处理
                return true;
            }

            char cDtmf = logicData.strValue[mapDtmfIndex[dsskeyDataID]];

            if ((cDtmf >= '0' && cDtmf <= '9') || (cDtmf >= 'A' && cDtmf <= 'E') || cDtmf == '*'
                    || cDtmf == '#')
            {
                int iKey = 0;
                if (cDtmf >= '0' && cDtmf <= '9')
                {
                    iKey = PHONE_KEY_0 + cDtmf - '0';
                }
                else if (cDtmf >= 'A' && cDtmf <= 'E')
                {
                    iKey = PHONE_KEY_A + cDtmf - 'A';
                }
                else
                {
                    switch (cDtmf)
                    {
                    case '*':
                        iKey = PHONE_KEY_STAR;
                        break;
                    case '#':
                        iKey = PHONE_KEY_POUND;
                        break;
                    default:
                        iKey = cDtmf;
                        break;
                    }
                }


                // 将按键发给当前焦点界面
                SendKeyMsg(iKey, true);
            }
            else if (cDtmf == COMMAND_START_TAG)
            {
                yl::string::size_type nStartPos = mapDtmfIndex[dsskeyDataID];
                //查找命令结束标记
                yl::string::size_type nEndPos = strDtmf.find(COMMAND_END_TAG, nStartPos);
                if (nEndPos != yl::string::npos)
                {
                    yl::string strCommand = strDtmf.substr(nStartPos, nEndPos - nStartPos + 1);
                    //获取延时命令时长
                    int iDelay = 0;
                    if (sscanf(strCommand.c_str(), COMMAND_PAUSE_FMT, &iDelay) == 1
                            && iDelay > 0)
                    {
                        dsskey_SetTimer(dsskeyDataID, iDelay * 1000);
                        mapDtmfIndex[dsskeyDataID] += strCommand.size();
                        //由于不能使用默认定时器和位置移动距离，直接返回
                        return true;
                    }
                }
                //找不到标记，按无效字符处理，跳过
            }
            else if (cDtmf == '\0') //容错
            {
                return true;
            }

            //移动字符位置，设置定时器
            ++mapDtmfIndex[dsskeyDataID];
            dsskey_SetTimer(dsskeyDataID, DTMF_TIMER_INTERVAL);
        }

        return true;
    }

    return false;
}

char CDsskeyCustomAction::GetNextChar(int dsskeyDataID)
{
    yl::string strDtmfStack;
    dsskey_GetDssekeyExtFiled(dsskeyDataID, CUSTOM_DTMF_KEY, strDtmfStack);
    char chResult = '\0';
    if (!strDtmfStack.empty())
    {
        chResult = strDtmfStack.back();
        strDtmfStack.pop_back();
        dsskey_SetDssekeyExtFiled(dsskeyDataID, CUSTOM_DTMF_KEY, strDtmfStack);
    }
    return chResult;
}

yl::string CDsskeyCustomAction::GetCustomStatus()
{
    // 获取焦点Session
    CSessionStateProxy focus;
    if (!focus)
    {
        return CUSTOM_NONE;
    }

    CCallInfo* pCallInfo = focus.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return CUSTOM_NONE;
    }

    switch (pCallInfo->m_eStateOfRoutine)
    {
    case ROUTINE_TALKING:
        {
            switch (pCallInfo->m_eTalkState)
            {
            case TS_TALK:
                return CUSTOM_TALKING;
            case TS_HOLD:
                return CUSTOM_HOLD;
            case TS_HELD:
                return CUSTOM_HELD;
            default:
                break;
            }
        }
        break;
    case ROUTINE_NETWORKCONF:
        return CUSTOM_CONFED;
    default:
        break;
    }

    if (pCallInfo->m_bIsColorRingback)
    {
        return CUSTOM_COLORRING;
    }

    return CUSTOM_NONE;
}

bool CDsskeyCustomAction::SendKeyMsg(int iKeyCode, bool bDown)
{
    commonUnits_SendSimulateKey(DEV_MSG_DIGITKEY_EVENT, iKeyCode, false);
    return true;
}
#endif  //IF_FEATURE_DSSKEY
