#include "keyboardlayoutmanager.h"
#include "datastruct.h"

#include "dsklog/log.h"

namespace
{
enum KEYBOARD_POSITION_TYPE
{
    POSITION_TYPE_ERROR = 0,
    POSITION_TYPE_FIRST,
    POSITION_TYPE_MIDDLE,
    POSITION_TYPE_LAST,
};

enum KEYBOARD_STATUS
{
    STATUS_ERROR = 0,
    STATUS_ONLY_ONE,
    STATUS_MORE,
};
}

CKeyboardLayoutManager::CKeyboardLayoutManager()
{

}

CKeyboardLayoutManager::~CKeyboardLayoutManager()
{

}

CKeyboardLayoutManager& CKeyboardLayoutManager::GetInstance()
{
    static CKeyboardLayoutManager myKeyboardLayoutManager;
    return myKeyboardLayoutManager;
}

const SKeyboardLayout* CKeyboardLayoutManager::GetLayoutByName(const yl::string& strLayoutName)
{
    if (m_vecKeyboardLayout.size() <= 0)
    {
        return NULL;
    }

    vecKeyboardLayout::iterator iterLayout = m_vecKeyboardLayout.begin();

    for (; iterLayout != m_vecKeyboardLayout.end(); ++iterLayout)
    {
        if (strLayoutName == (*iterLayout).m_strLayoutName)
        {
            return iterLayout;
        }
    }

    return NULL;
}

void CKeyboardLayoutManager::InitKeyboardFromXmlFile()
{
    m_vecKeyboardLayout.clear();

    CKeyboardManagerBase::LoadXmlFiles(g_KeyBoardXmlReader.GetFactoryLayoutList(), INPUT_FILES_FACTORY_PATH);
    CKeyboardManagerBase::LoadXmlFiles(g_KeyBoardXmlReader.GetConfigLayoutList(), INPUT_FILES_CONFIG_PATH);
}

void CKeyboardLayoutManager::LoadXmlFiles(const yl::string& strPath, const yl::string& strFile)
{
    INPUTMETHOD_INFO("CKeyboardLayoutManager::LoadXmlFiles, strFile = %s", strFile.c_str());

    yl::string strFullPath = strPath + strFile;

    if (!pathFileExist(strFullPath.c_str()))
    {
        return;
    }

    xml_document docLayoutXmlFile;
    if (!docLayoutXmlFile.load_file(strFullPath.c_str()))
    {
        INPUTMETHOD_INFO("CKeyboardLayoutManager::LoadXmlFiles, Load File Fail, strFile = %s", strFile.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_XML_FORMAT);
        return;
    }

    xml_node xmlLayoutData = docLayoutXmlFile.first_child();
    if (xmlLayoutData.empty())
    {
        INPUTMETHOD_INFO("CKeyboardLayoutManager::LoadXmlFiles, First Child is Empty, strFile = %s", strFile.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_FIRST_CHILD_EMPTY);
        return;
    }

    //判断名字
    yl::string strBaseName = xmlLayoutData.name();
    if (0 != strBaseName.compare(INPUT_LAYOUT_NAME))
    {
        INPUTMETHOD_INFO("CKeyboardLayoutManager::LoadXmlFiles, First Child Name Error, strFile = %s, strBaseName = %s", strFile.c_str(), strBaseName.c_str());
        g_KeyBoardHintManager.AddHint(strFile.c_str(), KEYBOARD_HINT_ERROR_TYPE_FIRST_CHILD_NAME);
        return;
    }

    SKeyboardLayout tKeyboardLayout;
    tKeyboardLayout.m_strLayoutName = strFile;

    //遍历Line节点
    xml_node xmlLayoutLine = xmlLayoutData.child(INPUT_XML_NODE_LINE);
    while (!xmlLayoutLine.empty())
    {
        // 检查节点是否正确
        if (strcmp(INPUT_XML_NODE_LINE, xmlLayoutLine.name()) != 0)
        {
            xmlLayoutLine = xmlLayoutLine.next_sibling();
            INPUTMETHOD_INFO("CKeyboardLayoutManager::LoadXmlFiles, Line Name Error, strFile = %s", strFile.c_str());
            continue;
        }

        SKeyboardLayoutLine tKeyboardLayoutLine;

        SKeyboardKeyPosition positionDefault;
        positionDefault.m_nLeftIndent = 0;
        positionDefault.m_nTopIndent = 0;
        positionDefault.m_nWidth = 0;
        positionDefault.m_nHeight = 0;

        //记录行默认值
        GetRectInfoByNode(xmlLayoutLine
                          , tKeyboardLayoutLine.m_LinePosition
                          , positionDefault);

        //遍历Key节点
        xml_node xmlLayoutKey = xmlLayoutLine.child(INPUT_XML_NODE_KEY);
        while (!xmlLayoutKey.empty())
        {
            // 检查节点是否正确
            if (strcmp(INPUT_XML_NODE_KEY, xmlLayoutKey.name()) != 0)
            {
                xmlLayoutKey = xmlLayoutKey.next_sibling();
                continue;
            }

            SKeyboardLayoutKey tKeyboardKey;

            //获取Key属性
            GetRectInfoByNode(xmlLayoutKey
                              , tKeyboardKey.m_KeyPosition
                              , tKeyboardLayoutLine.m_LinePosition);

            tKeyboardLayoutLine.m_vecKeyboardLine.push_back(tKeyboardKey);
            xmlLayoutKey = xmlLayoutKey.next_sibling();
        }

        tKeyboardLayout.m_vecKeyboardLayoutTable.push_back(tKeyboardLayoutLine);
        xmlLayoutLine = xmlLayoutLine.next_sibling();
    }

    CalculateKeyRect(tKeyboardLayout.m_vecKeyboardLayoutTable);

    m_vecKeyboardLayout.push_back(tKeyboardLayout);
}

