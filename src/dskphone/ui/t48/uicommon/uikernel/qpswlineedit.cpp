#include "qpswlineedit.h"

qPswLineEdit::qPswLineEdit(QWidget * parent)
    : QLineEdit(parent)
    , m_iHideLength(0)
{

    setObjectName("qPswLineEdit");

    // 连接相应的信号和槽
    connect(this, SIGNAL(textChanged(const QString &)),
            this, SLOT(OntextChanged(const QString &)));

    connect(this, SIGNAL(textEdited(const QString &)),
            this, SLOT(OntextEdited(const QString &)));
}

qPswLineEdit::~qPswLineEdit()
{

}

// 设置控件的Text
void qPswLineEdit::setText(const QString & strText)
{
    // 清空原有数据
    m_strPreDisplayText.clear();
    m_strText.clear();

    // 处理Text的改变
    ProcessTextChange(strText);

    // 发送Text改变的信号
    emit TextChanged(m_strText);
}

// 返回Text
QString qPswLineEdit::text()
{
    return m_strText;
}

// 返回选中的Text
QString qPswLineEdit::SelectText()
{
    return m_strText.mid(this->cursorPosition(), this->selectedText().length());
}

// 设置前置Text
void qPswLineEdit::SetPrefixText(const QString & strText)
{
    m_strPrefixText = strText;
}

// 设置隐藏文字的长度
void qPswLineEdit::SetHideTextLength(int iLength)
{
    if (iLength >= 0)
    {
        m_iHideLength = iLength;
    }
}

void qPswLineEdit::ProcessTextChange(const QString & strChangeText)
{
    // 记录光标位置
    int iCursorPosition = this->cursorPosition();

    if (m_strPreDisplayText.isEmpty() || !HasHideText(strChangeText))
    {
        m_strText = strChangeText;
    }
    else
    {
        int iChangeTextLen = strChangeText.length() - m_strText.length();

        if (iChangeTextLen < 0)// 判断text是添加还是删除
        {
            // 删除多余的字符
            m_strText = m_strText.remove(iCursorPosition, iChangeTextLen * -1);
        }
        else
        {
            // 添加字符
            m_strText.insert(qMax(iCursorPosition - 1, 0), strChangeText.mid(iCursorPosition - 1,
                             iChangeTextLen));
        }
    }

    // 将实际的Text转换为显示的Text
    m_strPreDisplayText = HideText(m_strText);

    bool bSignalBlocked = signalsBlocked();
    if (!bSignalBlocked)
    {
        //没有禁用信号发送才调用禁用
        QObject:: blockSignals(true);// 禁止信号的发送,防止循环调用
    }
    QLineEdit::setText(m_strPreDisplayText);
    this->setCursorPosition(iCursorPosition);
    if (!bSignalBlocked)
    {
        //没有禁用信号发送才调用解开
        QObject:: blockSignals(false);// 解开信号发送
    }
}

// 根据text返回隐藏部分文字的字符串
QString qPswLineEdit::HideText(const QString & text)
{
    // 前置字符为空,获取隐藏长度小于0则不进行替换
    if (!NeedHideText())
    {
        return text;
    }

    QString strHideText = text;

    if (strHideText.left(m_strPrefixText.length()) == m_strPrefixText)
    {
        // 计算需要隐藏的字符长度
        int iHideLength = qMin(strHideText.length() - m_strPrefixText.length(), m_iHideLength);

        // 替换需要隐藏的字符
        strHideText.replace(m_strPrefixText.length(), iHideLength, QString(iHideLength, '*'));
    }

    return strHideText;
}

// 是否需要隐藏text
bool qPswLineEdit::NeedHideText()
{
    return (!m_strPrefixText.isEmpty() && m_iHideLength > 0);
}

// 是否含有隐藏的text
bool qPswLineEdit::HasHideText(const QString & text)
{
    return text.contains('*');
}

//!!!!! 查看下Qt的源代码看能否取消TextChanged信号
void qPswLineEdit::OntextChanged(const QString & text)
{
    ProcessTextChange(text);

    emit TextChanged(m_strText);
}

void qPswLineEdit::OntextEdited(const QString & text)
{
    ProcessTextChange(text);

    emit TextEdited(m_strText);
}
