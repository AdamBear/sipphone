#include "uilogicconfuibase.h"

CUILogicConfUIBase::CUILogicConfUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicConfUIBase::~CUILogicConfUIBase(void)
{
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicConfUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

//绑定processor
void CUILogicConfUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CConfProcessor *)pBindProcessor;
}

