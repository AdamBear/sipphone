#ifndef CDLGANYWHERELOCATIONEDIT_H
#define CDLGANYWHERELOCATIONEDIT_H

#include "broadsoft/anywhere/include/anywherecommon.h"
#include "cdlgbasesubpage.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"

class QLineEdit;
class qCheckBox;

class CDlgAnyWhereLocationEdit : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgAnyWhereLocationEdit(QWidget * parent = 0);
    ~CDlgAnyWhereLocationEdit();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgAnyWhereLocationEdit();
    }

public:

    //设置页面数据
    void SetDetailData(const SAYWLocation & propSet);

    //获取页面数据
    SAYWLocation GetPageData();

    //////////基类纯虚函数 该界面未用///////////////
    virtual bool LoadPageData();

    ///////////////////////////////////////////////

    void LoadPageData(const yl::string & strPhoneNumber);

    virtual bool IsStatusChanged();

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    // 对输入框的数据进行验证
    virtual bool IsInputDataValid();

    virtual void InitData();

    virtual bool OnAction(const QString & strAction);

    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    virtual void
    OnCustomBack();            // 用于自定义退出界面操作（只回退一个界面）

    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

protected:
    void OnDeleteAnyWhere();

private:
    //判断是否Sip URI格式的号码
    BOOL CheckIsSipURI(const string & strNumber);

private:

    QLineEdit * m_pEditName;
    QLineEdit * m_pEditNumber;
    QLineEdit * m_pEditOutBndNumber;

    qCheckBox * m_pCheckBoxActive;      //Enabel this Location
    qCheckBox * m_pCheckBoxEnabInhb;    //Enable Diversion Inhibitor
    qCheckBox * m_pCheckBoxReqAnsConf;  // Required Answer Confirmation
    qCheckBox * m_pCheckBoxCallCtrl;    //User BroadWorks-based Call ...

    SAYWLocation m_LocationData;

};

#endif // CDLGANYWHERELOCATIONEDIT_H
