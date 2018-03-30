#include "talklogic_inc.h"

#ifdef IF_FEATURE_TIPTEL_ADAPTER
// 对外接口
void TiptelA_Init()
{
    _TiptelAdapter.Init();
}
void TiptelA_ParseUserInfo(CallInUserInfo * pUser)
{
    _TiptelAdapter.ParseUserInfo(pUser);
}
void TiptelA_ResetFlage()
{
    _TiptelAdapter.ResetFlage();
}
bool TiptelA_IsAutoAnswer()
{
    return _TiptelAdapter.IsAutoAnswer();
}
CHANNEL_MODE TiptelA_GetSpeekMode()
{
    return _TiptelAdapter.GetSpeekMode();
}
yl::string TiptelA_GetInvokeID()
{
    return _TiptelAdapter.GetInvokeID();
}
#else
// 对外接口
void TiptelA_Init() {  }
void TiptelA_ParseUserInfo(CallInUserInfo * pUser) {  }
void TiptelA_ResetFlage() {  }
bool TiptelA_IsAutoAnswer()
{
    return false;
}
CHANNEL_MODE TiptelA_GetSpeekMode()
{
    return CHANNEL_NULL;
}
LPCSTR TiptelA_GetInvokeID()
{
    return "";
}
#endif

#ifdef IF_FEATURE_TIPTEL_ADAPTER
LRESULT OnTiptelMessage(msgObject & msg)
{
    if (SIP_SUB_TIPTEL_PBX_CONTROL_NOTIFY != msg.message)
    {
        return 0;
    }

    TALK_INFO("SIP_SUB_TIPTEL_PBX_CONTROL_NOTIFY [%d][%d]", msg.wParam, msg.lParam);
    char * pBuffer = (char *)msg.GetExtraData();
    if (NULL == pBuffer)
    {
        return 0;
    }

    TALK_INFO("TIPTEL_PBX ExtraData:%s", pBuffer);
    if (1 == msg.lParam)
    {
        _TiptelAdapter.ParseParam(pBuffer);
        yl::string strNum =  _TiptelAdapter.GetNum();
        yl::string strName =  _TiptelAdapter.GetLCDDisplay();

        CHANNEL_MODE rChannel = _TiptelAdapter.GetSpeekMode();
        voice_SwitchChannel(rChannel);
        CallOut_Param tCallParam;
        tCallParam.eCallOutType = TCT_TIPTEL_AUTO_CALLOUT;
        talklogic_CallOut(strNum, strName, msg.wParam, &tCallParam);
    }
    else if (0 == msg.lParam)
    {
        TALK_INFO("Updata Account %d button", msg.wParam);
        _TiptelAdapter.ButtonStatusChange(pBuffer, msg.wParam);
    }

    return 1;
}

IMPLEMENT_GETINSTANCE(CTipTelAdapter)

void CTipTelAdapter::Init()
{
    SingleMsgReg(SIP_SUB_TIPTEL_PBX_CONTROL_NOTIFY, OnTiptelMessage);
}

CTipTelAdapter::CTipTelAdapter()
{
    m_eTiptelAnswerType = CHANNEL_AUTO;     //Tiptel服务器指定的通话声道模式
    m_strRemoteNum = "";            // 需要呼出的号码
    m_strLCD = "";
    m_bIsAutoAnswer = false;        //是否自动应答
    m_strInvokeID = "";
}

CTipTelAdapter::~CTipTelAdapter()
{
}

void CTipTelAdapter::ParseUserInfo(CallInUserInfo * pUser)
{
    if (pUser == NULL)
    {
        return;
    }

    m_eTiptelAnswerType = GetSpeekMode(pUser->bMic, pUser->bSpeak);

    if (m_eTiptelAnswerType != CHANNEL_AUTO)
    {
        m_bIsAutoAnswer = true;
    }
    else
    {
        /*http://bugfree.yealink.com/Bug.php?BugID=122855
         该处若未致false在来电时存在多话路的情况时，标记不会被重置，、
         将影响下一次单话路来电被自动接听
        */
        m_bIsAutoAnswer = false;
    }
}

CHANNEL_MODE CTipTelAdapter::GetSpeekMode(int iMic, int iSpeeker)
{
    if (iMic == 0 && iSpeeker == 1)
    {
        return CHANNEL_SPEAKER_ONLY;
    }
    else if (iMic == 1 && iSpeeker == 0)
    {
        return CHANNEL_MIC_ONLY;
    }
    else if (iMic == 1 && iSpeeker == 1)
    {
        return CHANNEL_HANDFREE;
    }
    else if (iMic == 0 && iSpeeker == 0)
    {
        //TALK_WARN("Tiptel::No Open Mic ang speaker,exit!");
        return CHANNEL_AUTO;
    }

    return CHANNEL_AUTO;
}

