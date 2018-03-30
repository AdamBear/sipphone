//mod-base
#include "talkui_define.h"
#include <ETLLib/ETLLib.hpp>
#include "commonfunction.h"

//mod-Talk
// #include "talk/talklogic_inc.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/callinfo.h"
#include "baseui/t4xpicpath.h"
#include "uimanager/uimanager_common.h"

#include "talkuidelegate.h"
#include "uicommon/uikernel/languagehelper.h"

#include "talkuiframemanager.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "dlgbasetalkui.h"

#include "callmanager/modcallmanager.h"
#include "imagemanager/modimagemanager.h"
#include "voice/include/modvoice.h"

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

QString GetChannelIcon()
{
    QString strChannelIcon = "";

    switch (voice_GetCurrentChannel())
    {
    case CHANNEL_HANDFREE:
    case CHANNEL_GROUP:
        {
            //免提
            strChannelIcon = PIC_ICON_TALKING_HANDFREE;
        }
        break;
    case CHANNEL_HANDSET:
        {
            //手柄
            strChannelIcon = PIC_ICON_TALKING_HANDSET;
        }
        break;
    case CHANNEL_HEADSET:
    case CHANNEL_2HEADSET:
        {
            //耳麦
            strChannelIcon = PIC_ICON_TALKING_HEADSET;
        }
        break;
    case CHANNEL_RING:
        {
            if (voice_IsRingHeadset())
            {
                // 耳麦响铃
                strChannelIcon = PIC_ICON_TALKING_HEADSET;
            }
            else
            {
                // 免提响铃
                strChannelIcon = PIC_ICON_TALKING_HANDFREE;
            }
        }
        break;
    default:
        {
            //未知
            strChannelIcon = "";
        }
        break;
    }

    return strChannelIcon;
}

