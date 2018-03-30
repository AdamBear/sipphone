#include "exp/include/modexp.h"
#include "exp/src/explogic/explogicmanager.h"
#include "exp/src/explogic/explogic.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"
#include "devicelib/phonedevice.h"


void exp_InitExtFollowPhoneType()
{
	PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();

	if (PT_T54S == ePhoneType || PT_T52 == ePhoneType)//exp50
	{
		exp_Init(EXP_DRIVE_TYPE_USB);
		exp_InitExt(DT_ET_COLOR);
	}
	else if (DEVICELIB_IS_T40(ePhoneType)//ehs
		|| DEVICELIB_IS_T42(ePhoneType))
	{
		exp_Init(EXP_DRIVE_TYPE_RJ);
		exp_InitExt(DT_ET_EHS);
	}
	else if (DEVICELIB_IS_T27(ePhoneType)//black exp. ehs
		|| DEVICELIB_IS_T46(ePhoneType)
		|| DEVICELIB_IS_T48(ePhoneType))
	{
		exp_Init(EXP_DRIVE_TYPE_RJ);
		exp_InitExt(DSSKEY_EXT_TYPE(DT_ET_EHS | DT_ET_BLACK));
	}
}

void exp_DumpExpModelInfo(yl::string& strDumpInfo)
{
	strDumpInfo.clear();
	char szbuff[128] = { 0 };

	sprintf(szbuff,"[Exp][Exp Info][Current ExpType: %d][Max ExpNum: %d][Inserted ExpNum: %d][ehs Index: %d]"
		, exp_GetEXPType() - DT_ALL
		, exp_GetMaxExpNum()
		, exp_GetEXPNum()
		, exp_GetEhsIndex());

	strDumpInfo = szbuff;
}

#if defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
int exp_GetEXPNum()
{
	return g_pEXPLogicManager->GetEXPNum();
}

int exp_GetExpKeyNumByType(DEVICE_TYPE_DEF devType)
{
	int iExpKeyNum = 0;
	switch (devType)
	{
	case DT_EXP_20:
	case DT_EXP_38:
	case DT_EXP_39:
	case DT_EXP_40:
	case DT_EXP_CM68:
	case DT_EXP_CM69:
		iExpKeyNum = 40;
		break;
	case DT_EXP_50:
		iExpKeyNum = 60;
		break;
	default:
		iExpKeyNum = 0;
		break;
	}

	return iExpKeyNum;
}

DEVICE_TYPE_DEF exp_GetEXPType()
{
	return g_pEXPLogicManager->GetEXPType();
}

DSSKEY_MODULE_TYPE exp_GetEXPModuleType()
{
	return g_pEXPLogicManager->EXPManager_GetEXPModuleType();
}

bool exp_IsEXPKey(int iDssKeyID)
{
	return g_pEXPLogicManager->IsEXPKey(iDssKeyID);
}

int exp_GetMaxExpNum()
{
	return g_pEXPLogicManager->GetMaxExpNum();
}

bool exp_OnExpInsert(int iExpIndex, DSSKEY_MODULE_TYPE eExpType)
{
	return g_pEXPLogicManager->OnExpInsert(iExpIndex, eExpType);
}

bool exp_OnExpRemove(int iExpIndex, DSSKEY_MODULE_TYPE eExpType)
{
	return g_pEXPLogicManager->OnExpRemove(iExpIndex, eExpType);
}

bool exp_InitExt(DSSKEY_EXT_TYPE eExtType)
{
	if (eExtType == DT_ET_NONE)
	{
		return false;
	}

	if ((eExtType & DT_ET_COLOR)
		|| (eExtType & DT_ET_BLACK))
	{
		g_pEXPLogicManager->Init();
		g_pEXPLogicManager->InitExpData();
	}

	if (DT_ET_EHS & eExtType)
	{
		exp_InitEHSData();
	}

	return true;
}

bool exp_GetExpColor()
{
	return g_pEXPLogicManager->GetUseEXPColor();
}

int exp_SetExpIcon(int dsskeyID, int icon)
{
	return g_pEXPLogicManager->SetExpIcon(dsskeyID, icon);
}