void CTipTelAdapter::ParseParam(LPCSTR param)
{
    /*
    InvokeID=5
    Destination=804
    LCD=804
    Mic=1
    Speaker=1
    */
    TALK_INFO("CTipTelAdapter::ParseParam[%s]", param);
    static char lpAttribute[5][32] = {"InvokeID", "Destination", "LCD", "Mic", "Speaker"};
    bool bMic = false;
    bool bSpeaker = false;
    char lpBuffer[128] = "";
    char * lpTemp = (char *)strstr(param, lpAttribute[0]);
    if (lpTemp != NULL)
    {
        sscanf(lpTemp, "%[^\r\n]", lpBuffer);
        //sscanf(lpTemp, "%*[^=]=%[^\r\n\0]", lpBuffer);
        m_strInvokeID = lpBuffer;
    }

    GetAttribute(param, lpAttribute[1], lpBuffer);
    m_strRemoteNum = lpBuffer;
    trim_both(m_strRemoteNum);

    GetAttribute(param, lpAttribute[2], lpBuffer);
    m_strLCD = lpBuffer;
    trim_both(m_strLCD);

    GetAttribute(param, lpAttribute[3], lpBuffer);
    bMic = (atoi(lpBuffer) == 1);

    GetAttribute(param, lpAttribute[4], lpBuffer);
    bSpeaker = (atoi(lpBuffer) == 1);

    m_eTiptelAnswerType = GetSpeekMode(bMic, bSpeaker);
}

void CTipTelAdapter::GetAttribute(LPCSTR lpBuffer, LPCSTR lpAttribute, LPSTR lpOut)
{
    if (NULL == lpAttribute || NULL == lpOut)
    {
        return;
    }

    char * lpTemp = (char *)strstr(lpBuffer, lpAttribute);
    if (lpTemp != NULL)
    {
        sscanf(lpTemp, "%*[^=]=%[^\r\n]", lpOut);
    }
}

// 辅助函数, Tiptel状态转换成灯状态
LIGHT_Type GetLightStatus(int iStatus)
{
    switch (iStatus)
    {
    case 0:
        return LT_OFF;
    case 1:
        return LT_RED_ON;
    case 2:
        return LT_RED_FLASH_4;
    case 3:
        return LT_GREEN_ON;
    case 4:
        return LT_GREEN_FLASH_4;
    case 5:
        return LT_RED_FLASH_1;
    case 6:
        return LT_GREEN_FLASH_1;
    default:
        return LT_NONE;
    }
}

// 辅助函数, 获取Tiptel服务器下发的Button对应的Dsskey
int GetDsskeyCode(int iIndex)
{
    // iIndex从0开始
    int iKeyCode = -1;
#ifdef IF_HAVE_MEMORY
    int iMemKeyNum = dsskey_GetMemKeyNum();
#else
    int iMemKeyNum = 0;
#endif  //IF_HAVE_MEMORY
    if (iIndex >= 0 && iIndex < iMemKeyNum)
    {
        return iIndex;
    }
    else
    {
        iIndex -= iMemKeyNum;

        // 神似应该固定40,需测试
#ifdef IF_HAVE_MEMORY
        int iExpKeyNum = dsskey_GetMemKeyNum();
#else
        int iExpKeyNum = 0;
#endif  //IF_HAVE_MEMORY
        if (iExpKeyNum == 0)
        {
            return -1;
        }

#if 0
        int iExpIndex = iIndex / iExpKeyNum;
        int iKey = iIndex % iExpKeyNum;
        iKey = OffsetIndex(iKey);
        iKeyCode = iExpIndex << 6;
        iKeyCode |= iKey & 0x3f;
#endif
    }

    return iKeyCode;
}

void CTipTelAdapter::ButtonStatusChange(char * lpEvent, int iLine)
{
    if (NULL == lpEvent)
    {
        return;
    }

    /*line=1;status=1;line=1;status=2;*/
    LPSTR lpTmp = lpEvent;
    lpTmp = strtok(lpTmp, ";");
    int iButton = -1;
    int iStatus = -1;
    LIGHT_Type eLightType = LT_NONE;
    while (NULL != lpTmp)
    {
        if (iButton != -1)
        {
            sscanf(lpTmp, "status=%d", &iStatus);
        }
        else
        {
            sscanf(lpTmp, "line=%d", &iButton);
        }

        if (iStatus != -1 && iButton != -1)
        {
            eLightType = GetLightStatus(iStatus);
            // 传过来的Button从1开始
            int iKey = GetDsskeyCode(iButton - 1);

            //TALK_INFO("Button:%d, Status->%d LightType:%d,ExpIndex:%d,KeyIndex:%d", iButton, iStatus, eLightType, GetEXPIndex(iKey), GetEXPKeyIndex(iKey));
            //TALK_INFO("Button:%d, Status->%d LightType:%d", iButton, iStatus, eLightType);

#if 0
            CDssKey * pDsskey = _DsskeyManager.GetDsskeyByArrayIndex(iKey);
            if (pDsskey != NULL && pDsskey->GetDsskeyType() == DT_TIPTEL_BUTTON
                    && ((CDssKeyTiptelBtn *)pDsskey)->m_iLine == iLine)
            {
                TALK_INFO("LightCtrl: Key[%d] LightType[%d]", iKey, eLightType);
                pDsskey->LightCtrl(eLightType);
            }
#endif

            iStatus = -1;
            iButton = -1;
        }

        lpTmp = strtok(NULL, ";");
    }
}
#endif // #ifdef IF_FEATURE_TIPTEL_ADAPTER
