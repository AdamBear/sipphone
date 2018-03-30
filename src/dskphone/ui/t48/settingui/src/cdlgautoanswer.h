#ifndef CDLG_AUTO_ANSWER_H
#define CDLG_AUTO_ANSWER_H

#include "setting/include/common.h"
#include "cdlgbasesubpage.h"
#include <QDialog>
#include <list>

class CDlgAutoAnswer : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgAutoAnswer(QWidget * parent = 0);
    ~CDlgAutoAnswer();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgAutoAnswer();
    }

    //从V层读取当前界面的值
    bool GetPageData(int & nIndex);

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
private:
    std::vector<int>
    m_vecAccountId;            //当只显示有效账号的时候存储有效账号的id
};


#endif //CDLG_AUTO_ANSWER_LINE_SELECT_H
