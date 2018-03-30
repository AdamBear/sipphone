#ifndef _DLG_E911_SET_LOCATION_H
#define _DLG_E911_SET_LOCATION_H

#include "config.h"
#if IF_FEATURE_GENBAND_E911
#ifdef _T48
#include "settingui/src/cdlgbasesubpage.h"
#else
#include "baseui/cdlgbasesubpage.h"
#endif

class CE911LocationDelegate;

class CDlgE911SetLocation : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgE911SetLocation(QWidget *parent = 0);
    ~CDlgE911SetLocation();

    static CBaseDialog* CreateInstance()
    {
        return new CDlgE911SetLocation();
    }

public:
    virtual void BeforeSetSubPage(void* pData);
    virtual void Uninit();

    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged()
    {
        return false;
    }
    //保存数据
    virtual bool SavePageData()
    {
        return false;
    }

    //状态显示softkey没有保存项
    virtual bool GetSoftkeyData(CArraySoftKeyBarData& objSoftData);

    int ParseIdByClickAction(const yl::string& strClickAction);
    virtual void SetOpenSubPageAction(const yl::string& strSubPageAction);

#ifdef _T48
    virtual bool IsRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle = RETURN_IDLE_TYPE_NORMAL);

    virtual bool PorcessBackBtnClick();
    // 回到主界面
    virtual bool PorcessHomeBtnClick();

    virtual void OnReShow();
#endif

    // Action操作
    virtual bool OnAction(const QString& strAction);

    //事件过滤器
    virtual bool eventFilter(QObject *pObject, QEvent *pEvent);

protected:
    virtual void showEvent(QShowEvent* e);

#ifdef _T48
    virtual bool ProcessFunctionkey(int nKeyCode, bool bPress);
#endif

signals:
    void DialogAction(QString strAction);
    void DialogAction(QString strAction, QVariant var);
    void NormallyExit();

protected Q_SLOTS:

    void OnDialogListUpdate(int nFocusIndex);
    void OnDialogListExit();

#ifdef _T48
    virtual void OnDialogRefreshSoftkey();
#endif

private:
    //界面初始化
    void InitPageData();
    void OnNext();
    void OnSave();

private:
    CE911LocationDelegate*      m_pE911LocationDelegate;

private:
    BOOL m_bRefreshOnReshow;

};
#endif
#endif //_DLG_E911_SET_LOCATION_H
