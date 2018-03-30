#include "fontoutput.h"
#include "dsklog/log.h"

#if LINUX_SYSTEM
#include <QApplication>
#include <QInputContext>


CFontOutput::CFontOutput(void)
{
    m_strPreeditData = "";
}

CFontOutput::~CFontOutput(void)
{
}

bool CFontOutput::SubmitCommitData(QWidget* pWidget, QString strInput)
{
    if (!m_strPreeditData.isEmpty())
    {
        INPUTMETHOD_ERR("m_strPreeditData is not empty when SubmitCommitData.");
        m_strPreeditData = "";
    }

    return sendCommitString(pWidget, strInput);
}

bool CFontOutput::SubmitCurrentCommitData(QWidget* pWidget)
{
    if (m_strPreeditData.isEmpty())
    {
        return false;
    }

    bool bSend = sendCommitString(pWidget, m_strPreeditData);

    m_strPreeditData = "";

    return bSend;
}

bool CFontOutput::SubmitCurrentPreeditData(QWidget* pWidget, QString strInput)
{
    m_strPreeditData = strInput;

    return sendPreeditString(pWidget, m_strPreeditData, 1);
}

bool CFontOutput::DealBackspace(QWidget* pWidget)
{
    if (!m_strPreeditData.isEmpty())
    {
        sendCommitString(pWidget, m_strPreeditData);
        m_strPreeditData = "";
    }

    return false;
}

bool CFontOutput::IsPreeditEmpty()
{
    return m_strPreeditData.isEmpty();
}

bool CFontOutput::IsOutStrEqual(QString strInput)
{
    if (m_strPreeditData == strInput)
    {
        return true;
    }
    return false;
}

bool CFontOutput::sendPreeditString(QWidget* pKeyWidget, const QString& preeditString, int cursorPosition, int selectionLength/* = 0*/)
{
    if (NULL == pKeyWidget)
    {
        return false;
    }

    QInputContext* pQic = pKeyWidget->inputContext();

    if (NULL == pQic)
    {
        return false;
    }

    QList<QInputMethodEvent::Attribute> attributes;

    int selPos = cursorPosition;

    if (selectionLength == 0)
    {
        selPos = 0;
    }
    else if (selectionLength < 0)
    {
        selPos += selectionLength;
        selectionLength = -selectionLength;
    }

    if (selPos > 0)
    {
        QVariant data = pQic->standardFormat(static_cast<QInputContext::StandardFormat>(QInputContext::PreeditFormat));

        attributes += QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, 0, selPos, data);
    }

    if (selectionLength)
    {
        QVariant data = pQic->standardFormat(static_cast<QInputContext::StandardFormat>(QInputContext::SelectionFormat));

        attributes += QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, selPos, selectionLength, data);
    }

    if (selPos + selectionLength < preeditString.length())
    {
        QVariant data = pQic->standardFormat(static_cast<QInputContext::StandardFormat>(QInputContext::PreeditFormat));

        attributes += QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
                      selPos + selectionLength,
                      preeditString.length() - selPos - selectionLength,
                      data);
    }

    attributes += QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, cursorPosition, 0, QVariant());

    QInputMethodEvent ime(preeditString, attributes);

    return QApplication::sendEvent(pKeyWidget, &ime);
}

bool CFontOutput::sendCommitString(QWidget* pKeyWidget, const QString& commitString, int replaceFrom/* = 0*/, int replaceLength/* = 0*/)
{
    if (NULL == pKeyWidget)
    {
        return false;
    }

    QInputMethodEvent ime;

    ime.setCommitString(commitString, replaceFrom, replaceLength);

    return QApplication::sendEvent(pKeyWidget, &ime);
}
#else
CFontOutput::CFontOutput(void)
{

}

CFontOutput::~CFontOutput(void)
{

}

void CFontOutput::SubmitCurrentCommitData(QWidget* pWidget, QString strInput)
{

}

void CFontOutput::SubmitCurrentCommitData(QWidget* pWidget)
{

}

void CFontOutput::SubmitCurrentPreeditData(QWidget* pWidget, QString strInput)
{

}

bool CFontOutput::DealBackspace(QWidget* pWidget)
{
    return false;
}

bool CFontOutput::IsPreeditEmpty()
{
    return false;
}

bool CFontOutput::IsOutStrEqual(QString strInput)
{
    return false;
}

#endif
