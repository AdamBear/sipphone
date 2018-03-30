#ifndef CDlgMTSWAcdDetail_H
#define CDlgMTSWAcdDetail_H

#include "config.h"
#if IF_FEATURE_METASWITCH_ACD
#include "setting/include/common.h"
#ifdef _T48
#include "settingui/src/cdlgbasesubpage.h"
#else
#include "baseui/cdlgbasesubpage.h"
#endif
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "broadsoft/callcenter/include/cccommon.h"
#include "uikernel/buttonwrapper.h"

#include <QtGui/QComboBox>

class CDlgMTSWAcdDetail : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgMTSWAcdDetail(QWidget * parent = 0);
    ~CDlgMTSWAcdDetail();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgMTSWAcdDetail();
    }


public:
    //收到logic层返回的消息
    void OnReceiveMsg(const msgObject & objMessage);

    virtual bool LoadPageData();

    virtual bool IsStatusChanged(); // 查看页面状态是否改变

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    void InitData();

    // 设置页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    // 设置子页面不可编辑时的SoftKey
    virtual bool GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

#if IF_SETTING_WND_PRE_NEXT_BTN
    // setting界面是否显示左右切换图标
    virtual bool ShowPreNextBtn()
    {
        return true;
    }
    // 向左翻页
    virtual void OnBtnPreClick();
    // 向右翻页
    virtual void OnBtnNextClick();
#endif

public Q_SLOTS:

    void OnShow();

protected:
    virtual void showEvent(QShowEvent * e);

    // 从服务器下载数据
    void DownloadMLGHDetail();

    //获取MLHG列表
    void GetMLHGDetail();

    // 按左右键翻页
    void OnPaging(bool bNext);

private:
    virtual bool OnAction(const QString & strAction);

private:
    QLabel * m_pLabelWaitingTime;
    QLabel * m_pLabelCallers;
    QLabel * m_pLabelMembers;
    QLabel * m_pLabelStatus;
};
#endif
#endif // CDlgMTSWAcdDetail_H
