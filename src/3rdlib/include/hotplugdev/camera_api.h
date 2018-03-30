/******************************************************************************
*
*  DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*  AUTHOR:
*       zhousw
*
*  DATE:2014-06-03
*
*       APIs for Camera device
******************************************************************************/
#ifndef __CAMERA_DRV_API_H__
#define __CAMERA_DRV_API_H__

/*
******************************************************************************
* video format control enum && struct
******************************************************************************
*/
typedef enum
{
    RES_VGA = 0,
    RES_PAL,
    RES_NTSC,
    RES_720P,
    RES_1080P

}RES_MODE;

typedef enum
{
    EXPOSURE_NEGA_4 = -4,
    EXPOSURE_NEGA_3,
    EXPOSURE_NEGA_2,
    EXPOSURE_MEGA_1,
    EXPOSURE_POSI_0,
    EXPOSURE_POSI_1,
    EXPOSURE_POSI_2,
    EXPOSURE_POSI_3,
    EXPOSURE_POSI_4,

}EXPOSURE_MODE;

typedef enum
{
    FLK_OFF = 0,
    FLK_50HZ = 50,
    FLK_60HZ = 60

}FLK_MODE;

typedef enum
{
    MIRROR_OFF = 0,
    MIRROR_FLIPH,
    MIRROR_FLIPV,
    MIRROR_FLIPHV

}MIRROR_MODE;

typedef enum
{
    RSZ_BINNING = 0,
    RSZ_SKIP
}RSZ_MODE;

typedef enum
{
    FLKDET_OFF = 0,
    FLKDET_ON

}FLKDET_MODE;

typedef struct
{
    int streamId;
    RES_MODE mode;

}cameraRes;

/*
******************************************************************************
* video tuning control && struct
******************************************************************************
*/
typedef enum
{
    METER_MATRIX = 0,
    METER_CENTERWEIGHT,
    METER_SPOT,
    METER_FACEAERA

}METER_MODE;

typedef enum
{
    AE_OFF = 0,
    AE_AUTO

}AE_MODE;

typedef enum
{
    AWB_OFF = 0,
    AWB_AUTO,
    AWB_D6500K,
    AWB_D5500K,
    AWB_FLORESCENT,
    AWB_INCANDESCENT,
    AWB_USER

}AWB_MODE;

typedef enum {

    AEPRI_EXPOSURE = 0,
    AEPRI_FRAMERATE

} AEWBPRI_MODE;


typedef enum
{
    GAIN_RED = 0,
    GAIN_BLUE

}GAIN_MODE;

typedef enum
{
    NF_OFF = 0,
    NF_LOW,
    NF_MID,
    NF_HIGH

}NF_MODE;

typedef enum
{
    LOGLEVEL_ERR = 0,
    LOGLEVEL_WARN,
    LOGLEVEL_NOTICE,
    LOGLEVEL_INFO,
    LOGLEVEL_DEBUG

}LOG_LEVEL;

typedef enum
{
    UPDATE_OFF = 0,
    UPDATE_EN

}UPDATE_MODE;

typedef enum
{
    STANDBY_OFF = 0,
    STANDBY_ON

}STANDBY_MODE;

typedef enum
{
    PG_OFF = 0,
    PG_ON

}PG_MODE;

typedef enum
{
    CAM_OFF = 0,
    CAM_ON

}CAM_MODE;

typedef enum
{
   POWER_OFF = 0,
   POWER_ON 
   
}POWER_MODE;

/*
******************************************************************************
* video debug control && struct
******************************************************************************
*/
#define NUM_OF_REF_1 30
#define NUM_OF_REF_2 15
#define NUM_OF_GRAY 4

typedef struct
{
    unsigned char *dcc_Default_Param;
    int dcc_init_done;
    unsigned int dccSize;
} cameraDccPrm;

typedef struct
{
    int regAddr;
    int regValue;
}cameraReg;

typedef struct
{
    unsigned long dccBuf;
    int dccSize;
}cameraDccUpdate;

typedef struct
{
    GAIN_MODE gain;
    int value;

}cameraGain;

typedef struct
{
    int minExposure;
    int maxExposure;
    int stepSize;
    int aGainMin;
    int aGainMax;
    int dGainMin;
    int dGainMax;
    int targetBrightnessMin;
    int targetBrightnessMax;
    int targetBrightness;
}cameraAEPrm;

