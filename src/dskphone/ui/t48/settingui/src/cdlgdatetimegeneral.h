#ifndef CDLGDATETIMEGENERAL_H
#define CDLGDATETIMEGENERAL_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QLineEdit>
#include <QComboBox>

class DataEditFrame;
class TimeEditFrame;
class qComboBoxEx;

class CDlgDateTimeGeneral : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgDateTimeGeneral(QWidget * parent = 0);
    ~CDlgDateTimeGeneral();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgDateTimeGeneral();
    }

    virtual void Init();

    virtual void Uninit();

public:
    //重新加载时区数据
    void UpdateTimeZoneListContent();

    //读取当前界面的值
    bool GetPageData(DateTimeGeneralData * pData);

    //通过控制层获取已有配置数据，通过V层在界面上显示出来
    virtual bool LoadPageData();

    //通过控制层获取配置数据，与GetPageData值比较，有改动时返回true
    virtual bool IsStatusChanged();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    //读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    //用于初始化数据。在构造函数中调用。设置控件的属性、样式、数据等都在其中进行
    virtual void InitData();

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 卸载界面上所有的输入法
    virtual void UninstallAllIME();

public slots:
    void OnCmbTypeChanged(int iIndex);

    void OnCmbDaylightSavingChanged(int iIndex);

    void SetCurrentTimezoneCmbIndex(yl::string strCurrentTimeZone);

    void UpdateTimeZoneCityListContent(int);

private:
    //根据页面类型刷新页面
    void RefreshByPageType(DateAndTimeType ePageType, bool bRefreshByDaylightSaving = false);

    void RefreshDateAndTime();

private:
    DataEditFrame * m_pDataFrame;
    TimeEditFrame * m_pTimeFrame;

    QComboBox   *   m_pCmbType;
    QComboBox   *   m_pCmbBoxTimeZone;
    QComboBox   *   m_pCmbBoxDaylightSaving;
    qComboBoxEx    *    m_pCmbLocation; //夏令时对应的城市

    QLineEdit   *   m_pEditNTPServer1;
    QLineEdit   *   m_pEditNTPServer2;

    DateAndTimeType m_ePageType;

    DateTimeGeneralData m_eDataTimeData;

    CSoftKeyBarData     m_eSoftKeyData;

    QDateTime m_pPreDatatime;
};

#endif // CDLGDATETIMEGENERAL_H
