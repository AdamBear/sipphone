#ifndef CDLGUPDATECONFIGURATIONPAGE_H
#define CDLGUPDATECONFIGURATIONPAGE_H

#include "cdlgbasesubpage.h"

class CDlgUpdateConfigurationPage : public CDlgBaseSubPage
{
    //��ťλ�ü���С
#define BTNSTARTX 100
#define BTNSTARTY 149
#define BTNWIDTH  349
#define BTNHEIGHT 53

    Q_OBJECT

public:
    CDlgUpdateConfigurationPage(QWidget * parent = 0);
    ~CDlgUpdateConfigurationPage();

public:
    //ͨ�����ƻ�ȡ�����������ݣ�ͨ��V���ڽ�������ʾ����
    virtual bool LoadPageData();

    //ͨ�����ƻ�ȡ�������ݣ���GetPageDataֵ�Ƚϣ��иĶ�ʱ����true
    virtual bool IsStatusChanged();

    //��ȡ��ǰ��������ã�ͨ��C�㱣��
    virtual bool SavePageData();

    //���ڳ�ʼ�����ݡ��ڹ��캯���е��á����ÿؼ������ԡ���ʽ�����ݵȶ������н���
    virtual void InitData();

    //����softkey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual void UpdateWnd();

    //ȡ��ʵ��
    static CBaseDialog * CreateInstance()
    {
        return new CDlgUpdateConfigurationPage();
    }

private slots:
    void OnBtnUpdateClicked();

private:
    qUIPushButton * m_qUIBtnUpdate;

};
#endif //CDLGUPDATECONFIGURATIONPAGE_H

