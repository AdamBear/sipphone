#ifndef CDLG_DSSKEYFRAME_H
#define CDLG_DSSKEYFRAME_H


#include "cdlgbasesubpage.h"
#include <QDialog>
#include "dsskeyui/src/dsskeymanager.h"

// 前向声明
class qDsskeyFrame;

class CDlgDsskeyFrame : public CDlgBaseSubPage, public CAbstractDsskeyBtnAction
{
    Q_OBJECT

public:
    explicit CDlgDsskeyFrame(QWidget * parent = NULL);
    ~CDlgDsskeyFrame();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgDsskeyFrame();
    }

    virtual void OnDsskeyAction(int iDsskeyID);

private:
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    // 设置子页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    // 初始化悬浮框的参数
    void InitSettingDsskeyFrame();

    virtual void showEvent(QShowEvent * pEvent);

private:
    qDsskeyFrame  * m_pSettingDsskey;
};

#endif // CDLG_DSSKEYFRAME_H
