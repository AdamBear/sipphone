#ifndef CKEYBOARDLAYOUTMANAGER_H
#define CKEYBOARDLAYOUTMANAGER_H

#include "keyboardmanagerbase.h"

class CKeyboardLayoutManager : public CKeyboardManagerBase
{
private:
    CKeyboardLayoutManager();
    virtual ~CKeyboardLayoutManager();

public:
    static CKeyboardLayoutManager& GetInstance();

    //从读取的文件中初始化Layout
    void InitKeyboardFromXmlFile();

    const vecKeyboardLayout& GetKeyboardLayout()
    {
        return m_vecKeyboardLayout;
    }
    //根据名字获取Layout
    const SKeyboardLayout* GetLayoutByName(const yl::string& strLayoutName);

protected:
    virtual void LoadXmlFiles(const yl::string& strPath, const yl::string& strFile);

private:
    //根据节点获取属性值
    //nLeftIndent、nTopIndent、nWidth、nHeight保存结果值
    //nDLeftIndent、nDTopIndent、nDWidth、nDHeight为默认值
    void GetRectInfoByNode(const xml_node& xmlNode
                           , SKeyboardKeyPosition& KeyPosition
                           , SKeyboardKeyPosition& DefalutKeyPosition);

    //根据读取的数据，计算出每个按钮的显示位置和响应位置
    void CalculateKeyRect(vecKeyboardLayoutTable& tLayoutTable);

    //根据显示位置和间距信息，分别计算响应位置的各个值
    void CalculateRectLeft(int& nLeft, int nPointLeft
                           , bool bOnlyOneKey, bool bFirstKey);

    void CalculateRectTop(int& nTop, int nPointTop
                          , bool bOnlyOneLine, bool bFirstLine);

    void CalculateRectWidth(SKeyboardLayoutKey& tLayoutKey, int nNextKeyLeftIndent
                            , bool bOnlyOneKey, bool bLastKey);

    void CalculateRectHeight(SKeyboardLayoutKey& tLayoutKey, int nCurrentLineHeight, int nNextLineTopIndent
                             , bool bOnlyOneLine, bool bLastLine);

private:
    vecKeyboardLayout                               m_vecKeyboardLayout;

private:

};

#define g_KeyBoardLayoutManager (CKeyboardLayoutManager::GetInstance())

#endif // CKEYBOARDLAYOUTMANAGER_H
