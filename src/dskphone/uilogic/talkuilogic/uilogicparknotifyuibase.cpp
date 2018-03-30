#include "uilogicparknotifyuibase.h"

CUILogicParkNotifyUIBase::CUILogicParkNotifyUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicParkNotifyUIBase::~CUILogicParkNotifyUIBase(void)
{
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicParkNotifyUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

//绑定processor
void CUILogicParkNotifyUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CParkNotifyProcessor *)pBindProcessor;
}
