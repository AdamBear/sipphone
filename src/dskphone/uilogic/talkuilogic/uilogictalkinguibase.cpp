#include "uilogictalkinguibase.h"

CUILogicTalkingUIBase::CUILogicTalkingUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicTalkingUIBase::~CUILogicTalkingUIBase(void)
{
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicTalkingUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

//绑定processor
void CUILogicTalkingUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CTalkingProcessor *)pBindProcessor;
}
