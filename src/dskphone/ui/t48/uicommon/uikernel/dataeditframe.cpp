#include "dataeditframe.h"
#define DAY_MAX_VALUE       0xFFFF
#define DAY_MAX_VALUE_STR   "0xFFFF"

namespace
{
// #define CHAR_NUM0           (0x30)      // 0
// #define CHAR_NUM9           (0x30+9)    // 9
// #define CHAR_STAR           35          // *
// #define CHAR_WELL           42          // #
#define  BigMonthMaxDay     31
#define  SmallMonthMaxDay   30
#define LINEEDIT_HEIGHT 36
}

typedef struct tagTimeData
{
    int minLen;     // 最小字节数  killfocus
    int maxLen;     // 最大字节数  input
    int minValue;   //
    int maxValue;
    char minValueStr[10];
    char maxValueStr[10];
} TimeData_t;
const TimeData_t TIME_DATA[] =
{
    {
        // year
        4, 4, 1970, 2037, "1970", "2037"
    },
    {
        // month
        1, 2, 1, 12, "01", "12"
    },
    {
        // day
        1, 2, 1, DAY_MAX_VALUE, "01", DAY_MAX_VALUE_STR
    },
    {
        // hour
        1, 2, 0, 23, "00", "23"
    },
    {
        // minute
        1, 2, 0, 59, "00", "59"
    },
    {
        // second
        1, 2, 0, 59, "00", "59"
    }
};
int BigMonth[7] = {1, 3, 5, 7, 8, 10, 12};
int SmallMonth[4] = {4, 6, 9, 11};
DataEditFrame::DataEditFrame(QWidget * parent, Qt::WindowFlags f)
    : QFrame(parent, f)
{
    //QRegExp regExp("[0-9]{2}");
    m_plineYear = new QLineEdit(this);
    m_plineYear->setGeometry(QRect(0, 0, 55, LINEEDIT_HEIGHT));
    m_plineYear->setAlignment(Qt::AlignCenter);

    //m_plineYear->setMaxLength(4);
    //m_plineYear->setValidator(new QRegExpValidator(QRegExp("[0-9]{4}"), this));

    m_plineMonth = new QLineEdit(this);
    m_plineMonth->setGeometry(QRect(75, 0, 55, LINEEDIT_HEIGHT));
    m_plineMonth->setAlignment(Qt::AlignCenter);

    //m_plineMonth->setMaxLength(2);
    //m_plineMonth->setValidator(new QRegExpValidator(regExp, this));

    m_plineDay = new QLineEdit(this);
    m_plineDay->setGeometry(QRect(150, 0, 55, LINEEDIT_HEIGHT));
    m_plineDay->setAlignment(Qt::AlignCenter);

    //m_plineDay->setMaxLength(2);
    //m_plineDay->setValidator(new QRegExpValidator(regExp, this));


    m_plabel = new QLabel(this);
    m_plabel->setGeometry(QRect(55, 0, 20, LINEEDIT_HEIGHT));
    m_plabel->setAlignment(Qt::AlignCenter);
    m_plabel->setText("-");

    m_plabel_2 = new QLabel(this);
    m_plabel_2->setGeometry(QRect(130, 0, 20, LINEEDIT_HEIGHT));
    m_plabel_2->setAlignment(Qt::AlignCenter);
    m_plabel_2->setText("-");

    this->resize(190, LINEEDIT_HEIGHT);
    connect(m_plineDay, SIGNAL(textChanged(QString)), this, SLOT(setDay()));
    connect(m_plineYear, SIGNAL(textChanged(QString)), this, SLOT(setYear()));
    connect(m_plineYear, SIGNAL(editingFinished()), this, SLOT(FocusOutsetYear()));
    connect(m_plineMonth, SIGNAL(editingFinished()), this, SLOT(FocusOutsetMounth()));
    connect(m_plineMonth, SIGNAL(textChanged(QString)), this, SLOT(setMounth()));
    connect(m_plineDay, SIGNAL(editingFinished()), this, SLOT(FocusOutSetDay()));

}

