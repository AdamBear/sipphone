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

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual void SetData(void* pData = NULL);

    // 读取页面数据
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

    //获取Softkey数据
    virtual void GetSoftkey(CArraySoftKeyBarData& objSoftkeyData);

    virtual bool eventFilter(QObject *pObject, QEvent *pEvent);

    yl::string GetBLFKey()
    {
        return m_strKey;
    }
private:
    virtual bool OnAction(const QString& strAction);

    //处理cancle事件
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
