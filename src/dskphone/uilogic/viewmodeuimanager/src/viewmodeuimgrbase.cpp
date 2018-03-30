#if (defined(IF_SUPPORT_BLUETOOTH ) || defined(IF_SUPPORT_WIFI))
#include "../include/viewmodeuimgrbase.h"
#include "../include/viewmodectrlerhelper.h"

CViewModeUIMgrBase::CViewModeUIMgrBase()
    : m_pViewModeControler(NULL),
      m_NextFocusIndex(0)
{
}

CViewModeUIMgrBase::~CViewModeUIMgrBase()
{
}

// 设置控制器
bool CViewModeUIMgrBase::SetControler(int iUIType)
{
    m_pViewModeControler = _ViewModeCtrlerHelper.GetCtrler(iUIType);
    return m_pViewModeControler != NULL;
}

// 释放控制器
void CViewModeUIMgrBase::ReleaseControler()
{
    _ViewModeCtrlerHelper.ReleaseCtrler(&m_pViewModeControler);
}

// 获取控制器
CViewModeControler * CViewModeUIMgrBase::GetControler()
{
    return m_pViewModeControler;
}

#endif // IF_SUPPORT_BLUETOOTH || IF_SUPPORT_WIFI
