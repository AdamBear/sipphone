#ifndef __UI_CALL_BACK_FUN_DEFINE_H__
#define __UI_CALL_BACK_FUN_DEFINE_H__

#include "basesettinguidelegate.h"
#include "settinguilogic/src/settinguidatavalidinfo.h"
#include "settinguicommondata.h"
#include "messagebox/messageboxcallback.h"

// 刷新界面
typedef void (*Fun_RefreshUI)(bool bLoadData, const yl::string & strPageId);
// 缓存界面数据
typedef void (*Fun_CacheDlgData)(const yl::string & strPageId);
// 刷新Softkey
typedef void (*Fun_RefreshSoftkey)(const yl::string & strPageId);
// 刷新Title
typedef void (*Fun_RefreshDlgTitle)(const yl::string & strTitle, const yl::string & strPageId);

// 回退到上级界面
typedef void (*Fun_FallBackToPrevousPage)(const yl::string & strPageId);

// 回退到Idle
typedef void (*Fun_BackToIdle)();

// 退出当前界面
typedef void (*Fun_DropOutCurrentPage)(bool bDelDlg, const yl::string & strPageId);

// 退出Setting界面
typedef void (*Fun_DropOutSettingUI)(const yl::string & strPageId);

// 进入界面
typedef bool (*Fun_EnterPage)(const PageOpenData & openPageData);

// 绑定Delegate
typedef void (*Fun_BindDelegate)(CSettingUIDelegateBasePtr pNewDelegate,
                                 const yl::string & strPageId);
// 解绑Delegate
typedef void (*Fun_UnBindDelegate)(const yl::string & strPageId);

// 显示Common弹出框
typedef void (*Fun_PopCommonMessageBox)(const yl::string & strPageId, const yl::string & strTitle,
                                        const yl::string & strNote,
                                        int eType, const yl::string & strCancelBtnTitle,
                                        const yl::string & strConfirmBtnTitle, int nMsTime,
                                        int iID, void * pExtraData, CMsgBoxCallBackAction * pAction);
// 数据不合法提示
typedef void (*Fun_DealWithValidData)(CSettingUIDataValidInfo & dataValid,
                                      const yl::string & strPageId);

// 设置子Tile为活动窗口
typedef void (*Fun_SetSubTileActive)(const yl::string & strPageId);



#endif
