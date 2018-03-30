#ifndef _Q_PSW_TEXT_EDIT_H_
#define _Q_PSW_TEXT_EDIT_H_

#include <QLineEdit>

class qPswLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    qPswLineEdit(QWidget * parent = 0);
    ~qPswLineEdit();

    // 返回Text
    QString text();

    // 返回选中的Text
    QString SelectText();

    // 设置前置Text
    void SetPrefixText(const QString & strText);

    // 设置隐藏文字的长度
    void SetHideTextLength(int iLength);

public slots:
    // 设置控件的Text
    void setText(const QString & strText);

private slots:
    void OntextChanged(const QString & text);
    void OntextEdited(const QString & text);

signals:
    void TextChanged(const QString & text);// Text改变时,发出信号
    void TextEdited(const QString & text); // Text编辑时,发出信号

private:
    // 处理Text改变
    void ProcessTextChange(const QString & strChangeText);

    // 根据text返回隐藏部分文字的字符串
    QString HideText(const QString & text);

    // 是否需要隐藏text
    bool NeedHideText();

    // 是否含有隐藏的text
    bool HasHideText(const QString & text);

private:
    int     m_iHideLength;       // 隐藏字符的长度
    QString m_strPrefixText;     // 前置匹配字符,匹配该字符后才进行隐藏
    QString m_strText;           // 编辑框的真实文本
    QString m_strPreDisplayText; // 编辑框上显示的文本
};

#endif // #ifndef _Q_PSW_TEXT_EDIT_H_
