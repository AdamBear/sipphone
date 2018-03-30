#include "dlgconferenceui.h"
#include "talkuidelegate.h"
#include "commonfunction.h"

#include "baseui/t4xpicpath.h"

#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"

#include "uicommon/uimanager/rectdefine.h"
#include "uicommon/imagemanager/modimagemanager.h"

#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/modtalklogic.h"

//mod-messagebox
#include "uicommon/messagebox/modmessagebox.h"


CDlgConferenceUI::CDlgConferenceUI()
    : m_iCurrentActionCallID(-1)
{
    InitData();
}

CDlgConferenceUI::~CDlgConferenceUI(void)
{
}

// 初始化
void CDlgConferenceUI::Init()
{
    CDlgConferenceBase::Init();

    //还原CallID
    m_iCurrentActionCallID = -1;

    //重新启动计时器
    m_qTimePiece.StopTimer();
    m_qTimePiece.StartTimer();

    for (int i = 0; i < m_veCConInfoFrame.size(); ++i)
    {
        if (NULL != m_veCConInfoFrame[i])
        {
            m_veCConInfoFrame[i]->Clear();
        }
    }

    //隐藏Hold遮罩
    m_HoldFrame.hide();
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgConferenceUI::Uninit()
{
    //停止计时器
    m_qTimePiece.StopTimer();

    for (int i = 0; i < m_veCConInfoFrame.size(); ++i)
    {
        if (NULL != m_veCConInfoFrame[i])
        {
            m_veCConInfoFrame[i]->Clear();
        }
    }

    CDlgConferenceBase::Uninit();
}

//给界面设置数据
void CDlgConferenceUI::SetTalkData(DataLogic2UIBase * pData)
{
    TALKUI_INFO("CDlgConferenceUI SetTalkData");

    //基类 先执行函数
    CDlgConferenceBase::SetTalkData(pData);

    //是否Hold
    bool bHold = false;

    // 清空控件的Tip
    DataLocalConf * pLocalConfData = dynamic_cast<DataLocalConf *>(pData);
    if (pLocalConfData != NULL)
    {
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
        CCallInfo * pCallInfo = GetFocusCallInfo();
        if (pCallInfo != NULL)
        {
            m_eRecordStatue = pCallInfo->m_eRecordStatus;
        }

        ShowErrorNote(pLocalConfData);
#endif

        m_bSecurity = pLocalConfData->bAllowSecurity;

        //获取Hold标志
        bHold = pLocalConfData->bHoldConf;//GetCallInfoListHoldFlag(pLocalConfData->listCallInfo, false);

        //获取联系人信息显示策略
        SHOW_CONTACTDETAIL_TYPE nDisplayMethod = (SHOW_CONTACTDETAIL_TYPE)
                _TalkUIDelegate.GetDisplayMethod();

        std::vector<bool>  vecContactCustomImage;

        bool bShowContactPhoto = false;

        if (configParser_GetConfigInt(kszDisplayContactPhoto) == 1)
        {
            bShowContactPhoto = true;
        }
        else if (configParser_GetConfigInt(kszDisplayContactPhoto) == 0)
        {
            bShowContactPhoto = false;
        }
        else if (configParser_GetConfigInt(kszDisplayContactPhoto) == 2)
        {
            //获取头像显示状态
            for (YLList<CCallInfo *>::iterator itCallInfo = pLocalConfData->listCallInfo.begin()
                    ; itCallInfo != pLocalConfData->listCallInfo.end()
                    ; ++itCallInfo)
            {
                CCallInfo * pCallInfo = (CCallInfo *)(*itCallInfo);
                if (NULL != pCallInfo)
                {
                    yl::string strContactImage = GetContactImageFromCallInfo(pCallInfo);

                    bool bIsCustomImage = IsCustomImage(strContactImage);

                    if (bIsCustomImage)
                    {
                        //只要存在一个非默认的头像，就全部显示头像
                        bShowContactPhoto = true;
                        break;
                    }
                }
            }
        }

        //根据列表设置话路信息
        int nConfFrameIndex = 0;
        for (YLList<CCallInfo *>::iterator itCallInfo = pLocalConfData->listCallInfo.begin()
                ; itCallInfo != pLocalConfData->listCallInfo.end() && nConfFrameIndex < m_veCConInfoFrame.size()
                ; ++itCallInfo, ++nConfFrameIndex)
        {
            CCallInfo * pCallInfo = (CCallInfo *)(*itCallInfo);
            if (NULL != pCallInfo && NULL != m_veCConInfoFrame[nConfFrameIndex])
            {
//                 if (!IsDisplayContactPhoto(nConfFrameIndex))
//                 {
//                     //如果不显示头像  那么加图标在逻辑就处理了
//                 }
                if (bShowContactPhoto)
                {
                    AUDIO_CODEC_TYPE eAudioCodec = pCallInfo->GetAudioCodecType();
                    m_veCConInfoFrame[nConfFrameIndex]->AddOrDeleteIcon(PIC_ICON_HD, (eAudioCodec & ACT_HD));
                    m_veCConInfoFrame[nConfFrameIndex]->AddOrDeleteIcon(PIC_ICON_AMR, (eAudioCodec & ACT_AMR));
                    m_veCConInfoFrame[nConfFrameIndex]->AddOrDeleteIcon(PIC_ICON_OPUS, (eAudioCodec & ACT_OPUS));
                }

                yl::string strName = GetNameFromCallInfo(pCallInfo);
                yl::string strNumber = GetNumberFromCallInfo(pCallInfo);
                yl::string strFullContactInfo = GetFullContactInfoFromCallInfo(pCallInfo);

                // 根据DisplayMethod转换数据
                //根据这些信息获取 需要显示的信息
                ChangeNameAndNumberByInfoMethod(strName, strNumber, strFullContactInfo, nDisplayMethod);

#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD
                if (!(m_bVoiceRecord && pCallInfo->IsAutoVoiceRecord()))
#endif
                {

                    if (pCallInfo->m_eStateOfRoutine == ROUTINE_RINGBACK
                            || pCallInfo->m_eStateOfRoutine == ROUTINE_CONNECTING)
                    {
                        yl::string strCalling = LANG_TRANSLATE(TRID_CALLING).toUtf8().data();
                        strName = strCalling + ":" + strName;
                    }

                    m_veCConInfoFrame[nConfFrameIndex]->SetContactName(strName);

                    yl::string strContactImage = GetContactImageFromCallInfo(pCallInfo);
                    m_veCConInfoFrame[nConfFrameIndex]->SetContactImage(strContactImage);

                    vecContactCustomImage.push_back(bShowContactPhoto);
                }

                m_veCConInfoFrame[nConfFrameIndex]->SetCallID(pCallInfo->GetCallID());
                //呼出则不显示farmute和Farhold
                if (pCallInfo->m_eStateOfRoutine == ROUTINE_RINGBACK
                        || pCallInfo->m_eStateOfRoutine == ROUTINE_CONNECTING)
                {
                    m_veCConInfoFrame[nConfFrameIndex]->SetFarMuteHoldShow(false);
                }
                else
                {
                    m_veCConInfoFrame[nConfFrameIndex]->SetFarMuteHoldShow(true);
                    m_veCConInfoFrame[nConfFrameIndex]->SetFarMuteStatus(pCallInfo->IsFarMute());
                    m_veCConInfoFrame[nConfFrameIndex]->SetFarHoldStatus(pCallInfo->IsFarHold());
                }
                //http://bugfree.yealink.com/Bug.php?BugID=116586
                //显示提示优先级为：FarHold、Mute、Held
                if (pCallInfo->IsFarHold())
                {
                    m_veCConInfoFrame[nConfFrameIndex]->SetTipInfo(LANG_TRANSLATE(TRID_FARHOLD), PIC_ICON_HOLD);
                }
                else if (pCallInfo->IsFarMute())
                {
                    // farMute的状态已经由前面 SetFarMuteStatus(pCallInfo->IsFarMute());
                }
                else if (pCallInfo->IsRemoteHold())
                {
                    m_veCConInfoFrame[nConfFrameIndex]->SetTipInfo(LANG_TRANSLATE(TRID_HELD), PIC_ICON_HOLD);
                }
                else
                {
                    m_veCConInfoFrame[nConfFrameIndex]->ClearTip();
                }

                m_veCConInfoFrame[nConfFrameIndex]->show();
            }
        }

        SetCustomImageMark(vecContactCustomImage);

        //http://10.2.1.199/Bug.php?BugID=96620 未有信息的联系人控件应该隐藏并且情况信息
        for (; nConfFrameIndex < m_veCConInfoFrame.size(); ++nConfFrameIndex)
        {
            m_veCConInfoFrame[nConfFrameIndex]->SetContactName("");
            m_veCConInfoFrame[nConfFrameIndex]->hide();
        }
    }

    //设置是否显示联系人头像
    for (int i = 0; i < MAX_CONF_PARTY_NUM; ++i)
    {
        if (NULL != m_veCConInfoFrame[i])
        {
            bool bIsShowContactImage = IsDisplayContactPhoto(i);
            m_veCConInfoFrame[i]->AdjustLayout(bIsShowContactImage);
        }
    }

    //是否mute
    bool bMute = talklogic_GetMuteStatus();

    //更新Hold、Mute 状态
    UpdateHoldAndMuteInfo(bHold, bMute);
}

// NetWork、Dtmf、音量条等Tip显示状态变化时的处理
void CDlgConferenceUI::OnTipChange(TALK_TIP_TYPE eTipType)
{

}

#ifdef _T48
// 调整控件布局
void CDlgConferenceUI::AdjustLayout()
{
    //设置 状态大小区域
    m_HoldFrame.setGeometry(0, 0, this->width(), this->height());
}

#endif


//获取通话时间
QCustomTime CDlgConferenceUI::GetTalkTime()
{
    UINT uHour = 0, uMinute = 0, uSecond = 0;
    m_qTimePiece.GetTime(uHour, uMinute, uSecond);
    QCustomTime timeTalk(uHour, uMinute, uSecond);
    return timeTalk;
}

//获取当前操作的CallID
int CDlgConferenceUI::GetCurrentActionCallID()
{
    //取值后 要重置
    int iCurrentActionCallID = m_iCurrentActionCallID;
    m_iCurrentActionCallID = -1 ;

    return iCurrentActionCallID;
}

#ifdef _T48

//添加或删除图标
void CDlgConferenceUI::AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd)
{
    //显示联系人头像才要继续
    for (int i = 0; i < m_veCConInfoFrame.size(); ++i)
    {
        if (NULL != m_veCConInfoFrame[i])
        {
            if (IsDisplayContactPhoto(i))
            {
                m_veCConInfoFrame[i]->AddOrDeleteIcon(strIconPath, bAdd);
            }
        }
    }
}

