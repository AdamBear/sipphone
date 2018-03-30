#include "uilogicconfmanageruibase.h"

CUILogicConfManagerUIBase::CUILogicConfManagerUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicConfManagerUIBase::~CUILogicConfManagerUIBase(void)
{
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicConfManagerUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

//绑定processor
void CUILogicConfManagerUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CConfManagerProcessor *)pBindProcessor;
}