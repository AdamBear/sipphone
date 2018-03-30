#include "modfwddnd.h"
#include "dndmanager.h"
#include "fwdmanager.h"

#include <dsklog/log.h>


void fwddnd_Init()
{
  _DndManager.Init();
  _FwdManager.Init();
}

bool dnd_IsActive()
{
  return _DndManager.IsActive();
}

FWDDND_MODE dnd_GetMode()
{
  return _DndManager.GetMode();
}

void dnd_SetMode(FWDDND_MODE eMode)
{
  _DndManager.SetMode(eMode);
}

bool dnd_IsEnable(int iAccount)
{
  bool ret = _DndManager.IsEnable(iAccount);
  FWDDND_INFO("dnd_IsEnable(%d)=%d", iAccount, ret);
  return ret;
}

bool dnd_SetEnable(int iAccount, bool bEnable,
                   DND_CONFIG_SOURCE_TYPE eSource/* = DND_CFG_SRC_TYPE_COMMON */)
{
  return _DndManager.SetEnable(iAccount, bEnable, eSource);
}

void dnd_Backup()
{
  _DndManager.Backup();
}

void dnd_Restore()
{
  _DndManager.Restore();
}

bool fwd_IsActive()
{
  return _FwdManager.IsActive();
}

FWDDND_MODE fwd_GetMode()
{
  return _FwdManager.GetMode();
}

void fwd_SetMode(FWDDND_MODE eMode)
{
  _FwdManager.SetMode(eMode);
}

bool fwd_GetInfo(int iAccount, FWD_TYPE eType, yl::string * pstrTarget/* = NULL */,
                 int * pRingDelay/* = NULL */)
{
  return _FwdManager.GetInfo(iAccount, eType, pstrTarget, pRingDelay);
}

bool fwd_SetInfo(int iAccount, FWD_TYPE eType, bool bEnabled,
                 const yl::string * pstrTarget/* = NULL */, int * pRingDelay/* = NULL */,
                 FWD_CONFIG_SOURCE_TYPE eSource/* = FWD_CFG_SRC_TYPE_COMMON */)
{
  return _FwdManager.SetInfo(iAccount, eType, bEnabled,
                             pstrTarget, pRingDelay, eSource);
}

bool fwd_SetMultiInfo(int iAccount, int eType, bool bEnabled,
                      FWD_CONFIG_SOURCE_TYPE eSource/* = FWD_CFG_SRC_TYPE_COMMON */)
{
  return _FwdManager.SetMultiInfo(iAccount, eType, bEnabled, eSource);
}

bool fwd_RestoreInfo(int iAccount, FWD_TYPE eType)
{
  return _FwdManager.RestoreInfo(iAccount, eType);
}

bool fwd_IsAllowForwardTo(const yl::string & strTarget)
{
  return CFwdManager::IsAllowForwardTo(strTarget);
}

int fwd_GetLastSavedType()
{
  return _FwdManager.GetLastSavedType();
}