//删除所有图标
void CDlgConferenceUI::DeleteAllIcon()
{
    //显示联系人头像才要继续
    for (int i = 0; i < m_veCConInfoFrame.size(); ++i)
    {
        if (NULL != m_veCConInfoFrame[i])
        {
            if (IsDisplayContactPhoto(i))
            {
                m_veCConInfoFrame[i]->DeleteAllIcon();
            }
        }
    }
}

// 设置Tip信息
void CDlgConferenceUI::SetTipInfo(const QString & strTip, const QString & strPicPath)
{
    for (int i = 0; i < m_veCConInfoFrame.size(); ++i)
    {
        if (NULL != m_veCConInfoFrame[i])
        {
            m_veCConInfoFrame[i]->SetTipInfo(strTip, strPicPath);
        }
    }
}

// 清空Tip信息
void CDlgConferenceUI::ClearTip()
{
    for (int i = 0; i < m_veCConInfoFrame.size(); ++i)
    {
        if (NULL != m_veCConInfoFrame[i])
        {
            m_veCConInfoFrame[i]->ClearTip();
        }
    }

}

#endif

//弹出框回调函数
void CDlgConferenceUI::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL != pMessageBox)
    {
        switch (pMessageBox->GetID())
        {
        case MSGBOX_ID_REMOVE:
            {
                if (pMessageBox->GetResult() == MessageBox_OK)
                {
                    OnUIAction(TALK_UI_ACTION_REMOVE);
                }
            }
            break;
        case MSGBOX_ID_SECURITY:
            {
                if (pMessageBox->GetResult() == MessageBox_OK)
                {
                    int iAccount = talklogic_GetAccountLeastLevFromLocalConf();
                    //设置当前账号的安全等级
                    talklogic_ModifyAccountOverrideLevel(iAccount, fromQString(pMessageBox->GetExtraData().toString()));
                }
            }
            break;
        default:
            break;
        }
    }

}

