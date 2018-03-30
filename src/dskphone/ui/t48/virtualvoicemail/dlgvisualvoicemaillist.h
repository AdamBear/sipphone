#ifndef _C_DLG_VISUAL_VOICEMAIL_LIST_H
#define _C_DLG_VISUAL_VOICEMAIL_LIST_H

#ifdef _T48
#include "settingui/src/cdlgbasesubpage.h"
#else
#include "baseui/cdlgbasesubpage.h"
#endif

#include "visualvoicemaillistdelegate.h"
#include "vvmfileplayercontroller.h"

class CDlgVisualVoiceMailList : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgVisualVoiceMailList(QWidget * parent = 0);
    ~CDlgVisualVoiceMailList();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgVisualVoiceMailList();
    }

public:
    virtual void BeforeSetSubPage(void * pData);
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
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objSoftData);

#ifdef _T48
    virtual bool IsRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle = RETURN_IDLE_TYPE_NORMAL);

    virtual bool PorcessBackBtnClick();
    // 回到主界面
    virtual bool PorcessHomeBtnClick();

    virtual void OnReShow();
#endif

    // Action操作
    virtual bool OnAction(const QString & strAction);

    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

#ifdef _T48
    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);
#endif

protected:
    virtual void showEvent(QShowEvent * e);

#ifdef _T48
    virtual bool ProcessFunctionkey(int nKeyCode, bool bPress);
#endif

signals:
    void DialogAction(QString strAction);
    void DialogAction(QString strAction, QVariant var);
    void NormallyExit();

protected Q_SLOTS:
    void OnListDialogOpenPage(VVM_DIALOG_TYPE eDialogPage);

    void OnDialogListUpdate(int nFocusIndex);
    void OnDialogListExit();

#ifdef _T48
    virtual void OnDialogRefreshSoftkey();
#endif

private:
    //界面初始化
    void InitPageData();
    void InitController();

private:
    CVisualVoiceMailListDelegate    *   m_pVisualVoiceMailListDelegate;

private:
    BOOL m_bRefreshOnReshow;

};

#endif //_C_DLG_VISUAL_VOICEMAIL_LIST_H
