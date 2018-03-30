#ifndef CDLGANYWHERELOCATIONLIST_H
#define CDLGANYWHERELOCATIONLIST_H

#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "broadsoft/anywhere/include/anywherecommon.h"
#include "cdlgbasesubpage.h"


class CDlgAnyWhereLocationList : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    explicit CDlgAnyWhereLocationList(QWidget * parent = 0);
    ~CDlgAnyWhereLocationList();

    static CBaseDialog * CreateInstance()
    {
        return new CDlgAnyWhereLocationList();
    }

public:
    //清除所有item
    void ClearPageData();

    //设置页面内容
    void SetLocationList(SAYWLocationArray & objList);

    //获取制定列的数据
    bool getLocationData(SAYWLocation & propSet, int idx) const;

    //获取当前选中项的数据
    bool GetSelectedLocationData(SAYWLocation & propSet) const;

    //删除制定号码的location
    void RemoveLocationItem(const yl::string & strPhoneNum);

    //////////基类纯虚函数 该界面未用///////////////
    virtual bool LoadPageData();
    virtual bool IsStatusChanged();
    ////////////////////////////////////////////////

    //从V层读取当前界面的配置，通过C层保存
    virtual bool SavePageData();

    virtual void InitData();

    // 设置小窗口数据
    virtual void SetData(void * pData = NULL);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 设置页面的SoftKey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    // 设置子页面前的操作
    virtual void BeforeSetSubPage(void * pData);

    // 窗口初始化完成后，需要额外的操作，比如一些阻塞或同步的动作
    virtual void ExtraDialogInitData();

    virtual void
    OnCustomBack();            // 用于自定义退出界面操作（只回退一个界面）

    virtual void ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox);

protected:
    virtual void showEvent(QShowEvent * e);

private:
    void OnEditAnyWhere();
    //void OnDeleteAnyWhere();
    void OnDeleteAllAnyWhere();

private:
    //设置数据
    void SetListData(std::vector<SAYWLocation> & orderVector);

    virtual bool OnAction(const QString & strAction);
private:
    QVector<QLabel *> m_vecLabel;

    std::vector<bool> m_vecbActiveChanged;
    std::vector<SAYWLocation> m_DataList;

    //当前账号ID
    int m_nAccountID;
};

#endif // CDLGANYWHERELOCATIONLIST_H
