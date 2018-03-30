#include "talkuidelegate.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "account/include/modaccount.h"
#include "baseui/t4xpicpath.h"
#include "talk/talklogic/include/modtalklogic.h"

#include "talkuifactory.h"
#include "talkuiframemanager.h"
#include "mainwnd/mainwnd.h"
#include "uikernel/languagehelper.h"

#include "uicommon/qtcommon/qmisc.h"

//mod-message
#include "uicommon/messagebox/modmessagebox.h"

#include "voice/include/modvoice.h"

CTalkUIDelegate::CTalkUIDelegate(void)
{
}

CTalkUIDelegate::~CTalkUIDelegate(void)
{
}

CTalkUIDelegate & CTalkUIDelegate::GetInstance()
{
    static CTalkUIDelegate s_cTalkUIDelegate;

    return s_cTalkUIDelegate;

}

//更新通道图标
void CTalkUIDelegate::UpdateChannelIcon()
{
    _TalkUIFrameMananger.UpdateChannelIcon();
//#ifdef _T48
//
//  if(_TalkUIFactory.GetTalkUICount() > 0)
//  {
//      _MainWND.SetTitleAccountIcon(::GetChannelIcon());
//  }
//
//#else
//  //处于通话界面
//  _TalkUIFrameMananger.UpdateChannelIcon();
//
//#endif
}

//设置通道图标图片
void CTalkUIDelegate::SetChannelIcon(const yl::string & strChannelIcon)
{
#ifdef _T48

    //if(_TalkUIFactory.GetTalkUICount() > 0)
    //{
    _MainWND.SetTitleAccountIcon(toQString(strChannelIcon));
    //}

#else


#endif
}

//更新 通话左上角 title
void CTalkUIDelegate::UpdateTitleInfo(yl::string & strTtileInfo)
{
    //t48
#ifdef _T48

    //T48 只有拨号界面需要显示title信息
    if (_TalkUIFrameMananger.GetCurrentFrameType() == FRAME_TYPE_CALLMANAGER)
    {
        //在这翻译
        _MainWND.SetTitleAccountDisplayName(LANG_TRANSLATE(strTtileInfo.c_str()));
        //  _MainWND.SetTitle(LANG_TRANSLATE(strTtileInfo.c_str()));
    }

#endif

}

//更新 其他图标  静音、加密、HD、蓝牙、录音
void CTalkUIDelegate::AddTitleIcon(const yl::string & strIconPath, bool bAdd)
{
    //t48在 talkuiframe 中更新
//#ifdef _T48

    _TalkUIFrameMananger.AddTitleIcon(strIconPath, bAdd);

//#endif

}

////清除所有其他图标
//void CTalkUIDelegate::DeleteAllTitleIcon()
//{
//  //t48在 talkuiframe 中清除
//#ifdef _T48
//
//
//
//#endif
//}

// 更新蓝牙图标
void CTalkUIDelegate::RefreshBlueToothIcon()
{
    _TalkUIFrameMananger.RefreshBlueToothIcon();
}

//获取弹出框个数
int CTalkUIDelegate::GetMessageBoxCount()
{
    return MessageBox_GetMessageBoxCount();
}

//移除所有弹出框
void CTalkUIDelegate::RemoveAllMessageBox()
{
    MessageBox_RemoveAllMessageBox(MessageBox_Destory);
}

//移除对话框相关的弹出框
void CTalkUIDelegate::RemoveBaseTalkuiMessageBox(CDlgBaseTalkUI * pBaseTalkUI)
{
    if (NULL != pBaseTalkUI)
    {
        MessageBox_RemoveMessageBox(pBaseTalkUI);
    }
}

//弹出普通弹出框
void CTalkUIDelegate::ShowNoteMessageBox(const QString & strNote, int nMsTime)
{
    MessageBox_ShowCommonMsgBox(NULL, strNote, MESSAGEBOX_NOTE, nMsTime);
}

//判断该图片是否是默认图片
bool CTalkUIDelegate::IsDefaultContactPhoto(yl::string strImg)
{
    bool bResult = false;

    if (PIC_CONTACTS_IMAGE_BLUETOOTH_DEFAULT == strImg || \
            PIC_CONTACTS_DEFAULT_CONTACT_IMAGE == strImg || \
            PIC_CONTACTS_CONFERENCE == strImg || \
            PIC_CONTACTS_IMAGE_DEFAULT == strImg)
    {
        bResult = true;
    }
    else
    {
        yl::string strPhoto = strImg;

        // 隐藏路径的格式则转换为全路径
        commonUnits_Real2Hide(strPhoto, PHFILE_TYPE_CONTACT_IMAGE);

        if (strPhoto.find("Resource:", 0) != yl::string::npos
                || strPhoto.find("Default:", 0) != yl::string::npos)
        {
            bResult = true;
        }
    }

    return bResult;
}

