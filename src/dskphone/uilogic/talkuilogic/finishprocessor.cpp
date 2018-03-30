#include "finishprocessor.h"
#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "uilogicfinishuibase.h"

CFinishProcessor::CFinishProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_FINISH)
{
    m_pTalkUI = NULL;
}

CFinishProcessor::~CFinishProcessor()
{
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CFinishProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    m_objFinishUIData.Reset();
}

//释放前解除UI绑定前调用
void CFinishProcessor::Uninit()
{
    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CFinishProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicFinishUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CFinishProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取数据
CFinishUIData * CFinishProcessor::GetFinishUIData()
{
    return &m_objFinishUIData;
}

//设置数据
void CFinishProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    DataCallFinished * pFinishData = (DataCallFinished *)pData;

    if (NULL == pFinishData->pCallInfo)
    {
        return ;
    }

    //获取联系人信息
    GetContactInfoFromCallInfo(m_objFinishUIData.objContactInfo, pFinishData->pCallInfo);

    //赋值是否错误
    m_objFinishUIData.bError = pFinishData->bError;
    //赋值是否账号错误
    m_objFinishUIData.bInvalidAccount = pFinishData->bInvalidAccount;
    //是否是最大session错误
    m_objFinishUIData.bMaxSessionError = pFinishData->bMaxSessionError;
    //是否允许创建新话路
    m_objFinishUIData.bAllowNewCall = pFinishData->bAllowNewCall;

    //判断是否是有错误提示
    if (pFinishData->bError)
    {
        //判断是否是账号错误
        if (true == pFinishData->bInvalidAccount)
        {
            m_objFinishUIData.strInfo = _UILOGIC_LANG_TRAN(TRID_INVALID_ACCOUNT);   //赋值账号错误信息
        }
        else
        {
            m_objFinishUIData.strInfo = _UILOGIC_LANG_TRAN(
                                            pFinishData->strInfo.c_str());           //赋值错误信息
        }
    }
    else
    {
        m_objFinishUIData.strInfo = _UILOGIC_LANG_TRAN(
                                        pFinishData->strInfo.c_str());           //赋值通话时间
    }
}

// 通道键处理
bool CFinishProcessor::OnChannelKeyPress(int iKey)
{
    bool bHandle = false;

    switch (iKey)
    {
#if IF_TALKUI_HANDFREE_DIAL
    case PHONE_KEY_HANDFREE:
        {
            if (acc_ExistAvailableAccount(ACCOUNT_TYPE_ALL))
            {
                bHandle = TRUE;
                SendUIRequestAction(TALK_ACTION_NEWCALL);
            }
        }
        break;
#endif

    default:
        bHandle = false;
        break;
    }

    if (!bHandle)
    {
        bHandle = CBaseTalkUIProcessor::OnChannelKeyPress(iKey);
    }

    return bHandle;
}

// 功能键处理
bool CFinishProcessor::OnFunckeyPress(int iKey)
{
    switch (iKey)
    {
    case PHONE_KEY_CANCEL:
        {
            //x键复用为 mute
            if (_TalkUILogicDelegate.IsMuteReplaceX())
            {
                SendUIRequestAction(TALK_ACTION_MUTE);

                return true;
            }

            SendUIRequestAction(TALK_ACTION_CANCEL);
            return true;
        }
        break;
    case PHONE_KEY_OK:
        {
            SendUIRequestAction(TALK_ACTION_OK);
            return true;
        }
        break;

    case PHONE_KEY_HANDSET_ON_HOOK:
    case PHONE_KEY_HANDFREE:
        {
            SendUIRequestAction(TALK_ACTION_CANCEL);
            return true;
        }
        break;

    default:
        break;
    }

    return false;

}

// 数字键处理
bool CFinishProcessor::OnDigitkeyPress(int iKey)
{
    return false;
}

//softKey 按键处理
bool CFinishProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = true;

    switch (eSoftkey)
    {
    case ST_NEWCALL:
        {
            SendUIRequestAction(TALK_ACTION_NEWCALL);
        }
        break;
    case ST_CANCEL:
        {
            SendUIRequestAction(TALK_ACTION_CANCEL, 0, 0, NULL);
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

//设置title
void CFinishProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    DataCallFinished * pFinishData = (DataCallFinished *)pData;

//  http://10.3.5.199/Bug.php?BugID=63029
    if (true == pFinishData->bError)
    {
        m_strTitle = _UILOGIC_LANG_TRAN(TRID_CALL_FAIL);
    }
    else
    {
        m_strTitle = _UILOGIC_LANG_TRAN(TRID_CALL_FINISH);
    }

}

//设置图标列表
void CFinishProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //先把数据重置一下
    m_listIcons.clear();

    //finish界面没有需要显示的图标

}

//给子界面设置数据
void CFinishProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objFinishUIData, eReason);
    }
}

//获取softkey数据信息
void CFinishProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //获取蓝牙的softkey
    if (GetSessionState() == SESSION_BTFINISHED)
    {
        GetBluetoothSoftkey(VecSoftkey);

        return ;
    }
#endif

    if (m_objFinishUIData.bInvalidAccount
            || !m_objFinishUIData.bAllowNewCall
            || m_objFinishUIData.bMaxSessionError)
    {
        VecSoftkey.push_back(ST_EMPTY);
        VecSoftkey.push_back(ST_EMPTY);
        VecSoftkey.push_back(ST_EMPTY);
        VecSoftkey.push_back(ST_CANCEL);
        return;
    }


    VecSoftkey.push_back(ST_NEWCALL);
    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_EMPTY);
    //  http://10.3.5.199/Bug.php?BugID=62052
    //统一成都显示cancel
    //http://10.3.5.199/Bug.php?BugID=56032
    //如果是call failed 那么显示的softkey 和 默认的softkey 相同
    VecSoftkey.push_back(ST_CANCEL);
}

//获取蓝牙softkey数据信息
void CFinishProcessor::GetBluetoothSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (!m_objFinishUIData.bAllowNewCall
            || m_objFinishUIData.bMaxSessionError)
    {
        VecSoftkey.push_back(ST_NEWCALL);
    }
    else
    {
        VecSoftkey.push_back(ST_EMPTY);
    }

    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_EMPTY);

    //  http://10.3.5.199/Bug.php?BugID=62052
    //统一成都显示cancel
    //http://10.3.5.199/Bug.php?BugID=56032
    //如果是call failed 那么显示的softkey 和 默认的softkey 相同
    VecSoftkey.push_back(ST_CANCEL);

}

// 是否允许自定义softkey
bool CFinishProcessor::IsAllowDefineSoftkey()
{
#if IF_TALKUI_BLUETOOTH_BT_ENABLE
    //蓝牙来电不需要显示自定义的softkey
    if (GetSessionState() == SESSION_BTFINISHED)
    {
        return false;
    }

#endif

    return true;
}

// softkey处理(自定义类型)
void CFinishProcessor::GetSoftkey(yl::string & strFile, yl::string & strStatus)
{
    strFile = "CallFailed.xml";
    strStatus = "CallFailed";
}





