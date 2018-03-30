#include "uilogicnetconfmgruibase.h"

CUILogicNetConfMgrUIBase::CUILogicNetConfMgrUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicNetConfMgrUIBase::~CUILogicNetConfMgrUIBase(void)
{
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicNetConfMgrUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

//绑定processor
void CUILogicNetConfMgrUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CNetConfMgrProcessor *)pBindProcessor;
}
