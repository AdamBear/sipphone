#ifndef __TIPTELADAPTER_H__
#define __TIPTELADAPTER_H__

// 对外接口
void TiptelA_Init();
void TiptelA_ParseUserInfo(CallInUserInfo * pUser);
void TiptelA_ResetFlage();
bool TiptelA_IsAutoAnswer();
CHANNEL_MODE TiptelA_GetSpeekMode();

#ifdef IF_FEATURE_TIPTEL_ADAPTER
yl::string TiptelA_GetInvokeID();
#else
LPCSTR TiptelA_GetInvokeID();
#endif

#ifdef IF_FEATURE_TIPTEL_ADAPTER
class CTipTelAdapter
{
    DECLARE_SINGLETON_CLASS(CTipTelAdapter)

public:
    void Init();
    bool IsAutoAnswer()
    {
        return m_bIsAutoAnswer;
    }

    CHANNEL_MODE GetSpeekMode()
    {
        return m_eTiptelAnswerType;
    }
    const yl::string & GetNum()
    {
        return m_strRemoteNum;
    }
    const yl::string & GetLCDDisplay()
    {
        return m_strLCD;
    }
    const yl::string & GetInvokeID()
    {
        return m_strInvokeID;
    }

    void ResetFlage()
    {
        m_bIsAutoAnswer = false;
    }
    // 更改灯状态
    void ButtonStatusChange(char * lpEvent, int iLine);

public:
    void ParseUserInfo(CallInUserInfo * pUser);
    void ParseParam(LPCSTR param);

private:
    void GetAttribute(LPCSTR lpBuffer, LPCSTR lpAttribute, LPSTR lpOut);
    CHANNEL_MODE GetSpeekMode(int iMic, int iSpeeker);

private:
    bool            m_bIsAutoAnswer;        //是否自动应答
    CHANNEL_MODE    m_eTiptelAnswerType;    //Tiptel服务器指定的通话声道模式
    yl::string      m_strRemoteNum;         // 需要呼出的号码
    yl::string      m_strLCD;               // LCD上需要显示的内容
    yl::string      m_strInvokeID;          // Tiptel控制发起通话时的ID;
};

#define _TiptelAdapter GET_SINGLETON_INSTANCE(CTipTelAdapter)

#endif // #ifdef IF_FEATURE_TIPTEL_ADAPTER
#endif // __TIPTELADAPTER_H__
