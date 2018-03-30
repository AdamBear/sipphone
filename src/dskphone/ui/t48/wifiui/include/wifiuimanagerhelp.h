#ifndef __WIFI_UI_MANAGER_HELP_H
#define __WIFI_UI_MANAGER_HELP_H

#include <string>
#include <QString>
#include "messagebox/messageboxbase.h"
#include "messagebox/messageboxcallback.h"

class CWifiUIManagerHelp : public CMsgBoxCallBackAction
{
public:
	static CWifiUIManagerHelp & GetInstance();

	void InitMsg();
	void UnInitMsg();

	LRESULT InterMessageHelp(msgObject & refMessage);

private:
	CWifiUIManagerHelp();
	~CWifiUIManagerHelp();

	// 处理弹出框回调
	virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

private:
	bool        m_bPopCountDownDlg;                     // count down pop dialog exist or not
};

#define _UIWifiManagerHelp (CWifiUIManagerHelp::GetInstance())

#endif
