#include "edkactionbase.h"

#if IF_FEATURE_EDK
CEdkActionBase::CEdkActionBase(EDKEXCUTETYPE eExcuteType)
{
    m_eExcuteType = eExcuteType;
}

CEdkActionBase::~CEdkActionBase()
{

}

// 处理消息
bool CEdkActionBase::OnMessage(msgObject & msg)
{
    return false;
}
#endif
