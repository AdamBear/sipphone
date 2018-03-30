#ifndef XMLHELPER_H
#define XMLHELPER_H
#include <QString>
#include <xmlparser/xmlparser.hpp>
#include "uikernel/buttonwrapper.h"

typedef struct Info
{
    int x_absolute;
    int y_absolute;
    int width;
    int height;
    int choose;
    yl::string id;
    yl::string text;
    yl::string type;
    Info()
    {
        choose = 0;
    }
} TestInfo;

// VirtualClass
#define VIRTUAL_LABEL "CVirtualLabel"
#define VIRTUAL_BTN "CVirtualBtn"

// ObjectName
#define LABEL_ICON "lblIcon"
#define LABEL_TITLE "lblTitle"
#define LABEL_INFO "lblInfo"
#define LABEL_INFOINDEX "lblInfoIndex"
#define LABEL_NOTE "lblNote"
#define LABEL_CONICON "lblConIcon"
#define LABEL_PAGE "lblPage"
#define LABEL_INDEX "lblIndex"
#define BTN_DETAIL "btnDetail"
#define BTN_DIRECTORY "btnDirectory"
#define BTN_SEARCH "btnSearch"
#define BTN_SETTING "btnSetting"
#define BTN_ADD "btnAdd"
#define BTN_MOVE "btnMove"
#define BTN_UP  "btnUp"
#define BTN_DOWN "btnDown"
#define BTN_PRE "btnPre"
#define BTN_NEXT "btnNext"
#define BTN_IMAGE "btnImage"
#define BTN_DIAL "btnSend"
#define BTN_EDIT "btnEdit"
#define BTN_BLACKLIST "btnBlacklist"
#define BTN_DELETE "btnDelete"
#define BTN_CHECK "btnCheckBox"
#define BTN_CONNECT "btnConnect"
#define BTN_LINE "btnLine"
#define BTN_RING "btnRing"
#define BTN_UPDATE "btnUpdate"
#define BTN_SORT "btnSort"

// 用于将xml内容导出到字符串，xml_document调用save
class XmlStringWriter : public pugi::xml_writer
{
public:
    XmlStringWriter(QString & data)
        : m_strData(data)
    {

    }

    virtual void write(const void * data, size_t size)
    {
        this->m_strData = QString::fromLatin1((char *)data, size);
    }

    QString & m_strData;
};

void AddNode(xml_node & node, const TestInfo & testInfo);

void AddBtnNode(xml_node & node, const CButtonWrapper btnWrapper, const yl::string strId,
                const yl::string strText = "");

void GetStringFromXml(const xml_document & docXml, QString & strXml);

#endif //XMLHELPER_H
