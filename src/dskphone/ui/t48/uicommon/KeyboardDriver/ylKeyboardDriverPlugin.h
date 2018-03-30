#ifndef YEALINK_KEYBOARD_DRIVER_PLUGIN_H
#define YEALINK_KEYBOARD_DRIVER_PLUGIN_H

#include <qkbddriverplugin_qws.h>

class YeaLinkKeyboardDriverPlugin : public QKbdDriverPlugin
{
public:
    YeaLinkKeyboardDriverPlugin(QObject * parent = 0);
    ~YeaLinkKeyboardDriverPlugin();
    QStringList keys() const;
    QWSKeyboardHandler * create(const QString & driver, const QString & device);

};

#endif // YEALINK_KEYBOARD_DRIVER_PLUGIN_H
