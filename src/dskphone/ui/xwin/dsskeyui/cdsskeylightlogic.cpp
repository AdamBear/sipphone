#include "cdsskeylightlogic.h"
#include "dsskey/include/moddsskey.h"
#include "exp/include/modexp.h"
#include "lamp/ledlamp/include/modledlamp.h"

#include "dsskey/src/dsskeycommonfunc.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsskeyuilogic.h"

#define DSSK_SLOWER_FLASH_TIMER	    1000
#define DSSK_SLOW_FLASH_TIMER	    500
#define DSSK_FAST_FLASH_TIMER		300
#define DSSK_FASTER_FLASH_TIMER		200


// �ص�������������Ϣ������
static LRESULT OnTimerMessage(msgObject& objMessage)
{
	BOOL bHandled = FALSE;
	switch (objMessage.message)
	{
	case TM_TIMER:
		{
			BOOL bHandled = g_pDssKeyLightLogic->OnTimer(objMessage.wParam);
		}
		break;
	default:
		break;
	}
	return bHandled;
}

CDssKeyLightLogic* CDssKeyLightLogic::s_pInst = NULL;

CDssKeyLightLogic* CDssKeyLightLogic::GetInstance()
{
	if (NULL == s_pInst)
	{
		s_pInst = new CDssKeyLightLogic();
	}

	return s_pInst;
}

void CDssKeyLightLogic::DestoryInstance()
{
	if (NULL != s_pInst)
	{
		delete s_pInst;
		s_pInst = NULL;
	}
}

CDssKeyLightLogic::CDssKeyLightLogic(void)
{
	m_lightStatus = 0;
	m_bForbidden = FALSE;
	m_iMaxNumber = 0;
	m_iMaxNumPerPage = 0;
	m_arryFlashTimer[0].iDelay = DSSK_FASTER_FLASH_TIMER;
	m_arryFlashTimer[0].iRunNum = 0;

	m_arryFlashTimer[1].iDelay = DSSK_FAST_FLASH_TIMER;
	m_arryFlashTimer[1].iRunNum = 0;

	m_arryFlashTimer[2].iDelay = DSSK_SLOW_FLASH_TIMER;
	m_arryFlashTimer[2].iRunNum = 0;

	m_arryFlashTimer[3].iDelay = DSSK_SLOWER_FLASH_TIMER;
	m_arryFlashTimer[3].iRunNum = 0;

	SingleMsgReg(TM_TIMER, OnTimerMessage);

}

CDssKeyLightLogic::~CDssKeyLightLogic(void)
{
	YLList<LIGHT_INFO*>::iterator itr = m_listLightInfo.begin();
	YLList<LIGHT_INFO*>::iterator itrEnd = m_listLightInfo.end();
    for (; itr != itrEnd; itr++)
	{
        if (*itr != NULL)
		{
            delete *itr;
            *itr = NULL;
		}
	}

	m_listLightInfo.clear();
}

//����
void CDssKeyLightLogic::UpdateLight(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType, 
									DSS_KEY_FLUSH_TYPE eFlushType, int iFlushuration /*= -1*/)
{
	DSSKEYUI_INFO("CDssKeyLightLogic::UpdateLight %d", iDsskeyID);
	if (iDsskeyID < 0)
	{
		return;
	}

	if (LED_COLOR_OFF == eColorType)
	{
		TurnOffLight(iDsskeyID);
		return;
	}

#ifdef _MAP_TEST_
	LightStatusMapIterator iter = LightStatusFind(iDsskeyID);
#else
	LightStatusMapIterator iter = m_lightStatusMap.find(iDsskeyID);
#endif
	if (iter != m_lightStatusMap.end())
	{
#ifdef _MAP_TEST_
        LightData &Tmp = *iter;
		if (Tmp.m_lightMeta.m_eColorType != eColorType)
		{
			// �ȹص�
			TurnOffLightByIndex(iDsskeyID);
		}

        (*iter).m_lightMeta.m_eColorType = eColorType;
        (*iter).m_lightMeta.m_eFlushType = eFlushType;
#else
		if (iter->second.m_eColorType != eColorType)
		{
			// �ȹص�
			TurnOffLightByIndex(iDsskeyID);
		}

		iter->second.m_eColorType = eColorType;
		iter->second.m_eFlushType = eFlushType;
#endif
	}
	else
	{
		SLightMeta lm;
		lm.m_iDssKeyID = iDsskeyID;
		lm.m_eColorType = eColorType;
		lm.m_eFlushType = eFlushType;
		lm.m_iPriority = 0;
#ifdef _MAP_TEST_
		LightData LightTmp;
		LightTmp.m_nLightKey = iDsskeyID;
		LightTmp.m_lightMeta = lm;
		m_lightStatusMap.push_back(LightTmp);
#else
		m_lightStatusMap[iDsskeyID] = lm;
#endif
		
	}

	if (iFlushuration > 0)
	{
		/*timerSetThreadTimer((UINT)&m_listTimerID[iDsskeyID], iFlushuration);*/

	}
	updateLightStatus(iDsskeyID, eColorType, eFlushType);
}

