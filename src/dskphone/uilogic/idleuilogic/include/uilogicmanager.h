#ifndef _UI_LOGIC_MANGER_H_
#define _UI_LOGIC_MANGER_H_

#include <singletonclassdefine.h>
#include "idlescreen/src/idlescreen_inc.h"
#include "baseidleui.h"
#include "features-define.h"

#define IDLE_KEY_TEST
//popup状态回调
void PopUpCallBack(LIST_IDLE_STATUS_ITEM& Statuslist);
//softkey 状态回调
void SoftkeyCallBack(LIST_IDLE_STATUS_ITEM& Statuslist);
//Icon状态回调
void IconCallBack(LIST_IDLE_STATUS_ITEM& Statuslist);
#if IF_SUPPORT_TEXT_NOTIFY
//Text状态回调
void TextCallBack(LIST_IDLE_STATUS_ITEM& Statuslist);
#endif
//账号状态回调
void AccountCallBack(LIST_IDLE_STATUS_ITEM& Statuslist);
//XmlBrowwer回调
void XmlBrowswerCallBack();

class CUiLogicManager
{
    DECLARE_SINGLETON_CLASS(CUiLogicManager)

public:
    //注册回调函数
    void Init(CBaseIdleUI * pIdleUi);
    //注册idle消息
    void RegeisterCallBack();
    //处理MessageBox的softkey按键处理
    bool OnMessageKeyAction(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode, int iMsgBoxId);
    //处理Idle softkey按键处理
    bool OnSoftkeyAction(SoftKey_TYPE eSoftkey);
    //将部分softkey转换逻辑按键
    bool TransSoftkeyToKeyCode(SoftKey_TYPE eSoftKey, PHONE_KEY_CODE & eKeyCode);
    bool OnChannelKeyPress(PHONE_KEY_CODE eKeyCode);
    bool IsChannelKey(PHONE_KEY_CODE eKeyCode);
    //idle ShortCut响应(不可配置成programbelkey的按键响应)
    bool ProcessShortCut(PHONE_KEY_CODE eKeyCode);
    //逻辑按键按下处理
    bool OnKeyPress(PHONE_KEY_CODE ekeyCode);
    //逻辑按键长按按下处理
    bool OnKeyLongPress(PHONE_KEY_CODE ekeyCode);
    //逻辑按键释放处理
    bool OnKeyRelease(PHONE_KEY_CODE ekeycode);
    //Idle是否处在空闲状态
    bool IsIdleFreeStatus();
    //弹出框回调
    bool IdleLogicPopupCallBack(LIST_IDLE_STATUS_ITEM& Statuslist);
    //softkey 回调
    bool IdleLogicSoftkeyCallBack(LIST_IDLE_STATUS_ITEM& Statuslist);
    //Icon回调
    bool IdleLogicIconCallBack(LIST_IDLE_STATUS_ITEM& Statuslist);
#if IF_SUPPORT_TEXT_NOTIFY
    //Text回调
    bool IdleLogicTextCallBack(LIST_IDLE_STATUS_ITEM& Statuslist);
#endif
    //账号回调
    bool IdleLogicAccountCallBack(LIST_IDLE_STATUS_ITEM& Statuslist);
    //_XmlBrowswer 回调
    bool IdleLogicUpdateCustomNotify();
    // 更新帐号信息
    bool UpdateAccountInfo(const yl::string & strName, ICON_TYPE eIcon);
    // CreateMessagebox
    bool CreateMessageBox(const PopupBoxInfo & popData, bool bOnlyInIdle = true);
    // RemoveMessagebox
    bool RemoveMessagebox(int nID);
    // ResetKeypadLock
    void ResetKeypadLock();
    // StopKeypadLock
    void StopKeypadLockTimer();
    // 更新idle的状态信息
    void UpdateStatus();
    // 状态消息处理
    static LRESULT OnStatusMessage(msgObject & msg);
    // 根据页面类型进入界面
    void EnterPageByType(IDLE_ENTER_PAGE_TYPE_E eEnterPageType);
    // 根据按键获取dsskeyID
    int GetDsskeyIDByKeyCode(int nKey);
    // 获取ACD的队列信息内容
    bool GetACDQueueStatus(ACD_STATUS eAcdStatus, yl::string& strQueue);
#if IF_SUPPORT_CENTER_ACD_STATUS
    void UpdateCenterNote(const yl::string& strNote);
#endif
#if IF_SUPPORT_EXTRA_IDLE_NOTE
    void UpdateExtraNote(const yl::string& strNote);
#endif
private:
    //获取XmlBrowser数据
    void GetXmlBrowser(VEC_CUSTOM_NOTIFY& CustomList);
    bool OnProgramKeyProcess(PHONE_KEY_CODE eKeyCode);
    bool OnShortKeyRelease(PHONE_KEY_CODE eKeyCode);
    bool PopOKCancelMessageBox(int nMesgBoxID, const yl::string& strTitle,
                               const yl::string& strNote, int nTime = 0);
    void SetLongPressTimeByKey(int nKey, int nTime);
    void ResetKeyLongPressTime(int nKey);
    bool IsExistMessageBox();
public:
    CBaseIdleUI  *  m_pIdleUi;               // idleui
private:
    unsigned int    m_uIdleStatus;           // IdleStatus 标识
    int             m_nPressedKey;           // 按下的按键
};

#define _UiIdleLogicManager GET_SINGLETON_INSTANCE(CUiLogicManager)

#endif