void CKeyboardLayoutManager::GetRectInfoByNode(const xml_node& xmlNode, SKeyboardKeyPosition& KeyPosition, SKeyboardKeyPosition& DefalutKeyPosition)
{
    yl::string strValue = "";

    if (DetectAttr(xmlNode, INPUT_LAYOUT_ATTRI_LEFTINDENT, strValue))
    {
        KeyPosition.m_nLeftIndent = atoi(strValue.c_str());
    }
    else
    {
        KeyPosition.m_nLeftIndent = DefalutKeyPosition.m_nLeftIndent;
    }
    if (DetectAttr(xmlNode, INPUT_LAYOUT_ATTRI_TOPINDENT, strValue))
    {
        KeyPosition.m_nTopIndent = atoi(strValue.c_str());
    }
    else
    {
        KeyPosition.m_nTopIndent = DefalutKeyPosition.m_nTopIndent;
    }
    if (DetectAttr(xmlNode, INPUT_LAYOUT_ATTRI_WIDTH, strValue))
    {
        KeyPosition.m_nWidth = atoi(strValue.c_str());
    }
    else
    {
        KeyPosition.m_nWidth = DefalutKeyPosition.m_nWidth;
    }

    if (DetectAttr(xmlNode, INPUT_LAYOUT_ATTRI_HEIGHT, strValue))
    {
        KeyPosition.m_nHeight = atoi(strValue.c_str());
    }
    else
    {
        KeyPosition.m_nHeight = DefalutKeyPosition.m_nHeight;
    }
}

