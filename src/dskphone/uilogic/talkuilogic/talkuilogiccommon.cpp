#include "talkuilogiccommon.h"
#include "talkuilogicdelegate.h"
#include "talkuilogicframemanager.h"

#if IF_BUG_27560
// 隐藏通话信息的字符串
#define HIDE_TALK_INFO_STRING "******"
#endif

bool IsContactInfoEqual(const CContactInfo & objContactInfo1, const CContactInfo & objContactInfo2)
{
    if (objContactInfo1.strName != objContactInfo2.strName)
    {
        return false;
    }

    if (objContactInfo1.strNumber != objContactInfo2.strNumber)
    {
        return false;
    }

    if (objContactInfo1.strFullContactInfo != objContactInfo2.strFullContactInfo)
    {
        return false;
    }

    if (objContactInfo1.strContactImage != objContactInfo2.strContactImage)
    {
        return false;
    }

    //这个不判断了  因为这个随时会变
    //if(objContactInfo1.nDisplayMethod != objContactInfo2.nDisplayMethod)
    //{
    //  return false;
    //}

    if (objContactInfo1.iCallID != objContactInfo2.iCallID)
    {
        return false;
    }

    if (objContactInfo1.iSipAccount != objContactInfo2.iSipAccount)
    {
        return false;
    }

    if (objContactInfo1.bLocalHold != objContactInfo2.bLocalHold)
    {
        return false;
    }

    if (objContactInfo1.bRemoteHold != objContactInfo2.bRemoteHold)
    {
        return false;
    }

    if (objContactInfo1.bVideoMute != objContactInfo2.bVideoMute)
    {
        return false;
    }

    if (objContactInfo1.bAudioMute != objContactInfo2.bAudioMute)
    {
        return false;
    }

    if (objContactInfo1.bFarMute != objContactInfo2.bFarMute)
    {
        return false;
    }

    if (objContactInfo1.m_eAudioCodec != objContactInfo2.m_eAudioCodec)
    {
        return false;
    }

    if (objContactInfo1.bVideoTalk != objContactInfo2.bVideoTalk)
    {
        return false;
    }

    return true;

}

//int数据 转化为 string数据
yl::string IntToString(int iIn)
{
    char szData[32] = {0};

    sprintf(szData, "%d", iIn);

    return yl::string(szData);
}