typedef struct
{
    unsigned int num_of_ref_1;
    unsigned int num_of_ref_2;
    unsigned int num_of_gray;
    unsigned int color_temp_1[NUM_OF_REF_1];
    int wbReferenceCb[NUM_OF_REF_1][NUM_OF_GRAY];
    int wbReferenceCr[NUM_OF_REF_1][NUM_OF_GRAY];
    unsigned int ref_gray_R_1[NUM_OF_REF_1][NUM_OF_GRAY];
    unsigned int ref_gray_G_1[NUM_OF_REF_1][NUM_OF_GRAY];
    unsigned int ref_gray_B_1[NUM_OF_REF_1][NUM_OF_GRAY];
    unsigned int ref_index_2[NUM_OF_REF_2];
    unsigned int color_temp_2[NUM_OF_REF_2];
    unsigned int img_ref[NUM_OF_REF_2 * 1120];
    int referencesCb_2[NUM_OF_REF_2][NUM_OF_GRAY];
    int referencesCr_2[NUM_OF_REF_2][NUM_OF_GRAY];
    unsigned int ref_gray_R_2[NUM_OF_REF_2][NUM_OF_GRAY];
    unsigned int ref_gray_G_2[NUM_OF_REF_2][NUM_OF_GRAY];
    unsigned int ref_gray_B_2[NUM_OF_REF_2][NUM_OF_GRAY];
    unsigned int radius;
    int luma_awb_min;
    int luma_awb_max;
    unsigned int low_color_temp_thresh;
    unsigned int apply_rgb_adjust;
    int R_adjust;
    int B_adjust;
    unsigned int SB_1;
    unsigned int SB_2;
    unsigned int SB_low_bound;
    unsigned int default_T_H;
    unsigned int default_T_MH;
    unsigned int default_T_ML;
    unsigned int default_T_L;
    unsigned int default_T_H_index;
    unsigned int default_T_MH_index;
    unsigned int default_T_ML_index;
    unsigned int default_T_L_index;
    unsigned int best_gray_index_default;
}cameraAWBPrm;

/*
******************************************************************************
* video format control macro
******************************************************************************
*/
#define VIDEO_CAM_FORMAT_SET_RESOLUTION      _IOW('T', 0x01, cameraRes)
#define VIDEO_CAM_FORMAT_GET_RESOLUTION      _IOWR('T', 0x02, cameraRes)

#define VIDEO_CAM_FORMAT_SET_FRAMERATE       _IOW('T', 0x03, int)
#define VIDEO_CAM_FORMAT_GET_FRAMERATE       _IOR('T', 0x04, int*)

#define VIDEO_CAM_FORMAT_SET_EXPOSURE        _IOW('T', 0x05, EXPOSURE_MODE)
#define VIDEO_CAM_FORMAT_GET_EXPOSURE        _IOR('T', 0x06, EXPOSURE_MODE *)

#define VIDEO_CAM_FORMAT_SET_FLICKER         _IOW('T', 0x07, FLK_MODE)
#define VIDEO_CAM_FORMAT_GET_FLICKER         _IOR('T', 0x08, FLK_MODE *)

#define VIDEO_CAM_FORMAT_SET_FLICKER_DETETCT _IOW('T', 0x09, FLKDET_MODE)
#define VIDEO_CAM_FORMAT_GET_FLICKER_DETETCT _IOR('T', 0x0A, FLKDET_MODE *)

#define VIDEO_CAM_FORMAT_SET_MIRROR          _IOW('T', 0x0B, MIRROR_MODE)
#define VIDEO_CAM_FORMAT_GET_MIRROR          _IOR('T', 0x0C, MIRROR_MODE *)

#define VIDEO_CAM_FORMAT_SET_BINNING         _IOW('T', 0x0D, RSZ_MODE)
#define VIDEO_CAM_FORMAT_GET_BINNING         _IOR('T', 0x0E, RSZ_MODE *)

/*
******************************************************************************
* video tuning control macro
******************************************************************************
*/
#define VIDEO_CAM_TUNING_SET_METERING        _IOW('T', 0x0F, METER_MODE)
#define VIDEO_CAM_TUNING_GET_METERING        _IOR('T', 0x10, METER_MODE *)

#define VIDEO_CAM_TUNING_SET_NF              _IOW('T', 0x13, NF_MODE)
#define VIDEO_CAM_TUNING_GET_NF              _IOR('T', 0x14, NF_MODE *)