void CKeyboardLayoutManager::CalculateKeyRect(vecKeyboardLayoutTable& tLayoutTable)
{
    //记录当前行、按钮的位置状态（是否只有一个、是否位于首、尾）
    KEYBOARD_STATUS                         eTableStatus = STATUS_ERROR;
    KEYBOARD_STATUS                         eLineStatus = STATUS_ERROR;
    KEYBOARD_POSITION_TYPE              eLinePosition = POSITION_TYPE_ERROR;
    KEYBOARD_POSITION_TYPE              eKeyPosition = POSITION_TYPE_ERROR;

    //Cursor
    vecKeyboardLayoutTable::iterator    iterLayoutLine;
    vecKeyboardLayoutTable::iterator    iterLayoutLineNext;
    vecKeyboardLayoutLine::iterator     iterLayoutKey;
    vecKeyboardLayoutLine::iterator     iterLayoutKeyNext;

    //记录位移累加值
    int                                                     nPointLeft;
    int                                                     nPointTop;

    //判断表格状态（是否超过一行）
    if (tLayoutTable.size() <= 0)
    {
        return;
    }
    else if (tLayoutTable.size() == 1)
    {
        eTableStatus = STATUS_ONLY_ONE;
    }
    else
    {
        eTableStatus = STATUS_MORE;
    }

    //整个列表开始前初始化垂直距离累计值
    nPointTop = 0;

    //遍历Line
    for (iterLayoutLine = tLayoutTable.begin(); iterLayoutLine != tLayoutTable.end(); ++iterLayoutLine)
    {
        //判断是否边界行
        if (iterLayoutLine == tLayoutTable.begin())
        {
            eLinePosition = POSITION_TYPE_FIRST;
        }
        else if (iterLayoutLine + 1 == tLayoutTable.end())
        {
            eLinePosition = POSITION_TYPE_LAST;
        }
        else
        {
            eLinePosition = POSITION_TYPE_MIDDLE;
        }

        iterLayoutLineNext = iterLayoutLine + 1;

        //整个行开始前初始化水平距离累计值
        nPointLeft = 0;

        SKeyboardLayoutLine& tLayoutLine = *iterLayoutLine;
        //遍历Key
        vecKeyboardLayoutLine& tLayoutLineData = tLayoutLine.m_vecKeyboardLine;

        //判断行状态（是否有超过一个按钮）
        if (tLayoutLineData.size() <= 0)
        {
            continue;
        }
        else if (tLayoutLineData.size() == 1)
        {
            eLineStatus = STATUS_ONLY_ONE;
        }
        else
        {
            eLineStatus = STATUS_MORE;
        }

        for (iterLayoutKey = tLayoutLineData.begin(); iterLayoutKey != tLayoutLineData.end(); ++iterLayoutKey)
        {
            //判断是否边界按钮
            if (iterLayoutKey == tLayoutLineData.begin())
            {
                eKeyPosition = POSITION_TYPE_FIRST;
            }
            else if (iterLayoutKey + 1 == tLayoutLineData.end())
            {
                eKeyPosition = POSITION_TYPE_LAST;
            }
            else
            {
                eKeyPosition = POSITION_TYPE_MIDDLE;
            }

            iterLayoutKeyNext = iterLayoutKey + 1;

            SKeyboardLayoutKey& tLayoutKey = *iterLayoutKey;
            SKeyboardKeyPosition& positionKey = tLayoutKey.m_KeyPosition;
            SKeyboardKeyRect& rctButton = tLayoutKey.m_rectButton;
            SKeyboardKeyRect& rctRespond = tLayoutKey.m_rectRespond;

            //根据累计值计算显示位置
            rctButton.m_nLeft = nPointLeft + positionKey.m_nLeftIndent;
            rctButton.m_nTop = nPointTop + positionKey.m_nTopIndent;
            rctButton.m_nWidth = positionKey.m_nWidth;
            rctButton.m_nHeight = positionKey.m_nHeight;

            //根据当前间隔信息和下一个按钮、下一行的间隔信息，计算当前按钮的响应区域
            CalculateRectLeft(rctRespond.m_nLeft, nPointLeft, STATUS_ONLY_ONE == eLineStatus, POSITION_TYPE_FIRST == eKeyPosition);
            CalculateRectTop(rctRespond.m_nTop, nPointTop, STATUS_ONLY_ONE == eTableStatus, POSITION_TYPE_FIRST == eLinePosition);

            if (iterLayoutKeyNext != tLayoutLineData.end())
            {
                CalculateRectWidth(*iterLayoutKey, (*iterLayoutKeyNext).m_KeyPosition.m_nLeftIndent
                                   , STATUS_ONLY_ONE == eLineStatus, POSITION_TYPE_LAST == eKeyPosition);
            }
            else
            {
                CalculateRectWidth(*iterLayoutKey, 0
                                   , STATUS_ONLY_ONE == eLineStatus, POSITION_TYPE_LAST == eKeyPosition);
            }

            if (iterLayoutLineNext != tLayoutTable.end())
            {
                CalculateRectHeight(*iterLayoutKey, (*iterLayoutLine).m_LinePosition.m_nHeight, (*iterLayoutLineNext).m_LinePosition.m_nTopIndent
                                    , STATUS_ONLY_ONE == eTableStatus, POSITION_TYPE_LAST == eLinePosition);
            }
            else
            {
                CalculateRectHeight(*iterLayoutKey, 0, 0
                                    , STATUS_ONLY_ONE == eTableStatus, POSITION_TYPE_LAST == eLinePosition);
            }

            //增加水平累计值
            nPointLeft += positionKey.m_nLeftIndent;
            nPointLeft += positionKey.m_nWidth;
        }

        //增加垂直累计值
        nPointTop += tLayoutLine.m_LinePosition.m_nTopIndent;
        nPointTop += tLayoutLine.m_LinePosition.m_nHeight;
    }
}