//�ص�
void CDssKeyLightLogic::TurnOffLight(int iDsskeyID)
{
#ifdef _MAP_TEST_
	LightStatusMapIterator iter = LightStatusFind(iDsskeyID);
#else
	LightStatusMapIterator iter = m_lightStatusMap.find(iDsskeyID);
#endif

	if (iter != m_lightStatusMap.end())
	{
		//ɾ����״̬��¼
		m_lightStatusMap.erase(iter);
	}

	TurnOffLightByIndex(iDsskeyID);
}

void CDssKeyLightLogic::TurnOffLightByIndex(int iDsskeyID)
{
	UpdateFlashTimer(iDsskeyID, 0, FALSE);

	DSSKEY_MODULE_TYPE eType = dsskey_GetModuleType(iDsskeyID);
	if (IsEXPModule(eType))
	{
		exp_SetExpLEDStatus(iDsskeyID, LT_OFF);
	}
	else if (DMT_LINEKEY_MODULE == eType)
	{
		if (iDsskeyID > -1
			&& (m_lightStatus & 1<<iDsskeyID) != 0)
		{
			m_lightStatus &= ~(1<<iDsskeyID);
			ledlamp_PostDsskeyEvent(iDsskeyID);
		}
	}
}

void CDssKeyLightLogic::UpdateFlashTimer(int iDsskeyID, int iDelay, BOOL bFlash)//delay����flushtime
{
	LIGHT_INFO* pLightInfo = NULL;
	YLList<LIGHT_INFO*>::iterator itr = m_listLightInfo.begin();
	YLList<LIGHT_INFO*>::iterator itrEnd = m_listLightInfo.end();
    for (; itr != itrEnd; itr++)
	{
        if ((*itr)->iLightID == iDsskeyID)//lcm��ǰdsskeyԭ���������Ƶ���Ϣ
		{
			// �ҵ��ɵĵ���Ϣ
            pLightInfo = *itr;
			break;
		}
	}

	int iOldTimerIndex = -1;
	int iNewTimerIndex = -1;

	for (int i = 0; i < LIGHT_TIMER_NUM; ++i)//lcm�����õĶ�ʱ��
	{
		// ��ȡ�ɶ�ʱ������
		if (iDelay == m_arryFlashTimer[i].iDelay)//M_arrayFlashTimer[i]�����ʱ����������Ҫʹ�õĶ�ʱ��������һ����
		{
			iNewTimerIndex = i;//������ǿ���ֱ��ʹ�ã����¶�ʱ����indexֱ������Ϊ���
		}
		// ��ȡ�¶�ʱ������
		if (pLightInfo != NULL
			&& m_arryFlashTimer[i].iDelay == pLightInfo->iDelay)//��ߵ������dsskeyԭ���������ù� m_arryFlashTimer[i]�����ʱ���ģ�
		{
			//����tpLightInfo��=NULL˵�����dsskey�ϴξ������ã���һ��˵�����õľ������
			iOldTimerIndex = i;
		}
	}

	if (bFlash)// true eColorType == LED_COLOR_RED_GREEN������ɫ��
	{
		if (iOldTimerIndex != -1)
		{
			// ��ʱ�����ͷ����仯,�ɶ�ʱ��������һ
			m_arryFlashTimer[iOldTimerIndex].iRunNum--;
			if (m_arryFlashTimer[iOldTimerIndex].iRunNum == 0)
			{
				timerKillThreadTimer((UINT)&m_arryFlashTimer[iOldTimerIndex]);
			}
		}

		if (iNewTimerIndex < 0)
		{
			return;
		}

		m_arryFlashTimer[iNewTimerIndex].iRunNum++;
		if (m_arryFlashTimer[iNewTimerIndex].iRunNum == 1)
		{
			timerSetThreadTimer((UINT)&m_arryFlashTimer[iNewTimerIndex], m_arryFlashTimer[iNewTimerIndex].iDelay);
		}

		if (NULL == pLightInfo)
		{
			pLightInfo = new LIGHT_INFO();
			m_listLightInfo.push_back(pLightInfo);
			pLightInfo->eLightType = LED_COLOR_TYPE_RED;
			pLightInfo->iLightID = iDsskeyID;
			pLightInfo->bFlash = bFlash;
		}
		
		pLightInfo->iDelay = m_arryFlashTimer[iNewTimerIndex].iDelay;
	}
	else
	{
		if (NULL == pLightInfo
			|| itr == itrEnd
			|| iOldTimerIndex == -1)
		{
			return;
		}
		
		// �ر��Ѿ����ڵĶ�ʱ��
		m_arryFlashTimer[iOldTimerIndex].iRunNum--;
		if (m_arryFlashTimer[iOldTimerIndex].iRunNum == 0)
		{
			timerKillThreadTimer((UINT)&m_arryFlashTimer[iOldTimerIndex]);
		}

		delete pLightInfo;
		pLightInfo = NULL;
		m_listLightInfo.erase(itr);
	}

	DSSKEYUI_INFO("listLightInfo.size=%d", m_listLightInfo.size());
}

