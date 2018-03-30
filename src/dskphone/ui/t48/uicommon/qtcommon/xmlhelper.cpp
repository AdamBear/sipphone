#include "xmlhelper.h"

void AddNode(xml_node & node, const TestInfo & testInfo)
{
    xml_node nodeItem = node.append_child("item");
    nodeItem.append_attribute("x_absolute", testInfo.x_absolute);
    nodeItem.append_attribute("y_absolute", testInfo.y_absolute);
    nodeItem.append_attribute("width", testInfo.width);
    nodeItem.append_attribute("height", testInfo.height);
    nodeItem.append_attribute("choose", testInfo.choose);
    nodeItem.append_attribute("id", testInfo.id.c_str());
    nodeItem.append_attribute("text", testInfo.text.c_str());
    nodeItem.append_attribute("type", testInfo.type.c_str());
}

void AddBtnNode(xml_node & node, const CButtonWrapper btnWrapper, const yl::string strId,
                const yl::string strText)
{
    TestInfo testInfo;
    // btn
    QRect rcBtn = btnWrapper.GetRect();
    QPoint ptAbsolute = rcBtn.topLeft();

    if (btnWrapper.GetParent() != NULL)
    {
        ptAbsolute = btnWrapper.GetParent()->mapToGlobal(rcBtn.topLeft());
    }

    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = rcBtn.width();
    testInfo.height = rcBtn.height();
    testInfo.id = strId;
    testInfo.text = strText;
    testInfo.type = VIRTUAL_BTN;
    AddNode(node, testInfo);
}

void GetStringFromXml(const xml_document & docXml, QString & strXml)
{
    XmlStringWriter writer(strXml);
    docXml.save(writer, (const pugi::char_t *)("   "),
                pugi::format_indent | pugi::format_no_declaration, pugi::encoding_auto);
}
