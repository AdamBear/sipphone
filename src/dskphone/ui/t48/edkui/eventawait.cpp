#include "eventawait.h"

///////////////////////////////////CEDKEventWait///////////////////////////////////////
CEDKEventWait::CEDKEventWait(const KeyPressCallBackData & callBackData)
    : m_pCallBackFun(callBackData.m_funcCallBack)
    , m_iEdkId(callBackData.m_iId)
{

}

CEDKEventWait::~CEDKEventWait()
{
    if (NULL != m_pCallBackFun)
    {
        m_pCallBackFun(m_iEdkId);
    }
}
