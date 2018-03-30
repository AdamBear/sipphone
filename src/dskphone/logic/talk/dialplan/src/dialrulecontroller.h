#ifndef __DialRuleController_H__
#define __DialRuleController_H__

#include "singletonclassdefine.h"
#include <yllist.h>
#include <ylstring.h>
#include "dialplan_data.h"

// 拨号规则类型
enum DIAL_RULE
{
    DR_PLAN		= 0x01 << 0,
    DR_NOW		= 0x01 << 1,
    DR_AREA		= 0x01 << 2,
    DR_BLOCKOUT = 0x01 << 3,
	DR_ALL		= DR_PLAN | DR_NOW | DR_AREA | DR_BLOCKOUT,
};

// 保存拨号规则的类
class  CRuleData
{
public:
    CRuleData() {}
    ~CRuleData() {}
    // 判断指定账号是否规则中要限制的账号
    bool IsValidLineId(int iLineId);
    // 设置账号
    void SetLineId(const yl::string& strLineId);
public:
    // 保存匹配字体串
    yl::string  m_strPattern;
    // 保存替换字符串
    yl::string  m_strReplace;
    // 保存账号的链表
    YLList<int> m_listLineId;
};

typedef YLList<CRuleData*>    ListData;

class CDialRuleController
{
public:
    CDialRuleController();
    ~CDialRuleController();

    // 根据类型初始化
    bool LoadDialRule(int iDRType = DR_ALL);
    // 清除资源
    void UnLoadDialRule(int iDRType = DR_ALL);
    // 判断是否是可立即呼出的号码
    bool IsDialNowNumber(const yl::string& strDialText, int iLineId);
    // 判断是否是限制呼出的号码
    bool IsBlockOutNumber(const yl::string& strNumber, int iLineId);
    // 根据替换规则,替换号码
    bool GetReplaceNumber(const yl::string& strDialText, yl::string& strReplaceNumber,
                          bool bReplaceAll, int iLineId = -1);
    // 获取地区号码
    bool GetAreaCodeNumber(const yl::string& strDialText, yl::string& strAreaCodeNumber, int iLineId);

public:
    // 数据输出
    void GetDialNowData(DialNowListData& listDialNowData);

private:
    // 释放资源
    void ReleaseDialRuleByType(int iDRType = DR_ALL);
    // 清除链表资源
    void ReleaseListData(ListData* pListData);
    // 加载替换规则
    bool LoadDialPlan();
    // 加载立即呼出
    bool LoadDialNow();
    // 加载地区号码
    bool LoadAreaCode();
    // 加载限制呼出
    bool LoadBlockOut();
    // 为保持与T38一致需要将字符串做转换处理
    void TransformString(yl::string& strTransfer);

    // 是否被动不生效
    bool IsDialRuleEnabled(int iLineId);

private:
    // 保存替换规则的链表
    ListData                    m_listReplace;
    // 保存立即呼出的链表
    ListData                    m_listDialNow;
    // 保存区号及需使用区号的账号
    CRuleData                   m_dataAreaCode;
    // 限制呼出号码及规则使用的账号
    ListData                    m_listBlocked;
    // 区号最小长度
    int                         m_iMinLen;
    // 区号最大长度
    int                         m_iMaxLen;
};

#define _DialRuleController GetControllerInstance()
CDialRuleController& GetControllerInstance() DECLARE_ATTR;

#endif // __DialRuleController_H__
