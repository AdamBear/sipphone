#include "uilogicnetconfuibase.h"

CUILogicNetConfUIBase::CUILogicNetConfUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicNetConfUIBase::~CUILogicNetConfUIBase(void)
{
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicNetConfUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

//绑定processor
void CUILogicNetConfUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CNetConfProcessor *)pBindProcessor;
}
