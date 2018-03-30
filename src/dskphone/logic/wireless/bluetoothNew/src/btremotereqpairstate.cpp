#ifdef IF_SUPPORT_BLUETOOTH
#include "bluetooth_inc.h"
#include <time.h>

#if IF_BT_CONNECTING_OVERTIME
#define  BT_PAIR_OVERTIMER   30*1000
#endif

#ifdef IF_BLUETOOTH_SUPPORT_REMOTE_PAIR_REQ
CBTRemoteReqPairState::CBTRemoteReqPairState(void)
{
	m_eState = SESSION_REMOTE_REQ_PAIRING_STATE;
}

CBTRemoteReqPairState::~CBTRemoteReqPairState(void)
{
	
}

bool CBTRemoteReqPairState::AfterStateCreate(long ulDeviceHandle, void* pUserData)
{
	m_lDeviceHandle = ulDeviceHandle;

	if (!CreatePairDevice(ulDeviceHandle))
	{
		ExitState();
		return false;
	}

	return true;
}

bool CBTRemoteReqPairState::BeforeStateTransfer(bool bEndConnect/* = false*/)
{
#if IF_BT_CONNECTING_OVERTIME
	SetTimer(false, BT_PAIR_OVERTIMER);
#endif
}

// 处理底层回调事件
bool CBTRemoteReqPairState::ProcessCallbackEvent(msgObject& objMsg)
{
	const btkit_msg_t* pMsg = (const btkit_msg_t*)objMsg.GetExtraData();
	if (!pMsg)
	{
		return false;
	}

	bool bHandle = true;
	bool bNeedExitState = false, bPairSucc = false;
	switch(pMsg->bt_event)
	{
	case BT_PIN_CONFIRM:
		{
			unsigned long ulHandle = pMsg->bt_callback_param.pin_info.dev_hnd;
			if (!ProcessRequestPin(ulHandle, pMsg->bt_callback_param.pin_info.pin))
			{
				ReplyPairReq(ulHandle, false);
				bNeedExitState = true;
			}
		}
		break;
	case BT_DEV_PAIR_CHANGED:
		{
			bPairSucc = ProcessPairChange(pMsg);
			bNeedExitState = true;
		}
		break;
	case BT_DEV_CONNECTED:
		{
			if (m_pSession)
			{
				m_pSession->EnterState(SESSION_CONNECTED_STATE, m_lDeviceHandle, (void*)m_pSessionInfo);
			}
		}
		break;
	case BT_DEV_CONNECTED_FAILE:
	case BT_PAIR_CANCEL:
		{
			bNeedExitState = true;
		}
		break;
	default:
		bHandle = false;
		break;
	}

	if (bNeedExitState)
	{
		ExitState(bPairSucc);
	}

	return bHandle;
}

// 处理option
bool CBTRemoteReqPairState::ProcessOption(const WirelessSessionData& wsData)
{
	bool bHandle = true;
	switch(wsData.ulMessage)
	{
	case WIRELESS_OPTION_BT_ANSWERPIN:
		{
			ProcessAnswerPin((const AuthPinInfo*)wsData.pData);
		}
		break;
	default:
		bHandle = false;
		break;
	}

	return bHandle;
}

// 退出state
bool CBTRemoteReqPairState::ExitState(bool bIsPaired /*= false*/)
{
	if (!bIsPaired)
	{
		_BTData.SetDeviceState(m_lDeviceHandle, WIRE_DEVICE_INIT_STATE);
		_BTManager.UpdateUI(OP_PAIR_RESULT, false, m_lDeviceHandle);
	}

	CBaseState::ExitState(false);
}

#if IF_BT_CONNECTING_OVERTIME
//定时器处理
bool CBTRemoteReqPairState::ProcessTimer(int iTimerID)
{
	if (m_iTimer == iTimerID)
	{
		WIRE_INFO("Wait for dev [%d] connect result  over time", m_lDeviceHandle);
		ExitState(true);
		return true;
	}

	return false;
}
#endif // IF_BT_CONNECTING_OVERTIM

// 创建配对设备
bool CBTRemoteReqPairState::CreatePairDevice(unsigned long ulDeviceHandle)
{
	// 不管之前是什么状态, 收到配对请求重新设置状态
	return _BTData.AddDevice(ulDeviceHandle)
			&& _BTData.SetDeviceState(ulDeviceHandle, WIRE_DEVICE_PAIRING_STATE);
}

