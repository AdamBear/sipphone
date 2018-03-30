#ifndef _CODEAUTOPPROCESSOR__H
#define _CODEAUTOPPROCESSOR__H
#include "baseautopprocessor.h"
class CCodeProcessor :
    public CBaseAutopProcessor
{
public:
    CCodeProcessor();
    virtual ~CCodeProcessor();

    virtual bool ProcessConnect();

    virtual bool ProcessRedirectMsg(msgObject & objMsg);

    /**
    * @brief  是否提示更新及更新结果
    */
    virtual bool IsNeedShowUpdating()
    {
        return false;
    }
protected:
    virtual bool ReadyToConnect();
    virtual void ShowAuthWnd();
    void SaveConfig();
private:
    LPAUTOP_PARAM m_pCodeParam;
};

#endif //_CODEAUTOPPROCESSOR__H