//构造函数调用初始化数据
void CDlgConferenceUI::InitData()
{
    //隐藏联系人控件  这边使用的是会议控件不需要联系人控件
    m_cConInfoFrame.hide();

    // 隐藏计时控件
    m_qTimePiece.hide();

    // 根据不同的机型设置控件
    for (int i = 0; i < MAX_CONF_PARTY_NUM; i++)
    {
        CConferenceInfoFrame * pTempFrame = new CConferenceInfoFrame(this);
        if (NULL != pTempFrame)
        {
            pTempFrame->setObjectName(QString("TempFrame").arg(i));
            pTempFrame->setGeometry(0, 116 * i, 480, 120);
            connect(pTempFrame, SIGNAL(BtnClicked(CConferenceInfoFrame *, const QString &)), this,
                    SLOT(OnBtnClicked(CConferenceInfoFrame *, const QString &)));
            m_veCConInfoFrame.push_back(pTempFrame);
        }
    }

    //Hold遮罩
    m_HoldFrame.setObjectName("HoldFrame");
    m_HoldFrame.setParent(this);
}

// 刷新Hold、Mute的信息
void CDlgConferenceUI::UpdateHoldAndMuteInfo(bool bLocalHold, bool bMute)
{
    //本地Hold
    if (true == bLocalHold)
    {
        //显示Hold图标
        m_HoldFrame.SetTipInfo(LANG_TRANSLATE(TRID_HOLD), PIC_ICON_HOLD_MASK);
        m_HoldFrame.show();

        return ;
    }
    else if (true == bMute)
    {
        m_HoldFrame.SetTipInfo(LANG_TRANSLATE(TRID_MUTE), PIC_ICON_MUTE_MASK);
        m_HoldFrame.show();

        return ;
    }
    else    //什么状态都没有 那么直接不显示遮罩
    {
        m_HoldFrame.SetTipInfo("", "");
        m_HoldFrame.hide();
    }

}

