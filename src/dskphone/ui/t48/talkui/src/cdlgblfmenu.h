#ifndef _CDLGBLFMENU_H_
#define _CDLGBLFMENU_H_

#if 1//IF_FEATURE_SHOW_BLF_CALLID
#include "settingui/src/cdlgbasesubpage.h"
#include "interfacedefine.h"

class CDlgBLFMenu : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    static CBaseDialog* CreateInstance()
    {
        return new CDlgBLFMenu();
    }

public:
    CDlgBLFMenu(QWidget* parent = 0);

    virtual ~CDlgBLFMenu();

    // ��ʼ��
    virtual void Init();
    // �ͷ����ݣ���������ջظ�����ʼ��״̬
    virtual void Uninit();

    virtual void SetData(void* pData = NULL);

    // ��ȡҳ������
    virtual bool LoadPageData();

    virtual bool IsStatusChanged()
    {
        return false;
    }

    virtual bool SavePageData()
    {
        return false;
    }

    virtual QPixmap GetCustomBackGround();

    //��ȡSoftkey����
    virtual void GetSoftkey(CArraySoftKeyBarData& objSoftkeyData);

    virtual bool eventFilter(QObject *pObject, QEvent *pEvent);

    yl::string GetBLFKey()
    {
        return m_strKey;
    }
private:
    virtual bool OnAction(const QString& strAction);

    //����cancle�¼�
    void OnCancel();

    bool SetBLFMenuData(int iDsskeyId);

    yl::string GetNameByStatus(int iStatus);

private:
    void InitData();

private:
    yl::string  m_strKey;
};

#endif
#endif
