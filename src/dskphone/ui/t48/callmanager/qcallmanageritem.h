#ifndef QBTNBARITEM_H
#define QBTNBARITEM_H

#include "QPushButton"

class qCallManagerBarItem : public QPushButton
{
    Q_OBJECT
public:
    explicit qCallManagerBarItem(QWidget * parent = 0);
    ~qCallManagerBarItem(void);

    // 设置、获取左偏距离
    void SetLeftDistance(const int iLeftDis);
    int GetLeftDistance();
    // 设置、获取上偏距离
    void SetTopDistance(const int iTopDis);
    int GetTopDistance();
    // 设置字体font
    // 设置、获取Icon
    void SetIconPicture(const QString & strIconPic);
    QString GetIconPicture();
    // 设置、获取Text
    void SetText(const QString & strText);
    QString GetText();
    // 设置、获取SelectPic
    void SetSelectPicture(const QString & strSelectPic);
    QString GetSelectPicture();
    // 设置、获取Action
    void SetClickAction(const QString & strAction);
    QString GetClickAction();
    // 设置、获取是否绘制Background
    void SetDrawBg(const bool bBg);
    bool GetDrawBg();

protected:
    void paintEvent(QPaintEvent * pPaintEvent);

signals:
    void ClickAction(QString strAction);

private slots:
    void OnClick();

private:
    QString         m_strSelectPic;             // 选中选中时图片
    QString         m_strIcon;                  // Btn Icon
    QString         m_strText;                  // Btn Text
    QString         m_strClickAction;           // 点击Action
    int             m_iLeftDis;                 // 左偏距离
    int             m_iTopDis;                  // 上偏距离
    bool            m_bBg;                      // 是否绘制Background
};

#endif
