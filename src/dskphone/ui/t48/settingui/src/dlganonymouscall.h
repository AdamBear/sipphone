#ifndef DLGANONYMOUSCALL_H
#define DLGANONYMOUSCALL_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>

class qCheckBox;
struct AnonymousCallData;

class CDlgAnonymousCall : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgAnonymousCall(QWidget * parent = 0);
    ~CDlgAnonymousCall();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgAnonymousCall();
    }

public:
    //加载数据
    virtual bool LoadPageData();
    //获取数据状态是否改变
    virtual bool IsStatusChanged();
    //保存数据
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid(InputDataErrorInfo & objErrorContent);

private:
    //界面初始化
    virtual void InitData();
    //拷贝页面数据
    void CopyPageData(AnonymousCallData & dest_anonymousCallData,
                      const AnonymousCallData & src_anonymousCallData);
    //比较页面前后数据
    bool ComparePageData(const AnonymousCallData & before_anonymousCallData,
                         const AnonymousCallData & after_anonymousCallData) const;
    //取得当前页面数据
    AnonymousCallData & GetCurrentPageData(AnonymousCallData & current_anonymousCallData) const;
    //设置当前页面数据
    void SetCurrentPageData(AnonymousCallData & current_anonymousCallData);

    void InitListItem();

public:
    //用于记载当前页面的数据，并且记忆数据用于比较
    AnonymousCallData m_anonymousCallData;

    qCheckBox * m_pCheckBoxAnonymousCall;
    qCheckBox * m_pCheckBoxCode;
    qCheckBox * m_pCheckBoxReject;
    qCheckBox * m_pCheckBoxAnonymousRejectCode; // Send RejectCode
    QLineEdit * m_pEditCallOnCode;
    QLineEdit * m_pEditCallOffCode;
    QLineEdit * m_pEditRejectOnCode;
    QLineEdit * m_pEditRejectOffCode;

    bool m_bInitSuccess;
};

#endif // DLGANONYMOUSCALL_H
