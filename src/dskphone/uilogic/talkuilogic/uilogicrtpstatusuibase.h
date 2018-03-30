#ifndef _UILOGIC_RTPSTATUS_UI_BASE_H
#define _UILOGIC_RTPSTATUS_UI_BASE_H

#include "rtpstatusprocessor.h"
#include "uilogicbasetalkui.h"
#include "talkuilogic/talkuilogiccondition.h"

class CUILogicRTPStatusUIBase: public CUILogicBaseTalkUI
{
public:
    CUILogicRTPStatusUIBase(void);
    virtual ~CUILogicRTPStatusUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);

public:
//更新RTPStatus数据
    virtual void UpdateUIRTPStatusData(const talkui_vector<CRTPStatusData> & vectorRTPStatusData) = 0;


protected:
    //保存的processor
    CRTPStatusProcessor * m_pProcessor;
};

#endif
