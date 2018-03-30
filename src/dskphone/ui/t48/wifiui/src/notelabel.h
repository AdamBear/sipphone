#ifndef __NOTE_LABEL_H
#define __NOTE_LABEL_H

#include <QLabel>
#include "wifiui/include/wirelesscommondata.h"

class NoteLabel : public QLabel
{
    Q_OBJECT

public:
    NoteLabel(QWidget * parent = NULL);
    ~NoteLabel();

    void SetLabelText(QString strNote);

    void SetLabelType(Item_Note_Type eType);

private slots:
    void OnTimeOut();

private:
    QTimer     *    m_pTimer;           // disappear or animation cycle
    QString         m_strDis;           // display content
    Item_Note_Type  m_eType;            // display type
    int             m_iCount;           // the number of "."
};

#endif
