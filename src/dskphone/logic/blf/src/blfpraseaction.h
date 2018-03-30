#ifndef _BLF_PARSER_ACTION_H__
#define _BLF_PARSER_ACTION_H__

#include "taskaction/taskaction.h"
#include "blfcommon.h"
#include "xmlparser/xmlparser.hpp"

class CBLFPraseAction : public NS_TA::CTaskAction
{
public:

    CBLFPraseAction(char * strStr, bool IsPath);
    virtual ~CBLFPraseAction();

    // 重写
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

    YLList<BLFBaseData *> & GetListData()
    {
        return m_listData;
    }

    bool IsFullStatus()
    {
        return m_bIsFull;
    }

    void SetExtID(int nExtId)
    {
        m_nExtId = nExtId;
    }

    int GetExtId()
    {
        return m_nExtId;
    }

    // 由外部控制是否释放
    void ClearListInfo();

private:

    bool PraseDialogInfo(xml_document & doc);

    bool PraseList(char * pStr, int nSize);

    Dsskey_Status GetStatus(const yl::string & strStatus,
                            const yl::string & strDirection = "recipient");

private:

    yl::string m_strContent;

    YLList<BLFBaseData *> m_listData;

    YLHashMap<yl::string, yl::string> m_mapDisplayName;

    bool m_bIsFull;

    int m_nExtId;
};

#endif