//根据逻辑层上传的数据获取  使用的processor 的名称
yl::string GetProcessorNameFromData(DataLogic2UI * pDataLogic2UI, DataLogic2UI & newDataLogic2UI)
{
    if (NULL == pDataLogic2UI)
    {
        return yl::string("");
    }

    TALKUI_INFO("GetProcessorNameFromData state %d", pDataLogic2UI->eSessionState);

    //先针对新逻辑数据进行赋值
    newDataLogic2UI.eSessionState = pDataLogic2UI->eSessionState;
    newDataLogic2UI.eUpateReason = pDataLogic2UI->eUpateReason;
    newDataLogic2UI.iSessionID = pDataLogic2UI->iSessionID;
    newDataLogic2UI.pData = pDataLogic2UI->pData;

    switch (pDataLogic2UI->eSessionState)
    {
    case SESSION_DIALING:
    case SESSION_PRETRAN:
    case SESSION_PREDIAL:
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case SESSION_PRE_RETRIEVEPARK:
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    case SESSION_BTDIALING:     //蓝牙拨号
#endif
        {
            return PROCESSORNAME_DIAL;
        }
        break;
    case SESSION_RECALL:
    case SESSION_RINGING:
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    case SESSION_BTRINGING: //蓝牙来电， 蓝牙没有transfer callpark forward 等功能, 所以涉及到蓝牙的就是来电界面
#endif
        {
            if (pDataLogic2UI->pData != NULL)
            {
                DataRing * pRingData = (DataRing *)pDataLogic2UI->pData;
                if (pRingData->pDialData != NULL)
                {
                    //RingForward情况，需要生成DataDial赋值给拨号窗口

                    newDataLogic2UI.pData = pRingData->pDialData;

                    TALKUI_INFO("GetProcessorNameFromData Change to dial");

                    return PROCESSORNAME_DIAL;
                }
                else if (pRingData->bShowCallCenterInfo && (pRingData->pCCInfo != NULL))
                {
                    //ACD Call Info情况，需要生成CallInfo赋值给ACD Call Info窗口

                    newDataLogic2UI.pData = pRingData->pCCInfo;

                    TALKUI_INFO("GetProcessorNameFromData Change to acdcallinfo");

                    return PROCESSORNAME_ACDCALLINFO;
                }
            }
            return PROCESSORNAME_RING;
        }
        break;
    case SESSION_FINISHED:
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    case SESSION_BTFINISHED:        //蓝牙通话结束
#endif
        {
            return PROCESSORNAME_FINISH;
        }
    case SESSION_AUTO_REDIAL:
        {
            return PROCESSORNAME_AUTOREDIAL;
        }
        break;
    case SESSION_CONNECTING:
    case SESSION_RINGBACK:
    case SESSION_BLACONNECTING:
    case SESSION_NETWORKCONF_CONNECT:
    case SESSION_CONNECTING_TRAN:
    case SESSION_RINGBACK_TRAN:
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    case SESSION_BTCONNECTING:      //蓝牙呼出界面
#endif
        {
            return PROCESSORNAME_CONNECT;
        }
        break;
    case SESSION_TALKING:
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    case SESSION_BTTALKING:         //蓝牙通话
#endif
        {
            if (pDataLogic2UI->pData != NULL)
            {
                DataTalking2UI * pTalkingData = (DataTalking2UI *)pDataLogic2UI->pData;
                if (pTalkingData->pDialData != NULL)
                {
                    //RingForward情况，需要生成DataDial赋值给拨号窗口

                    newDataLogic2UI.pData = pTalkingData->pDialData;

                    TALKUI_INFO("GetProcessorNameFromData Change to dial");

                    //说明是Call Park
                    return PROCESSORNAME_DIAL;
                }

                TALKUI_INFO("GetProcessorNameFromData netconf size %d", pTalkingData->listNetUserInfo.size());

                if (pTalkingData->listNetUserInfo.size() > 0)
                {
                    TALKUI_INFO("GetProcessorNameFromData Change to netconf, in conf manager %d", pTalkingData->bInConfManager);

                    if (pTalkingData->bInConfManager)
                    {
                        return PROCESSORNAME_NETCONFMGR;
                    }
                    else
                    {
                        return PROCESSORNAME_NETCONF;
                    }
                }
            }
            return PROCESSORNAME_TALKING;
        }
        break;
    case SESSION_NETWORKCONF:
    case SESSION_BETRANSFERRED:
    case SESSION_TRANSFERRED:
        {
            return PROCESSORNAME_TALKING;
        }
        break;
    case SESSION_LOCALCONF:
        {
            if (pDataLogic2UI->pData != NULL)
            {
                DataLocalConf * pConfData = (DataLocalConf *)pDataLogic2UI->pData;

                TALKUI_INFO("GetProcessorNameFromData in conf manager %d", pConfData->bInConfManager);

                if (pConfData->bInConfManager)
                {
                    //如果有获取到新来电号码 就要退回到conf
                    yl::string strIncomingCallNum = "";
                    if (talklogic_GetIncomingCallNum(strIncomingCallNum))
                    {
                        return PROCESSORNAME_CONF;
                    }

                    //会议管理器情况，需要生成DataDial赋值给拨号窗口
                    return PROCESSORNAME_CONFMANAGER;
                }
            }
            return PROCESSORNAME_CONF;
        }
        break;
    case SESSION_ATSCONF:
        {
            //pNewTalkUI = new CATSConfUI(objData.eSessionState, iSessionID);
        }
        break;
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case SESSION_PARK_NOTIFY:
        {
            return PROCESSORNAME_PARKNOTIFY;
        }
        break;
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case SESSION_LINEDEVICE:
        {
            return PROCESSORNAME_TALKING;
        }
        break;
    case SESSION_UNKNOWN:
        {
            TALKUI_WARN("Unknown session state");
        }
        break;
    default:
        break;
    }

    return "";
}

