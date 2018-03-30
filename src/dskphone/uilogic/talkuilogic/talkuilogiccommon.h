#ifndef _TALK_UILOGIC_COMMON_H_
#define _TALK_UILOGIC_COMMON_H_

#include "../uilogiccommon/include/uilogiclanghelper.h"

#include <ETLLib.hpp>
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talklogic/include/callinfo.h"
#include <ylvector.h>
#include "talk/talkadapter/include/modtalkadapter.h"

#include "talkuilogic/talkuilogiccondition.h"
#include "talkuilogicactiondefine.h"
#include "talkuilogicprocessorname.h"
#include "talkuilogicenum.h"

#ifndef uint
typedef unsigned int uint;
#endif

enum TALK_RECORD_STATUS
{
    TALK_RECORD_STATUS_NONE = -1,
    TALK_RECORD_STATUS_START = 0,
    TALK_RECORD_STATUS_PAUSE,
    TALK_RECORD_STATUS_STOP,
};

enum TALK_NETWORK_STATUS
{
    TALK_NETWORK_STATUS_UNKNOWN,
    TALK_NETWORK_STATUS_READY,
    TALK_NETWORK_STATUS_FAIL,

#if IF_TALKUI_PSTN
    TALK_NETWORK_STATUS_PSTN_FAIL,
#endif
};

struct LINE_SELECT_DATA
{
    yl::string m_strName;
    yl::string m_strContent;
    int m_nAccount;
};
typedef talkui_vector<LINE_SELECT_DATA> vecLineSelectData;

enum LINE_SELECT_RESULT
{
    LINE_SELECT_RESULT_SELECT,
    LINE_SELECT_RESULT_CANCEL,
};

//联系人信息
struct CContactInfo
{
    yl::string strName;                     //联系人名称
    yl::string strNumber;                   //联系人号码
    yl::string strFullContactInfo;          //联系人完整的信息
    yl::string strContactImage;             //联系人头像
    //yl::string strDepartment;             //部门
    //yl::string strPosition;                   //职位

    yl::string strSecurityLevel;                //安全等级

    SHOW_CONTACTDETAIL_TYPE nDisplayMethod; //通话信息显示方式

    int iCallID;                            //通话ID
    int iSipAccount;                        //通话所使用的账号ID
    bool bLocalHold;                        //本方hold状态
    bool bFarHold;                        //远端hold状态
    bool bRemoteHold;                       //对方hold状态
    bool bVideoMute;                        //视频 静音
    bool bAudioMute;                        //语音静音
    bool bFarMute;                          //远端静音
    AUDIO_CODEC_TYPE m_eAudioCodec;  //高清通话类型
    bool bVideoTalk;                            //是否是视频通话

    CALL_LINE_TYPE  m_eCallLineType;        //线路类型（sip、pstn、deviceline）

    bool m_bReceived;               //是否已经处于接起的状态

    CContactInfo()
    {
        Reset();
    }

    void Reset()
    {
        strName = "";                       //联系人名称
        strNumber = "";                     //联系人号码
        strFullContactInfo = "";            //联系人完整的信息
        strContactImage = "";               //联系人头像
        //strDepartment = "";                   //部门
        //strPosition = "";                 //职位

        strSecurityLevel = "";

        nDisplayMethod =
            SHOW_NAME_AND_NUMBER;  //通话信息显示方式  默认是名称和号码都显示

        iCallID = 0;                            //通话ID
        iSipAccount = -1;                       //通话所使用的账号ID
        bLocalHold = false;                     //本方hold状态
        bFarHold = false;                       //远端hold状态
        bRemoteHold = false;                        //对方hold状态
        bVideoMute = false;                     //视频 静音
        bAudioMute = false;                     //语音静音
        bFarMute = false;                           //远端静音
        m_eAudioCodec = ACT_NONE;     //高清通话类型
        bVideoTalk = false;                         //是否是视频通话

        m_eCallLineType = CALL_LINE_SIP;

        m_bReceived = true;
    }


    CContactInfo & operator=(const CContactInfo & data)
    {
        strName = data.strName;
        strNumber = data.strNumber;
        strFullContactInfo = data.strFullContactInfo;
        strContactImage = data.strContactImage;
        nDisplayMethod = data.nDisplayMethod;

        iCallID = data.iCallID;
        iSipAccount = data.iSipAccount;
        bLocalHold = data.bLocalHold;
        bFarHold = data.bFarHold;
        bRemoteHold = data.bRemoteHold;
        bVideoMute = data.bVideoMute;
        bAudioMute = data.bAudioMute;
        bFarMute = data.bFarMute;
        m_eAudioCodec = data.m_eAudioCodec;
        bVideoTalk = data.bVideoTalk;

        m_eCallLineType = data.m_eCallLineType;

        m_bReceived = data.m_bReceived;

        return *this;
    }
};

