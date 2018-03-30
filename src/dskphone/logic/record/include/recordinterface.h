#ifndef __RECORD_INTERFACE_H__
#define __RECORD_INTERFACE_H__

#ifdef IF_USB_SUPPORT
#include <service_ipvp.h>

struct MediaInfo
{
    yl::string strFile; // 全名
    int nSecTotal;      // 总时长
    int nSecCurrent;    // 当前时长

    MediaInfo()
    {
        strFile = "";
        nSecCurrent = 0;
        nSecTotal = 0;
    }
};

bool RecordInterface_RecordStart(const char * pFile, int sync = 0);

bool RecordInterface_RecordStop(int sync = 0);

bool RecordInterface_RecordPause(int sync = 0);

bool RecordInterface_RecordResume(int sync = 0);

bool RecordInterface_PlayStart(const char * pFile, int sync = 0);

bool RecordInterface_PlayPause(int sync = 0);

bool RecordInterface_PlayResume(int sync = 0);

bool RecordInterface_PlayStop(int sync = 0);

bool RecordInterface_PlaySeek(unsigned int locate, int sync = 0);

bool RecordInterface_GetInfo(MediaInfo & mediaInfo, const char * pFile);

#ifdef IF_SUPPORT_VIDEO
void RecordInterface_SetVideoLCDLayout();

void RecordInterface_SetVideoHDMILayout();

void RecordInterface_ClearVideoLCDLayout();

void RecordInterface_ClearVideoHDMILayout();
#endif //#ifdef IF_SUPPORT_VIDEO

#endif // IF_USB_RECORD

#ifdef _T49
bool RecordInterface_Screenshot(const char * pFile);
#endif

#endif // __RECORD_INTERFACE_H__
