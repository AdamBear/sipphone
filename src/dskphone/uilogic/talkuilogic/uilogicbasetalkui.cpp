#include "uilogicbasetalkui.h"

CUILogicBaseTalkUI::CUILogicBaseTalkUI()
{

}

CUILogicBaseTalkUI::~CUILogicBaseTalkUI()
{
}

//获取processorname
yl::string CUILogicBaseTalkUI::GetProcessorName()
{
    CBaseTalkUIProcessor * pTalkUIProcessor = GetBindUIProcessor();

    if (NULL != pTalkUIProcessor)
    {
        return pTalkUIProcessor->GetProcessorName();
    }

    return "";
}
