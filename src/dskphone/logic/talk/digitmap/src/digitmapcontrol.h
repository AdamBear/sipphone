#ifndef __DIAL_DIGITMAP_CONTROL_H__
#define __DIAL_DIGITMAP_CONTROL_H__

#include "digitmapitem.h"

#ifdef IF_FEATURE_DIGIT_MAP

// DigitMap逻辑层单例
class CDigitMapManger
{
public:
    static CDigitMapManger & GetInstance()
    {
        static CDigitMapManger s_DialDigitMap;
        return s_DialDigitMap;
    }

    typedef YLVector<DigitMapInfo>          VecDigitMapAccountInfo;
    typedef DigitMapInfo::VecDigitMap       VecDigitMap;
    typedef DigitMapInfo::VecDigitMapTimer  VecDigitMapTimer;
    typedef DigitMapInfo::DMAction          DMAction;

public:

    CDigitMapManger();

    // 初始化
    void Init();

    // 获取结果
    const DMAction & GetAction(const yl::string & strText, int iAccount = -1);

    // 接收外部模块消息
    static LRESULT OnMessage(msgObject & msg);

private:

    // 模式下的通用匹配，参考dmInfo的设置，将结果保存在DmRefer中
    void GetAction(DigitMapInfo & dmInfo, DigitMapInfo & dmRefer, const yl::string & strText, bool bFullMatchOnly = false);

    // 依序号初始化
    void Init(int iIndex);

private:
    // 配置重新加载
    bool Reload();
    // 获取对应模式的数据
    DigitMapInfo & GetDMInfo(int iIndex = -1);

    static bool IsAccountModeEnable(int iAccount);

private:
    VecDigitMapAccountInfo      m_vecDMInfo;   // 每个账号对应的
    DigitMapInfo                m_DMGlobleInfo;// 全局的账号
    bool                        m_bInited;
    DMAction                    m_dmAction;
public:
    bool Init_CallLogDM();
    bool ReplaceIncomingNumber(yl::string & strNumber);
    DigitMapInfo                m_DMIncomingInfo;//匹配来电规则
};

#define _DigitMapManger CDigitMapManger::GetInstance()

#endif

#endif