//通过Session状态获取窗口名
const char * GetDlgNameBySessionState(SESSION_STATE eSessionState)
{
    switch (eSessionState)
    {
    case SESSION_DIALING:
    case SESSION_PRETRAN:
    case SESSION_PREDIAL:
#ifdef IF_FEATURE_BROADSOFT_CALLPARK
    case SESSION_PRE_RETRIEVEPARK:
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
        {
            return DLG_Dial;
        }
        break;
    case SESSION_RECALL:
    case SESSION_RINGING:
        {
            return DLG_Ringing;
        }
        break;
    case SESSION_FINISHED:
    case SESSION_AUTO_REDIAL:
        {
            return DLG_Finished;
        }
        break;
    case SESSION_CONNECTING:
    case SESSION_RINGBACK:
    case SESSION_BLACONNECTING:
    case SESSION_NETWORKCONF_CONNECT:
    case SESSION_CONNECTING_TRAN:
    case SESSION_RINGBACK_TRAN:
        {
            return DLG_Connect;
        }
        break;
    case SESSION_TALKING:
    case SESSION_NETWORKCONF:
    case SESSION_BETRANSFERRED:
    case SESSION_TRANSFERRED:
        {
            return DLG_Talking;
        }
        break;
    case SESSION_LOCALCONF:
        {
            return DLG_Conference;
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
            return DLG_ParkNotifyUI;
        }
        break;
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
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

//通过Logic数据获取窗口名
const yl::string GetDlgNameAndCheckLogicData(const DataLogic2UI & objLogicData,
        DataLogic2UI * pNewLogicData /* = NULL */)
{
    //先针对新逻辑数据进行赋值
    if (pNewLogicData != NULL)
    {
        pNewLogicData->eSessionState = objLogicData.eSessionState;
        pNewLogicData->iSessionID = objLogicData.iSessionID;
        pNewLogicData->pData = objLogicData.pData;
    }
    switch (objLogicData.eSessionState)
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
            return DLG_Dial;
        }
        break;
    case SESSION_RECALL:
    case SESSION_RINGING:
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    case SESSION_BTRINGING: //蓝牙来电， 蓝牙没有transfer callpark forward 等功能, 所以涉及到蓝牙的就是来电界面
#endif
        {
            DataRing * pRingData = dynamic_cast<DataRing *>(objLogicData.pData);
            if (pRingData != NULL)
            {
                if (pRingData->pDialData != NULL)
                {
                    //RingForward情况，需要生成DataDial赋值给拨号窗口
                    if (pNewLogicData != NULL)
                    {
                        pNewLogicData->pData = pRingData->pDialData;
                    }
                    return DLG_Dial;
                }
                else if (pRingData->bShowCallCenterInfo && (pRingData->pCCInfo != NULL))
                {
                    //ACD Call Info情况，需要生成CallInfo赋值给ACD Call Info窗口
                    if (pNewLogicData != NULL)
                    {
                        pNewLogicData->pData = pRingData->pCCInfo;
                    }
                    return DLG_CDlgACDCallInfo;
                }
            }
            return DLG_Ringing;
        }
        break;
    case SESSION_FINISHED:
    case SESSION_AUTO_REDIAL:
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    case SESSION_BTFINISHED:        //蓝牙通话结束
#endif
        {
            return DLG_Finished;
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
            return DLG_Connect;
        }
        break;
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    case SESSION_BTTALKING:         //蓝牙通话
#endif
    case SESSION_TALKING:
        {
            DataTalking2UI * pTalkingData = dynamic_cast<DataTalking2UI *>(objLogicData.pData);
            if (pTalkingData != NULL && pTalkingData->pDialData != NULL)
            {
                //RingForward情况，需要生成DataDial赋值给拨号窗口
                if (pNewLogicData != NULL)
                {
                    pNewLogicData->pData = pTalkingData->pDialData;
                }
                //说明是Call Park
                return DLG_Dial;
            }
            return DLG_Talking;
        }
        break;
    case SESSION_NETWORKCONF:
    case SESSION_BETRANSFERRED:
    case SESSION_TRANSFERRED:
        {
            return DLG_Talking;
        }
        break;
    case SESSION_LOCALCONF:
        {
            DataLocalConf * pConfData = dynamic_cast<DataLocalConf *>(objLogicData.pData);
            if (pConfData != NULL && pConfData->bInConfManager)
            {
                //会议管理器情况，需要生成DataDial赋值给拨号窗口
                return DLG_ConfManagerUI;
            }
            return DLG_Conference;
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
            return DLG_ParkNotifyUI;
        }
        break;
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
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

//判断sessionID 是否有效
bool IsSessionIDValid(int iSessionID)
{
    if (iSessionID >= 0)
    {
        return true;
    }

    return false;
}

//暂时还未实现
yl::string GetSoftKeyActionByType(SoftKey_TYPE eSoftkey)
{
    return "";
}

void PlaySoftKeyTone(yl::string strSoftKeyAction)
{
    int iKey = -1;
    if (TALK_UI_ACTION_SEND == strSoftKeyAction)
    {
        iKey = 1;
    }
    else if (TALK_UI_ACTION_DELETE == strSoftKeyAction)
    {
        iKey = 3;
    }

    if (-1 != iKey)
    {
        _TalkUIDelegate.PlayKeyTone(iKey);
    }
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
            && configParser_GetConfigInt(kszHideFeatureAccessCodes) != 0)
    {
        yl::string::size_type nPos = 0;
        if ((nPos = strText.find(':')) != yl::string::npos)
        {
            yl::string strTemp = strText;
            yl::string strFACValue = strTemp.substr(0, nPos);
            strTemp = strTemp.substr(nPos);
            strFACValue = LANG_TRANSLATE(strFACValue.c_str()).toUtf8().data();
            strFACValue.append(strTemp);

            return strFACValue;
        }
        else
        {
            return LANG_TRANSLATE(strText.c_str()).toUtf8().data();
        }
    }
    else
    {
        return strText;
    }
}

// 根据callinfo获取显示名称
yl::string GetNameFromCallInfo(CCallInfo * pCallInfo)
{
    if (pCallInfo != NULL)
    {
#if IF_BUG_31481
        if (!pCallInfo->m_tRemoteInfo.m_strCallTextInfo.empty())
        {
            return TransTalkText(pCallInfo->m_tRemoteInfo.m_strCallTextInfo);
        }
#endif

        if (pCallInfo->m_tRemoteInfo.sDisplayName.empty())
        {
            return TransTalkText(pCallInfo->m_tRemoteInfo.sSIPName);
        }
        return TransTalkText(pCallInfo->m_tRemoteInfo.sDisplayName);
    }
    return "";
}

// 根据callinfo获取显示号码
yl::string GetNumberFromCallInfo(CCallInfo * pCallInfo)
{
    if (pCallInfo != NULL)
    {
        return TransTalkText(pCallInfo->m_tRemoteInfo.sSIPName);
    }
    return "";
}

//根据callinfo获取fullcontactinfo
yl::string GetFullContactInfoFromCallInfo(CCallInfo * pCallInfo)
{
    if (NULL != pCallInfo)
    {
        return pCallInfo->m_tRemoteInfo.sFullContactInfo;
    }

    return "";
}

//根据callinfo获取头像
yl::string GetContactImageFromCallInfo(CCallInfo * pCallInfo)
{
    yl::string strContactImage = "";

    if (pCallInfo == NULL)
    {
        return strContactImage;
    }

    //http://10.2.1.199/Bug.php?BugID=92573 当是蓝牙头像的时候显示的路径是/phone/resource/system/contact/color/bluetooth_default.png
    strContactImage = pCallInfo->m_strRemotePhoto;

    TALKUI_INFO("Get Contact Image from logic : [%s]", strContactImage.c_str());

#ifdef IF_BT_SUPPORT_PHONE
    if (strContactImage.empty() && pCallInfo->IsBtTalk())
    {
        strContactImage = PIC_CONTACTS_IMAGE_BLUETOOTH_DEFAULT;
    }
#endif // IF_BT_SUPPORT_PHONE
    return strContactImage;
}

// 来电显示方式 有两个显示控件 一个 名称控件  一个 号码 控件
//执行该函数后  strName 用于给名称控件 赋值    strNumber用于给号码控件赋值
void ChangeNameAndNumberByInfoMethod(yl::string & strName, yl::string & strNumber,
                                     yl::string & strFullContactInfo, SHOW_CONTACTDETAIL_TYPE showContactDetailType,
                                     bool bIsMergerNameAndNumber /*= true*/)
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
        //如果是选择 显示 name + number  那么不需要做任何操作即可
    }
    else if (showContactDetailType == SHOW_NUMBER_AND_NAME)
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

