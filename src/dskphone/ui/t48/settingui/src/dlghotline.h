#ifndef DLGHOTLINE_H
#define DLGHOTLINE_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>
#include "setting/src/advancedcallfeaturecontroller.h"

struct AdvancedCallFeatureHotlineData;

typedef struct HotlinePageDataType
{
    QString m_strNumber;
    QString m_strDelay;
} HotlinePageData;

class CDlgHotline : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    CDlgHotline(QWidget * parent = 0);
    ~CDlgHotline();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgHotline();
    }

public:
    //界面初始化
    void InitData();
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();
    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

    // 重写接受按钮消息
    virtual void OnExtraAction(int eActionType);
    // 写入联系人数据
    void writeData(QString name, QString number);

private:
    //拷贝页面数据
    void CopyPageData(HotlinePageData & dest_HotlinePageData,
                      const HotlinePageData & src_HotlinePageData);
    //比较页面前后数据
    bool ComparePageData(const HotlinePageData & before_HotlinePageData,
                         const HotlinePageData & after_HotlinePageData) const;
    //取得当前页面数据
    HotlinePageData & GetCurrentPageData(HotlinePageData & current_HotlinePageData) const;
    //设置当前页面数据
    void SetCurrentPageData(HotlinePageData & current_HotlinePageData);

    /*private:*/
public:
    HotlinePageData m_hotlinePageData;
    AdvancedCallFeatureHotlineData m_featureHotlineData;

private:
    QLineEdit   *   m_pEditNumber;
    QLineEdit   *   m_pEditHotlineDelay;

};

#endif // DLGHOTLINE_H
