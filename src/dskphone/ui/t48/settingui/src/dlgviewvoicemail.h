#ifndef DLGVOICEMAIL_H
#define DLGVOICEMAIL_H

#include "cdlgbasesubpage.h"

class CDlgViewVoiceMail : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgViewVoiceMail(QWidget * parent = 0);
    ~CDlgViewVoiceMail();
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgViewVoiceMail();
    }

public:
    // 读取页面数据
    virtual bool LoadPageData();
    //语音信箱呼出：未设置语音信箱号码，则会先跳转到语音信箱号码设置界面。
    bool VoiceMailCallOut(int nAccountId);

#if IF_FEATURE_BROADSOFT_VOICEMAIL
    bool EnterVisualVoiceMail(int nAccountId);
#endif

    // 查看页面状态是否改变
    virtual bool IsStatusChanged()
    {
        return false;
    }
    // 保存页面数据
    virtual bool SavePageData()
    {
        return true;
    }

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 获取softkey数据
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual bool OnAction(const QString & strAction);

    //根据账号id设置编辑框焦点
    void SetVoiceMailNumberFocus(int nAccountId);

private:
    //页面数据初始化
    void InitData();

    //点击item响应插槽函数
    void VoiceMailItemClicked(int nAccountId, bool bIsSetVoiceMailNumber);

private:
    std::vector<QLabel *> m_vecLabStatus;
    std::vector<int>
    m_vecAccountId;            //当只显示有效账号的时候存储有效账号的id
};

#endif // DLGVOICEMAIL_H