// 处理pin码请求
bool CBTRemoteReqPairState::ProcessRequestPin(unsigned long ulDeviceHandle, const char* pPinCode/* = NULL*/)
{
	DeviceInfo* pDevice = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);

/*#if IF_BT_CONNECTING_OVERTIME
	SetTimer(false, BT_PAIR_OVERTIMER);
#endif
*/
	if (NULL == pDevice
		//|| !_ISBTPHONE(pDevice->eType)
		|| pDevice->eState != WIRE_DEVICE_PAIRING_STATE)
	{
		WIRE_WARN("device[%d], it's can't pair. type[%d], state[%d]",
				 	ulDeviceHandle, pDevice->eType, pDevice->eState);
		return false;
	}
	
	if (_BTData.GetWaittingDeviceCount() > 0
		&& ulDeviceHandle != _BTData.GetWaittingDevice(false))
	{
		WIRE_WARN("have waitting device, do not req pin");
		return false;
	}

	yl::string strCode = commonAPI_FormatString("%s&%s", pPinCode, pDevice->strMac.c_str());
			WIRE_INFO("show pin code[%s]", strCode.c_str());

	etl_NotifyAppEx(false, BLUETOOTH_MSG_SHOW_PIN, pDevice->eType,
					pDevice->devHandle, strCode.length() + 1, (void*)strCode.c_str());

	return true;
}

// 处理pin码回复
bool CBTRemoteReqPairState::ProcessAnswerPin(const AuthPinInfo* pAuthInfo)
{
	if (!pAuthInfo || !pAuthInfo->bConfirmPair)
	{
		// 拒绝配对请求
		return ReplyPairReq(pAuthInfo->ulDeviceHandle, false);
	}

	DeviceInfo* pDevice = _BTData.GetDeviceInfoByHandle(pAuthInfo->ulDeviceHandle);
	if (!pDevice
		|| strlen(pAuthInfo->szPin) <= 0)
	{
		// 拒绝配对请求
		return ReplyPairReq(pAuthInfo->ulDeviceHandle, false);
	}

	WIRE_INFO("device[%d] answer pin[%s]", pAuthInfo->ulDeviceHandle, pAuthInfo->szPin);

/*#if IF_BT_CONNECTING_OVERTIME
	SetTimer(true, BT_PAIR_OVERTIMER);
#endif*/

	bool bResult = ReplyPairReq(pAuthInfo->ulDeviceHandle, true);
	_BTData.SetDeviceState(pAuthInfo->ulDeviceHandle, bResult ? WIRE_DEVICE_ANSWERPIN_STATE : WIRE_DEVICE_INIT_STATE);

	return true;
}

// 处理配对结果
bool CBTRemoteReqPairState::ProcessPairChange(const btkit_msg_t* pMsg)
{
	if (!pMsg)
	{
		return false;
	}

/*#if IF_BT_CONNECTING_OVERTIME
	SetTimer(false, BT_PAIR_OVERTIMER);
#endif*/

	DeviceInfo *pDeviceInfo = _BTData.GetDeviceInfoByHandle(pMsg->bt_callback_param.pair_info.dev_hnd);
	if (!pDeviceInfo || !m_pSession)
	{
		return false;
	}

	bool bPairedSucc = (pMsg->bt_callback_param.pair_info.paired == 1);
	WIRE_INFO("device[%s] pair change, state[%d]", pDeviceInfo->devHandle, bPairedSucc);

	if (!bPairedSucc)
	{
		return false;
	}

	if (!_BTData.IsDevicePaired(pDeviceInfo->eState))
	{
		if (_BTDriver.GetDeviceType(pDeviceInfo->devHandle) == BT_UNKNOWN_TYPE)
		{
			WIRE_WARN("Device type still unknown.");
			return false;
		}

		pDeviceInfo->bPaired = true;
		_BTData.SetDeviceState(pDeviceInfo->devHandle, WIRE_DEVICE_PAIRED_STATE);

		_BTData.UpdateProfilesActivity(pDeviceInfo->devHandle);
	}

	// 通知UI刷新列表界面
	_BTManager.UpdateUI(OP_UPDATE_LIST, true, pDeviceInfo->devHandle);

	return true;
}

// 回复配对请求
bool CBTRemoteReqPairState::ReplyPairReq(unsigned long ulDeviceHandle, bool bAccept)
{
	return _BTDriver.PinConfirm(ulDeviceHandle, bAccept);
}

#endif // IF_BLUETOOTH_SUPPORT_REMOTE_PAIR_REQ
#endif // IF_SUPPORT_BLUETOOTH
