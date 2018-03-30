#include "uilogicautoredialuibase.h"

CUILogicAutoRedialUIBase::CUILogicAutoRedialUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicAutoRedialUIBase::~CUILogicAutoRedialUIBase(void)
{
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicAutoRedialUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}
//绑定processor
void CUILogicAutoRedialUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CAutoRedialProcessor *)pBindProcessor;
}