int exp_SetExpLabel(int dsskeyID, const char * pStrLabel)
{
	return g_pEXPLogicManager->SetExpLabel(dsskeyID, pStrLabel);
}

int exp_SetExpContrast(int expIndex, int contrast)
{
	return g_pEXPLogicManager->SetExpContrast(expIndex, contrast);
}

//正确返回0，错误返回1
int exp_SetExpKeyDisp(int dsskeyID, int icon, const char * pStrLabel, int led_status,
	int displayFlag /*=1*/, int nDsskeyBgType /*= -1*/, const char * pStrIcon/* = NULL*/,
	int nStatus/* = 0*/,
	int nAuxiliaryStatus/* = 0*/)
{
	return g_pEXPLogicManager->SetExpKeyDisp(dsskeyID, icon, pStrLabel, led_status, nDsskeyBgType,
		displayFlag, pStrIcon, nStatus, nAuxiliaryStatus);
}

int exp_RefreshExp(int expIndex)
{
	return g_pEXPLogicManager->RefreshExp(expIndex);
}

void exp_Exp39Switch(int iExpIndex)
{
	g_pEXPLogicManager->Exp39Switch(iExpIndex);
}

int exp_SetExpLEDStatus(int dsskeyID, int led_status)
{
	if (!exp_IsExpKeyInCurrentPage(dsskeyID))
	{
		return 1;
	}

	return g_pEXPLogicManager->SetExpLEDStatus(dsskeyID, led_status);
}

int exp_SetExpBackGround(const char * pStrBackGround, int nTransparentLevel/* = 1*/)
{
	return g_pEXPLogicManager->SetEXPBackGround(pStrBackGround, nTransparentLevel);
}

int exp_EnterExpScreenSaver(bool bEnter)
{
	return g_pEXPLogicManager->EnterExpScreenSaver(bEnter);
}

int exp_SetExpBKLight(int expIndex, int bkLight)
{
	return g_pEXPLogicManager->SetExpBKLight(expIndex, bkLight);
}

bool exp_IsExpKeyInCurrentPage(int dsskeyID)
{
	return g_pEXPLogicManager->IsExpKeyInCurrentPage(dsskeyID);
}

void exp_FlushExpList()
{
	g_pEXPLogicManager->SendEXPInfo();
}

bool exp_UnInitExp()
{
	return true;
}
#else
int exp_GetEXPNum()
{
	return -1;
}

int exp_GetExpKeyNumByType(DEVICE_TYPE_DEF devType)
{
	return 0;
}

DEVICE_TYPE_DEF exp_GetEXPType()
{
	return DT_INVALID;
}

DSSKEY_MODULE_TYPE exp_GetEXPModuleType()
{
	return DMT_NON;
}

bool exp_IsEXPKey(int iDssKeyID)
{
	return false;
}

int exp_GetMaxExpNum()
{
	return 0;
}

bool exp_OnExpInsert(int iExpIndex, DSSKEY_MODULE_TYPE eExpType)
{
	return false;
}

bool exp_OnExpRemove(int iExpIndex, DSSKEY_MODULE_TYPE eExpType)
{
	return false;
}

bool exp_InitExt(DSSKEY_EXT_TYPE eExtType)
{
	return false;
}

bool exp_GetExpColor()
{
	return false;
}

int exp_SetExpIcon(int dsskeyID, int icon)
{
	return 0;
}

int exp_SetExpLabel(int dsskeyID, const char * pStrLabel)
{
	return 0;
}

int exp_SetExpContrast(int expIndex, int contrast)
{
	return 0;
}

int exp_SetExpKeyDisp(int dsskeyID, int icon, const char * pStrLabel, int led_status,
	int displayFlag /*=1*/, int nDsskeyBgType /*= -1*/, const char * pStrIcon/* = NULL*/,
	int nStatus/* = 0*/,
	int nAuxiliaryStatus/* = 0*/)
{
	return 0;
}

int exp_RefreshExp(int expIndex)
{
	return 0;
}

void exp_Exp39Switch(int iExpIndex)
{
	return;
}

int exp_SetExpLEDStatus(int dsskeyID, int led_status)
{
	return 1;
}

int exp_SetExpBackGround(const char * pStrBackGround, int nTransparentLevel/* = 1*/)
{
	return 0;
}