//获取TargetInfo 信息 主要是给 通话、会议 提供新来电 targetinfo 信息  获取的优先级为 Label、DisplayName、UserName
yl::string GetTargetInfo()
{
    yl::string strTargetInfo = "";

    //需要显示targetinfo
    if (_TalkUIDelegate.IsShowTargetInfo())
    {
        int iAccountID = _TalkUIDelegate.GetTalkingIncomingCallTargetAccountId();

        strTargetInfo = _TalkUIDelegate.GetHotelingUserName(iAccountID);

        //获取到的hoteling账号为空
        if (strTargetInfo.empty())
        {
            strTargetInfo = GetLabel(iAccountID);
        }
    }

    return strTargetInfo;
}

//获取Label  Label 不存在那么 获取GetDisplayName 函数数据
yl::string GetLabel(int iAccountID)
{
    yl::string strLabel = _TalkUIDelegate.GetLabelFromLogic(iAccountID);

    if (strLabel.empty())
    {
        strLabel = GetDisplayName(iAccountID);
    }

    return strLabel;
}

//获取Displayname Display 如果获取不成功 那么用 GetUserName 函数数据
yl::string GetDisplayName(int iAccountID)
{
    yl::string strDisplayName = _TalkUIDelegate.GetDisplayNameFromLogic(iAccountID);

    if (strDisplayName.empty())
    {
        strDisplayName = GetUserName(iAccountID);
    }

    return strDisplayName;
}

//获取UserName 数据
yl::string GetUserName(int iAccountID)
{
    return _TalkUIDelegate.GetUserNameFromLogic(iAccountID);
}


//给callmanager模块调用的进入拨号界面的函数
void talkui_enterdialingui(const yl::string & strUIName)
{
    //直接调用逻辑层的进入拨号界面的函数
    talklogic_EnterDialUI("", AUTO_SELECT_LINE_ID);
}

//给callmanager 模块调用的退出拨号界面的函数
void talkui_putdialingui(const yl::string & strUIName)
{
    if (DLG_Dial == strUIName)
    {
        CCallManagerBaseDlg * pCurrentDlg = callmanager_GetPageByUIName(DLG_Dial);

        if (NULL != pCurrentDlg)
        {
            //确定当前是的界面是拨号界面
            if (pCurrentDlg->inherits(DLG_Dial))
            {
                CDlgBaseTalkUI * pTalkUI = (CDlgBaseTalkUI *)pCurrentDlg;

                //SessionID是有效的
                if (IsSessionIDValid(pTalkUI->GetSessionId()))
                {
                    DataLogic2UI dataToUI;

                    //从逻辑层获取数据
                    talklogic_GetDataBySessionID(pTalkUI->GetSessionId(), dataToUI);

                    //如果当前显示的界面还是拨号界面
                    if (GetDlgNameAndCheckLogicData(dataToUI) == DLG_Dial)
                    {
                        //这边拨号界面退出还是通过给逻辑层发送消息来完成 ， 因为逻辑层的session 还要清除
                        pTalkUI->OnBackButtonClick();

                        return ;
                    }
                }

                //直接通过设置 窗口为NULL 来退出
                _TalkUIFrameMananger.SetTalkUI(NULL, NULL);

                return ;
            }
        }
    }
}

//callmanager 解绑拨号对话框时的回调函数
void talkui_releasedialinguidlg(const yl::string & strUIName, CCallManagerBaseDlg * pDlg)
{
    //这边不做  处理  在释放的同时 做处理了
    return ;
}




