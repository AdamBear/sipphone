#ifndef __TALK_RING_HELP_HEADER_H__
#define __TALK_RING_HELP_HEADER_H__

/* 辅助接口 */

struct CRingHelpImpl
{
    /* 获取特定路径下所有铃声文件集合 */
    static bool GetRingFileByPath(const yl::string& strPath, YLVector<yl::string>& vecFile);

    /* 依web配置的区别振铃获取铃声完全路径 */
    static bool GetRingPathByWeb(const yl::string& strUrl, yl::string& strPath);

    /* 依关键字获取铃声完全路径 */
    static bool GetRingPathByKeyword(const yl::string& strKey, yl::string& strPath);

    /* 依sip上报字段获取铃声完全路径 */
    static bool GetRingFromSIPInfo(int CallId, int iAccount, CallInUserInfo* pCallData,
                                   yl::string& strPath, int& iStatus, int& iRingType);
    /* 依序号获取铃声完全路径 */
    static bool GetResRingByIndex(int iIndex, yl::string& strPath);

    /* 依账号获取铃声 */
    static bool GetAccountRing(int iAccountID, yl::string& strPath);

    /* 依序号获取铃声 */
    static bool GetRingPathByIndexOrName(const yl::string& strRing, yl::string& strPath,
                                         bool bIncludeUserPath = false, bool bIncludeBsftPath = false);
};

#endif
