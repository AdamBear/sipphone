#ifndef __BASE_SETTING_DIALOG_H__
#define __BASE_SETTING_DIALOG_H__

#include "settinguilogic/include/settinguicommondata.h"
#include "settinguilogic/src/settinguidatavalidinfo.h"
#include "xwindow-kernel/include/xmessagehandler.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/messageboxmanager.h"
#include "uimanager/basedialog.h"
#include "ETLLib/ETLLib.hpp"
#include "messagebox/messageboxdefine.h"


class CSettingUIDelegateBase;
class xView;
class CTitleBase;

enum
{
    SETTTING_DIALOG_TYPE_NONE       = 0,
    SETTTING_DIALOG_TYPE_MASK       = 0xff000000,
};

class CSettingDialogBase : public CBaseDialog
{
public:
    CSettingDialogBase(xView * pDialogView, EWindowType eWndType = eWT_PageMenu);
    virtual ~CSettingDialogBase();

public:
    virtual void Init() {}
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit() {}

    // 绑定Delegate
    virtual void BindDelegate(CSettingUIDelegateBase * pDelegate);
    // 解绑Delegate
    virtual void UnBindDelegate();
    // 刷新界面，bReLoadData为true时，重新从配置中读取数据刷新；否则，读取缓存数据刷新
    virtual void RefreshUI(bool bReLoadData = false);
    // 刷新Softkey
    virtual void RefreshSoftkey();
    //刷新Title
    virtual void RefreshTitle();
    virtual void RefreshTitle(const yl::string & strTitle);
    // 退出当前界面
    virtual void DropOutCurrentPage(bool bDelDlg = true);
    // 回退到上一层界面
    virtual void FallBackToThePreviousInterface();
    // 缓存当前界面数据
    virtual void CachePageData() {}
    // 保存时数据校验，非法数据处理函数
    virtual void DealWithDataValid(CSettingUIDataValidInfo & dataValid);
    // 相关提示语弹出框
    virtual void PopCommonMessageBox(const yl::string & strTitle, const yl::string & strNote,
                                     int eType, const yl::string & strCancelBtnTitle = "",
                                     const yl::string & strConfirmBtnTitle = "", int nMsTime = -1,
                                     int iID = -1, void * pExtraData = NULL, CMsgBoxCallBackAction * pAction = NULL);
    // 弹出框回调处理
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    // 根据ItemID设置界面焦点项
    virtual void UpdateFocus(const yl::string & strItemId, bool bFocus) {}

    virtual bool ProcessDeactiveKeyPress(int nKey, bool bPress, bool & bResult)
    {
        return false;
    };

    virtual bool OnSoftkeyAction(int iType, bool bLongPress);

public:
    // 加载界面数据，若bReLoadData为true，从配置中读取；否则，从内存中读取
    virtual bool LoadPageData(bool bReLoadData = false);
    // 保存界面数据到配置中
    virtual bool SavePageData();
    // 处理界面事件
    virtual bool OnAction(int eActionType, int nDataIndex);
    // 打开界面附加操作
    virtual void InitExtraData(void * pData);
    // 创建界面后初始化
    virtual bool DoInitAfterCreateDialog(const yl::string & strPageID)
    {
        return true;
    }
    // 删除界面前资源释放
    virtual bool DoUninitBeforeDeleteDialog()
    {
        return true;
    }
    // 当前状态是否可成为活动窗口
    virtual bool CanBeActivityDialog()
    {
        return true;
    }

    /////////////////////////////////////////// CBaseDialog ///////////////////////////////////////////
public:
    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle();
    //回到Idle前的处理函数
    virtual void OnReturnToIdle();

    ////////////////////音量条///////////////////////
    // 获取当前页面音量条的数据
    // 返回值：是否显示音量条,默认不显示音量条
    // iVolumeType | [out] 音量条的类型
    // iMin | [out] 音量条的最小值
    // iMax | [out] 音量条的最大值
    // strBGPath | [out] 音量条的背景图片
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, const yl::string & strPix)
    {
        return true;
    }

    //用于判断该窗口当前状态是否可被移除
    virtual bool IsSubDialogRemovable();

/////////////////////////////////////////// 界面属性 ///////////////////////////////////////////
public:
    // 加入大界面后，是否设置为活动窗口
    virtual bool IsSetActiveWhileAddToBigWnd()
    {
        return false;
    }
    // 获取界面ID
    const yl::string & GetPageID() const;

    CSettingUIDelegateBase * GetBindDelegate()
    {
        return m_pSettingUIDelegate;
    }
    CSettingUIDelegateBase * GetFirstBindDelegate()
    {
        return m_pFirstBindDelegate;
    }

    // 现在是否只绑定1个Delegate
    bool IsOnlyBindOneDelegate();
    // 清空首个绑定的Delegate
    void ClearFirstBindDelegate();

    int GetDialogType() const
    {
        return m_nDialogType;
    }

    /////////////////////////////////////////// 其他函数 ///////////////////////////////////////////
protected:
    // Setting弹出框类型与MessageBox转换
    MESSAGEBOX_TYPE SettingType2MsgType(SETTING_MESSAGEBOX_TYPE eSetType);
    SETTING_MESSAGEBOX_RESULT MsgResultType2SettingResultType(MESSAGEBOX_RESULT eResultType);
    // Setting弹出框Icon显示类型与MessageBox转换
#if IF_MSG_BOX_SHOW_ICON
    MSGBOX_ICON_TYPE SettingIconType2MsgIconType(SETTING_MESSAGEBOX_ICON_TYPE eSetIconType);
    SETTING_MESSAGEBOX_ICON_TYPE MsgIconType2SettingIconType(MSGBOX_ICON_TYPE eMsgIconType);
#endif

    void BindTitle(xView * pTitle);

protected:
    CSettingUIDelegateBase     *    m_pSettingUIDelegate;           // 绑定的Delegate
    CSettingUIDelegateBase     *    m_pFirstBindDelegate;           // 绑定的第1个Delegate
    CTitleBase           *          m_pTitle;
    int                             m_nDialogType;
};

#endif // __BASE_SETTING_DIALOG_H__
