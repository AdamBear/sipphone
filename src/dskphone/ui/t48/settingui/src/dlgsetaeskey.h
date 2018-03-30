#ifndef DLGSETAESKEY_H
#define DLGSETAESKEY_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

class qCheckBox;
struct AesKeyData;

class CDlgSetAESKey : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgSetAESKey(QWidget * parent = 0);
    ~CDlgSetAESKey();

public:
    //通过C层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过C层获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgSetAESKey();
    }

private:
    //比较页面前后数据
    bool ComparePageData(const AesKeyData & beforePageData, const AesKeyData & afterPageData) const;
    //取得当前页面数据
    AesKeyData & GetCurrentPageData(AesKeyData & currentPageData) const;
    //设置当前页面数据
    void SetCurrentPageData(AesKeyData & currentPageData);

private:
    //用于记录加载时数据
    AesKeyData m_AesKeyData;

    QLineEdit * m_pEditCommon;
    QLineEdit * m_pEditMACOriented;
};

#endif // DLGSETAESKEY_H
