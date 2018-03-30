#ifndef _DLG_E911_LOCATION_H
#define _DLG_E911_LOCATION_H

#include "config.h"
#if IF_FEATURE_GENBAND_E911
#ifdef _T48
#include "settingui/src/cdlgbasesubpage.h"
#else
#include "baseui/cdlgbasesubpage.h"
#endif

class CE911LocationDelegate;

class CDlgE911Location : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgE911Location(QWidget *parent = 0);
    ~CDlgE911Location();

    void InitData();
    static CBaseDialog* CreateInstance()
    {
        return new CDlgE911Location();
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

#ifdef _T48
    virtual bool IsRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle = RETURN_IDLE_TYPE_NORMAL);

    virtual bool PorcessBackBtnClick();
    // 回到主界面
    virtual bool PorcessHomeBtnClick();

    virtual void OnReShow();
#endif

    void UpdateText();

    // Action操作
    virtual bool OnAction(const QString& strAction);

protected:
    virtual void showEvent(QShowEvent* e);

#ifdef _T48
    virtual bool ProcessFunctionkey(int nKeyCode, bool bPress);
#endif
signals:
    void DialogAction(QString strAction);
    void DialogAction(QString strAction, QVariant var);
    void NormallyExit();

#ifdef _T48
    //virtual void OnDialogRefreshSoftkey();
#endif

private:
    BOOL m_bRefreshOnReshow;

};
#endif
#endif //_DLG_E911_LOCATION_H
