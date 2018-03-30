#ifndef _RES_DATA_DEFINE_HEADER_
#define  _RES_DATA_DEFINE_HEADER_

#include <ylstl/ylstring.h>
#include <xmlparser/xmlparser.hpp>

#define  IMPORT_TYPE "imported"
#define  SYSTEM_TYPE "system"
#define  RES_USER_TYPE "user"
#define  DEFAULT_TYPE "default"

#define  RING_TYPE "ringtone"
#define  PIC_TYPE "image"
#define  LANG_TYPE "language"
#define  FONT_TYPE "font"
#define  WALLPAPER_TYPE "wallpaper"
#define  SCREENSAVER_TYPE "screensaver"
#define  EXP_WALLPAPER_TYPE "expwallpaper"

struct BaseResItem
{
    yl::string resItemName;

    yl::string resItemDispLabel;

    yl::string resItemType;

    yl::string resPath;

    int resSort;//排序位置

    int attr;

    bool resIsDefault;

    virtual bool parseFromXmlNode(xml_node pXmlNode);

    bool parseFromXmlNode(const yl::string & filePath, const yl::string strType);

    virtual bool toXmlNode(xml_node pXmlNode);

    virtual ~BaseResItem()
    {
        int i = 0;
    }

    BaseResItem()
    {
        resSort = 0;
        attr = 0;
        resIsDefault = false;
        resItemName.clear();
        resItemDispLabel.clear();
        resItemType.clear();
        resPath.clear();
    }

    bool isReadOnly();

    bool isImported();

    bool isDefault();

    int compareToResItem(BaseResItem * pItem);

    int isPathMatch(const yl::string & strPath);

    void printData();
};




#endif



