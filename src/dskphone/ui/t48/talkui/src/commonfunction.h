#ifndef __COMMON_FUNCTION_H__
#define __COMMON_FUNCTION_H__

#include <yllist.h>
#include "talk/talklogic/include/uiandlogic_common.h"
#include "callmanager/callmanagerstruct.h"
#include "talk/talkadapter/include/modtalkadapter.h"

class CCallInfo;

//获取Hold标志!!!
bool GetCallInfoListHoldFlag(YLList<CCallInfo *> & listCallInfo, bool bAllFlag = true);

//获取Held标志!!!
bool GetCallInfoListHeldFlag(YLList<CCallInfo *> & listCallInfo, bool bAllFlag/* = true */);

QString GetChannelIcon();

const char * GetDlgNameBySessionState(SESSION_STATE eSessionState);

const yl::string GetDlgNameAndCheckLogicData(const DataLogic2UI & objLogicData,
        DataLogic2UI * pNewLogicData = NULL);

// 判断是否数字键
extern bool IsDigitKey(int iKey);

//判断sessionID 是否有效
bool IsSessionIDValid(int iSessionID);

//暂时还未实现
yl::string GetSoftKeyActionByType(SoftKey_TYPE eSoftkey);

//播放 softkey 的按键音
void PlaySoftKeyTone(yl::string strSoftKeyAction);

yl::string TransTalkText(yl::string & strText);

// 根据callinfo获取显示名称
yl::string GetNameFromCallInfo(CCallInfo * pCallInfo);

// 根据callinfo获取显示号码
yl::string GetNumberFromCallInfo(CCallInfo * pCallInfo);

//根据callinfo获取fullcontactinfo
yl::string GetFullContactInfoFromCallInfo(CCallInfo * pCallInfo);

//根据callinfo获取头像
yl::string GetContactImageFromCallInfo(CCallInfo * pCallInfo);

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
void ChangeNameAndNumberByInfoMethod(yl::string & strName, yl::string & strNumber,
                                     yl::string & strFullContactInfo, SHOW_CONTACTDETAIL_TYPE showContactDetailType,
                                     bool bIsMergerNameAndNumber = true);


//获取TargetInfo 信息 主要是给 通话、会议 提供新来电 targetinfo 信息  获取的优先级为 Label、DisplayName、UserName
yl::string GetTargetInfo();

//获取Label  Label 不存在那么 获取GetDisplayName 函数数据
yl::string GetLabel(int iAccountID);

//获取Displayname Display 如果获取不成功 那么用 GetUserName 函数数据
yl::string GetDisplayName(int iAccountID);

//获取UserName 数据
yl::string GetUserName(int iAccountID);

//给callmanager模块调用的进入拨号界面的函数
void talkui_enterdialingui(const yl::string & strUIName);

//给callmanager 模块调用的退出拨号界面的函数
void talkui_putdialingui(const yl::string & strUIName);

//callmanager 解绑拨号对话框时的回调函数
void talkui_releasedialinguidlg(const yl::string & strUIName, CCallManagerBaseDlg * pDlg);

#endif //__COMMON_FUNCTION_H__