void CKeyboardLayoutManager::CalculateRectLeft(int& nLeft, int nPointLeft, bool bOnlyOneKey, bool bFirstKey)
{
    if (bOnlyOneKey || bFirstKey)
    {
        nLeft = g_iFullKeyBoardX;
    }
    else
    {
        nLeft = g_iFullKeyBoardX + nPointLeft;
    }
}

void CKeyboardLayoutManager::CalculateRectTop(int& nTop, int nPointTop, bool bOnlyOneLine, bool bFirstLine)
{
    if (bOnlyOneLine || bFirstLine)
    {
        nTop = g_iFullKeyBoardY;
    }
    else
    {
        nTop = g_iFullKeyBoardY + nPointTop;
    }
}

void CKeyboardLayoutManager::CalculateRectWidth(SKeyboardLayoutKey& tLayoutKey, int nNextKeyLeftIndent, bool bOnlyOneKey, bool bLastKey)
{
    //如果整行只有一个按钮，则响应宽度设置为整个键盘的宽度
    if (bOnlyOneKey)
    {
        tLayoutKey.m_rectRespond.m_nWidth = g_iFullKeyBoardWidth;
    }
    //如果超过一个按钮
    else
    {
        //如果是最后一个按钮，则后面的空位全部都作为该按钮的响应区域
        if (bLastKey)
        {
            tLayoutKey.m_rectRespond.m_nWidth = g_iFullKeyBoardWidth - tLayoutKey.m_rectRespond.m_nLeft;
        }
        //如果不是最后一个，则可以获取下一个
        else
        {
            //将下一个按钮左边间距的一半加入到该按钮的有效范围内
            tLayoutKey.m_rectRespond.m_nWidth = tLayoutKey.m_KeyPosition.m_nLeftIndent
                                                + tLayoutKey.m_KeyPosition.m_nWidth
                                                + nNextKeyLeftIndent / 2;
        }
    }
}

void CKeyboardLayoutManager::CalculateRectHeight(SKeyboardLayoutKey& tLayoutKey, int nCurrentLineHeight, int nNextLineTopIndent, bool bOnlyOneLine, bool bLastLine)
{
    //如果总共行数只有一行，则响应高度设置为整个键盘的高度
    if (bOnlyOneLine)
    {
        tLayoutKey.m_rectRespond.m_nHeight = g_iFullKeyBoardHeight;
    }
    //如果超过一行
    else
    {
        //如果是最后一行，则下方的空位都作为该按钮的响应区域
        if (bLastLine)
        {
            tLayoutKey.m_rectRespond.m_nHeight = g_iFullKeyBoardY + g_iFullKeyBoardHeight - tLayoutKey.m_rectRespond.m_nTop;
        }
        //如果不是最后一行，则可以获取下一行
        else
        {
            tLayoutKey.m_rectRespond.m_nHeight = tLayoutKey.m_KeyPosition.m_nTopIndent
                                                 + tLayoutKey.m_KeyPosition.m_nHeight;
            if (tLayoutKey.m_KeyPosition.m_nHeight <= nCurrentLineHeight)
            {
                //如果没有超过一行，则获取下一行的顶部间距
                //将下一行顶部间距的一半加入到该按钮的有效范围内
                tLayoutKey.m_rectRespond.m_nHeight += nNextLineTopIndent / 2;
            }
            else
            {
                //如果该按钮高度超过行高
                //则响应高度应为本行默认高度加上下一行topIndent（中间总高度）
                //再减去该按钮高度，再平分
                tLayoutKey.m_rectRespond.m_nHeight +=
                    (nCurrentLineHeight
                     + nNextLineTopIndent
                     - tLayoutKey.m_rectButton.m_nHeight) / 2;

                //计算默认扩展高度
                int nHeightByRect =
                    tLayoutKey.m_KeyPosition.m_nTopIndent
                    + tLayoutKey.m_rectButton.m_nHeight
                    + tLayoutKey.m_KeyPosition.m_nTopIndent / 2;

                //取大值
                tLayoutKey.m_rectRespond.m_nHeight =
                    tLayoutKey.m_rectRespond.m_nHeight > nHeightByRect
                    ? tLayoutKey.m_rectRespond.m_nHeight
                    : nHeightByRect;
            }
        }
    }
}
