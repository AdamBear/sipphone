#ifndef __DIAL_DIGITMAP_ITEM_H__
#define __DIAL_DIGITMAP_ITEM_H__

#include "digitmapdata.h"

#ifdef IF_FEATURE_DIGIT_MAP

struct DigitMapItem
{
public:

    DigitMapItem() : m_uTimer(invalid_timer), m_uAttr(DM_MATCH_RESULT_NONE),
        m_iLineID(AUTO_SELECT_LINE_ID) {}

public: // 查询函数集合
    // 完全匹配接口
    UINT RegFullMatch(const yl::string & strSource) const;

    // 部分匹配接口
    UINT RegSemiMatch(const yl::string & strSource) const;

    // 获取替换后的结果
    yl::string GetResult(const yl::string & strSource) const;

    // 获取时间
    UINT GetTimer() const;

    // 获取时间
    int GetLineID() const;

    // 获取原始字符串
    LPCSTR GetOriginalString()
    {
        return m_strOrg.c_str();
    }

public: // 解析函数入口
    // 解析，预处理，去除不必要的属性，将对应属性保存在结果中
    void Parse(const yl::string & strSource);

private:

    // 部分匹配接口
    UINT isSemiMatched(const yl::string & strSource) const;

    // 标记操作函数
    bool isFlag(DM_MAP_FLAG eAttr) const
    {
        return m_uAttr & eAttr;
    }
    void addFlag(DM_MAP_FLAG eAttr)
    {
        m_uAttr |= eAttr;
    }
    void clearFlag()
    {
        m_uAttr = DM_MATCH_RESULT_NONE;
    }
    UINT GetCommonFlag(const yl::string & strSource, UINT uBaseAttr) const;
    UINT GetExtraAttr(int uBaseAttr) const;

    // 清空操作
    void clear();

    // 实现正则表达式的替换
    bool GetRegReplace(const yl::string & strHead, const yl::string & strMask,
                       const yl::string & strDst, const yl::string & strPost, yl::string & strResult) const;

private:
    // 获取最小的正则表达式字符个数
    static int GetMinRegCount(const yl::string & strReg, bool & bConfirm);

    // 获取正则表达式字符串
    yl::string GetRegByCount(int iCount) const;

    // 校验格式
    bool CheckFormat();

private:
    // 正则表达式匹配函数
    static bool isMatched(const yl::string & strSource, const yl::string & strPattern);

    // 正则表达式匹配结果，返回匹配的个数，取到匹配的序号
    static bool MatchInfo(const yl::string & strSource, const yl::string & strPattern, int & iStartPos,
                          int & iEndPos);

    // 获取正则表达式匹配的长度
    static int regMatchedLength(const yl::string & strSource, const yl::string & strPattern);

    // 获取正则表达式元素个数
    static int GetRegElementCount(const yl::string & strReg);
private:
    // 格式化替换字符 如<22:3>，RRR类型在预处理已经转化了
    static bool TrimReplaceFlag(yl::string & strReg);
    // DigitMap转化为标准的正则表达式格式
    static bool Trim2StdRegexp(yl::string & strReg);

private:
    yl::string              m_strOrg;          // 部分格式化字符，去掉时间等
    yl::string              m_strRegMatch;     // 格式化字符
    UINT                    m_uTimer;          // 时间
    UINT                    m_uAttr;           // 记录的属性（是否自定义时间等）
    int                     m_iLineID;         // 账号ID
    YLVector<yl::string>    m_vecCommaPos;     // 逗号的位置信息
};

#endif

#endif // __DIAL_DIGITMAP_ITEM_H__
