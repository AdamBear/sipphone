#ifndef CDIALBUTTON_H
#define CDIALBUTTON_H

#include "uikernel/quipushbutton.h"
#include "imagemanager/modimagemanager.h"
#include <QLabel>
#include <QFontMetrics>
#include <QtGui>

// 拨号按键
class CDialButton : public qUIPushButton
{
    Q_OBJECT

public:
    CDialButton(QWidget * parent = 0);
    ~CDialButton();

    void SetText(const QString & digit, const QString & letters = QString());

    void SetFontSize(const int & digit_size = 30, const int & letter_size = 14);

    void SetMargin(const int & left = 10, const int & top = 10);

    void SetMarginBetweenDigitAndLetters(const int & val = 10);

    void EnableAutoMargin(const bool & horizontal = false, const bool & vertical = true);

    void SetKey(const int & key);

    void Init();

signals:
    void BtnPressed(int iKey);
    void BtnReleased(int iKey);

protected:
    void paintEvent(QPaintEvent * pe);

private slots:
    void OnButtonPressed();

    void OnButtonReleased();

private:
    QString m_qstrDigit;

    QString m_qstrLetters;

    int m_iMarginLeft;

    int m_iMarginTop;

    int m_iMarginBetweenDigitAndLetters;

    bool m_bAutoMarginHorizontal;

    bool m_bAutoMarginVertical;

    int m_iDigitMarginLeftCal;

    int m_iMarginTopCal;

    int m_iLetterMarginLeftCal;

    QFont m_fontDigit;

    QFont m_fontLetter;

    int m_iKey;
};


#endif  // CDIALBUTTON_H
