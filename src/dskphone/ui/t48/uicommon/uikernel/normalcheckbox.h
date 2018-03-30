#ifndef __NORMALCHECKBOX_H__
#define __NORMALCHECKBOX_H__

#include <QCheckBox>
#include <ylstring.h>

class qNormalCheckBox : public QCheckBox
{
    Q_OBJECT

public:
    qNormalCheckBox(QWidget * parent = NULL);
    virtual ~qNormalCheckBox();

    void SetCheckBoxPic(const char * pszNormal, const char * pszSelect);
    void SetCheckBoxPic(const yl::string & strNormal, const yl::string & strSelect);

protected:
    void paintEvent(QPaintEvent * pEvent);

private:
    yl::string m_strNormalPic;
    yl::string m_strSelectPic;
};

#endif // __NORMALCHECKBOX_H__
