#include "parknotifyprocessor.h"
#include "talkuilogiccommon.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talkuilogicdelegate.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "broadsoft/callpark/include/callparkcommon.h"
#include "uilogicparknotifyuibase.h"

CParkNotifyProcessor::CParkNotifyProcessor()
    : CBaseTalkUIProcessor(PROCESSORNAME_PARKNOTIFY)
{
    m_pTalkUI = NULL;

    m_bShowTalkTime = true;     //是否显示通话时间
}

CParkNotifyProcessor::~CParkNotifyProcessor()
{
    m_pTalkUI = NULL;
}

//构造完成后 绑定了UI后调用
void CParkNotifyProcessor::Init()
{
    //基类先初始化
    CBaseTalkUIProcessor::Init();

    m_objParkNotifyUIData.Reset();
}

//释放前解除UI绑定前调用
void CParkNotifyProcessor::Uninit()
{
    CBaseTalkUIProcessor::Uninit();

}

//设置绑定的UI
void CParkNotifyProcessor::SetBindUI(CUILogicBaseTalkUI * pTalkUI)
{
    m_pTalkUI = (CUILogicParkNotifyUIBase *)pTalkUI;
}

//获取绑定的UI
CUILogicBaseTalkUI * CParkNotifyProcessor::GetBindUI()
{
    return m_pTalkUI;
}

//获取数据
CParkNotifyUIData * CParkNotifyProcessor::GetParkNotifyUIData()
{
    return &m_objParkNotifyUIData;
}

//设置数据
void CParkNotifyProcessor::SetTalkData(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    // 设置数据
    DataParkNotify * pNotifyData = (DataParkNotify *)pData;

//  GetContactInfoFromCallInfo(m_objParkNotifyUIData.objContactInfo, pNotifyData->pC)
    m_objParkNotifyUIData.objContactInfo.strName = _UILOGIC_LANG_TRAN(
                pNotifyData->strDiaplayName.c_str());
    //因为可能显示的 方法是 number + name
    m_objParkNotifyUIData.objContactInfo.strNumber = _UILOGIC_LANG_TRAN(pNotifyData->strNumber.c_str());
    m_objParkNotifyUIData.objContactInfo.strFullContactInfo = "";
    m_objParkNotifyUIData.objContactInfo.nDisplayMethod = (SHOW_CONTACTDETAIL_TYPE)
            _TalkUILogicDelegate.GetDisplayMethod(); //显示策略
    m_objParkNotifyUIData.objContactInfo.strContactImage =
        "";          //这边就不赋值了  让ui去获取、显示默认的头像
    //联系人的其他数据就不需要赋值了


}

// 通道键处理
bool CParkNotifyProcessor::OnChannelKeyPress(int iKey)
{
    bool bHandled = false;

    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
    case PHONE_KEY_EHS_OPEN:
    case PHONE_KEY_HANDSET_OFF_HOOK:
        {
            //  http://10.2.1.199/Bug.php?BugID=36135
            // park notify界面按免提不需要去取回通话，而是要进入预拨号界面
            bHandled = true;
            talklogic_EnterDialUI("", AUTO_SELECT_LINE_ID);
            voice_SwitchChannel();
        }
        break;
    default:
        break;
    }

    if (!bHandled)
    {
        bHandled = CBaseTalkUIProcessor::OnChannelKeyPress(iKey);
    }

    return bHandled;
}

// 功能键处理
bool CParkNotifyProcessor::OnFunckeyPress(int iKey)
{
    switch (iKey)
    {
    case PHONE_KEY_OK:
        {
            SendUIRequestAction(TALK_ACTION_RETRIEVE_PARK);
            return true;
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            SendUIRequestAction(TALK_ACTION_CANCEL);
            return true;
        }
        break;
    case PHONE_KEY_MESSAGE:     //彩屏 黑白屏统一处理
        {
            SendUIRequestAction(TALK_ACTION_SMS);

            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

// 数字键处理
bool CParkNotifyProcessor::OnDigitkeyPress(int iKey)
{
    //暂不处理数字键
    return false;
}

//softKey 按键处理
bool CParkNotifyProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = false;

    switch (eSoftkey)
    {
    case ST_RETRIEVE_PARK:
        {
            SendUIRequestAction(TALK_ACTION_RETRIEVE_PARK);
            bHandled = true;
        }
        break;
    case ST_EXIT:
    case ST_CANCEL:
    case ST_REJECT:
        {
            SendUIRequestAction(TALK_ACTION_CANCEL);
            bHandled = true;
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

//设置title
void CParkNotifyProcessor::SetTitle(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //设置title
    m_strTitle = _UILOGIC_LANG_TRAN(TRID_CALL_PARK);
}

//设置图标列表
void CParkNotifyProcessor::SetIcons(void * pData, UPDATE_REASON eReason)
{
    if (NULL == pData)
    {
        return ;
    }

    //先把数据重置一下
    m_listIcons.clear();

    //不显示icon
}

//给子界面设置数据
void CParkNotifyProcessor::SetTalkUIData(UPDATE_REASON eReason)
{
    //给UI设置数据
    if (NULL != m_pTalkUI)
    {
        m_pTalkUI->SetUILogicData(&m_objParkNotifyUIData, eReason);
    }
}

//获取softkey数据信息
void CParkNotifyProcessor::GetSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    // http://10.3.5.199/Bug.php?BugID=71884
    //broadsoft 开启的时候才能显示相关 softkey
	//http://bugfree.yealink.com/Bug.php?BugID=124363
// 	if (configParser_GetConfigInt(kszBroadsoftActive) != 0)
// 	{
        VecSoftkey.push_back(ST_RETRIEVE_PARK);
// 	}
// 	else
// 	{
// 		VecSoftkey.push_back(ST_EMPTY);
// 	}

    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_EMPTY);
    VecSoftkey.push_back(ST_EXIT);
}

// softkey处理(自定义类型)
void CParkNotifyProcessor::GetSoftkey(yl::string & strFile, yl::string & strStatus)
{
    //不允许自定义softkey
}

// 是否允许自定义softkey
//不允许自定义softkey
bool CParkNotifyProcessor::IsAllowDefineSoftkey()
{
    return false;
}