//���µƵ�״̬
void CDssKeyLightLogic::updateLightStatus(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType, DSS_KEY_FLUSH_TYPE eFlushType)
{
	DSSKEYUI_INFO("CDssKeyLightLogic::updateLightStatus:iDsskeyID:%d eColorType:%d eFlushType:%d\n", iDsskeyID, eColorType, eFlushType);
	if (iDsskeyID < 0)
	{
		return;
	}
	int iFlashTime = 0;

	//��������
	switch (eFlushType)
	{
	case REFRESH_TYPE_SLOW_FLUSH://����
		iFlashTime = DSSK_SLOWER_FLASH_TIMER;//1000ms
		break;

	case REFRESH_TYPE_FLUSH://����500ms
		iFlashTime = DSSK_SLOW_FLASH_TIMER;
		break;

	case REFRESH_TYPE_FAST_FLUSH://���� 300ms
		iFlashTime = DSSK_FAST_FLASH_TIMER;
		break;

	case REFRESH_TYPE_FASTER_FLUSH://����� 200ms
		iFlashTime = DSSK_FASTER_FLASH_TIMER;
		break;
	default:
		iFlashTime = 0;
		break;
	}
	
	if (eColorType == LED_COLOR_RED_GREEN)//�������� //������˸ʹ�õĶ�ʱ��
	{
		UpdateFlashTimer(iDsskeyID, iFlashTime, TRUE);
		return;
	}
	else
	{
		UpdateFlashTimer(iDsskeyID, iFlashTime, FALSE);
	}

	LightCtrl(iDsskeyID, eColorType, iFlashTime);
}

void CDssKeyLightLogic::LightCtrl(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType, int iDelay,bool bFlash/* = false*/)
{
	bool bGreen = false;
	bool bRed = false;
	LIGHT_Type lType = LT_OFF;
	//�Ƶ���ɫ
	switch (eColorType)
	{
	case LED_COLOR_TYPE_GREEN:
		bGreen = true;
		lType = LT_GREEN_ON;
		break;
	case LED_COLOR_TYPE_RED:
		bRed = true;
		lType = LT_RED_ON;
		break;
	case LED_COLOR_TYPE_ORANGE:
		bGreen = true;
		bRed = true;
		lType = LT_RED_GREEN_ON;
		break;
	default:
		bGreen = true;
		break;
	}

	DSSKEY_MODULE_TYPE eType = dsskey_GetModuleType(iDsskeyID);
	if (IsEXPModule(eType))
	{
		exp_SetExpLEDStatus(iDsskeyID, lType);
	}
	else if (DMT_LINEKEY_MODULE == eType)
	{
		m_lightStatus |= (1 << iDsskeyID);
		ledlamp_PostDsskeyEvent(iDsskeyID, iDelay, bFlash ? 0 : iDelay, bGreen, bRed);
	}
}

