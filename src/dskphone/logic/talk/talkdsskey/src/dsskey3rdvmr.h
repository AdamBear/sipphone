#ifdef IF_FEATURE_DSSKEY

#ifndef __TALK_DSSKEY_DSSKEY3RDVMRACTION_H__
#define __TALK_DSSKEY_DSSKEY3RDVMRACTION_H__

#ifdef IF_SUPPORT_VIDEO

#include "dsskeybindaction.h"

class CDsskey3rdVMRAciton: public CDsskeyBindAction
{
public:
    CDsskey3rdVMRAciton()
        : CDsskeyBindAction(DT_3RD_VMR)
    {
    }

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
    {
        DSSKEY_INFO("3rd vmr on key press");
        // 支持无Id呼出
        return CallOutByDsskey(logicData);
    }

    virtual bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0)
    {
        // extension保存id；value保存服务地址
        DSSKEY_INFO("GetCallOutNumber va[%s] ext[%s]", logicData.strValue.c_str(),
                    logicData.strExtension.c_str());
        // 号码为空返回服务器地址zoomcrc.com
        if (logicData.strExtension.empty())
        {
            strCallOutNumber = logicData.strValue;
        }
        else if (!logicData.strValue.empty()
                 && !logicData.strExtension.empty())
        {
            // 服务器地址不为空
            strCallOutNumber =  logicData.strExtension + "@" + logicData.strValue;
        }
        else
        {
            strCallOutNumber = logicData.strValue;
        }

        return true;
    }
};

#endif
#endif // __TALK_DSSKEY_DSSKEY3RDVMRACTION_H__
#endif

