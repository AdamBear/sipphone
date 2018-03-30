#pragma once

#include "setting/include/common.h"
#include "settingui/src/cdlgbasesubpage.h"
#include <QDialog>

class CDlgSilentAlerting : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgSilentAlerting(QWidget * parent = 0);
    ~CDlgSilentAlerting();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgSilentAlerting();
    }

public:

    // ��ʼ��
    virtual void Init();
    // ����ʼ��
    virtual void Uninit();
    //�յ�logic�㷵�ص���Ϣ
    void OnReceiveMsg(msgObject & objMessage);

    //��ȡFWDNotReachData����
    void GetSilentAlertingData();

    virtual bool LoadPageData();

    virtual bool IsStatusChanged();

    //��V���ȡ��ǰ��������ã�ͨ��C�㱣��
    virtual bool SavePageData();

    // �����������ݽ�����֤
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    virtual void InitData();

    // ������ҳ��ǰ�Ĳ���
    virtual void BeforeSetSubPage(void * pData);

    // ��ȡsoftkey����
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

    // �������뵯��
    bool ProcessMsgBoxPassword(int nLineId);

public Q_SLOTS:
    // ������ύ��Ӧ����
    void OnSubmitXSIPassword(const yl::string & strPswd);

    // �����ȡ����Ӧ����
    void OnCancelInputXSIPassword();

    void OnShow();

protected:
    virtual void showEvent(QShowEvent * e);


private:
    bool       m_bIsSilent;
    int        m_iAccountId;

    //��ʾ�Ǽ��ػ��Ǳ���
    bool m_bIsInLoad;


private:
    QComboBox * m_pCmbBoxStatus;
};