#ifdef _MAP_TEST_
CDssKeyLightLogic::LightStatusMapIterator CDssKeyLightLogic::LightStatusFind(int index)
{
	CDssKeyLightLogic::LightStatusMapIterator iter = m_lightStatusMap.begin();
	while (iter != m_lightStatusMap.end())
	{
        if ((*iter).m_nLightKey == index)
		{
			break;
		}
        iter++;
	}
	return iter;
}
#endif

BOOL CDssKeyLightLogic::OnTimer(UINT uTimerID)
{
	listInt_it it = m_listTimerID.begin();
	for (; it != m_listTimerID.end(); ++it)
	{
		if (uTimerID == (UINT)&(*it))
		{
			timerKillThreadTimer(uTimerID);
			OnStopLightTimer(*it);
			m_listTimerID.erase(it);
			return TRUE;
		}
	}

	
	int iDelay = -1;
	for (int i = 0; i < LIGHT_TIMER_NUM; ++i)
	{
		if (uTimerID == (UINT)&(m_arryFlashTimer[i]))
		{
			iDelay = m_arryFlashTimer[i].iDelay;
			break;
		}
	}

	if (iDelay != -1
		&& m_listLightInfo.size() > 0)
	{
		YLList<LIGHT_INFO*>::iterator itr = m_listLightInfo.begin();
		YLList<LIGHT_INFO*>::iterator itrEnd = m_listLightInfo.end();

        for (; itr != itrEnd; itr++)
		{
            LIGHT_INFO* pLightInfo = *itr;
			if (pLightInfo != NULL
				&& iDelay == pLightInfo->iDelay)
			{
				if (LED_COLOR_TYPE_RED == pLightInfo->eLightType)
				{
					pLightInfo->eLightType = LED_COLOR_TYPE_GREEN;
				}
				else
				{
					pLightInfo->eLightType = LED_COLOR_TYPE_RED;
				}

				LightCtrl(pLightInfo->iLightID,
					pLightInfo->eLightType,
					pLightInfo->iDelay,
					pLightInfo->bFlash);
			}
		}
	}

	return FALSE;
}

// ����ÿҳ��ʾ��Dsskey����
void CDssKeyLightLogic::SetMaxNumPerPage(int iMaxNumPerPage)
{
	if (iMaxNumPerPage < 0)
	{
		return;
	}
	m_iMaxNumPerPage = iMaxNumPerPage;
// 	m_listTimerID.clear();
// 	BYTE TimerID;
// 	for (int i = 0; i < m_iMaxNumPerPage; ++i)
// 	{
// 		m_listTimerID.push_back(TimerID);
// 	}
}

void CDssKeyLightLogic::SetMaxNumber(int iMaxNumber)
{
	if (iMaxNumber < 0)
	{
		return;
	}
	m_iMaxNumber = iMaxNumber;
}

void CDssKeyLightLogic::SetStopLightTimerByDsskeyID(int iDsskeyID, int iStopTime/* = 5000*/)
{
	listInt_it it = m_listTimerID.begin();
	for (; it != m_listTimerID.end(); ++it)
	{
		if (iDsskeyID == *it)
		{
			// �Ѵ��ڣ�ֱ�����ö�ʱ��
			timerKillThreadTimer((UINT)&(*it));
			timerSetThreadTimer((UINT)&(*it), iStopTime);
			return;
		}
	}
	// �����ڣ�����ӵ�������
	m_listTimerID.push_back(iDsskeyID);
	int TIDsize = m_listTimerID.size();
	if (TIDsize > 0)
	{
		//  ȡ��Ӧ��ʱֵ�ĵ�ַ��Ϊ��ʱ��ID
		timerSetThreadTimer((UINT)&(m_listTimerID.back()), iStopTime);
	}
}

// ��ʱ�رյƴ���
void CDssKeyLightLogic::OnStopLightTimer(int iDsskeyID)
{
	// �صƴ���
	TurnOffLightByIndex(iDsskeyID);
	// Linekeyˢ����ʾ����
	_DssKeyUILogic.UpdateDsskeyLight(iDsskeyID, LED_COLOR_OFF, REFRESH_TYPE_NO_FLUSH);
}

void CDssKeyLightLogic::Forbid(BOOL bForbidden)
{
	// ��������Dsskey��
	if (bForbidden)
	{
		for (int i = 0; i < m_iMaxNumPerPage; ++i)
		{
			TurnOffLight(i);
		}
	}

	m_bForbidden = bForbidden;
}