#define VIDEO_CAM_TUNING_SET_GAIN            _IOW('T', 0x15, cameraGain)
#define VIDEO_CAM_TUNING_GET_GAIN            _IOR('T', 0x16, cameraGain)

#define VIDEO_CAM_TUNING_SET_AE              _IOW('T', 0x17, AE_MODE)
#define VIDEO_CAM_TUNING_GET_AE              _IOR('T', 0x18, AE_MODE *)

#define VIDEO_CAM_TUNING_SET_AWB             _IOW('T', 0x19, AWB_MODE)
#define VIDEO_CAM_TUNING_GET_AWB             _IOR('T', 0x1A, AWB_MODE *)

#define VIDEO_CAM_TUNING_SET_AEWB_PRIORITY   _IOW('T', 0x1D, AEWBPRI_MODE)
#define VIDEO_CAM_TUNING_GET_AEWB_PRIORITY   _IOR('T', 0x1E, AEWBPRI_MODE *)

#define VIDEO_CAM_TUNING_SET_BRIGHTNESS      _IOW('T', 0x1F, int)
#define VIDEO_CAM_TUNING_GET_BRIGHTNESS      _IOR('T', 0x20, int *)

#define VIDEO_CAM_TUNING_SET_CONTRAST        _IOW('T', 0x21, int)
#define VIDEO_CAM_TUNING_GET_CONTRAST        _IOR('T', 0x22, int *)

#define VIDEO_CAM_TUNING_SET_SHARPNESS       _IOW('T', 0x23, int)
#define VIDEO_CAM_TUNING_GET_SHARPNESS       _IOR('T', 0x24, int *)

#define VIDEO_CAM_TUNING_SET_SATURATION      _IOW('T', 0x25, int)
#define VIDEO_CAM_TUNING_GET_SATURATION      _IOR('T', 0x26, int *)

#define VIDEO_CAM_TUNING_SET_ZOOM            _IOW('T', 0x27, int)

#define VIDEO_CAM_TUNING_SET_LOGLEVEL        _IOW('T', 0x28, LOG_LEVEL)
#define VIDEO_CAM_TUNING_GET_LOGLEVEL        _IOR('T', 0x29, LOG_LEVEL *)

#define VIDEO_CAM_TUNING_SET_CAM_STANDBY      _IOW('T', 0x2A, STANDBY_MODE)

#define VIDEO_CAM_TUNING_SET_CAM_PG           _IOW('T', 0x2B, PG_MODE)

#define VIDEO_CAM_TUNING_SET_CAM_ONOFF        _IOW('T', 0x2C, CAM_MODE)

#define VIDEO_CAM_TUNING_GET_SENSORID         _IOR('T', 0x2D, int)

#define VIDEO_CAM_TUNING_SET_CAM_PW           _IOW('T', 0x2E, POWER_MODE)

/*
******************************************************************************
* video debug control macro
******************************************************************************
*/
#define VIDEO_CAM_DEBUG_SET_SENSOR_REG       _IOWR('T', 0x2E, cameraReg)
#define VIDEO_CAM_DEBUG_GET_SENSOR_REG       _IOWR('T', 0x2F, cameraReg)

#define VIDEO_CAM_DEBUG_SET_ISS_REG          _IOWR('T', 0x30, cameraReg)
#define VIDEO_CAM_DEBUG_GET_ISS_REG          _IOWR('T', 0x31, cameraReg)

#define VIDEO_CAM_DEBUG_SET_MEM_REG          _IOWR('T', 0x32, cameraReg)
#define VIDEO_CAM_DEBUG_GET_MEM_REG          _IOWR('T', 0x33, cameraReg)

#define VIDEO_CAM_DEBUG_SET_DCC_UPDATE       _IOW('T', 0x34, cameraDccUpdate)
#define VIDEO_CAM_DEBUG_GET_DCC_UPDATE       _IOWR('T', 0x35, cameraDccUpdate)

#define VIDEO_CAM_DEBUG_SET_DCC_PARAM        _IOW('T', 0x36, cameraDccPrm)
#define VIDEO_CAM_DEBUG_GET_DCC_PARAM        _IOWR('T', 0x37, cameraDccPrm)

#define VIDEO_CAM_DEBUG_SET_AE_PARAM         _IOW('T', 0x38, cameraAEPrm)
#define VIDEO_CAM_DEBUG_GET_AE_PARAM         _IOWR('T', 0x39, cameraAEPrm)

