///////////////talkui/////////////////////
#include "talkuicommonfunction.h"
#include "modtalkui.h"
#include "talkuipath.h"
//////////////talkuilogic//////////////////
#include "talkuilogic/talkuilogiccommon.h"
#include "talkuilogic/talkuilogicsoftkey.h"
#include "talkuilogic/talkuilogicprocessorname.h"

///////////////xwindow////////////////
#include "xwindow/xWindow.h"
#include "xwindow/xWindowManager.h"

#include "contacts/directory/include/directorystructure.h"

namespace talkui
{

//根据processor名称获取对话框的名称
yl::string GetDlgNameFromProcessorName(yl::string strProcessorName)
{
    if (PROCESSORNAME_ACDCALLINFO == strProcessorName)
    {
        return DLG_ACDCallInfoUI;
    }
    else if (PROCESSORNAME_AUTOREDIAL == strProcessorName)
    {
        return DLG_AutoRedialUI;
    }
    else if (PROCESSORNAME_CONF == strProcessorName)
    {
        return DLG_ConfUI;
    }
    else if (PROCESSORNAME_CONFMANAGER == strProcessorName)
    {
        return DLG_ConfManagerUI;
    }
    else if (PROCESSORNAME_CONNECT == strProcessorName)
    {
        return DLG_ConnectUI;
    }
    else if (PROCESSORNAME_DIAL == strProcessorName)
    {
        return DLG_DialingUI;
    }
    else if (PROCESSORNAME_FINISH == strProcessorName)
    {
        return DLG_FinishUI;
    }
    else if (PROCESSORNAME_PARKNOTIFY == strProcessorName)
    {
        return DLG_ParkNotifyUI;
    }
    else if (PROCESSORNAME_RING == strProcessorName)
    {
        return DLG_RingUI;
    }
    else if (PROCESSORNAME_RTPSTATUS == strProcessorName)
    {
        return DLG_RTPStatusUI;
    }
    else if (PROCESSORNAME_TALKING == strProcessorName)
    {
        return DLG_TalkingUI;
    }

    return "";

}
//chString和ylstring转换函数
//  yl::string fromchString(const chConstStringA& other)
//  {
//      return other.c_str();
//  }

//  chConstStringA tochString(const yl::string& other)
//  {
//      return other.c_str();
//  }

//根据路径获取Pixmap
xPixmap GetPixmapByString(const yl::string & strPath)
{
    xPixmap pixMap;
    g_WorkStation.GetImageByPath(strPath, pixMap);
    return pixMap;
}

//根据通道类型获取通道图标
yl::string GetChannelIconPicByChannelIconType(TALKUI_CHANNEL_ICON eTalkUIChannelIcon)
{
    switch (eTalkUIChannelIcon)
    {
    //免提图标
    case TALKUI_CHANNEL_ICON_HANDFREE:
        {
            //免提
            return PIC_TALKUI_ICON_TAB_CHANNEL_HANDFREE;
        }
        break;
    //手柄
    case TALKUI_CHANNEL_ICON_HANDSET:
        {
            //手柄
            return PIC_TALKUI_ICON_TAB_CHANNEL_HANDSET;
        }
        break;
    //耳麦
    case TALKUI_CHANNEL_ICON_HEADSET:
        {
            //耳麦
            return PIC_TALKUI_ICON_TAB_CHANNEL_HEADSET;
        }
        break;
    default:
        {
            return "";
        }
        break;
    }

    return "";
}

LCD_ICON_TYPE GetChannelIconTypeByChannelIconType(TALKUI_CHANNEL_ICON eTalkUIChannelIcon)
{
    switch (eTalkUIChannelIcon)
    {
    //免提图标
    case TALKUI_CHANNEL_ICON_HANDFREE:
        {
            //免提
            return LCD_ICON_HF;
        }
        break;
    //手柄
    case TALKUI_CHANNEL_ICON_HANDSET:
        {
            //手柄
            return LCD_ICON_HS;
        }
        break;
    //耳麦
    case TALKUI_CHANNEL_ICON_HEADSET:
        {
            //耳麦
            return LCD_ICON_HEADS;
        }
        break;
    default:
        {
            return LCD_ICON_LAST;
        }
        break;
    }

    return LCD_ICON_LAST;
}

//根据图标类型获取 图片路径
yl::string GetIconPathByIconType(TALKUI_ICON iconType)
{
    switch (iconType)
    {
    case TALKUI_ICON_SIPENCRY:
        {
            return PIC_TALK_ICON_ENCRY;
        }
        break;
    case TALKUI_ICON_VOICEENCRY:
        {
            return PIC_TALK_ICON_ENCRY;
        }
        break;
    case TALKUI_ICON_SILENCE:
        {
            return PIC_TALK_ICON_SILENCE;
        }
        break;
    case TALKUI_ICON_HD:
        {
            return PIC_TALK_ICON_HD;
        }
        break;
    case TALKUI_ICON_AMR:
        {
            return PIC_TALK_ICON_AMR;
        }
        break;
    case TALKUI_ICON_OPUS:
        {
            return PIC_TALK_ICON_OPUS;
        }
        break;
    case TALKUI_ICON_RECORD:
        {
            return PIC_ICON_RECORD;
        }
        break;
    case TALKUI_ICON_CANNOT_BE_RECORDED:
        {
            return PIC_CANNOT_RECORD;
        }
        break;
    case TALKUI_ICON_CANNOT_BE_STOPPED_RECORD:
        {
            return PIC_CANNOT_STOPPED_RECORD;
        }
        break;
    case TALKUI_ICON_CANNOT_BE_STARTED_RECORD:
        {
            return PIC_CANNOT_STARTED_RECORD;
        }
        break;
    case TALKUI_ICON_RECORD_FULL:
        {
            return PIC_RECORD_FULL;
        }
        break;

    case TALKUI_ICON_CALL_RECORD_NORMAL:
        {
            return PIC_RECORD_NORMAL;
        }
        break;
    case TALKUI_ICON_CALL_RECORD_PAUSE:
        {
            return PIC_RECORD_PAUSE;
        }
        break;
    case TALKUI_ICON_DND:
        {
            return PIC_IDLE_DND;
        }
        break;

    default:
        {

        }
        break;

    }

    return "";

}

//根据蓝牙类型 获取蓝牙图标路径
yl::string GetBlueToothIconPathByBlueToothIconType(TALKUI_BLUETOOTH_ICON eTalkUIBlueToothIcon)
{
    switch (eTalkUIBlueToothIcon)
    {
    //未知类型
    case TALKUI_BLUETOOTH_ICON_UNKNOWN:
        {
            return "";
        }
        break;
    //蓝牙未连接
    case TALKUI_BLUETOOTH_ICON_UNCONNECTED:
        {
            return PIC_TALK_ICON_BLUETOOTH_UNCONNECTED;
        }
        break;
    //蓝牙手机
    case TALKUI_BLUETOOTH_ICON_MOBILE:
        {
            return PIC_TALK_ICON_BLUETOOTH_MOBILE;
        }
        break;
    //蓝牙耳机
    case TALKUI_BLUETOOTH_ICON_HEADSET:
        {
            return PIC_TALK_ICON_BLUETOOTH_HEADSET;
        }
        break;
    default:
        {
            return "";
        }
        break;
    }

    return "";
}

//根据softkeytype 获取 softkeybardata
void GetSoftkeyBarDataFromSoftKeyType(CSoftKeyBarData & softkeyBarData, SoftKey_TYPE eSoftkeyType)
{
    //设置softkeyaction
    softkeyBarData.m_iType = (int)eSoftkeyType;
    //设置softkeytitle
    softkeyBarData.m_strSoftkeyTitle = _UILOGIC_LANG_TRAN(GetSoftkeyTitleBySoftkeyType(
                                           eSoftkeyType).c_str());
}

bool ExistLocalDirectory(YLList<DirShortcutData> & listData)
{
    YLList<DirShortcutData>::iterator iter = listData.begin();

    for (; iter != listData.end(); ++iter)
    {
        DirShortcutData objDirShortcutData = static_cast<DirShortcutData>(*iter);

        if (POOL_ACTION_LOCAL_CONTACT == objDirShortcutData.m_strIdName)
        {
            return true;
        }
    }

    return false;
}

void ProcessNormalOpenPool()
{
#if IF_BUG_28455
    yl::string strXmlUrl = configParser_GetConfigString(kszXmlPhonebookUrl);
    if (strXmlUrl.empty())
    {
        talkui_OpenPool(false);
    }
    else
    {
        xmlbrowser_Enter(strXmlUrl.c_str());
    }
#else
    talkui_OpenPool(false);
#endif
}

void ChangeNameAndNumberBySecurityLevel(yl::string & strName, yl::string & strNumber,
                                        const yl::string strSecurityLev, bool bInsertFirstLine/* = false*/)
{
    if (strSecurityLev.empty())
    {
        return;
    }

    // 需要加在第一行的情况
    if (bInsertFirstLine)
    {
        if (!strName.empty())
        {
            strName = strSecurityLev + "-" + strName;
        }
        else
        {
            strName = strSecurityLev;
        }
    }
    else
    {
        // 多行显示的机型, 优先保证在第二行；
        // 本来第二行有显示信息，就插在它前面，中间加一个“-”；
        // 本来第二行没显示，就直接显示安全级别，也没有“-”；
        if (strName != strNumber && !strNumber.empty())
        {
            strNumber = strSecurityLev + "-" + strNumber;
        }
        else
        {
            strNumber = strSecurityLev;
        }
    }
}

////根据softkeytype 获取 softkeybardata
//void GetSoftkeyBarDataFromSoftKeyType(CSoftKeyBarData& softkeyBarData, SoftKey_TYPE eSoftkeyType)
//{
//  //设置softkeyaction
//  softkeyBarData.m_iType = (int)eSoftkeyType;
//  //设置softkeytitle
//  softkeyBarData.m_strSoftkeyTitle = _UILOGIC_LANG_TRAN(GetSoftkeyTitleBySoftkeyType(eSoftkeyType).c_str());

//}

////xwindow 使用 根据控件、窗口名称 获取mostleve
//BYTE GetTopMostLevelByName(const yl::string& strWidgetName)
//{
//  return TML_DEFAULT + 1;
//}

}       //namespace talkui
