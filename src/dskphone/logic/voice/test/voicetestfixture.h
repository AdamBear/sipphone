#ifndef __VOICE_TEST_FIXTURE_H__
#define __VOICE_TEST_FIXTURE_H__

#include <gmock/gmock.h>

#include "mocklibs/configparser/mockconfigparser.h"
#include "mocklibs/etlmsghandler/mocketlmsghandler.h"
#include "mocklibs/msgkit/mockmkservice.h"
#include "mocklibs/ETLLib/mocksub-interface.h"
#include "mockvpm.h"


class ModVoiceTest : public testing::Test
{
public:
    ModVoiceTest();

protected:
    testing::StrictMock<MockConfigParser> m_objMockConfigParser;
    testing::StrictMock<MockEtlMsgHandler> m_objMockEtlMsgHandler;
    testing::StrictMock<MockMkService> m_objMockMkService;
    testing::StrictMock<MockSubInterface> m_objMockSubInterface;
    testing::StrictMock<MockVPM> m_objMockVPM;
    int m_iVolumeTalkHandset; // kszVolumeTalkOutputHandset
    int m_iVolumeTalkHeadset; // kszVolumeTalkOutputHeadset
    int m_iVolumeTalkHandfree; // kszVolumeTalkOutputHandfree
    int m_iVolumeToneHandset; // kszVolumeToneHandset
    int m_iVolumeToneHeadset; // kszVolumeToneHeadset
    int m_iVolumeToneHandfree; // kszVolumeToneHandfree
    int m_iVolumeFoceRing; // kszForceVoiceRing
    int m_iVolumeRing; // kszVolumeRing
    int m_iVolumeGroup; // kszVolumeOutputGroup

    virtual void SetUp();
    virtual void TearDown();

    // 各声道权限配置
    void ExpectConfigChannelModeEnable(bool bHandfree, bool bHandset, bool bHeadset);

    // 各声道音量配置
    void ExpectConfigVolume();
};

#endif // __VOICE_TEST_FIXTURE_H__
