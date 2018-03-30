#include "uilogicringuibase.h"

CUILogicRingUIBase::CUILogicRingUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicRingUIBase::~CUILogicRingUIBase(void)
{
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicRingUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

//绑定processor
void CUILogicRingUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CRingProcessor *)pBindProcessor;
}
