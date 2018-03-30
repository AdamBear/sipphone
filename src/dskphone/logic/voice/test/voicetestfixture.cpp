#include "voicetestfixture.h"

#include <service_config.h>
#include <configiddefine.h>
#include "exp/include/exp_struct.h"
#include "extended/bluetoothmessagedefine.h"
#include "extended/usbmessagedefine.h"

bool g_bAfterInit = true;

using namespace testing;

ModVoiceTest::ModVoiceTest()
    : m_iVolumeTalkHandset(1)
    , m_iVolumeTalkHeadset(2)
    , m_iVolumeTalkHandfree(3)
    , m_iVolumeToneHandset(4)
    , m_iVolumeToneHeadset(5)
    , m_iVolumeToneHandfree(6)
    , m_iVolumeFoceRing(0)
    , m_iVolumeRing(7)
    , m_iVolumeGroup(8)
{
}

void ModVoiceTest::SetUp()
{
    g_pMockConfigParser = &m_objMockConfigParser;
    g_pMockEtlMsgHandler = &m_objMockEtlMsgHandler;
    g_pMockMkService = &m_objMockMkService;
    g_pMockSubInterface = &m_objMockSubInterface;
    g_pMockVPM = &m_objMockVPM;
    EXPECT_REG_MSG_HANDLE(TM_TIMER);
    EXPECT_REG_MSG_HANDLE(DSK_MSG_SELECT_CHANNEL);
    EXPECT_REG_MSG_HANDLE(MKIT_MSG_SRVREG);
    EXPECT_REG_MSG_HANDLE(CONFIG_MSG_BCAST_CHANGED);
    EXPECT_REG_MSG_HANDLE(DEV_MSG_EHS_INSERT);
    EXPECT_REG_MSG_HANDLE(DEV_MSG_EHS_REMOVE);
    EXPECT_REG_MSG_HANDLE(DEV_MSG_EHS_KEY);
#ifdef IF_SUPPORT_BLUETOOTH
    EXPECT_REG_MSG_HANDLE(BLUETOOTH_MSG_CONNECT_STATE); // 蓝牙耳机消息
#endif // IF_SUPPORT_BLUETOOTH
#ifdef IF_SUPPORT_USB_AUDIO
    EXPECT_REG_MSG_HANDLE(USB_MSG_HEADSET_CONNECT_STATE); // USB耳机消息
#endif // IF_SUPPORT_USB_AUDIO
    ExpectConfigChannelModeEnable(true, true, true);
    ExpectConfigVolume();
}

void ModVoiceTest::TearDown()
{
    g_pMockConfigParser = NULL;
    g_pMockEtlMsgHandler = NULL;
    g_pMockMkService = NULL;
    g_pMockSubInterface = NULL;
    g_pMockVPM = NULL;
}

void ModVoiceTest::ExpectConfigChannelModeEnable(bool bHandfree, bool bHandset, bool bHeadset)
{
    EXPECT_CALL_GetConfigInt(kszHandFreePortEnable, bHandfree);
    EXPECT_CALL_GetConfigInt(kszHandsetPortEnable, bHandset);
    EXPECT_CALL_GetConfigInt(kszHeadsetPortEnable, bHeadset);
}

void ModVoiceTest::ExpectConfigVolume()
{
    EXPECT_CALL(m_objMockConfigParser, GetConfigInt(StrEq(kszVolumeTalkOutputHandset), _)).WillRepeatedly(Return(m_iVolumeTalkHandset));
    EXPECT_CALL(m_objMockConfigParser, GetConfigInt(StrEq(kszVolumeTalkOutputHeadset), _)).WillRepeatedly(Return(m_iVolumeTalkHeadset));
    EXPECT_CALL(m_objMockConfigParser, GetConfigInt(StrEq(kszVolumeTalkOutputHandfree), _)).WillRepeatedly(Return(m_iVolumeTalkHandfree));
    EXPECT_CALL(m_objMockConfigParser, GetConfigInt(StrEq(kszVolumeToneHandset), _)).WillRepeatedly(Return(m_iVolumeToneHandset));
    EXPECT_CALL(m_objMockConfigParser, GetConfigInt(StrEq(kszVolumeToneHeadset), _)).WillRepeatedly(Return(m_iVolumeToneHeadset));
    EXPECT_CALL(m_objMockConfigParser, GetConfigInt(StrEq(kszVolumeToneHandfree), _)).WillRepeatedly(Return(m_iVolumeToneHandfree));
    EXPECT_CALL(m_objMockConfigParser, GetConfigInt(StrEq(kszForceVoiceRing), _)).WillRepeatedly(Return(m_iVolumeFoceRing));
    EXPECT_CALL(m_objMockConfigParser, GetConfigInt(StrEq(kszVolumeRing), _)).WillRepeatedly(Return(m_iVolumeRing));
    EXPECT_CALL(m_objMockConfigParser, GetConfigInt(StrEq(kszVolumeOutputGroup), _)).WillRepeatedly(Return(m_iVolumeGroup));
}