//比较两个联系人信息是否相同
bool IsContactInfoEqual(const CContactInfo & objContactInfo1, const CContactInfo & objContactInfo2);


//UILogic  给  通话UI 的数据结构体
struct CUILogicToTalkUIData
{
    int iSessionID;
    SESSION_STATE sessionState;
    // 更新区域
    UPDATE_REASON eUpateReason;
    int iSessionCount;
    int iSessionIndex;

    CUILogicToTalkUIData()
    {
        Reset();
    }

    void Reset()
    {
        iSessionID = -1;
        sessionState = SESSION_UNKNOWN;
        eUpateReason = UPDATE_DEFAULT;
        iSessionCount = 0;
        iSessionIndex = -1;
    }
};

//dailuilogic to ui
struct CDialUILogicToDialUI
{
    yl::string strTile;
    yl::string strPreContent;

    CDialUILogicToDialUI()
    {
        Reset();
    }

    void Reset()
    {
        strTile = "";
        strPreContent = "";
    }
};

//int数据 转化为 string数据
yl::string IntToString(int iIn);

/*
根据逻辑层上传的数据获取使用的processor的名称
newDataLogic2UI保存新的数据这个数据用来更新流程中赋值
*/
yl::string GetProcessorNameFromData(DataLogic2UI * pDataLogic2UI, DataLogic2UI & newDataLogic2UI);

//获取Hold标志!!!
bool GetCallInfoListHoldFlag(YLList<CCallInfo *> & listCallInfo, bool bAllFlag = true);

//获取Held标志!!!
bool GetCallInfoListHeldFlag(YLList<CCallInfo *> & listCallInfo, bool bAllFlag/* = true */);

//SessionID 是否有效
bool IsSessionIDValid(int iSessionID);

//判断是否通道按键
bool talkuiIsChannelKey(int iKey);

//判断是否 数字按键
bool talkuiIsDigitKey(int iKey);

//根据按键值  获取字符
yl::string GetDtmfStringByKeyCode(int iKey);

yl::string TransTalkText(yl::string & strText);


// 根据callinfo获取显示号码
yl::string GetNameFromCallInfo(CCallInfo * pCallInfo);
// 根据callinfo获取显示名称
yl::string GetNumberFromCallInfo(CCallInfo * pCallInfo);
//根据callinfo获取fullcontactinfo
yl::string GetFullContactInfoFromCallInfo(CCallInfo * pCallInfo, bool bLocalInfo = false);

//根据CCallInfo获取联系人信息
void GetContactInfoFromCallInfo(CContactInfo & objContactInfo, CCallInfo * pCallInfo);


// 名字号码显示方式
// 配置中有几种显示方式 0.只显示名称 1.只显示号码 2.名称+号码都显示 3.号码+名称显示 4. 显示FullContactInfo
// 1. 只有strName有值 存放的是名称
// 2. 只有strName有值 存放的是号码
// 3. strName 和 strNumber 都有值  strName存放的是名称 strNumber存放的是号码
// 4. strName 和 strNumber 都有值  strName存放的是号码 strNumber 存放的是名称
// 5. strName有值 存放的是 FullInfo
//第四个参数 是显示联系人详情策略
// 第五个参数是 联系人姓名和号码相同时 是否隐藏号码  默认是隐藏的
//这个函数 是用于 两个控件显示 联系人详情的时候 使用的
//最后 strName 对应联系人姓名控件   strNumber 对应于联系人号码控件
void ChangeNameAndNumberByInfoMethod(yl::string & strName
                                     , yl::string & strNumber
                                     , yl::string & strFullContactInfo
                                     , SHOW_CONTACTDETAIL_TYPE showContactDetailType
                                     , bool bIsMergerNameAndNumber = true
                                     , bool bNameNumberInOneLine = false);

// http://10.2.1.199/Bug.php?BugID=77252
//根据显示模式获取名称
yl::string GetNameByDisplayModeAndCallInfo(CCallInfo * pCallInfo);

//输入的SessionID 中是否含有该callID
bool IsSessionHaveCallID(int iSessionID, int iCallID);

#endif

