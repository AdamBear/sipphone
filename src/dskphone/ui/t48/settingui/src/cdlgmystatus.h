#ifndef CDLGMYSTATUS_H
#define CDLGMYSTATUS_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"

struct MystatusItemData
{
    int         iIndex;
    yl::string strDisplayName;
    yl::string strAcction;

    MystatusItemData()
    {
        Reset();
    }

    void Reset()
    {
        iIndex = 0;
        strDisplayName = "";
        strAcction = "";
    }
};

// 重命名
typedef std::list<MystatusItemData>                 ListMystatusData;
typedef std::list<MystatusItemData>::const_iterator ListMystatusItemCIter;

class CDlgMyStatus : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgMyStatus(QWidget * parent = 0);
    ~CDlgMyStatus();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgMyStatus();
    }

private:
    //通过C层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    virtual bool IsStatusChanged()
    {
        return false;
    }

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    int GetPresenceStatus();

    bool m_bChangeStatus;
};

#endif // CDLGMYSTATUS_H