#define VIDEO_CAM_DEBUG_SET_AWB_PARAM        _IOW('T', 0x3A, int)
#define VIDEO_CAM_DEBUG_GET_AWB_PARAM        _IOWR('T', 0x3B, int)

#define VIDEO_CAM_DEBUG_SET_UPDATE_MODE      _IOWR('T', 0x3C, UPDATE_MODE)

#define VIDEO_CAM_DEBUG_GET_914ID            _IOR('T', 0x3D, int)

/************************************************************************
*           user helper API  for camera  - format                       *
************************************************************************/
/*
* function: open camera
*/
static inline int Camera_helper_open(void)
{
    return open("/dev/camera", O_RDWR);
}

/*
* function   : close camera
* fd         : file descriptions
*/
static inline int Camera_helper_close(int fd)
{
    return close(fd);
}

/*
* function   : get resolution
* fd         : file descriptions
* StreamId   : channel Id
* mode       : resolution value
*/
static inline int Camera_helper_set_Resolution(int fd, int StreamId, RES_MODE mode)
{
    cameraRes resParams;

    if ((mode > RES_1080P) || (mode < RES_VGA))
    {
        return -1;
    }

    resParams.mode = mode;
    resParams.streamId = StreamId;

    if (fd > 0)
    {
        if (ioctl(fd, VIDEO_CAM_FORMAT_SET_RESOLUTION, (unsigned long)&resParams) >= 0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get resolution
* fd         : file descriptions
* StreamId   : channel Id
* mode       : resolution value
*/
static inline int Camera_helper_get_Resolution(int fd, int StreamId, RES_MODE *mode)
{
    cameraRes resParams = {0};

    resParams.streamId = StreamId;

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_FORMAT_GET_RESOLUTION,(unsigned long)&resParams)>=0)
        {
            *mode = resParams.mode;
            if ((*mode <= RES_1080P) && (*mode >= RES_VGA))
            {
                return 0;
            }
        }
    }
    return -1;
}

/*
* function   : get exposure
* fd         : file descriptions
* StreamId   : channel Id
* mode       : exposure value
*/
static inline int Camera_helper_set_Exposure(int fd, EXPOSURE_MODE mode)
{
    if ((mode > EXPOSURE_POSI_4) || (mode < EXPOSURE_NEGA_4))
    {
        return -1;
    }

    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_FORMAT_SET_EXPOSURE,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get exposure
* fd         : file descriptions
* StreamId   : channel Id
* mode       : exposure value
*/
static inline int Camera_helper_get_Exposure(int fd, EXPOSURE_MODE *mode)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_FORMAT_GET_EXPOSURE,(unsigned long)mode)>=0)
        {
            if ((*mode <= EXPOSURE_POSI_4) && (*mode >= EXPOSURE_NEGA_4))
            {
                return 0;
            }
        }
    }
    return -1;
}


/*
* function   : set framerate
* fd         : file descriptions
* rate       : framerate value
*/
static inline int Camera_helper_set_Framerate(int fd, int rate)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_FORMAT_SET_FRAMERATE,(unsigned long)&rate)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get framerate
* fd         : file descriptions
* framerate  : framerate value
*/
static inline int Camera_helper_get_Framerate(int fd, int *rate)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_FORMAT_GET_FRAMERATE,(unsigned long)rate)>=0)
        {
           return 0;
        }
    }
    return -1;
}

/*
* function   : set flicker
* fd         : file descriptions
* mode       : flicker value
*/
static inline int Camera_helper_set_Flicker(int fd, FLK_MODE mode)
{
    if ((mode > FLK_60HZ) || (mode < FLK_OFF))
    {
        return -1;
    }

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_FORMAT_SET_FLICKER,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get flicker
* fd         : file descriptions
* mode       : flicker value
*/
static inline int Camera_helper_get_Flicker(int fd, FLK_MODE *mode)
{
    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_FORMAT_GET_FLICKER,(unsigned long)mode)>=0)
        {
            if((*mode >= FLK_OFF) && (*mode <= FLK_60HZ))
            {
                return 0;
            }
        }
    }
    return -1;
}

