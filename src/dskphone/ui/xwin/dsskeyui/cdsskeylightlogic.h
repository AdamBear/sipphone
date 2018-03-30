/************************************************************************
 * FileName  : CDssKeyLightLogic.h (header file)
 * Purpose   : 
 * Date      : 2012/10/18 11:05:47
 ************************************************************************/

#ifndef _CDSSKEYLIGHTLOGIC_H_
#define _CDSSKEYLIGHTLOGIC_H_

#include "lamp/ledlamp/include/modledlamp.h"
#include <ylhashmap.h>
#include <yllist.h>
#include <ylvector.h>
#include "dsskeyuicommondefine.h"

struct LIGHT_INFO 
{
	LIGHT_INFO()
	{
		iLightID = -1;
		eLightType = LED_COLOR_OFF;
		iDelay = 0;
		bFlash = false;
	}

	int					iLightID;
	DSS_KEY_COLOR_TYPE	eLightType; // ����ɫ,���̽�����ʱָʾ��ǰ��״̬,ȡֵ��ɫ����ɫ
	int					iDelay;
	bool                bFlash;     // �Ƿ����ɫ������˸
};
// ������չ̨���̽�����˸ʱ�䶨ʱ������
#define LIGHT_TIMER_NUM		4

struct TimerInfo
{
	int		iDelay;
	int		iRunNum;		// ����������Ϊ0��ʱ��رն�ʱ��
};

typedef YLList<int>		listInt;
typedef listInt::iterator	listInt_it;

class CDssKeyLightLogic
{
private:

	struct SLightMeta
	{
		int m_iDssKeyID;
		int m_iPriority;
		DSS_KEY_COLOR_TYPE m_eColorType;
		DSS_KEY_FLUSH_TYPE m_eFlushType;

		SLightMeta& operator= (SLightMeta& tmp)
		{
			if (this != &tmp)
			{
				m_iDssKeyID = tmp.m_iDssKeyID;
				m_iPriority = tmp.m_iPriority;
				m_eColorType = tmp.m_eColorType;
				m_eFlushType = tmp.m_eFlushType;
			}
			return *this;
		}
	};
#ifdef _MAP_TEST_
	struct LightData
	{
		int m_nLightKey;
		SLightMeta m_lightMeta;
	};

	typedef YLList<LightData>	LightStatusMap;
#else
	typedef YLHashMap<int, SLightMeta>	LightStatusMap;
#endif
	typedef LightStatusMap::iterator	LightStatusMapIterator;


	struct SPageLightMeta
	{
		SPageLightMeta() : m_iPage(0), m_iDssKey(-1), m_iPriority(DSS_KEY_LED_PRIORITY_NORMAL), m_bGreen(true), m_bRed(false), m_iFlashTime(-1) {}
		int m_iPage;
		int m_iDssKey;
		int m_iPriority;
		bool m_bGreen;
		bool m_bRed;
		int m_iFlashTime;
	};
	typedef YLList<SPageLightMeta>	PageLightMetaList;	//page��״̬
	typedef PageLightMetaList::iterator	PageLightMetaListIterator;

public:
	static CDssKeyLightLogic* GetInstance();
	static void DestoryInstance();

public:
	// ���µƵ�ǰ��״̬
	void UpdateLight(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType
		, DSS_KEY_FLUSH_TYPE eFlushType, int iFlushuration = -1);
	// �ص�
	void TurnOffLight(int iDsskeyID);
	// ��˸��ʱ����Ӧ
	BOOL OnTimer(UINT uTimerID);
	// �������е�
	void Forbid(BOOL bForbidden);
	// ����ÿҳ��ʾ��Dsskey����
	void SetMaxNumPerPage(int iMaxNumPerPage);
	// ����Dsskey��Ӧ�ĵƸ���
	void SetMaxNumber(int iMaxNumber);
	// ���ö�ʱ�رյ�
	void SetStopLightTimerByDsskeyID(int iDsskeyID, int iStopTime = 5000);
	
private:
	// ��ʱ�رյƴ���
	void OnStopLightTimer(int iDsskeyID);
	// �����صƵĲ���
	void TurnOffLightByIndex(int iDsskeyID);
	// ���µƵ�״̬
	void updateLightStatus(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType, DSS_KEY_FLUSH_TYPE eFlushType);
	// ��������ʾ�Ĳ���
	void LightCtrl(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType, int iDelay,bool bFlash = false);
	// ������˸��ʱ������Ҫ���ڲ�ʵ�ֺ�����˸�Ͷ�ʱ��ƵĲ���
	void UpdateFlashTimer(int iDsskeyID, int iDelay, BOOL bFlash);
private:

#ifdef _MAP_TEST_
	LightStatusMapIterator LightStatusFind(int index);
#endif

	CDssKeyLightLogic();

	virtual ~CDssKeyLightLogic();

private:
	LightStatusMap		m_lightStatusMap;				//��¼DssKey�Ƶ�״̬����DssKeyIDΪ��ʶ
	UINT				m_lightStatus;	//��¼DssKey�Ƶ�״̬��10���Ʒֱ���10��bit��ʶ��DssKeyLight1:1<<1,DssKeyLight2:1<<2,...
	listInt				m_listTimerID;					// ��ʱ��ID��ȡ��ַ��
	PageLightMetaList	m_pageLightList;				//��¼Page��״̬
	BOOL				m_bForbidden;
	int					m_iMaxNumPerPage;				// ÿҳ��ʾ��Dsskey����
	int					m_iMaxNumber;					// Dsskey��Ӧ�ĵƸ���
private:
	static CDssKeyLightLogic* s_pInst;
	TimerInfo		m_arryFlashTimer[LIGHT_TIMER_NUM];
	YLList<LIGHT_INFO*>		m_listLightInfo;
};

#define g_pDssKeyLightLogic (CDssKeyLightLogic::GetInstance())

#endif
