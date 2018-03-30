/***********************************************
*** 说明
***     这部分接口是在idleuilogic中使用接口，所有
*** IDLEUI都需要继承这个基类
************************************************/
#ifndef _BASEIDLEUI_H_
#define _BASEIDLEUI_H_
#include <ylvector.h>
#include <ylstring.h>
//#include "talk/talklogic/include/uiandlogic_common.h"
#include "common/common_data_define.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "idle/idle_data_define.h"
#include "idlehandledefine.h"

enum SoftKey_TYPE;
struct CustomNotify;
struct PopupBoxInfo;
struct StatusItem;
class CDateAndTimeInfo;

//class msgObject;
class CBaseIdleUI
{
public:
    CBaseIdleUI() {}
    virtual ~CBaseIdleUI() {}
public:
    //更新softkey
    virtual bool UpdateSoftkey(VEC_SOFTKET_TYPE& VecSoftkey)
    {
        return true;
    };
#if IF_SUPPORT_TEXT_NOTIFY
    //更新文本通知
    virtual bool UpdatePromptText(const ST_TextNotifyData& stNotifyText)
    {
        return true;
    };
#endif
    //更新Icon图标显示
    virtual bool UpdateIcon(const VEC_ICON_INFO& iconList)
    {
        return true;
    };
    //更新帐号信息
    virtual bool UpdateAccountInfo(const yl::string & strName, ICON_TYPE eIcon)
    {
        return true;
    };
    //UI配置处理(不同UI可能有专门要处理消息（彩屏有更换壁纸等类似消息），但消息入口都在UIlogic中调用)
    virtual LRESULT OnConfigChangeMsg(msgObject & msg)
    {
        return FALSE;
    };
    //IdleUI空闲状态变化通知
    virtual bool IdleFreeStatusChangeNotify(bool bIsFreeStatus)
    {
        return false;
    };
    //Idle弹窗
    virtual bool CreateMessageBox(const PopupBoxInfo & popData)
    {
        return true;
    };
    //清除Idle弹窗
    virtual bool RemoveIdleMessageBox(int nID)
    {
        return true;
    };
    virtual void EnterPasswordCheckPage() {}
    //更新自定义显示内容
    virtual bool UpdateCustomNotify(const VEC_STATUS_ITEM& refCusNotify, bool bCusNotify)
    {
        return true;
    }
    virtual void SetLongPressTimeByKey(int nKey, int nTime) {}
    virtual void ResetLongPressTimeByKey(int nKey) {}
    virtual bool IsExistMessageBox()
    {
        return false;
    }
    virtual int GetDsskeyIDByKeyCode(int nKey)
    {
        return -1;
    }
    virtual void EnterPageByType(IDLE_ENTER_PAGE_TYPE_E eEnterPageType) {}
#if IS_COLOR
    virtual void UpdateCenterNote(const yl::string& strNote) {}
#endif
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    virtual void UpdateExtraNote(const yl::string& strNote) {}
#endif
};
#endif