//获取Hold标志!!!
bool GetCallInfoListHoldFlag(YLList<CCallInfo *> & listCallInfo, bool bAllFlag/* = true */)
{
    //根据列表设置Hold信息
    //是否所有话路都处于Hold
    bool bAllHoldTip = true;
    YLList<CCallInfo *>::iterator itCallInfo = listCallInfo.begin();
    for (int nIndex = 0; itCallInfo != listCallInfo.end()
            && nIndex < MAX_CONF_PARTY_NUM; ++itCallInfo, ++nIndex)
    {
        CCallInfo * pCallInfo = (CCallInfo *)(*itCallInfo);
        if (pCallInfo != NULL)
        {
            bAllHoldTip = bAllHoldTip && pCallInfo->IsLocalHold();
            if (!bAllFlag)
            {
                return bAllHoldTip;
            }
        }
    }

    return bAllHoldTip;
}

//获取Held标志!!!
bool GetCallInfoListHeldFlag(YLList<CCallInfo *> & listCallInfo, bool bAllFlag/* = true */)
{
    //根据列表设置Held信息
    //是否所有话路都处于Held
    bool bAllHeldTip = true;
    YLList<CCallInfo *>::iterator itCallInfo = listCallInfo.begin();
    for (int nIndex = 0; itCallInfo != listCallInfo.end()
            && nIndex < MAX_CONF_PARTY_NUM; ++itCallInfo, ++nIndex)
    {
        CCallInfo * pCallInfo = (CCallInfo *)(*itCallInfo);
        if (pCallInfo != NULL)
        {
            bAllHeldTip = bAllHeldTip && pCallInfo->IsRemoteHold();
            if (!bAllFlag)
            {
                return bAllHeldTip;
            }
        }
    }

    return bAllHeldTip;
}

//SessionID 是否有效
bool IsSessionIDValid(int iSessionID)
{
    if (iSessionID < 0)
    {
        return false;
    }
    return true;
}

//判断是否通道按键
bool talkuiIsChannelKey(int iKey)
{
    return voice_IsChannelKey(iKey);
}

//判断是否 数字按键
bool talkuiIsDigitKey(int iKey)
{
    return IsDigitKey(iKey);
}

//根据按键值  获取字符
yl::string GetDtmfStringByKeyCode(int iKey)
{
    if ((iKey >= PHONE_KEY_0) && (iKey <= PHONE_KEY_9))
    {
        char szKey[8] = {0};
        sprintf(szKey, "%d", iKey - PHONE_KEY_0);

        return yl::string(szKey);
    }
    else if (iKey == PHONE_KEY_STAR)
    {
        return "*";
    }
    else if (PHONE_KEY_POUND == iKey)
    {
        return "#";
    }

    switch (iKey)
    {
    case PHONE_KEY_A:
        {
            return "A";
        }
        break;
    case PHONE_KEY_B:
        {
            return "B";
        }
        break;
    case PHONE_KEY_C:
        {
            return "C";
        }
        break;
    case PHONE_KEY_D:
        {
            return "D";
        }
        break;
    default:
        {

        }
        break;
    }

    return "";
}

yl::string TransTalkText(yl::string & strText)
{
    // http://10.3.5.199/Bug.php?BugID=61933
    //柯工意见  这个不翻译  跟服务器返回有关
    //if (strText == "Anonymous"
    //  || strText == "anonymous")
    //{
    //  return LANG_TRANSLATE(strText.c_str()).toUtf8().data();
    //}

    if ((strText.find(kszReplaceCallParkCode) != yl::string::npos
            || strText.find(kszReplaceCallRetriveCode) != yl::string::npos
            || strText.find(kszReplaceGroupParkCode) != yl::string::npos
            || strText.find(kszReplaceGPickUpCode) != yl::string::npos
            || strText.find(kszReplaceDPickUpCode) != yl::string::npos
            || strText.find(kszReplaceVoiceMailCode) != yl::string::npos
            || strText.find(kszReplaceBargeInCode) != yl::string::npos
            || strText.find(kszReplaceCallPullCode) != yl::string::npos
#if IF_BUG_23548
            || strText.find(kszReplaceCallBackCode) != yl::string::npos
#endif
        )
            && _TalkUILogicDelegate.IsHideFeatureAccessCodes())
    {
        yl::string::size_type nPos = 0;
        if ((nPos = strText.find(':')) != yl::string::npos)
        {
            yl::string strTemp = strText;
            yl::string strFACValue = strTemp.substr(0, nPos);
            strTemp = strTemp.substr(nPos);
            strFACValue = _UILOGIC_LANG_TRAN(strFACValue.c_str());
            strFACValue.append(strTemp);

            return strFACValue;
        }
        else
        {
            return _UILOGIC_LANG_TRAN(strText.c_str());
        }
    }
    else
    {
        return strText;
    }
}

