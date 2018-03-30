#include "uilogicdialuibase.h"

CUILogicDialUIBase::CUILogicDialUIBase()
    : m_pProcessor(NULL)
{
}

CUILogicDialUIBase::~CUILogicDialUIBase()
{
    m_pProcessor = NULL;
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicDialUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

//绑定processor
void CUILogicDialUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CDialProcessor *)pBindProcessor;
}