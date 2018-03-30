#ifndef CDLGSENDTOWINDOW_H
#define CDLGSENDTOWINDOW_H

#include "settingui/src/cdlgbasesubpage.h"
#include <QTextEdit>

class QComboBox;
class QLineEdit;
class QStringList;

class CDlgMessageView : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgMessageView();
    }

public:
    virtual void Init();

    virtual void Uninit();

    virtual bool LoadPageData()
    {
        return true;
    };   // 读取页面数据
    virtual bool IsStatusChanged()
    {
        return false;    // 查看页面状态是否改变
    }
    virtual bool SavePageData()
    {
        return false;    // 保存页面数据
    }

public:
    //设置子界面softkey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

    virtual void paintEvent(QPaintEvent * event);

public:
    //设置消息窗口的详细信息
    void SetMessageDetail(const TextMsgItemDetailData & sDetail);

private:
    void InitData();

private:
    virtual bool OnAction(const QString & strAction);

    //打开编辑当前消息的窗口
    void OpenEditPage();

private:
    CDlgMessageView(QWidget * parent = 0);
    virtual ~CDlgMessageView();

private:
    //回复
    void OnReplyBtnClicked();
    //转发
    void OnForwardBtnClicked();
    //删除
    void OnDeleteBtnClicked();

public:
    //退出
    void OnBtnBackClick();

private:
    //显示发件人、收件人、时间的标签
    QLabel   *  m_pLabelFromAddr;
    QLabel   *  m_pLabelToAddr;
    QLabel   *  m_pLabelTime;

    //显示消息内容
    QTextEdit * m_pTextEditor;

    //当前阅读的短信的索引
    int m_iMsgIdx;

    //当前box类型
    yl::string m_strCurrentBox;
};

#endif // CDLGSENDTOWINDOW_H