yl::string GetNameFromCallInfo(CCallInfo * pCallInfo)
{
    if (pCallInfo != NULL)
    {
#if IF_BUG_31481
        if (!pCallInfo->m_strCallTextInfo.empty())
        {
            return pCallInfo->m_strCallTextInfo;
        }
#endif

        yl::string strTemp = "";

        if (pCallInfo->m_tRemoteInfo.sDisplayName.empty())
        {
            strTemp = TransTalkText(pCallInfo->m_tRemoteInfo.sSIPName);

#if IF_BUG_27560
            if (pCallInfo->m_bHideInfo && !strTemp.empty())
            {
                return HIDE_TALK_INFO_STRING;
            }
#endif
            return strTemp;
        }

        strTemp = TransTalkText(pCallInfo->m_tRemoteInfo.sDisplayName);

#if IF_BUG_27560
        // 隐藏name为*
        if (pCallInfo->m_bHideInfo && !strTemp.empty()
                && pCallInfo->m_tRemoteInfo.sSIPName == pCallInfo->m_tRemoteInfo.sDisplayName)
        {
            strTemp = HIDE_TALK_INFO_STRING;
        }
#endif

        return strTemp;
    }
    return "";
}


yl::string GetNumberFromCallInfo(CCallInfo * pCallInfo)
{
    if (pCallInfo != NULL)
    {
        yl::string strTemp = TransTalkText(pCallInfo->m_tRemoteInfo.sSIPName);

#if IF_BUG_27560
        // 隐藏number为*
        if (pCallInfo->m_bHideInfo && !strTemp.empty())
        {
            strTemp = HIDE_TALK_INFO_STRING;
        }
#endif

        return strTemp;
    }
    return "";
}

//根据callinfo获取fullcontactinfo
yl::string GetFullContactInfoFromCallInfo(CCallInfo * pCallInfo, bool bLocalInfo/* = false*/)
{
    if (NULL != pCallInfo)
    {
#if IF_BUG_27560
        if (pCallInfo->m_bHideInfo)
        {
            if (bLocalInfo)
            {
                if (!pCallInfo->m_tLocalInfo.sFullContactInfo.empty())
                {
                    return HIDE_TALK_INFO_STRING;
                }
            }
            else
            {
                if (!pCallInfo->m_tRemoteInfo.sFullContactInfo.empty())
                {
                    return HIDE_TALK_INFO_STRING;
                }
            }
        }
#endif

        if (bLocalInfo)
        {
            return pCallInfo->m_tLocalInfo.sFullContactInfo;
        }
        else
        {
            return pCallInfo->m_tRemoteInfo.sFullContactInfo;
        }
    }

    return "";
}