/*
* function   : set flicker detect
* fd         : file descriptions
* mode       : flicker detect value
*/
static inline int Camera_helper_set_FlickerDetect(int fd, FLKDET_MODE mode)
{
    if((mode < FLKDET_OFF) || (mode > FLKDET_ON))
    {
        return -1;
    }

    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_FORMAT_SET_FLICKER_DETETCT,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get flicker detect
* fd         : file descriptions
* mode       : flicker detect value
*/
static inline int Camera_helper_get_FlickerDetect(int fd, FLKDET_MODE *mode)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_FORMAT_GET_FLICKER_DETETCT,(unsigned long)mode)>=0)
        {
            if ((*mode <= FLKDET_ON) && (*mode >= FLKDET_OFF))
            {
                return 0;
            }
        }
    }
    return -1;
}

/*
* function   : set mirror
* fd         : file descriptions
* mode       : mirror value
*/
static inline int Camera_helper_set_Mirrormode(int fd, MIRROR_MODE mode)
{
    if ((mode > MIRROR_FLIPHV) || (mode < MIRROR_OFF))
    {
        return -1;
    }

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_FORMAT_SET_MIRROR,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get mirror
* fd         : file descriptions
* mode       : mirror value
*/
static inline int Camera_helper_get_Mirrormode(int fd, MIRROR_MODE *mode)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_FORMAT_GET_MIRROR,(unsigned long)mode)>=0)
        {
            if ((*mode <= MIRROR_FLIPHV) && (*mode >= MIRROR_OFF))
            {
                return 0;
            }
        }
    }
    return -1;
}


/*
* function   : set binning
* fd         : file descriptions
* mode       : binning value
*/
static inline int Camera_helper_set_Binning(int fd, RSZ_MODE mode)
{
    if ((mode > RSZ_SKIP) || (mode < RSZ_BINNING))
    {
        return -1;
    }

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_FORMAT_SET_BINNING,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get binning
* fd         : file descriptions
* mode       : binning value
*/
static inline int Camera_helper_get_Binning(int fd, RSZ_MODE *mode)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_FORMAT_GET_BINNING,(unsigned long)mode)>=0)
        {
            if ((*mode <= RSZ_SKIP) && (*mode >= RSZ_BINNING))
            {
                return 0;
            }
        }
    }
    return -1;
}

/************************************************************************
*           user helper API  for camera  - Tunning                      *
************************************************************************/
/*
* function   : set noise filter mode
* fd         : file descriptions
* mode       : noise filter mode value
*/
static inline int Camera_helper_set_NoiseFilter(int fd, NF_MODE mode)
{
    if ((mode > NF_HIGH) || (mode < NF_OFF))
    {
        return -1;
    }

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_SET_NF,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get noise filter mode
* fd         : file descriptions
* mode       : noise filter  mode value
*/
static inline int Camera_helper_get_NoiseFilter(int fd, NF_MODE *mode)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_GET_NF,(unsigned long)mode)>=0)
        {
            if ((*mode <= NF_HIGH) && (*mode >= NF_OFF))
            {
                return 0;
            }
        }
    }
    return -1;
}

/*
* function   : set meter mode
* fd         : file descriptions
* mode       : meter mode value
*/
static inline int Camera_helper_set_MeteringMode(int fd, METER_MODE mode)
{
    if ((mode >METER_FACEAERA) || (mode < METER_MATRIX))
    {
        return -1;
    }

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_SET_METERING,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get meter mode
* fd         : file descriptions
* mode       : meter mode value
*/
static inline int Camera_helper_get_MeteringMode(int fd,METER_MODE *mode)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_GET_METERING,(unsigned long)mode)>=0)
        {
            if ((*mode <= METER_FACEAERA) && (*mode >= METER_MATRIX))
            {
                return 0;
            }
        }
    }
    return -1;
}


/*
* function   : set gain Mode
* fd         : file descriptions
* mode       : gain Mode value
*/
static inline int Camera_helper_set_Gain(int fd, GAIN_MODE mode, int value)
{
    cameraGain camGain = {(GAIN_MODE)0,0};

    if ((mode > GAIN_BLUE) || (mode < GAIN_RED))
    {
        return -1;
    }

    if ((value > 100) || (value < 0))
    {
        return -1;
    }

    camGain.gain = mode;
    camGain.value = value;

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_SET_GAIN,(unsigned long)&camGain)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get gain Mode
* fd         : file descriptions
* mode       : gain Mode value
*/
static inline int Camera_helper_get_Gain(int fd, GAIN_MODE mode, int *value)
{
    cameraGain camGain = {(GAIN_MODE)0,0};

    if((mode > GAIN_BLUE) || (mode < GAIN_RED))
    {
        return -1;
    }

    camGain.gain = mode;

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_GET_GAIN,(unsigned long)&camGain)>=0)
        {
            if ((camGain.value <= 100) && (camGain.value >= 0))
            {
                *value = camGain.value;
                return 0;
            }
        }
    }
    return -1;
}