int exp_EnterExpScreenSaver(bool bEnter)
{
	return 0;
}

int exp_SetExpBKLight(int expIndex, int bkLight)
{
	return 0;
}

bool exp_IsExpKeyInCurrentPage(int dsskeyID)
{
	return false;
}

void exp_FlushExpList()
{
	return;
}

bool exp_UnInitExp()
{
	return true;
}
#endif //defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)

#ifdef _EXP_COLOR_SUPPORT
void exp_SetExpKeyEdit(bool bEdit, int nDsskeyID/* = -1*/)
{
	CEXPLogicColorManager * pManager = dynamic_cast<CEXPLogicColorManager *> g_pEXPLogicManager;
	if (pManager == NULL)
	{
		return;
	}
	pManager->SetEditingKey(bEdit, nDsskeyID);
}

int exp_SetExpLEDStatusByIndex(int iExpIndex, int nKeyIndex, int led_status)
{
	CEXPLogicColorManager * pManager = dynamic_cast<CEXPLogicColorManager *> g_pEXPLogicManager;
	if (pManager == NULL)
	{
		return 1;
	}

	return pManager->SetExpLEDStatusByIndex(iExpIndex, nKeyIndex, led_status);
}

void exp_SetExpEditingText(const char * pStrLabel)
{
	CEXPLogicColorManager * pManager = dynamic_cast<CEXPLogicColorManager *> g_pEXPLogicManager;

	if (pManager == NULL)
	{
		return;
	}

	pManager->SetEditingText(pStrLabel);
}

void exp_EXPBackLightUpdate(int nLightLevel, int nIndex/* = -1*/)
{
	CEXPLogicColorManager * pManager = dynamic_cast<CEXPLogicColorManager *> g_pEXPLogicManager;

	if (pManager == NULL)
	{
		return;
	}

	pManager->EXPBackLightUpdate(nLightLevel, nIndex);
}

void exp_ExpFillWithColor(const EXP_FILL_COLOR & objExpColor, int nIndex /*= 0*/)
{
	CEXPLogicColorManager * pManager = dynamic_cast<CEXPLogicColorManager *> g_pEXPLogicManager;

	if (pManager == NULL)
	{
		return;
	}

	pManager->EXPFillWithColor(objExpColor, nIndex);
}

void exp_ExpFillWithPicture(const char * pStrPicture, int nIndex /*= 0*/)
{
	CEXPLogicColorManager * pManager = dynamic_cast<CEXPLogicColorManager *> g_pEXPLogicManager;

	if (pManager == NULL)
	{
		return;
	}

	pManager->EXPFillWithPicture(pStrPicture, nIndex);
}

void exp_ExpDrawText(const char * pStrText, int nIndex /*= 0*/)
{
	CEXPLogicColorManager * pManager = dynamic_cast<CEXPLogicColorManager *> g_pEXPLogicManager;

	if (pManager == NULL)
	{
		return;
	}

	pManager->EXPDrawText(pStrText, nIndex);
}

void exp_ExpShowLogo(int nIndex /*= 0*/)
{
	CEXPLogicColorManager * pManager = dynamic_cast<CEXPLogicColorManager *> g_pEXPLogicManager;

	if (pManager == NULL)
	{
		return;
	}

	pManager->EXPShowLogo(nIndex);
}
#else
void exp_SetExpKeyEdit(bool bEdit, int nDsskeyID/* = -1*/)
{
	return;
}

int exp_SetExpLEDStatusByIndex(int iExpIndex, int nKeyIndex, int led_status)
{
	return 1;
}

void exp_SetExpEditingText(const char * pStrLabel)
{
	return;
}

void exp_EXPBackLightUpdate(int nLightLevel, int nIndex/* = -1*/)
{
	return;
}

void exp_ExpFillWithColor(const EXP_FILL_COLOR & objExpColor, int nIndex /*= 0*/)
{
	return;
}

void exp_ExpFillWithPicture(const char * pStrPicture, int nIndex /*= 0*/)
{
	return;
}

void exp_ExpDrawText(const char * pStrText, int nIndex /*= 0*/)
{
	return;
}

void exp_ExpShowLogo(int nIndex /*= 0*/)
{
	return;
}
#endif//_EXP_COLOR_SUPPORT