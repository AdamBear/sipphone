#include "vvmfileplayer.h"

#include "broadsoft/voicemail/include/modvirtualvoicemail.h"

CVVMFilePlayer::CVVMFilePlayer(QObject * pParent)
    : CFilePlayer(pParent)
{
}

CVVMFilePlayer::~CVVMFilePlayer()
{
}

bool CVVMFilePlayer::LoadFilePlayData(const FilePlayerData & structFilePlayerData)
{
    if (FILEPLAYERTYPE_INDEX != structFilePlayerData.m_eFilePlayerType
            || structFilePlayerData.m_nFileIndex < 0)
    {
        return false;
    }

    return CFilePlayer::LoadFilePlayData(structFilePlayerData);
}

bool CVVMFilePlayer::LogicPlay()
{
    return vvMail_Play();
}

bool CVVMFilePlayer::LogicPause()
{
    return vvMail_Pause();
}

bool CVVMFilePlayer::LogicResume()
{
    return vvMail_Resume();
}

bool CVVMFilePlayer::LogicStop()
{
    return vvMail_Stop();
}

bool CVVMFilePlayer::LogicSeek(long lSeekTimePoint)
{
    if (vvMail_GetPlayStatus() == PLAYSTATUS_Stop)
    {
        if (!vvMail_Play())
        {
            return false;
        }
    }

    return vvMail_Seek(lSeekTimePoint);
}