/*
* function   : set AEMode
* fd         : file descriptions
* mode       : AEMode value
*/
static inline int Camera_helper_set_AEMode(int fd, AE_MODE mode)
{
    if ((mode >AE_AUTO) || (mode < AE_OFF))
    {
        return -1;
    }

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_SET_AE,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get AEMode
* fd         : file descriptions
* mode       : AEMode value
*/
static inline int Camera_helper_get_AEMode(int fd, AE_MODE *mode)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_GET_AE,(unsigned long)mode)>=0)
        {
            if ((*mode <= AE_AUTO) && (*mode >= AE_OFF))
            {
                return 0;
            }
        }
    }
    return -1;
}

/*
* function   : set AWB_MODE
* fd         : file descriptions
* mode       : AWB_MODE value
*/
static inline int Camera_helper_set_AWBMode(int fd, AWB_MODE mode)
{
    if ((mode >AWB_USER) || (mode < AWB_OFF))
    {
        return -1;
    }

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_SET_AWB,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get AWB_MODE
* fd         : file descriptions
* mode       : AWB_MODE value
*/
static inline int Camera_helper_get_AWBMode(int fd, AWB_MODE *mode)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_GET_AWB,(unsigned long)mode)>=0)
        {
            if((*mode <= AWB_USER) && (*mode >= AWB_OFF))
            {
                return 0;
            }
        }
    }
    return -1;
}

/*
* function   : set AEWB Priority Mode
* fd         : file descriptions
* mode       : AEWB Priority Mode value
*/
static inline int Camera_helper_set_AEWBPriorityMode(int fd, AEWBPRI_MODE mode)
{
    if((mode < AEPRI_EXPOSURE) || (mode > AEPRI_FRAMERATE))
    {
        return -1;
    }

    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_TUNING_SET_AEWB_PRIORITY,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get AEWB Priority Mode
* fd         : file descriptions
* mode       : AEWB Priority Mode value
*/
static inline int Camera_helper_get_AEWBPriorityMode(int fd, AEWBPRI_MODE *mode)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_GET_AEWB_PRIORITY,(unsigned long)mode)>=0)
        {
            if ((*mode <= AEPRI_FRAMERATE) && (*mode >= AEPRI_EXPOSURE))
            {
                return 0;
            }
        }
    }
    return -1;
}


/*
* function   : set brightness
* fd         : file descriptions
* Brightness : brightness value
*/
static inline int Camera_helper_set_Brightness(int fd, int Brightness)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_SET_BRIGHTNESS,(unsigned long)&Brightness)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get brightness
* fd         : file descriptions
* Brightness : brightness value
*/
static inline int Camera_helper_get_Brightness(int fd, int *Brightness)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_GET_BRIGHTNESS,(unsigned long)Brightness)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : set contrast
* fd         : file descriptions
* Brightness : contrast value
*/
static inline int Camera_helper_set_Contrast(int fd, int Contrast)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_SET_CONTRAST,(unsigned long)&Contrast)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get contrast
* fd         : file descriptions
* Brightness : contrast value
*/
static inline int Camera_helper_get_Contrast(int fd, int *Contrast)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_GET_CONTRAST,(unsigned long)Contrast)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : set sharpness
* fd         : file descriptions
* Brightness : sharpness value
*/
static inline int Camera_helper_set_Sharpness(int fd, int Sharpness)
{
    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_TUNING_SET_SHARPNESS,(unsigned long)&Sharpness)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get sharpness
* fd         : file descriptions
* Brightness : brightness value
*/
static inline int Camera_helper_get_Sharpness(int fd, int *Sharpness)
{
    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_GET_SHARPNESS,(unsigned long)Sharpness)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : set saturation
* fd         : file descriptions
* Brightness : sharpness value
*/
static inline int Camera_helper_set_Saturation(int fd, int Saturation)
{
    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_TUNING_SET_SATURATION,(unsigned long)&Saturation)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get saturation
* fd         : file descriptions
* Brightness : brightness value
*/
static inline int Camera_helper_get_Saturation(int fd,int *Saturation)
{
    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_TUNING_GET_SATURATION,(unsigned long)Saturation)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : set zoom value
