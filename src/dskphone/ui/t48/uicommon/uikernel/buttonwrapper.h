
#ifndef __BUTTON_WRAPPER_H__
#define __BUTTON_WRAPPER_H__

#include <QWidget>

class QKeyEvent;
class QMouseEvent;
class QStylePainter;

class CButtonWrapper
{
public:
    explicit CButtonWrapper(QWidget * pWidget);
    virtual ~CButtonWrapper(void);

public:
    void SetRect(const QRect & rect);
    void SetRect(int nX, int nY, int nWidth, int nHeight);
    void ResetDown();
    void SetDownKey(int nDownKey);

    const QRect & GetRect() const;
    bool IsDown() const;

    bool ProcessMousePress(QMouseEvent * pEvent);
    bool ProcessMouseRelease(QMouseEvent * pEvent);
    bool ProcessKeyPress(QKeyEvent * pEvent);
    bool ProcessKeyRelease(QKeyEvent * pEvent);

    void PaintButton(QStylePainter & stylePainter, const char * pszNoraml, const char * pszClick = NULL,
                     const char * pszDisable = NULL, QRect rcBtn = QRect());

    void PaintButton(QStylePainter & stylePainter, QPixmap & pm, QRect rcBtn = QRect());

    const QWidget * GetParent() const;

protected:
    void RepaintButton();

protected:
    QWidget * m_pWidget;
    QRect m_rect;

private:
    bool m_bDown;
    int m_nDownKey;
};

#endif
