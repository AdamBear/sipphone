#ifndef _UILOGIC_BASETALKUI_H_
#define _UILOGIC_BASETALKUI_H_

#include "talkuilogiccommon.h"

#include "basetalkuiprocessor.h"

class CUILogicBaseTalkUI
{
public:
    CUILogicBaseTalkUI();
    virtual ~CUILogicBaseTalkUI();

public:
    //设置UILogic 数据
    virtual void SetUILogicData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT) = 0;


    //在ui基类实现
public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor() = 0;
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor) = 0;

    //停止界面的动画、文字滚动
    virtual void StopUIAnimation() {}

public:
    //获取processorname
    virtual yl::string GetProcessorName();

    virtual void ShowDealLastNote() {}
    virtual bool ShowErrorNote(CCallInfo * pCallInfo)
    {
        return false;
    }
    virtual bool ShowErrorNote(CALL_RECORD_ACTION_RESULT eFailResult)
    {
        return false;
    }

public:
    virtual void OpenLineSelectPage(vecLineSelectData & vecLineData, yl::string strTitle,
                                    bool bShowIndex, int nSelectID) {}

};

#endif
