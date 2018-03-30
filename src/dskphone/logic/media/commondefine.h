#ifndef _MEDIA_COMMON_H_
#define _MEDIA_COMMON_H_

/************************************************************************/
/*
    Media 模块提供GUI的外部模块使用定义
*/
/************************************************************************/
// 接口测试，开启宏
//#define IS_MEDIA_TEST

// media 模块调试宏
#define  DEBUG_MEDIA 1

#include "yllist.h"
#include "ETLLib.hpp"
#include "service_ipvp_types.h"
#include "service_dsk_define.h"

enum LayoutType
{
    LT_VIDEO,
    LT_CAMERA,
    LT_FILE,
    // 辅流
    LT_EXTVIDEO,
};

enum DisplayDeviceType
{
    DDT_LCD,
    DDT_HDMI,
};

struct LayoutWindow
{
    LayoutWindow()
    {
        m_iCallId = -1;
        m_eLayoutType = LT_VIDEO;
        m_eDisplayDevice = DDT_LCD;
    }

    bool operator==(LayoutWindow & window)
    {
        return (m_iCallId == window.m_iCallId
                && m_rectVideo == window.m_rectVideo
                && m_eLayoutType == window.m_eLayoutType
                && m_eDisplayDevice == window.m_eDisplayDevice);
    }

    LayoutWindow & operator = (const LayoutWindow & window)
    {
        if (this != &window)
        {
            m_iCallId = window.m_iCallId;
            m_eLayoutType = window.m_eLayoutType;
            m_eDisplayDevice = window.m_eDisplayDevice;
            m_rectVideo = window.m_rectVideo;
        }
        return *this;
    }

    // 通话id
    int             m_iCallId;
    // 布局
    chRect          m_rectVideo;
    // 布局类型
    LayoutType      m_eLayoutType;
    // 布局在哪个设备上显示
    DisplayDeviceType m_eDisplayDevice;
};

//视频显示设备
enum VideoDisplayDevice
{
    VDD_NONE = 0x00,
    VDD_HDMI = 0x01 << 1,          //HDMI
    VDD_LCD  = 0x01 << 2,          //液晶
    VDD_ALL  = 0xFF                //都显示
};
typedef YLList<LayoutWindow>    LayoutWindowList;

struct DisplayMonitor
{
    LayoutWindowList   m_listLayout;
    VideoDisplayDevice m_eDisplay;

    DisplayMonitor()
    {
        Reset();
    }

    void Reset()
    {
        m_listLayout.clear();
        m_eDisplay = VDD_ALL;
    }

    // 添加通话布局,Lcd 布局要求基于1920/1080而不是实际Lcd分辨率的1280*800
    void AddVideoLayout(int iCallId, const chRect & rect)
    {
        //VOICE_INFO("Add video [%d]", iCallId);
        LayoutWindow layout;
        layout.m_iCallId = iCallId;
        layout.m_rectVideo = rect;
        layout.m_eLayoutType = LT_VIDEO;
        layout.m_eDisplayDevice = DDT_LCD;

        m_listLayout.push_back(layout);
        //VOICE_INFO("Add video [%d][%d]", iCallId, m_listLayout.size());
    }

    // 添加本地视频布局
    void AddCameraLayout(const chRect & rect)
    {
        LayoutWindow layout;
        layout.m_iCallId = -1;
        layout.m_rectVideo = rect;
        layout.m_eLayoutType = LT_CAMERA;
        layout.m_eDisplayDevice = DDT_LCD;
        m_listLayout.push_back(layout);
        //VOICE_INFO("Add camera [%d]",  m_listLayout.size());
    }

    //  添加文件视频布局
    void AddFileLayout(const chRect & rect)
    {
        LayoutWindow layout;
        layout.m_iCallId = -1;
        layout.m_rectVideo = rect;
        layout.m_eLayoutType = LT_FILE;
        layout.m_eDisplayDevice = DDT_LCD;
        m_listLayout.push_back(layout);
        m_eDisplay = VDD_LCD;
    }

    // 添加通话到扩展屏布局
    void AddHdmiVideoLayout(int iCallId, const chRect & rect)
    {
        LayoutWindow layout;
        layout.m_iCallId = iCallId;
        layout.m_rectVideo = rect;
        layout.m_eLayoutType = LT_VIDEO;
        layout.m_eDisplayDevice = DDT_HDMI;
        m_listLayout.push_back(layout);
    }

    // 添加本地视频到扩展屏布局
    void AddHdmiCameraLayout(const chRect & rect)
    {
        LayoutWindow layout;
        layout.m_iCallId = -1;
        layout.m_rectVideo = rect;
        layout.m_eLayoutType = LT_CAMERA;
        layout.m_eDisplayDevice = DDT_HDMI;
        m_listLayout.push_back(layout);
    }

    //  添加文件视频到扩展屏布局
    void AddHdmiFileLayout(const chRect & rect)
    {
        LayoutWindow layout;
        layout.m_iCallId = -1;
        layout.m_rectVideo = rect;
        layout.m_eLayoutType = LT_FILE;
        layout.m_eDisplayDevice = DDT_HDMI;
        m_listLayout.push_back(layout);
        m_eDisplay = VDD_HDMI;
    }

