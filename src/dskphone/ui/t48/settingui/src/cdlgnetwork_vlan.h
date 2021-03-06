#ifndef CDLGNETWORK_VLAN_H
#define CDLGNETWORK_VLAN_H

#include "cdlgbasesubpage.h"
#include <QDialog>

struct NetworkVlanData;
class qCheckBox;

class CDlgNetwork_VLAN : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgNetwork_VLAN(QWidget * parent = 0);
    ~CDlgNetwork_VLAN();

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgNetwork_VLAN();
    }

public:

    //读取当前界面的值
    //bool GetPageData(* pData);

    //通过控制获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过控制获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

private:
    //比较页面前后数据
    bool ComparePageData(const NetworkVlanData & beforePageData,
                         const NetworkVlanData & afterPageData) const;
    //取得当前页面数据
    NetworkVlanData & GetCurrentPageData(NetworkVlanData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(NetworkVlanData & currentPageData);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

private:
    qCheckBox * m_qCheckBoxVLAN;

    QLineEdit * m_pEditVID_Number_WAN;
    QLineEdit * m_pEditPriority_WAN;

    //表示是WAN Port页面还是PC Port页面(默认为WAN Port)
    int        m_iPageType;
};

#endif // CDLGNETWORK_VLAN_H
