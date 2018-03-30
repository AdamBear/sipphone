#include "fontpreeditbase.h"

CFontPreeditBase::CFontPreeditBase(void)
{
    m_iAllPressTimes            = 0;

}

CFontPreeditBase::~CFontPreeditBase(void)
{
}


QString CFontPreeditBase::DealPreeditKeyCode(int iKeyCode, QString qstrArry)
{
    //如果字符串中字符个数小于等于0  那么就直接返回了 因为没的操作
    if (qstrArry.count() <= 0)
    {
        return "";
    }

    //如果个数是1的 那么直接提交确定字符
    if (1 == qstrArry.count())
    {
        m_iAllPressTimes                = 0;
        return qstrArry.at(m_iAllPressTimes);
    }
    else
    {
        QString strFont = qstrArry.at(m_iAllPressTimes);

        m_iAllPressTimes++;

        //这边如果已经超过了字符串的最大长度 那么要从新开始转
        if (m_iAllPressTimes >= qstrArry.count())
        {
            m_iAllPressTimes            = 0;
        }
        return strFont;
    }
    return "";
}

QString CFontPreeditBase::GetFontData(QObject* pObject, int iKeyCode, int iKeyboardStatus)
{
    // bool v_bResult               = true;


    switch (iKeyCode)
    {
    case Qt::Key_0:
        return DealPreeditKeyCode(Qt::Key_0, m_qstrArry0);

    case Qt::Key_1:
        return DealPreeditKeyCode(Qt::Key_1, m_qstrArry1);

    case Qt::Key_2:
        return DealPreeditKeyCode(Qt::Key_2, m_qstrArry2);

    case Qt::Key_3:
        return DealPreeditKeyCode(Qt::Key_3, m_qstrArry3);

    case Qt::Key_4:
        return DealPreeditKeyCode(Qt::Key_4, m_qstrArry4);

    case Qt::Key_5:
        return DealPreeditKeyCode(Qt::Key_5, m_qstrArry5);

    case Qt::Key_6:
        return DealPreeditKeyCode(Qt::Key_6, m_qstrArry6);

    case Qt::Key_7:
        return DealPreeditKeyCode(Qt::Key_7, m_qstrArry7);

    case Qt::Key_8:
        return DealPreeditKeyCode(Qt::Key_8, m_qstrArry8);

    case Qt::Key_9:
        return DealPreeditKeyCode(Qt::Key_9, m_qstrArry9);

    case Qt::Key_Asterisk:
        return DealPreeditKeyCode(Qt::Key_Asterisk, m_qstrArry10);

    case Qt::Key_NumberSign:
        return DealPreeditKeyCode(Qt::Key_NumberSign, m_qstrArry11);

    default:
        break;
    }

    return "";

}

QString CFontPreeditBase::GetFirstChar(int _iKeyCode)
{

    QString v_qstrResult        = "";

    switch (_iKeyCode)
    {
    case Qt::Key_0:
        if (m_qstrArry0.count() > 0)
        {
            v_qstrResult            = m_qstrArry0.at(0);
        }
        break;
    case Qt::Key_1:
        if (m_qstrArry1.count() > 0)
        {
            v_qstrResult            = m_qstrArry1.at(0);
        }
        break;
    case Qt::Key_2:
        if (m_qstrArry2.count() > 0)
        {
            v_qstrResult            = m_qstrArry2.at(0);
        }
        break;
    case Qt::Key_3:
        if (m_qstrArry3.count() > 0)
        {
            v_qstrResult            = m_qstrArry3.at(0);
        }
        break;
    case Qt::Key_4:
        if (m_qstrArry4.count() > 0)
        {
            v_qstrResult            = m_qstrArry4.at(0);
        }
        break;
    case Qt::Key_5:
        if (m_qstrArry5.count() > 0)
        {
            v_qstrResult            = m_qstrArry5.at(0);
        }
        break;
    case Qt::Key_6:
        if (m_qstrArry6.count() > 0)
        {
            v_qstrResult            = m_qstrArry6.at(0);
        }
        break;
    case Qt::Key_7:
        if (m_qstrArry7.count() > 0)
        {
            v_qstrResult            = m_qstrArry7.at(0);
        }
        break;
    case Qt::Key_8:
        if (m_qstrArry8.count() > 0)
        {
            v_qstrResult            = m_qstrArry8.at(0);
        }
        break;
    case Qt::Key_9:
        if (m_qstrArry9.count() > 0)
        {
            v_qstrResult            = m_qstrArry9.at(0);
        }
        break;
    case Qt::Key_Asterisk:
        if (m_qstrArry10.count() > 0)
        {
            v_qstrResult            = m_qstrArry10.at(0);
        }
        break;
    case Qt::Key_NumberSign:
        if (m_qstrArry11.count() > 0)
        {
            v_qstrResult            = m_qstrArry11.at(0);
        }
        break;

    default:
        v_qstrResult            = "";
        break;
    }

    return v_qstrResult;


}

bool CFontPreeditBase::IsPerInput(int iKeyCode)
{
    switch (iKeyCode)
    {
    case Qt::Key_0:
        if (m_qstrArry0.count() > 1)
        {
            return true;
        }
        break;
    case Qt::Key_1:
        if (m_qstrArry1.count() > 1)
        {
            return true;
        }
        break;
    case Qt::Key_2:
        if (m_qstrArry2.count() > 1)
        {
            return true;
        }
        break;
    case Qt::Key_3:
        if (m_qstrArry3.count() > 1)
        {
            return true;
        }
        break;
    case Qt::Key_4:
        if (m_qstrArry4.count() > 1)
        {
            return true;
        }
        break;
    case Qt::Key_5:
        if (m_qstrArry5.count() > 1)
        {
            return true;
        }
        break;
    case Qt::Key_6:
        if (m_qstrArry6.count() > 1)
        {
            return true;
        }
        break;
    case Qt::Key_7:
        if (m_qstrArry7.count() > 1)
        {
            return true;
        }
        break;
    case Qt::Key_8:
        if (m_qstrArry8.count() > 1)
        {
            return true;
        }
        break;
    case Qt::Key_9:
        if (m_qstrArry9.count() > 1)
        {
            return true;
        }
        break;
    case Qt::Key_Asterisk:
        if (m_qstrArry10.count() > 1)
        {
            return true;
        }
        break;
    case Qt::Key_NumberSign:
        if (m_qstrArry11.count() > 1)
        {
            return true;
        }
        break;

    default:
        break;
    }
    return false;
}

void CFontPreeditBase::Init()
{
    m_iAllPressTimes = 0;
}

void CFontPreeditBase::Release()
{
    m_iAllPressTimes = 0;
}

void CFontPreeditBase::ResetPressTimes()
{
    m_iAllPressTimes = 0;
}
