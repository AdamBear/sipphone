#ifndef _UILOGIC_AUTOREDIAL_UI_BASE_H
#define _UILOGIC_AUTOREDIAL_UI_BASE_H

#include "uilogicbasetalkui.h"
#include "autoredialprocessor.h"

class CUILogicAutoRedialUIBase: public CUILogicBaseTalkUI
{
public:
    CUILogicAutoRedialUIBase(void);
    virtual ~CUILogicAutoRedialUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);

public:
    //更新 autoredial界面提示
    //AUTOREDIAL_STATUS eAutoRedialStatus;      //自动重拨状态
    ////显示自动重拨询问信息
    //剩余的重拨次数
    // 保存自动重拨倒计时超时秒数
    virtual void UpdateAutoRedialNote(AUTOREDIAL_STATUS eAutoRedialStatus, yl::string strInfo,
                                      int nRemainRedialTimes, int nRedialInterval) = 0;

protected:
    //保存的processor
    CAutoRedialProcessor * m_pProcessor;

};

#endif