//根据CCallInfo获取联系人信息
void GetContactInfoFromCallInfo(CContactInfo & objContactInfo, CCallInfo * pCallInfo)
{
    objContactInfo.nDisplayMethod = (SHOW_CONTACTDETAIL_TYPE)_TalkUILogicDelegate.GetDisplayMethod();

    if (NULL != pCallInfo)
    {
        //获取名称
        objContactInfo.strName = GetNameFromCallInfo(pCallInfo);
        objContactInfo.strNumber = GetNumberFromCallInfo(pCallInfo);
        objContactInfo.strFullContactInfo = GetFullContactInfoFromCallInfo(pCallInfo);

        //根据显示模式替换name 和 number
        ChangeNameAndNumberByInfoMethod(objContactInfo.strName, objContactInfo.strNumber,
                                        objContactInfo.strFullContactInfo, objContactInfo.nDisplayMethod);

        objContactInfo.strContactImage = pCallInfo->m_strRemotePhoto;

        objContactInfo.iCallID = pCallInfo->GetCallID();                          //通话ID
        objContactInfo.iSipAccount =
            pCallInfo->GetAccountID();                      //通话所使用的账号ID
        objContactInfo.bLocalHold = pCallInfo->IsLocalHold();                        //本方hold状态
        objContactInfo.bFarHold = pCallInfo->IsFarHold();                        //远端hold状态
        objContactInfo.bRemoteHold = pCallInfo->IsRemoteHold();                      //对方hold状态
        objContactInfo.bAudioMute = _TalkUILogicDelegate.GetAudioMuteStatus();      //是否音频mute
        objContactInfo.bFarMute = pCallInfo->IsFarMute();                            //远端静音

        objContactInfo.m_eAudioCodec = pCallInfo->GetAudioCodecType(); //高清通话类型

        objContactInfo.bVideoTalk = false;                  //是否视频通话

        objContactInfo.m_eCallLineType = pCallInfo->GetCallLineType();

        //是否已经是接起状态
        objContactInfo.m_bReceived = (ROUTINE_CONNECTING != pCallInfo->m_eStateOfRoutine
                                      && ROUTINE_RINGBACK != pCallInfo->m_eStateOfRoutine);
    }
}

// 来电显示方式 有两个显示控件 一个 名称控件  一个 号码 控件
//执行该函数后  strName 用于给名称控件 赋值    strNumber用于给号码控件赋值
void ChangeNameAndNumberByInfoMethod(yl::string & strName
                                     , yl::string & strNumber
                                     , yl::string & strFullContactInfo
                                     , SHOW_CONTACTDETAIL_TYPE showContactDetailType
                                     , bool bIsMergerNameAndNumber /*= true*/
                                     , bool bNameNumberInOneLine/* = false*/)
{
    ////暂时不做任何操作  直接返回
    //if(bIsMergerNameAndNumber)
    //{
    //if(strName == strNumber)
    //{
    //  strNumber = "";
    //}
    //}
    //return ;

    //三个当中 至少有一个 有值的时候 先保证 strName 有值
    if (strName.empty())
    {
        strName = strNumber;

        if (strName.empty())
        {
            strName = strFullContactInfo;

            //这边如果是3个联系人信息全是为空 那么直接返回就好了
            if (strName.empty())
            {
                return ;
            }
        }
    }

    //这三个 信息中strName 有用于最后赋值给 名称控件    strNumber 有用于赋值给 号码控件   strFullContactInfo 没有用于赋值给任何控件
    //所以 其实只是给 strName 和 strNumber 两个变量赋值就行了

    //  int nDisplayMethod = configParser_GetConfigInt(kszDisplayMethod);
    if (showContactDetailType == SHOW_NAME_AND_NUMBER)
    {
        if (bNameNumberInOneLine)
        {
            if (strName == strNumber)
            {
                strNumber = "";
            }
            else
            {
                strName = (strName + " ") + strNumber;
            }
        }
        //如果是选择 显示 name + number  那么不需要做任何操作即可
    }
    else if (showContactDetailType == SHOW_NUMBER_AND_NAME)
    {
        if (bNameNumberInOneLine)
        {
            if (strName == strNumber)
            {
                strNumber = "";
            }
            else
            {
                strName = (strNumber + " ") + strName;
            }
        }
        else
        {
            //如果 是选择 number + name  那么 要对调一下
            //但是首先得保证 strNumber 不为空
            if (!strNumber.empty())
            {
                yl::string strTemp = strName;
                strName = strNumber;
                strNumber = strTemp;
            }
        }
    }
    else if (showContactDetailType == SHOW_ONLY_NAME)
    {
        //如果只显示 名称  那么把号码赋值为空
        strNumber = "";
    }
    else if (showContactDetailType == SHOW_ONLY_NUMBER)
    {
        //如果只显示号码  那么把号码赋值给 名称变量  然后号码变量设置为空
        //但是首先要保证 号码不为空  否则仍然显示号码
        if (!strNumber.empty())
        {
            strName = strNumber;
            strNumber = "";
        }
    }
    else if (showContactDetailType == SHOW_FULL_CONTACT_INFO)
    {
        //如果只显示 FullContactInfo 那么把 FullContactInfo 赋值个 名称 并且 号码 赋值为空
        strNumber = "";

        //但是首先要保证FullContactInfo 不为空
        if (!strFullContactInfo.empty())
        {
            strName = strFullContactInfo;
        }
    }
#if IF_BUG_29709
    else if (showContactDetailType == SHOW_NULL)
    {
        if ("anonymous" != strNumber.MakeLower()
                && "private number" != strNumber.MakeLower())
        {
            strName = "";
        }
        strNumber = "";
    }
#endif

    //如果需要合并号码和名称
    if (bIsMergerNameAndNumber)
    {
        //名称和号码相同才需要合并
        if (strName == strNumber)
        {
            //把 号码赋值为空就可以了
            strNumber = "";
        }
    }
}

