#ifndef CDLGMTSWACDLIST_H
#define CDLGMTSWACDLIST_H

#include "config.h"
#if 1//IF_FEATURE_METASWITCH_ACD
#include "setting/include/common.h"
#ifdef _T48
#include "settingui/src/cdlgbasesubpage.h"
#else
#include "baseui/cdlgbasesubpage.h"
#endif
#include "broadsoft/callcenter/include/cccommon.h"

#include <QtGui/QComboBox>

class CDlgMTSWAcdList : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgMTSWAcdList(QWidget * parent = 0);
    ~CDlgMTSWAcdList();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgMTSWAcdList();
    }

public:
    //获取MLHG列表
    void GetMLHGList();

    //收到logic层返回的消息
    void OnReceiveMsg(const msgObject & objMessage);

    void InitData();

    virtual bool LoadPageData();

    virtual bool IsStatusChanged(); // 查看页面状态是否改变

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 设置页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    // 设置子页面不可编辑时的SoftKey
    virtual bool GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual void ExtraDialogInitData();

public Q_SLOTS:
    void OnShow();

protected:
    virtual void showEvent(QShowEvent * e);

    // 从服务器下载数据
    void DownloadMLGHList();

    // 是否所有组都登出，不包含登录登录的组
    bool IsAllLogout();

private:
    virtual bool OnAction(const QString & strAction);

    // 设置当前默认组
    bool SetCurrentMLHG(void);

    //获取ACD状态
    QString GetAcdStatusToShow();

private:
    // MLHG组列表数据
    YLVector<SMLHGData> m_vecMLGHData;

    int m_iFocusIndex;
};
#endif
#endif // CDLGMTSWACDLIST_H