//机型差异判断
//是否显示swap 的softkey
bool CTalkUIDelegate::IsShowSwapSoftkey()
{
    //根据lxh 意见  t48不需要显示  swap
    if (PT_T48 == devicelib_GetPhoneType())
    {
        return false;
    }

    if (PT_T49 == devicelib_GetPhoneType())
    {
        return false;
    }

    return true;

}


//获取显示策略
int CTalkUIDelegate::GetDisplayMethod()
{
    return configParser_GetConfigInt(kszDisplayMethod);
}

//在有name和number的情况下 是否优先显示name  默认是优先显示name
bool CTalkUIDelegate::IsShowNameFirst()
{
    return true;
}

//是否显示Via 中转信息
bool CTalkUIDelegate::IsShowViaInfo()
{
    return (configParser_GetConfigInt(kszDiversionEnable) != 0);
}

//是否显示TargetInfo 信息
bool CTalkUIDelegate::IsShowTargetInfo()
{
    return (configParser_GetConfigInt(kszDisplayIncomingCallInfo) != 0);
}

//获取在拨号界面优先显示的 信息  1=Label 2=DisplayName 3=UserName 默认为3
int CTalkUIDelegate::GetDisplayMethodOnDialing()
{
    return configParser_GetConfigInt(kszDisplayMethodOnDialing);
}

//是否呼出按键 播放按键音
bool CTalkUIDelegate::IsVolumeSendKeySoundOn()
{
    return (configParser_GetConfigInt(kszVolumeSendKeySoundOn) != 0);
}

//获取联系人默认头像路径
yl::string CTalkUIDelegate::GetDefaultContactImage()
{
    return PIC_CONTACTS_DEFAULT_CONTACT_IMAGE;

}

//获取通话、会议 新来电目标账号ID
int CTalkUIDelegate::GetTalkingIncomingCallTargetAccountId()
{
    //调用逻辑层 接口 获取当前来电的目标账号ID
    return talklogic_GetIncomingCallAccountId();
}

//从逻辑层获取hoteling 账号
yl::string CTalkUIDelegate::GetHotelingUserName(int iAccountID)
{
    return acc_GetCustomShowText(iAccountID);
}

//从逻辑层获取Label
yl::string CTalkUIDelegate::GetLabelFromLogic(int iAccountID)
{
    return acc_GetLabel(iAccountID);
}

//从逻辑层获取Displayname
yl::string CTalkUIDelegate::GetDisplayNameFromLogic(int iAccountID)
{
    return acc_GetDisplayName(iAccountID);
}

//从逻辑层获取UserName
yl::string CTalkUIDelegate::GetUserNameFromLogic(int iAccountID)
{
    return acc_GetUsername(iAccountID);
}

/************************************************************************
** 函数描述:   获取账号的全名 User@Domain
** 参数：      账号ID
** 返回:       服务器名
************************************************************************/
yl::string CTalkUIDelegate::GetFullName(int iAccountID)
{
    return acc_GetFullName(iAccountID);
}

// 是否*、#呼出键
int CTalkUIDelegate::GetKeyAsSend()
{
    // 0--Disable，1--#，2--*
    return configParser_GetConfigInt(kszKeyAsSend);
}

//是否是配置中的呼出键
bool CTalkUIDelegate::IsDigitkeyAsSend(int iKey)
{
    int iKeyAsSendType = GetKeyAsSend();

    //类型1 为 #呼出
    if (1 == iKeyAsSendType)
    {
        if (PHONE_KEY_POUND == iKey)
        {
            return true;
        }
    }
    else if (2 == iKeyAsSendType)
    {
        if (PHONE_KEY_STAR == iKey)
        {
            return true;
        }
    }

    return false;
}

//这个配置的意思 应该是能不能呼出## 这么理解
int CTalkUIDelegate::IsEnableSendPoundKey()
{
    return (configParser_GetConfigInt(kszSendPoundKey) != 0);
}

//在通话界面 有新来电的时候 是否改变softkey
bool CTalkUIDelegate::IsSoftkeyChangedCallIncome()
{
    //return false;
    return (configParser_GetConfigInt(kszSoftkeyChangedCallIncome) != 0);
}

//播放按键音
void CTalkUIDelegate::PlayKeyTone(int iKey)
{
    // 调用voic 接口 播放按键音
    voice_PlayKeyTone(iKey);
}

int CTalkUIDelegate::GetScrollTimer()
{
    int iScrollTimeout = configParser_GetConfigInt(kszRemoteDispScrollTimer);
    return (iScrollTimeout == 0) ? 500 : chMID(100, iScrollTimeout, 1000 * 10);
}