    DisplayMonitor & operator = (const DisplayMonitor & display)
    {
        if (this != &display)
        {
            m_eDisplay = display.m_eDisplay;
            m_listLayout = display.m_listLayout;
        }

        return *this;
    }

    bool operator ==(DisplayMonitor & display)
    {
        if (m_listLayout.size() != display.m_listLayout.size())
        {
            return false;
        }

        // 遍历每一个布局
        LayoutWindowList::iterator it = display.m_listLayout.begin();
        for (; it != display.m_listLayout.end(); ++it)
        {
            LayoutWindow & windowDisp = *it;
            bool bFind = false;
            for (LayoutWindowList::iterator itTmp = m_listLayout.begin(); itTmp != m_listLayout.end(); ++itTmp)
            {
                LayoutWindow & window = *itTmp;
                if (windowDisp == window)//memcmp(&windowDisp, &window, sizeof(LayoutWindow)))
                {
                    bFind = true;
                    break;
                }
            }

            if (!bFind)
            {
                // 有一个不相等，即认为不相等
                return false;
            }
        }

        // 全部相等
        return true;
    }
};

// 视频分辨率类型
// video size type
enum VideoSizeType
{
    // 视频
    VST_1080P,   // 1920*1080
    VST_720P,    // 1280*720
    VST_4CIF,    // 704*576
    VST_CIF,     // 352*288

    // 辅流
    VST_SXGA,       // 1280*1024
    VST_WXGA_5_3,   // 1280*768
    VST_XGA,        // 1024*768
    VST_SVGA,       // 800*600
    VST_VGA,        // 640*480

    // wide screen
    VST_W576P,     // 1024*576
    VST_W448P,     // 768*448
    VST_WQVGA,     // 400*240

    VST_SIZE_MAX
};

struct OsdLayoutInfo
{
    // Osd Id > 0
    int             m_iId;
    // 在视频布局上的位置，左上角
    int             m_iOffsetX;
    int             m_iOffsetY;
    // 布局类型
    LayoutType      m_eLayoutType;
    // Osd 图片路径
    yl::string      m_strPath;

    OsdLayoutInfo()
    {
        m_iId = -1;
        m_iOffsetX = 0;
        m_iOffsetY = 0;
        m_eLayoutType = LT_CAMERA;
        m_strPath.clear();
    }
};

/************************************************************************/
/*
profile_idc: 就是叫做profile，主要用于低消耗的应用程序，需要保证丢包时的鲁棒性。
profile_iop：主要用于数字电视广播，在2004 HIGH PROFILE出现后已经较少使用。
max-mbps：每秒处理宏区块数最大值。
4、max-fs：单位宏区块最大帧尺寸。
5、max-br：最大视频比特率。
6、packetization-mode：打包模式，对NAL单元的打包模式，H.264一共规定了3种模式，值为0到2。如何打包我们不需要关心，只需知道有3种模式即可，该参数不在时值为0。
*/
/************************************************************************/
// 视频编码扩展参数
struct VideoCodecExtParam
{
    int m_iCIF;
    int m_iQCIF;
    int m_iSQCIF;
    int m_iCIF4;

    // H264 Params
    int m_iProfileIDC;
    int m_iLevel;
    int m_iProfileIOP;
    int m_iMaxMBPS; //In sip is max_mbps;
    int m_iMaxFS;   //In sip is max_fs;
    int m_iMaxDPB;
    int m_iMaxCPB;
    int m_iMaxBR;
    int m_iMaxnalunitsize;
    int m_iSampleAspectRatiosSupported;// In sip is sar;
    int m_iPacketizationMode;

    // H263+ params
    int m_iAdvancedintra;
    int m_iModifiedquantization;
};

// 视频扩展参数信息
struct VideoExtInfo
{
    VideoCodecExtParam m_stLocal;   // 本地解码
    VideoCodecExtParam m_stRemote;  // 对方用于编码
};

struct VideoCodecInfo
{
    ipvp_vcodec_t codec;
    VideoCodecExtParam extParam;
};

typedef YLList<VideoCodecInfo> VideoCodecList;

struct VideoStreamInfo
{
    yl::string      strCapset;
    // 视频通话可用的codec列表
    VideoCodecList  listLocalCodec;
    VideoCodecList  listRemoteCodec;

    void Reset()
    {
        strCapset.clear();
        listLocalCodec.clear();
        listRemoteCodec.clear();
    }
};


typedef YLList<ipvp_acodec_t> AudioCodecList;

struct AudioStreamInfo
{
    AudioCodecList  listLocalCodec;
    AudioCodecList  listRemoteCodec;

    void Reset()
    {
        listLocalCodec.clear();
        listRemoteCodec.clear();
    }
};

struct MediaStreamInfo
{
    // 音频
    AudioStreamInfo m_stAudio;

    // 视频
    VideoStreamInfo m_stVideo;

    // 辅流
    VideoStreamInfo m_stShare;
};

// 该定义提供给外部模块使用
struct VideoSizeInfo
{
    VideoSizeInfo(int _w = 0, int _h = 0, int _fps = 0)
        : w(_w), h(_h), fps(_fps)
    {
    };

    int w;
    int h;
    int fps;

    // 是否是宽屏模式
    bool IsWidescreen()
    {
        return w >= h;
    }
};

#endif