* fd         : file descriptions
* Brightness : zoom value value
*/
static inline int Camera_helper_set_Zoom(int fd, int Zoom)
{
    if(Zoom > 40 || Zoom < 10)
        return -1;

    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_TUNING_SET_ZOOM,(unsigned long)&Zoom)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : set log level
* fd         : file descriptions
* Brightness : log level value
*/
static inline int Camera_helper_set_LogLevel(int fd,LOG_LEVEL level)
{
    if((level >LOGLEVEL_DEBUG) || (level < LOGLEVEL_ERR))
        return -1;

    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_TUNING_SET_LOGLEVEL,(unsigned long)&level)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get log level
* fd         : file descriptions
* Brightness : log level value
*/
static inline int Camera_helper_get_LogLevel(int fd,LOG_LEVEL *level)
{
    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_TUNING_GET_LOGLEVEL,(unsigned long)level)>=0)
        {
            if((*level <= LOGLEVEL_DEBUG) && (*level >= LOGLEVEL_ERR))
            {
                return 0;
            }
        }
    }
    return -1;
}

/*
* function   : set standby mode
* fd         : file descriptions
* mode     : STANDBY_MODE
*/
static inline int Camera_helper_set_StandbyMode(int fd,STANDBY_MODE mode)
{
    if((mode < STANDBY_OFF) || (mode > STANDBY_ON))
    {
        return -1;
    }

    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_TUNING_SET_CAM_STANDBY,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : set PG mode
* fd         : file descriptions
* mode     : PG_MODE
*/
static inline int Camera_helper_set_PGMode(int fd,PG_MODE mode)
{
    if((mode < PG_OFF) || (mode > PG_ON))
    {
        return -1;
    }

    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_TUNING_SET_CAM_PG,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : set camera on off
* fd         : file descriptions
* mode     : CAM_MODE
*/
static inline int Camera_helper_set_CamMode(int fd,CAM_MODE mode)
{
    if((mode < CAM_OFF) || (mode > CAM_ON))
    {
        return -1;
    }

    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_TUNING_SET_CAM_ONOFF,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get sensor id
* fd         : file descriptions
* sensorId   : sensor id
*/
static inline int Camera_helper_get_SensorId(int fd, int *sensorId)
{
    if(!sensorId)
    {
        return -1;
    }

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_TUNING_GET_SENSORID,(unsigned long)sensorId)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : set camera power on off
* fd         : file descriptions
* mode       : POWER_MODE
*/
static inline int Camera_helper_set_PowMode(int fd,POWER_MODE mode)
{
    if((mode < POWER_OFF) || (mode > POWER_ON))
    {
        return -1;
    }

    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_TUNING_SET_CAM_PW,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/************************************************************************
*           user helper API  for camera  - Debug                        *
************************************************************************/
/*
* function: open camera debug
*/
static inline int Camera_helper_open_d(void)
{
    return open("/dev/camera_debug", O_RDWR);
}

/*
* function   : set update mode
* fd         : file descriptions
* mode     : UPDATE_MODE
*/
static inline int Camera_helper_set_UpdateMode(int fd,UPDATE_MODE mode)
{
    if((mode < UPDATE_OFF) || (mode > UPDATE_EN))
    {
        return -1;
    }

    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_DEBUG_SET_UPDATE_MODE,(unsigned long)&mode)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : raw image capture
* fd         : file descriptions
* rawBuf     : buf to store raw image
* max_size   : max size of buf
*/
static inline int Camera_helper_get_RAWImage(int fd,char *rawBuf,int max_size)
{
    int ret = 0;

    if ((rawBuf == NULL) || (max_size == 0))
    {
        return -1;
    }

    if (fd > 0)
    {
        if ((ret = read(fd,rawBuf,max_size))>0)
        {
            return ret;
        }
    }
    return -1;
}

/*
* function   : write sensor register
* fd         : file descriptions
* regAddr    : sensor register address
* regValue   : sensor register value
*/
static inline int Camera_helper_set_SensorRegister
(
    int fd, int regAddr, int regValue
)
{
    cameraReg reg = {0};

    if (!regAddr)
    {
        return -1;
    }

    reg.regAddr = regAddr;
    reg.regValue = regValue;

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_DEBUG_SET_SENSOR_REG,(unsigned long)&reg)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : read sensor register
* fd         : file descriptions
* regAddr    : sensor register address
* regValue   : sensor register value
*/
static inline int Camera_helper_get_SensorRegister
(
    int fd, int regAddr, int *regValue
)
{
    cameraReg reg = {0};

    if (!regAddr)
    {
        return -1;
    }

    reg.regAddr = regAddr;

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_DEBUG_GET_SENSOR_REG,(unsigned long)&reg)>=0)
        {
            *regValue = reg.regValue;
            return 0;
        }
    }
    return -1;
}

/*
* function   : write iss register
* fd         : file descriptions
* regAddr    : iss register address
* regValue   : iss register value
*/
static inline int Camera_helper_set_ISSRegister
(
    int fd, int regAddr, int regValue
)
{
    cameraReg reg = {0};

    if (!regAddr)
    {
        return -1;
    }

    reg.regAddr = regAddr;
    reg.regValue = regValue;

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_DEBUG_SET_ISS_REG,(unsigned long)&reg)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : read iss register
* fd         : file descriptions
* regAddr    : iss register address
* regValue   : iss register value
*/
static inline int Camera_helper_get_ISSRegister
(
    int fd, int regAddr, int *regValue
)
{
    cameraReg reg = {0};

    if (!regAddr)
    {
        return -1;
    }

    reg.regAddr = regAddr;

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_DEBUG_GET_ISS_REG,(unsigned long)&reg)>=0)
        {
            *regValue = reg.regValue;
            return 0;
        }
    }
    return -1;
}

