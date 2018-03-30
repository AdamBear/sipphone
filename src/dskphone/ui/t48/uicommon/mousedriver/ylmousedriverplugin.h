#ifndef YEALINK_MOUSE_DRIVER_PLUGIN_H
#define YEALINK_MOUSE_DRIVER_PLUGIN_H

#include <qmousedriverplugin_qws.h>

class YeaLinkMouseDriverPlugin : public QMouseDriverPlugin
{
public:
    YeaLinkMouseDriverPlugin(QObject * parent = 0);
    ~YeaLinkMouseDriverPlugin();
    QStringList keys() const;
    QWSMouseHandler * create(const QString & driver, const QString & device);

};

#endif // YEALINK_MOUSE_DRIVER_PLUGIN_H
