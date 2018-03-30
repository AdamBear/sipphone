#ifndef DLGMESSAGE_H
#define DLGMESSAGE_H
#include "settingui/src/cdlgbasesubpage.h"

struct TextMsgItemDetailData;

///////////////////////////////////////////////////////////////////////////////
class CDlgMessageEdit : public CDlgBaseSubPage
{
    Q_OBJECT

public:
    //取得实例
    static CBaseDialog * CreateInstance()
    {
        return new CDlgMessageEdit();
    }

private:
    void InitData();

public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual bool LoadPageData()
    {
        return false;
    };  // 读取页面数据
    virtual bool IsStatusChanged()
    {
        return false;
    }; // 查看页面状态是否改变
    virtual bool SavePageData()
    {
        return false;
    };  // 保存页面数据

    //退出
    virtual void OnBtnBackClick();

    virtual bool PorcessHomeBtnClick();

    // 重写接受按钮消息
    virtual void OnExtraAction(int eActionType);
    // 写入联系人数据
    void writeData(QString name, QString number);


public:
    //设置当前子界面的softkey
    virtual bool GetSoftkeyData(CArraySoftKeyBarData & objWndData);

    //将当前发件人账号选择框的焦点设为和传入的参数一致，如果没有对应账号则不设置
    void SetFromCmbCurrentIndex(QString addrText);

    //设置消息窗口的详细信息
    void SetMessageDetail(const TextMsgItemDetailData & sDetail);

    //获取所编辑消息的内容
    //获取发送账号
    int GetFromAccountID() const;
    //获取目的账号
    QString GetToAddr() const;
    //返回消息的内容
    QString GetTextContent() const;

    //保存到草稿
    void SaveToDraftBox();

    //窗口显示到界面上时的操作
    virtual void UpdateWnd();

    virtual void OnKeyboardShow();
    virtual void OnKeyboardHide();
    void AdjustMsgEdit(bool bShow = true);

public:
    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

    // 处理弹出框回调
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

private:
    CDlgMessageEdit(QWidget * parent = 0);
    virtual ~CDlgMessageEdit();

private:
    virtual bool OnAction(const QString & strAction);

private slots:
    //当编辑的短信字数发生变化时调整右下角标签的显示
    void SetAlphNumText();

private:
    //输入法切换
    void OnIMEBtnClicked();
    //输入法退格
    void OnIMEDeleteBtnClicked();
    //发送
    void OnSendBtnClicked();

    //判断是否需要保存
    bool ProcessInputs(int nUIAction);

private:
    //当前阅读的短信的索引
    int m_iMsgIdx;

    //发送账号选择
    QComboBox * m_pCmbFrom;
    bool                    m_bIsAccountEmpty;
    //目的账号填写
    QLineEdit * m_pLineEditTo;
    //输入框背景
    QLabel * m_pTextEditorBackground;
    //输入框
    QTextEdit * m_pTextEditor;
    //右下角显示当前字数的标签
    QLabel   *  m_pLabelWordNum;

    //记录界面加载时下拉菜单的序号
    int             m_nInstallCmbIndex;
    //记录界面加载时的收件人
    QString         m_strInstallTo;
    //记录界面加载时的文本消息内容
    QString         m_strInstallText;

    //记录退出时是否需要自动保存
    bool                m_bAutoSave;
};

#endif // DLGCALLLOG_H
