#ifndef __UI_COMMON_FUN_H__
#define __UI_COMMON_FUN_H__

#include "basesettinguidelegate.h"
#include "settinguilogic/src/settinguidatavalidinfo.h"
#include "settinguicommondata.h"
#include "messagebox/messageboxcallback.h"

class CDlgSettingUI;
class xListView;

////////////////////////////////////////// 回调函数 begin ///////////////////////////////////////////
// 刷新界面
void RefreshUI(bool bLoadData = false, const yl::string & strPageId = "");
// 缓存控件数据
void CacheDlgData(const yl::string & strPageId = "");
// 刷新Softkey
void RefreshSoftkey(const yl::string & strPageId = "");
// 设置界面标题
void RefreshDlgTitle(const yl::string & strTitle, const yl::string & strPageId = "");
// 回退到上一个界面
void FallBackToPreviousPage(const yl::string & strPageId = "");
// 回退到Idle
void BackToIdle();
// 退出当前界面
void DropOutCurrentPage(bool bDelDlg = true, const yl::string & strPageId = "");
// 退出Setting界面
void DropOutSettingUI(const yl::string & strPageId = "");
// 绑定Delegate
void BindDelegate(CSettingUIDelegateBasePtr pDelegate, const yl::string & strPageId = "");
// 解绑Delegate
void UnBindDelegate(const yl::string & strPageId = "");
// 弹出Common弹出框
void PopCommonMessageBox(const yl::string & strPageId, const yl::string & strTitle,
                         const yl::string & strNote,
                         int eType, const yl::string & strCancelBtnTitle,
                         const yl::string & strConfirmBtnTitle, int nMsTime,
                         int iID, void * pExtraData,
                         CMsgBoxCallBackAction * pAction);
// 数据不合法处理
void DealWithValidData(CSettingUIDataValidInfo & dataValid, const yl::string & strPageId = "");

// 进入界面
bool EnterPage(const PageOpenData & openPageData);

#endif