/*
* function   : write mem register
* fd         : file descriptions
* regAddr    : mem register address
* regValue   : mem register value
*/
static inline int Camera_helper_set_mem(int fd,int memAddr,int value)
{
    cameraReg reg = {0};

    if (!memAddr)
    {
        return -1;
    }

    reg.regAddr = memAddr;
    reg.regValue = value;

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_DEBUG_SET_MEM_REG,(unsigned long)&reg)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : read mem register
* fd         : file descriptions
* regAddr    : mem register address
* regValue   : mem register value
*/
static inline int Camera_helper_get_mem(int fd, int memAddr, int *value)
{
    cameraReg reg = {0};

    if (!memAddr)
    {
        return -1;
    }

    reg.regAddr = memAddr;

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_DEBUG_GET_MEM_REG,(unsigned long)&reg)>=0)
        {
            *value = reg.regValue;
            return 0;
        }
    }
    return -1;
}

/*
* function   : set AE Param
* fd         : file descriptions
* AePrm     : AE Param
*/
static inline int Camera_helper_set_AEParam(int fd,cameraAEPrm AePrm)
{
    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_DEBUG_SET_AE_PARAM,(unsigned long)&AePrm)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get AE Param
* fd         : file descriptions
* AePrm     : AE Param
*/
static inline int Camera_helper_get_AEParam(int fd, cameraAEPrm *AePrm)
{
    if(!AePrm)
    {
        return -1;
    }

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_DEBUG_GET_AE_PARAM,(unsigned long)AePrm)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : set AWB Param
* fd         : file descriptions
* AwbPrm     : AWB Param
*/
static inline int Camera_helper_set_AWBParam(int fd,cameraAWBPrm AwbPrm)
{
    if (fd > 0)
    {
        if(ioctl(fd,VIDEO_CAM_DEBUG_SET_AWB_PARAM,(unsigned long)&AwbPrm)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : get AWB Param
* fd         : file descriptions
* AwbPrm     : AWB Param
*/
static inline int Camera_helper_get_AWBParam(int fd, cameraAWBPrm *AwbPrm)
{
    if(!AwbPrm)
    {
        return -1;
    }

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_DEBUG_GET_AWB_PARAM,(unsigned long)AwbPrm)>=0)
        {
            return 0;
        }
    }

    return -1;
}

/*
* function   : get 914 id
* fd         : file descriptions
* sensorId   : sensor id
*/
static inline int Camera_helper_get_914Id(int fd, int *id)
{
    if(!id)
    {
        return -1;
    }

    if (fd > 0)
    {
        if (ioctl(fd,VIDEO_CAM_DEBUG_GET_914ID,(unsigned long)id)>=0)
        {
            return 0;
        }
    }
    return -1;
}

/*
* function   : close camera debug
* fd         : file descriptions
*/
static inline int Camera_helper_close_d(int fd)
{
    return close(fd);
}


#endif