void CDlgConferenceUI::OnBtnClicked(CConferenceInfoFrame * pInfoFrame, const QString & strBtnAction)
{
    if (NULL == pInfoFrame)
    {
        return;
    }

    //保存当前的CallID
    m_iCurrentActionCallID = pInfoFrame->GetCallID();

    TALKUI_INFO("Conference OnBtnClicked CallID[%d] Action[%s]", pInfoFrame->GetCallID(),
                strBtnAction.toUtf8().data());


    if (strBtnAction == TALK_UI_ACTION_FARMUTE)
    {
        OnUIAction(TALK_UI_ACTION_FARMUTE);

        //  SendUIRequestAction(TALK_ACTION_CONF_FAR_MUTE, pInfoFrame->GetCallID(), 1);
    }
    else if (strBtnAction == TALK_UI_ACTION_UNMUTE)
    {
        OnUIAction(TALK_UI_ACTION_UNMUTE);
    }
    else if (strBtnAction == TALK_UI_ACTION_FARHOLD)
    {
        OnUIAction(TALK_UI_ACTION_FARHOLD);
    }
    else if (strBtnAction == TALK_UI_ACTION_RESUME)
    {
        OnUIAction(TALK_UI_ACTION_RESUME);
    }
    else if (strBtnAction == TALK_UI_ACTION_REMOVE)
    {
        //弹框
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_ARE_YOU_SURE_TO_REMOVE) + pInfoFrame->GetContactName() + " ?",
                                    MESSAGEBOX_OkCancel, \
                                    -1, "", "", MSGBOX_ID_REMOVE);

    }
}

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
bool CDlgConferenceUI::ShowErrorNote(DataLocalConf * pLocalConfData)
{
    if (pLocalConfData == NULL)
    {
        return false;
    }

    bool bShowFail = true;
    CALL_RECORD_ACTION_RESULT eFailResult = CALL_RECORD_NONE;
    YLList<CCallInfo *>::iterator iter = pLocalConfData->listCallInfo.begin();
    while (iter != pLocalConfData->listCallInfo.end() && *iter)
    {
        CALL_RECORD_ACTION_RESULT eResult = (*iter)->m_eRecordResult;
        if (eResult >= CALL_RECORD_START_FAILED
                && eResult <= CALL_RECORD_STOP_FAILED)
        {
            eFailResult = eResult;
        }
        else
        {
            bShowFail = false;
            break;
        }
        ++iter;
    }

    if (bShowFail)
    {
        if (eFailResult == CALL_RECORD_START_FAILED)
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_START_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
        }
        else if (eFailResult == CALL_RECORD_STOP_FAILED)
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_STOP_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
        }
        else if (eFailResult == CALL_RECORD_PAUSE_FAILED)
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_PAUSE_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
        }
        else if (eFailResult == CALL_RECORD_RESUME_FAILED)
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_RESUME_RECORD_FAILED), MESSAGEBOX_NOTE, 2000);
        }

        //提示完之后清除状态
        YLList<CCallInfo *>::iterator iter = pLocalConfData->listCallInfo.begin();
        while (iter != pLocalConfData->listCallInfo.end() && *iter)
        {
            CALL_RECORD_ACTION_RESULT eResult = (*iter)->m_eRecordResult;
            if (eResult >= CALL_RECORD_START_FAILED
                    && eResult <= CALL_RECORD_STOP_FAILED)
            {
                (*iter)->m_eRecordResult = CALL_RECORD_NONE;
            }
            ++iter;
        }
    }
    return true;
}
#endif
