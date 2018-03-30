/************************************************************************
 * FileName  : CDlgSCAMenu.h (header file)
 * Purpose   :
 * Date      : 2013/01/15 20:41:19
 ************************************************************************/

#ifndef _CDLGSCAMENU_H_
#define _CDLGSCAMENU_H_

#include "settingui/src/cdlgbasesubpage.h"
#include "interfacedefine.h"
#include "talk/broadsoftsca/include/scamanager_def.h"

class CDlgSCAMenu : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgSCAMenu();
    }

public:
    CDlgSCAMenu(QWidget * parent = 0);

    virtual ~CDlgSCAMenu();

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual void SetData(void * pData = NULL);

    // 读取页面数据
    virtual bool LoadPageData();

    // 查看页面状态是否改变
    virtual bool IsStatusChanged();

    //http://10.3.5.199/Bug.php?BugID=65420
    //实际上  数据是不可能为空的
    //获取Softkey数据
    virtual void GetSoftkey(CArraySoftKeyBarData & objSoftkeyData);

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

public:
    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

public Q_SLOTS:
    void OnProcessPageAction(const QString & strAction);
    void OnRefreshSoftkey();

private:
    virtual bool OnAction(const QString & strAction);
    virtual bool SavePageData();    // 保存页面数据

    // 更新Softkey
    VOID UpdateSoftKey();

    // 获取当前焦点对应的账号分线信息
    SCA_Info * GetInfoByID(UINT uIndex);

    // 根据SCA数据，获取SCA显示内容
    // | refSCAList | [in] SCA菜单数据
    // 返回是否获取成功
    BOOL SetSCAMenuData(ListSCAInfo & refSCAList, int iAccountID);

    // 根据SCA状态获取状态显示名
    yl::string GetNameByStatus(BLF_STATUS_CODE eStatus);
    // 获取SCA账号分线的显示名
    yl::string GetDisplayName(SCA_Info * pInfo);

    // 获取关联的账号序号
    int GetBindAccountID();

    // 子界面响应Back按钮的操作
    virtual void OnBackButtonClick();

    // 子界面响应Home按钮的操作
    virtual void OnHomeButtonClick();

private:
    //处理new call事件
    void OnNewCallPress();
    //处理cancle事件
    void OnCancel();
    //处理Answer事件
    void OnAnswerPress();
    //处理Reject事件
    void OnRejectPress();
    //处理switch事件
    void OnSwitchPress();
    //处理bargein事件
    void OnBargeInPress();
    //处理resume事件
    void OnResumePress();
    //处理callpull事件
    void OnCallPullPress();
    //处理callpull事件
    void OnRetrievePress();

    void OnSilentBargeInPress();

    void InitData();

private Q_SLOTS:
    // 定时器处理函数
    void OnTimer();

private:
    ScaMenuData    *    m_pScaMenuData;     // SCA状态的菜单数据

    //自动退出定时器
    QTimer       *      m_pQuitTimer;
    int                 m_nReturnInterval;
};

#endif
