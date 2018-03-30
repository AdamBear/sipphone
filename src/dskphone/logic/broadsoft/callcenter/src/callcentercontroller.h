#ifndef __CALLCENTER_CONTROLLER_H__
#define __CALLCENTER_CONTROLLER_H__

#if defined(IF_FEATURE_ACD) || defined(IF_FEATURE_BROADSOFT_CALL_CENTER) || defined(IF_FEATURE_GENESYS_ACD) || IF_FEATURE_METASWITCH_ACD

#include "cccommon.h"
#include <ETLLib.hpp>
//struct msgObject;
class CACDModelBase;

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
class HotelingModel;
#endif

class CallCenterController: public CMKProcessor
{
public:
    static CallCenterController * GetInstance() DECLARE_ATTR;

    void RegistMsg();

    //ACD和Hoteling登陆接口
    bool Login(ACDLogin & refData);

//  //请求ACD状态改变，单独登陆接口
//  bool ReqChangeACDStatus(ACD_ACTION eACDAction);
//
//  //Hoteling 单独登陆登出接口
//  bool ReqChangeHotelingStatus(hoteling_Request& refData);

    // 处理外部消息，ACD状态改变消息，Hoteling状态改变消息，账号状态改变消息
    void ProcessMessage(msgObject & objMessage);

    //ACD功能是否开启
    bool IsACDOn();

    CACDModelBase * GetAcdModel() const
    {
        return m_pAcdModel;
    }

    //ACD和Hoteling登出接口
    bool Logout();

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

    // 获取Hoteling状态
    HOTELING_STATUS GetHotelingStatus();

    //Hoteling功能是否开启
    bool IsHotelingOn();

    bool IsFlexibleSeatingEnable(int iAccountId = -1);

    //功能是否开启接口
    bool IsTraceOn();

    //功能是否开启接口
    bool IsDispCodeOn();

    //功能是否开启接口
    bool IsEmergencyEscalationOn();

    //CallInfo是否开启
    bool IsCallInfoOn();

    //队列状态灯是否开启
    bool IsEnableQueueStatusLightOn();

    //队列状态信息功能是否开启，开启登陆acd时订阅，否则不订阅
    bool IsQueueStatusOn();

    //获取CallInfo显示时间
    int GetCallInfoShowTime();

    HotelingModel * GetHotelingModel() const
    {
        return m_pHotelingModel;
    }

    void OnConfigChange(msgObject & objMessage);

#if IF_BUG_25622
    // 处理sip返回的订阅结果
    void ProcessSubscibeResult(bool bHotelingOn, int nAccountId);
#endif

#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER

#if IF_FEATURE_METASWITCH_ACD
    // metaswitch acd是否可用
    bool IsMTSWACDEnable();
#endif
private:
    CallCenterController(void);
    virtual ~CallCenterController(void);

    // 处理ACD计时器到时消息.
    BOOL OnACDTimeoutMessage(msgObject & refObj);

    // 处理计时器到时消息.
    BOOL OnTimeoutMessage(msgObject & refObj);
    // 处理Hotel登陆等待超时定时器到时消息
    BOOL OnTimeoutHotelMessage(msgObject & refObj);

    void MyInit();

    void MyFinal();

    // 根据账号确认ACD
    void SetACDModelByAccID(int iAccID);

    // 获取登陆名称
    yl::string  GetLoginName();
private:
    static CallCenterController * m_pInst; // 单键实例.

    CACDModelBase * m_pAcdModel; //Acd Model指针

    mkThreadTimer m_ACDLoginTimer;  //ACD登录计时器
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

    HotelingModel * m_pHotelingModel; //Hoteling Model指针

    mkThreadTimer m_hotelingTimer;  // hoteling请求计时器.
    mkThreadTimer m_hotelwaitTimer; // hoteling登陆成功后等待二次订阅定时器

    bool m_bIsLaterLoginACD;//Hoteling 登陆之后是否还需登陆ACD

    bool m_bIsLaterLogoutHoteling;//登出时先acd登出成功在登出hoteling
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER
};

#define g_CallCenterController (CallCenterController::GetInstance())
#define g_ACDModel (g_CallCenterController->GetAcdModel())

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
#define g_HotelingModel (g_CallCenterController->GetHotelingModel())
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER

#endif //IF_FEATURE_ACD || IF_FEATURE_BROADSOFT_CALL_CENTER

#endif
