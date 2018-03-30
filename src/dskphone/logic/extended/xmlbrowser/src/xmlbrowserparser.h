#ifndef __XML_BROWSER_PARSER_H__
#define __XML_BROWSER_PARSER_H__

#ifdef IF_FEATURE_XMLBROWSER

#include <xmlparser/xmlparser.hpp>
#include "xmlbrowseraction.h"

// ! Release
void ReleaseParseData(XMLParseData * pParseData);

// 删除XBStatusData数据
void ReleaseStatusData(XBStatusData * pStatusData);

//////////////////////////////////////////////////////////////////////////
class CXMLParseAction : public CXMLBrowserAction
{
public:
    CXMLParseAction(const char * szFilePath)
        : CXMLBrowserAction(OPT_XML_PARSE)
    {
        if (NULL != szFilePath)
        {
            m_strXmlFilePath = szFilePath;
        }
        m_pParseData = NULL;
        m_iErrCode = ERR_OK;
    }
    virtual ~CXMLParseAction()
    {
        ReleaseData();
    }

    enum
    {
        ERR_OK,

        ERR_EXIST,
        ERR_OPEN,

        ERR_ITEM_LIMIT,
        ERR_ROOT_NODE,
        ERR_ROOT_ATT,

        ERR_NODE,
        ERR_STR_LIMIT,

        ERR_MEM,
        ERR_UNKNOWN,
    };

    // 重写读写函数
    virtual bool Exec();

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed)
    {
        return false;
    }

    // 获取文件名
    const yl::string & GetFileName()
    {
        return m_strXmlFilePath;
    }

    // 获取出错类型
    int GetErrorCode()
    {
        return m_iErrCode;
    }

    // !! 取走解析的数据
    XMLParseData * TakeoutData()
    {
        XMLParseData * pData = m_pParseData;
        m_pParseData = NULL;
        return pData;
    }

private:
    // 根据名称获取Element类型
    XML_ELEMENT_TYPE GetTypeByName(const yl::string & strName);

    // 释放数据
    void ReleaseData()
    {
        if (m_pParseData != NULL)
        {
            ReleaseParseData(m_pParseData);
            m_pParseData = NULL;
        }
    }

    // 根据类型解析相应的数据
    bool ParseData(XML_ELEMENT_TYPE eType, xml_node xmlTypeData);
    bool ParseTextMenu(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    bool ParseTextScreen(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    bool ParseInputScreen(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    bool ParseDirectory(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    bool ParseExecute(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    bool ParseStatus(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    bool ParseConfiguration(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    bool ParseImageMenu(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    bool ParseImageScreen(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    bool ParseFormattedTextScreen(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
#if IF_FEATURE_XMLB_SCREENSAVER
    bool ParseScreenSaver(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    void ParseSystemTimeData(xml_node xmlSysTimeNode, SysTimeData & strSysTimeData);
    void ParseStatusIconData(xml_node xmlStatusIconNode, StatusIconData & strStatusIconData);
    bool GetShowOfSysTimeorStatusIcon(const yl::string & strTemp);
    int GetMaxLineNum();
    bool IsRGB(yl::string strRGB);
#endif

#ifdef IF_FEATURE_GENBAND
    bool ParseInterface(xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    // base64解密是否正确
    bool Base64Dec(yl::string & strData);
    // 转化{'Connection': 'keep-alive', 'Content-Type': 'application/json', 'Authorization': 'Basic XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX', 'Accept': 'application/json'}
    void ChangeHeadFormat(yl::string & strData);
#endif

    // 解析Softkey
    bool ParseSoftkey(xml_node xmlTypeData, VecSoftkeyParam & vecSoftkey);
#if IF_BUG_31595
    // 解析DSSkey
    bool ParseDssKeyData(xml_node xmlDsskeyData, StatusItem * statusItem);
#endif

#if IF_BUG_32431
    bool ParseRefreshAttr(xml_node xmlTypeData);
#endif

    bool ParseBeep(xml_node xmlTypeData, bool& bBeep);

    // 配置的第三个版本，M7模板。定版
    bool ParseConfigV7(XBConfigData * pData, xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    // 配置的第一个版本，类似M1的autop配置
    bool ParseConfigV1(XBConfigData * pData, xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);
    // 配置的第二个版本，原T3X和VPX所用
    bool ParseConfigV2(XBConfigData * pData, xml_node xmlTypeData, XB_TIP_TYPE & eErrorType);

    // 检查属性,存在就获取,不存在就返回false
    bool DetectAttr(xml_node xmlNode, const char * lpszAttr, yl::string & strTemp);

    // 获取节点内容,节点不存在返回false
    bool GetItemValue(xml_node xmlFatherNode, const char * lpszItem, yl::string & strValue);

    // 获取style
    MENU_ITEM_STYLE GetMenuStyle(const yl::string & strTemp);

    // 获取type
    INPUT_TYPE GetInputType(const yl::string & strTemp);
    // 获取输入语言类型
    INPUT_LANGUAGE GetInputLanguage(const yl::string & strTemp);
    // 获取DisplayMode
    INPUT_DISPLAY_MODE GetInputDisplayMode(const yl::string & strTemp);
    // 得到第一个不为empty的输入框的index
    bool GetFirstUnEmptyIndex(int & iIndex, List_Input_Field ListInput);
    // 解析某个输入项的节点
    bool ParserInputField(xml_node xmlInputNode, InputField * pInputData);
    // 获取输入框的默认值
    bool GetInputValue(INPUT_TYPE eInputType, yl::string & strValue);

    // 获取一个Line的数据
    LineData * GetLineData(xml_node xmlItemNode);
    // 获取字体大小
    X_FONT_TYPE GetLineSize(const yl::string & strTemp);
    // 获取对齐方式
    int GetLineAlign(const yl::string & strTemp);
    // 获取字体颜色
    TEXT_FONT_COLOR GetLineColor(const yl::string & strTemp);
    // 获取IconPath
    yl::string GetIconPath(const yl::string & strTemp);

    // 获取一个Image的数据
    ImageItem * GetImageItem(xml_node xmlItemNode);
    // 获取下载图片的文件名
    yl::string GetImagePathByURL(yl::string strImageURL);
    // 获取垂直对齐方式
    IMAGE_VERTICAL_ALIGN GetVerticalAlign(const yl::string & strTemp);
    // 获取水平对齐方式
    IMAGE_HORIZONTAL_ALIGN GetHorizontalAlign(const yl::string & strTemp);
    // 获取显示方式
    IMAGE_MODE GetImageMode(const yl::string & strTemp);

    // 根据path获得一个可用的PathItem
    PathItem * GetPathItemByPath(const char * lpszPath, List_Path_Item ListPathItem);
    // 解析配置信息
    bool ConfigParser(ConfigItem * pConfigItem, xml_node xmlItemNode);
    // 去掉首尾的空格
    yl::string TrimString(const yl::string & strTemp);
    // 解析配置信息
    void ScanConfig(yl::string & strSection, yl::string & strParam, yl::string & strValue,
                    const yl::string & strTemp);
    // 整形转为字符串
    yl::string Myitoa(int i);

    // Dsskey的类型转换
    int TransformDsskeyType(const char * szValue);
private:
    yl::string m_strXmlFilePath;    // 文件路径
    XMLParseData * m_pParseData;    // 解析后的结构体指针
    int           m_iErrCode;
};

#endif // #ifdef IF_FEATURE_XMLBROWSER
#endif // __XML_BROWSER_PARSER_H__
