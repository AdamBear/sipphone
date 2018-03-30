#include "testphonet46.h"

#if IS_COLOR
#include "test_include.h"
#include "testmodeinterface.h"

CTestPhoneT46::CTestPhoneT46()
{
}

CTestPhoneT46::~CTestPhoneT46()
{
}

void CTestPhoneT46::FlushLightIndex()
{
	m_vecLightIndex.push_back(T46_LIGHT_LK1_GREEN);
	m_vecLightIndex.push_back(T46_LIGHT_LK2_GREEN);
	m_vecLightIndex.push_back(T46_LIGHT_LK3_GREEN);
	m_vecLightIndex.push_back(T46_LIGHT_LK4_GREEN);
	m_vecLightIndex.push_back(T46_LIGHT_LK5_GREEN);
	m_vecLightIndex.push_back(T46_LIGHT_LK6_GREEN);
	m_vecLightIndex.push_back(T46_LIGHT_LK7_GREEN);
	m_vecLightIndex.push_back(T46_LIGHT_LK8_GREEN);
	m_vecLightIndex.push_back(T46_LIGHT_LK9_GREEN);
	m_vecLightIndex.push_back(T46_LIGHT_LK10_GREEN);

	m_vecLightIndex.push_back(T46_LIGHT_LK1_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK2_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK3_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK4_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK5_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK6_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK7_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK8_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK9_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK10_RED);

	m_vecLightIndex.push_back(T46_LIGHT_LK1_GREEN | T46_LIGHT_LK1_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK2_GREEN | T46_LIGHT_LK2_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK3_GREEN | T46_LIGHT_LK3_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK4_GREEN | T46_LIGHT_LK4_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK5_GREEN | T46_LIGHT_LK5_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK6_GREEN | T46_LIGHT_LK6_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK7_GREEN | T46_LIGHT_LK7_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK8_GREEN | T46_LIGHT_LK8_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK9_GREEN | T46_LIGHT_LK9_RED);
	m_vecLightIndex.push_back(T46_LIGHT_LK10_GREEN | T46_LIGHT_LK10_RED);

	m_vecLightIndex.push_back(T46_LIGHT_HEADSET);
	m_vecLightIndex.push_back(T46_LIGHT_MUTE);
	m_vecLightIndex.push_back(T46_LIGHT_SPK);
}

void CTestPhoneT46::InitCmpString()
{
	CTestPhoneT54::InitCmpString();
	m_strKey[m_uMaxKeyNum++] = "AnyEXPKey";
}

bool CTestPhoneT46::ChangeEchoState(int iKey, ECHO_MODE & eMode)
{
	ECHO_MODE eOldMode = eMode;
	if (CTestPhone::ChangeEchoState(iKey, eMode))
	{
		if (EM_HEADSET != eOldMode
			&& EM_HEADSET == eMode)
		{
			// 进入耳麦回音测试，关闭免提灯
			LightOff(T46_LIGHT_SPK);
		}
		else if (EM_HEADSET == eOldMode
			&& EM_HANDSET == eMode)
		{
			// 进入手柄回音测试，关闭耳麦
			LightOff(T46_LIGHT_HEADSET);
		}

		return true;
	}

	return false;
}

void CTestPhoneT46::TestAllLightOn(int mLineColor, bool bFuncKey/* = true*/)
{
	TurnLightAllOff(true);

    switch (mLineColor)
    {
	case LS_GREEN:
        {
            for (int i = 0; i < m_iLineNum; ++i)
            {
				TurnLightOn(m_vecLightIndex[i]);
            }

			TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN]);
			TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN + 2]);
        };
        break;
	case LS_RED:
        {
            for (int i = 0; i < m_iLineNum; ++i)
            {
				TurnLightOn(m_vecLightIndex[m_iLineNum + i]);
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
