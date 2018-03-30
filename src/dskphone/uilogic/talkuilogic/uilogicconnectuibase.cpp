#include "uilogicconnectuibase.h"

CUILogicConnectUIBase::CUILogicConnectUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicConnectUIBase::~CUILogicConnectUIBase(void)
{
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicConnectUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

//绑定processor
void CUILogicConnectUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CConnectProcessor *)pBindProcessor;

}

