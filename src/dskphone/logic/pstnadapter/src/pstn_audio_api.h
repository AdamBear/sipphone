#ifndef _PSTN_API_H_
#define _PSTN_API_H_

#include <sys/ioctl.h>


#define MAX_DEV_NUM                  5
#define MAX_SW_NUMBER                8

struct pstn_param
{
    int    line;
    int    active;
};


#define PSTNIOCS_MIN                 _IO('x',0xF0)
#define PSTNIOCS_MAX                 _IO('X',0xFF)
/*read software version*/
#define PSTNIOCG_SWVERSION           _IOR('X', 0xF1, char[MAX_SW_NUMBER])
/*set audio device work condition*/
#define PSTNIOCS_SETAUDIO            _IOW('X', 0xF2, struct pstn_param*)
/*get audio device work condition*/
#define PSTNIOCG_GETAUDIO            _IOR('X', 0xF3, struct pstn_param*)
/*set audio data dump*/
#define PSTNIOCS_SETAUDIODUMP        _IOW('X', 0xF4, int *)
/*get audio data dump condition*/
#define PSTNIOCS_GETAUDIODUMP        _IOR('X', 0xF5, int *)

#define pstndev "/dev/pstnaud%d"

#endif
