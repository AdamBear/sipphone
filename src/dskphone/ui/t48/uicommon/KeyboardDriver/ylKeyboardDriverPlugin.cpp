#include "ylKeyboardDriverPlugin.h"

#include "ylkeyboarddriver.h"

YeaLinkKeyboardDriverPlugin::YeaLinkKeyboardDriverPlugin(QObject * parent)
    : QKbdDriverPlugin(parent)
{

}

YeaLinkKeyboardDriverPlugin::~YeaLinkKeyboardDriverPlugin()
{

}

QStringList YeaLinkKeyboardDriverPlugin::keys() const
{
    QStringList list;
    // 这个ylmousedriverplugin一定要和系统的环境变量QWS_MOUSE_PROTO中的标识字符串一致，否则会判断不是所需的设备
    list << QLatin1String("ylkeyboarddriverplugin");
    return list;

}

QWSKeyboardHandler * YeaLinkKeyboardDriverPlugin::create(const QString & driver,
        const QString & device)
{
    return new QYealinkInputKeyboardHandler(device);
}

Q_EXPORT_PLUGIN2(ylKeyboardDriverPlugin, YeaLinkKeyboardDriverPlugin)

