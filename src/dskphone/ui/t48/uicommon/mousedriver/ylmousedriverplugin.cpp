#include "ylmousedriverplugin.h"

#include "ylmousedriver.h"

YeaLinkMouseDriverPlugin::YeaLinkMouseDriverPlugin(QObject * parent)
    : QMouseDriverPlugin(parent)
{

}

YeaLinkMouseDriverPlugin::~YeaLinkMouseDriverPlugin()
{

}

QStringList YeaLinkMouseDriverPlugin::keys() const
{
    QStringList list;
    // 这个ylmousedriverplugin一定要和系统的环境变量QWS_MOUSE_PROTO中的标识字符串一致，否则会判断不是所需的设备
    list << QLatin1String("ylmousedriverplugin");
    return list;

}

QWSMouseHandler * YeaLinkMouseDriverPlugin::create(const QString & driver, const QString & device)
{
    return new QYealinkInputMouseHandler(driver, device);
}

Q_EXPORT_PLUGIN2(ylMouseDriverPlugin, YeaLinkMouseDriverPlugin)