DataEditFrame::~DataEditFrame(void)
{
}
void DataEditFrame::setYear()
{
    QString strYear = m_plineYear->text();
    if (strYear.length() > 4)
    {
        m_plineYear->setText("1970");
    }
    int nYear = strYear.toInt();
    if (nYear > 1000)
    {
        if (nYear < TIME_DATA[0].minValue)
        {
            m_plineYear->setText("1970");
        }
        if (nYear > TIME_DATA[0].maxValue)
        {
            m_plineYear->setText("2037");
        }
    }
}
void DataEditFrame::FocusOutsetYear()
{
    QString strYear = m_plineYear->text();
    if (strYear.length() > 4 || strYear.isEmpty())
    {
        m_plineYear->setText("1970");
    }
    int nYear = strYear.toInt();
    if (nYear < TIME_DATA[0].minValue)
    {
        m_plineYear->setText("1970");
    }
    if (nYear > TIME_DATA[0].maxValue)
    {
        m_plineYear->setText("2037");
    }

}
void DataEditFrame::setDay()
{
    QString strDay = m_plineDay->text();
    if (strDay.length() > 2)
    {
        m_plineDay->setText("01");
        return;
    }
    int nDay = strDay.toInt();
    QString strMonth = m_plineMonth->text();
    int nMonth = strMonth.toInt();
    QString strYear = m_plineYear->text();
    int nYear = strYear.toInt();
    if (nMonth == 2 && nDay > 28)
    {
        if (IsLeapyear(nYear))
        {
            m_plineDay->setText("29");
        }
        else
        {
            m_plineDay->setText("28");
        }
        return;
    }
    for (int i = 0; i < 7 ; i++)
    {
        if (nMonth == BigMonth[i] && nDay > BigMonthMaxDay)
        {
            m_plineDay->setText("01");
            return;
        }
    }
    for (int i = 0; i < 4; i++)
    {
        if (nMonth == SmallMonth[i] && nDay > SmallMonthMaxDay)
        {
            m_plineDay->setText("01");
            return;
        }
    }
}

void DataEditFrame::checkDate()
{
    FocusOutsetYear();
    FocusOutsetMounth();
    FocusOutSetDay();
}

void DataEditFrame::FocusOutSetDay()
{
    QString strDay = m_plineDay->text();
    if (strDay.length() > 2 || strDay.isEmpty())
    {
        m_plineDay->setText("01");
        return;
    }
    int nDay = strDay.toInt();
    QString strMonth = m_plineMonth->text();
    int nMonth = strMonth.toInt();
    QString strYear = m_plineYear->text();
    int nYear = strYear.toInt();
    if (nMonth == 2 && nDay > 28)
    {
        if (IsLeapyear(nYear))
        {
            m_plineDay->setText("29");
        }
        else
        {
            m_plineDay->setText("28");
        }
    }
    if (nDay > 31)
    {
        m_plineDay->setText("30");
    }
    else if (nDay == 0)
    {
        m_plineDay->setText("01");
    }
    else
    {
        strDay.sprintf("%.2d", nDay);
        m_plineDay->setText(strDay);
    }
}
void DataEditFrame::FocusOutsetMounth()
{
    QString str = m_plineMonth->text();
    if (str.length() > 2 || str.isEmpty())
    {
        m_plineMonth->setText("01");
        return;
    }
    int nMounth = str.toInt();
    if (nMounth == 0 || nMounth > 12)
    {
        m_plineMonth->setText("01");
    }
    else
    {
        str.sprintf("%.2d", nMounth);
        m_plineMonth->setText(str);
    }

}
void DataEditFrame::setMounth()
{
    QString strMounth = m_plineMonth->text();
    if (strMounth.length() > 2)
    {
        m_plineMonth->setText("01");
    }
    int nMounth = strMounth.toInt();
    if (nMounth > 12)
    {
        if (nMounth < TIME_DATA[0].minValue || nMounth > TIME_DATA[0].maxValue)
        {
            m_plineMonth->setText("01");
        }
    }
}
DataEditFrame * qReplaceDataEditFrame(QWidget * pUIWidget)
{
    QRect rect = pUIWidget->geometry();
    QWidget * pParent = (QWidget *)pUIWidget->parent();
    pUIWidget->setAttribute(Qt::WA_DeleteOnClose, true);
    pUIWidget->close();

    DataEditFrame * pDataEditFrame = new DataEditFrame(pParent);
    pDataEditFrame->setGeometry(rect);
    return pDataEditFrame;
}
bool IsLeapyear(int year)
{
    /*闰年的判断:(1)year被4整除,并且year不被100整除
    或(2)year被4整除,并且year被400整除*/
    if (year % 4 == 0 && year % 100 || year % 400 == 0)
    {
        return true;
    }

    return false;
}
