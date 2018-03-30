#ifndef DLGWIFIDETAIL_H
#define DLGWIFIDETAIL_H

#include "settingui/src/cdlgbasesubpage.h"
#include "baseui/titlebutton.h"
#include "baseui/iframetitleaction.h"

class WifiSignalItem;
class QTimer;
class NoteLabel;

class CDlgWifiDetail : public CDlgBaseSubPage, public IFrameTitleAction
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgWifiDetail();
    }

public:
    CDlgWifiDetail(QWidget * parent = NULL);
    ~CDlgWifiDetail(void);

    // 初始化
    virtual void Init();

    virtual bool LoadPageData();
    virtual bool IsStatusChanged()
    {
        return false;
    }
    virtual bool SavePageData()
    {
        return false;
    }

    //获取Softkey数据
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData
#ifdef _T49
                                , QObject * pSignalReceiver
#endif
                               );

    // 设置连接结果提示
    void SetConnectResult(int iResult);
    void SetDisConnectResult(int iResult);

    // 标题框Action处理
    virtual void OnTitleAction(CFrameList * pFrameList, CFrameListTitle * pTitle, int nAction);

    void Load();
    void PopPwdDialog(bool bAuthentication = false);
    int GetID()
    {
        return m_iCurNetID;
    }
    /**************************************** used only in class *******************************************/

private Q_SLOTS:
    void OnSubmitNameAndPassword(const yl::string & strName, const yl::string & strPswd);
    void OnSubmitPassword(const yl::string & strPswd);
    void OnCancelPassword();

protected:
    void DoRelayout();
    bool JudgeWidgetNULL();

private:
    void InitData();
#ifdef _T49
private slots:
    void OnTimeOut();
#endif

private:
    QLabel         *        m_pLabSSID;
#ifdef _T49
    int                     m_iSignalLevel;
#else
    QLabel         *        m_pLabSignalStrength;
#endif
    QLabel         *        m_pLabSecurityMode;
    QLabel         *        m_pLabEncryptionType;
    QLabel         *        m_pLabFrequence;
    QLabel         *        m_pLabProfileName;
    QLabel         *        m_pLabAPMacAddress;
    bool                    m_bConnectOrNot;
    int                     m_iCurNetID;
#ifdef _T49
    QTimer         *        m_pTimer;
#else
    NoteLabel       *       m_pLabNote;
#endif
};

#endif
