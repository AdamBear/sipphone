#include "testphonet52.h"

#if IS_COLOR
#include "test_include.h"
#include "testmodeinterface.h"

#define TESEMODE_RESOURCE_PATH  RESOURCE_PATH"testmode/t52/"

CTestPhoneT52::CTestPhoneT52()
{
    m_uMaxKeyNum = 0;
    m_iLineNum = 8;
}

CTestPhoneT52::~CTestPhoneT52()
{
}

void CTestPhoneT52::FlushLightIndex()
{
    m_vecLightIndex.push_back(T5XS_LIGHT_LK1_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK2_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK3_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK4_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK6_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK7_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK8_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK9_GREEN);

    m_vecLightIndex.push_back(T5XS_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK4_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK6_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK7_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK8_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK9_RED);

    m_vecLightIndex.push_back(T5XS_LIGHT_LK1_GREEN | T5XS_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK2_GREEN | T5XS_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK3_GREEN | T5XS_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK4_GREEN | T5XS_LIGHT_LK4_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK6_GREEN | T5XS_LIGHT_LK6_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK7_GREEN | T5XS_LIGHT_LK7_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK8_GREEN | T5XS_LIGHT_LK8_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK9_GREEN | T5XS_LIGHT_LK9_RED);

    m_vecLightIndex.push_back(T5XS_LIGHT_MUTE);
    m_vecLightIndex.push_back(T5XS_LIGHT_HEADSET);
    m_vecLightIndex.push_back(T5XS_LIGHT_SPK);
}

void CTestPhoneT52::InitCmpString()
{
    int i = 0;
    for (i = 0; i < 9; i++)
    {
        m_strKey[m_uMaxKeyNum++] = commonAPI_FormatString("%d", i + 1);
    }
    m_strKey[m_uMaxKeyNum++] = "*";
    m_strKey[m_uMaxKeyNum++] = "0";
    m_strKey[m_uMaxKeyNum++] = "#";

    for (i = 0; i < m_iLineNum; i++)
    {
        m_strKey[m_uMaxKeyNum++] = commonAPI_FormatString("LK%d", i + 1);
    }

    for (i = 0; i < 4; i++)
    {
        m_strKey[m_uMaxKeyNum++] = commonAPI_FormatString("HK%d", i + 1);
    }

    m_strKey[m_uMaxKeyNum++] = "Up";
    m_strKey[m_uMaxKeyNum++] = "Dn";
    m_strKey[m_uMaxKeyNum++] = "Lf";
    m_strKey[m_uMaxKeyNum++] = "Rt";
    m_strKey[m_uMaxKeyNum++] = "OK";
    m_strKey[m_uMaxKeyNum++] = "Cancel";
    m_strKey[m_uMaxKeyNum++] = "Vol-";
    m_strKey[m_uMaxKeyNum++] = "Vol+";

    m_strKey[m_uMaxKeyNum++] = "Mute";
    m_strKey[m_uMaxKeyNum++] = "Headset";
    m_strKey[m_uMaxKeyNum++] = "Msg";
    m_strKey[m_uMaxKeyNum++] = "RD";
    m_strKey[m_uMaxKeyNum++] = "HandFree";
}

void CTestPhoneT52::FillPicture(const yl::string& strPic)
{
    yl::string strFullPath = TESEMODE_RESOURCE_PATH + strPic;
    testmode_FillRect(strFullPath);
}

int CTestPhoneT52::GetNoteTextFont(int nTextType)
{
    switch (nTextType)
    {
    case TT_CONTENT_CENTER:
        return TEST_FONT_30;
        break;
    case TT_CONTENT_FULL_SCREEN:
        return TEST_FONT_24;
        break;
    default:
        break;
    }

    return CTestPhoneT54::GetNoteTextFont(nTextType);
}

void CTestPhoneT52::TestAllLightOn(int mLineColor, bool bFuncKey/* = true*/)
{
    TurnLightAllOff(true);

    switch (mLineColor)
    {
    case LS_GREEN:
        {
            for (int i = 0; i < m_iLineNum / 2; ++i)
            {
                TurnLightOn(m_vecLightIndex[m_iLineNum * 2 + i]);
            }

            TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN]);
            TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN + 2]);
        };
        break;
    case LS_RED:
        {
            for (int i = 0; i < m_iLineNum / 2; ++i)
            {
                TurnLightOn(m_vecLightIndex[m_iLineNum * 2 + i + m_iLineNum / 2]);
            }

            TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN + 1]);
        }
        break;
    case LS_ORANGEONE:
        {
            for (int i = 0; i < m_iLineNum / 2; ++i)
            {
                if (i % 2 != 0)
                {
                    continue;
                }

                // 对角亮
                TurnLightOn(m_vecLightIndex[m_iLineNum * 2 + i]);
                TurnLightOn(m_vecLightIndex[m_iLineNum * 2 + i + m_iLineNum / 2]);
            }
        }
        break;
    case LS_ORANGETWO:
        {
            for (int i = 0; i < m_iLineNum / 2; ++i)
            {
                if (i % 2 != 1)
                {
                    continue;
                }

                // 对角亮
                TurnLightOn(m_vecLightIndex[m_iLineNum * 2 + i]);
                TurnLightOn(m_vecLightIndex[m_iLineNum * 2 + i + m_iLineNum / 2]);
            }

            for (int i = T66_FUNCTION_LIGHT_BEGIN; i < m_vecLightIndex.size(); ++i)
            {
                // 点亮所有功能键
                TurnLightOn(m_vecLightIndex[i]);
            }
        }
        break;
    default:
        break;
    }

    DoLight();
}

#endif //IS_COLOR