// http://10.2.1.199/Bug.php?BugID=77252
//根据显示模式获取名称
yl::string GetNameByDisplayModeAndCallInfo(CCallInfo * pCallInfo)
{
    if (NULL == pCallInfo)
    {
        return "";
    }

    yl::string strName = "";


    // http://10.2.1.199/Bug.php?BugID=77252
    //根据显示模式获取名称
    switch (_TalkUILogicDelegate.GetDisplayMethod())
    {
    case SHOW_NAME_AND_NUMBER:
    case SHOW_ONLY_NAME:
        {
            strName = GetNameFromCallInfo(pCallInfo);
        }
        break;
    case SHOW_NUMBER_AND_NAME:
    case SHOW_ONLY_NUMBER:
        {
            strName = GetNumberFromCallInfo(pCallInfo);
        }
        break;
    case SHOW_FULL_CONTACT_INFO:
        {
            strName = GetFullContactInfoFromCallInfo(pCallInfo);
        }
        break;
    default:
        {
            strName = GetNameFromCallInfo(pCallInfo);
        }
        break;
    }

    return strName;
}

//输入的SessionID 中是否含有该callID
//sessionID 从0开始 也就是可以为0
bool IsSessionHaveCallID(int iSessionID, int iCallID)
{
    if (iSessionID < 0)
    {
        return false;
    }

    if (iCallID < 0)
    {
        return false;
    }

    int iGetCallID = talklogic_GetCallIdBySessionID(iSessionID);
    //获取到相同那么就一定是的
    if (iGetCallID == iCallID)
    {
        return true;
    }

    //获取到不同 那么不能认为就一定不是  因为还有会议可能含有多个callID
    DataLogic2UI dataToUI;
    talklogic_GetDataBySessionID(iSessionID, dataToUI);

    DataLogic2UI objNewDataToUI;

    //获取处理类名称
    yl::string strProcessorName = GetProcessorNameFromData(&dataToUI, objNewDataToUI);

    //如果获取到的不是会议  那么直接返回false
    if ((PROCESSORNAME_CONF != strProcessorName) && (PROCESSORNAME_CONFMANAGER != strProcessorName))
    {
        return false;
    }

    DataLocalConf * pConfData = (DataLocalConf *)objNewDataToUI.pData;

    if (NULL == pConfData)
    {
        return false;
    }

    //根据列表设置话路信息
    YLList<CCallInfo *>::iterator itCallInfo = pConfData->listCallInfo.begin();

    //循环读取联系人信息
    for (; pConfData->listCallInfo.end() != itCallInfo; itCallInfo++)
    {
        CCallInfo * pCallInfo = *itCallInfo;

        if (NULL != pCallInfo)
        {
            if (iCallID == pCallInfo->GetCallID())
            {
                return true;
            }
        }
    }

    return false;

}

