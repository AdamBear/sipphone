#include "uilogicfinishuibase.h"

CUILogicFinishUIBase::CUILogicFinishUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicFinishUIBase::~CUILogicFinishUIBase(void)
{
}

//获取绑定的processor
CBaseTalkUIProcessor * CUILogicFinishUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

//绑定processor
void CUILogicFinishUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CFinishProcessor *)pBindProcessor;

}