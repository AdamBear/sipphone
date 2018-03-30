#include "idlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"
#include "dsklog/log.h"

#include "idle/idle_data_define.h"
#include "service_config.h"

#include "idlescreen/include/modidlescreen.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsskey/include/moddsskey.h"
#include "record/include/modrecord.h"

CIdleHandle::CIdleHandle(int nType /*= PS_STATE_IDLE*/)
	: CBaseIdleHandle(nType)
{

}

CIdleHandle::~CIdleHandle()
{

}

bool CIdleHandle::GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey)
{
	m_vecSoftkey.clear();
	CBaseIdleHandle::GetSoftkey(m_vecSoftkey);
	m_vecSoftkey.push_back(ST_IDLE_HOTKEY1);
	m_vecSoftkey.push_back(ST_IDLE_HOTKEY2);
	m_vecSoftkey.push_back(ST_IDLE_HOTKEY3);
	m_vecSoftkey.push_back(ST_IDLE_HOTKEY4);
	return true;
}
