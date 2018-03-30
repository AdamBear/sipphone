#ifndef _BASE_IDLE_HANDLE_H_
#define _BASE_IDLE_HANDLE_H_

#include "idlehandledefine.h"
#include "keymap.h"
#include "uilogichelper.h"

int UpdateNotify(char * strId, PhoneState estatus);
// 在某个位置插入Softkey，原来的SoftKey移到最后
bool SwapSoftKeyToEnd(VEC_SOFTKET_TYPE& vecSoftkey, int iIndex, SoftKey_TYPE eSoftKey);

class CBaseIdleHandle
{
public:
public:
    CBaseIdleHandle(int nType = PS_STATE_NONE);
    virtual ~CBaseIdleHandle();
    // 返回类型
    int GetType()
    {
        return m_iType;
    }
    // 设置类型
    bool SetType(int iType)
    {
        m_iType = iType;
        return true;
    }
    // 设置提示文本
    void SetHint(const yl::string strHint);
public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData)
    {
        return false;
    }
    // 获取按键信息
    virtual bool GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey);
    // 获取显示的图标信息
    virtual bool GetIconInfo(IdleIconInfo & IconData);
    // 获取显示的提示文本信息
    virtual bool GetTextNotifyData(ST_TextNotifyData& stNotifyData)
    {
        return false;
    }
    // 获取自定义显示信息内容
    virtual bool GetCustomNotifyInfo(VEC_CUSTOM_NOTIFY& CumtomList)
    {
        return false;
    }
    // 返回数据信息
    virtual bool GetAccountInfo(yl::string & strDisplayName, ICON_TYPE & eIcon)
    {
        return false;
    }
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
protected:
    unsigned int  GetNeedClearNotifyType();//获取状态需要清除标志
    bool  TransSoftkeyToKeyCode(SoftKey_TYPE eSoftKey, PHONE_KEY_CODE & eKeyCode);
protected:
    int                 m_iType;
    yl::string          m_strHint;
};

#endif //end of _BASE_IDLE_HANDLE_H_

