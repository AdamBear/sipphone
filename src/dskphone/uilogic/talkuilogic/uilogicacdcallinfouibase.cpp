#include "uilogicacdcallinfouibase.h"

CUILogicACDCallInfoUIBase::CUILogicACDCallInfoUIBase(void)
    : m_pProcessor(NULL)
{
}

CUILogicACDCallInfoUIBase::~CUILogicACDCallInfoUIBase(void)
{
}

CBaseTalkUIProcessor * CUILogicACDCallInfoUIBase::GetBindUIProcessor()
{
    return m_pProcessor;
}

void CUILogicACDCallInfoUIBase::BindProcessor(CBaseTalkUIProcessor * pBindProcessor)
{
    m_pProcessor = (CACDCallInfoProcessor *)pBindProcessor;
}
