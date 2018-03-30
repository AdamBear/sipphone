#include "exp40msgprocess.h"

#ifdef _EXP_BLACK_SUPPORT
CExp40MsgProcess::CExp40MsgProcess()
{

}

CExp40MsgProcess::~CExp40MsgProcess()
{

}

void CExp40MsgProcess::KeyProcess(int iKeyStatus, msgObject & msg)
{

    DsskeyID dssId = msg.wParam;

    DEVICE_TYPE_DEF expType = static_cast<DEVICE_TYPE_DEF>(msg.lParam);

    ExpID expId = GetExpIndexFromDssKeyID(dssId);

    DEV_MESSAGE_CODE msgContent = GetMsgFromKeyStatus(iKeyStatus);

    // 根据按键状态 设置或者关闭定时器
    AutoSetKillLongPressTimer(iKeyStatus);

    // 是否为测试模式按键
    if (IsTestMode())
    {
        if (IsSameKeyUp(dssId, iKeyStatus))
        {
            PostMsgToApp(msgContent, dssId, iKeyStatus);
        }
        // 记录下按下的键，如果是抬起则清除记录
        RegisterKeyDown(dssId, iKeyStatus);
        return;
    }

    /**************************************翻页键例程BEGIN********************************/

    // 如果是翻页键按下，直接发消息给绘制线程，无需设置长按定时器
    // 翻页键抬起直接return
    int iPageIndex = GetPageIndex(dssId);

    if (iPageIndex != -1)
    {
        if (iKeyStatus == EXP_KEY_STATUS_PRESS
                && GetCurrentExpPage(expId) != iPageIndex)
        {
            SetCurrentExpPage(expId, iPageIndex);
            // 不直接发送给绘制线程，先发送给GUI，处理一些业务，再转发给绘制线程
            PostMsgToApp(EXP_MESSAGE_SWITCH_PAGE, expId, iPageIndex);
        }

        return;
    }

    /************************************翻页键例程END************************************/

    // 是否为刚才按下的键抬起
    if (IsSameKeyUp(dssId, iKeyStatus))
    {

        PostMsgToApp(msgContent, AddPageInfoForDsskeyID(dssId), iKeyStatus);
    }
    // 长按之后的弹起消息单独发送给MetaSwitch
    if (IsKeyReleaseAfterLongPress(iKeyStatus))
    {
        PostMsgToApp(DEV_MSG_EXP_KEY_AFTER_LONG_PRESS_RELEASE, AddPageInfoForDsskeyID(dssId), iKeyStatus);
        EXP_INFO("[EXP-INFO]:DEV_MSG_EXP_KEY_AFTER_LONG_PRESS_RELEASE post to gui");
        m_bKeyReleaseAfterLongPress = false;
    }
    // 记录下按下的键，如果是抬起则清除记录
    RegisterKeyDown(dssId, iKeyStatus);
}

int CExp40MsgProcess::GetPageIndex(DsskeyID dssId)
{

    int iPageIndex = -1;

    int iKeyCode = GetKeyCodeFromDsskeyID(dssId);

    switch (iKeyCode)
    {
    case EXP_KEY_PAGE1:
        iPageIndex = 0;
        break;
    case EXP_KEY_PAGE2:
        iPageIndex = 1;
        break;
    case EXP_KEY_PAGE3:
        iPageIndex = 2;
        break;
    default:
        break;
    }
    return iPageIndex;
}

#endif