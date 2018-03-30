#include "uilogicrtpstatusuibase.h"

CUILogicRTPStatusUIBase::CUILogicRTPStatusUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicRTPStatusUIBase::~CUILogicRTPStatusUIBase(void)
{
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicRTPStatusUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

//绑定processor
void CUILogicRTPStatusUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CRTPStatusProcessor *)pBindProcessor;
}

