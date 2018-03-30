#ifndef __VQRTCP_DATA_INC_H__
#define __VQRTCP_DATA_INC_H__

#include "innermsgdefine.h"

//////////////////////////////////VQReport 数据结构定义////////////////////////////////////////
#define kszLocalNumber              "LocalUser"
#define kszRemoteNumber             "RemoteUser"
#define kszStartTime                "StartTime"
#define kszCurrentTime              "CurrentTime"
#define kszJitter                   "Jitter"
#define kszPktLost                  "RecvPktLost"
#define kszSymmOneWayDelay          "SymmOneWayDelay"
#define kszRoundTripDelay           "RoundTripDelay"
#define kszMOSLQ                    "MOS-LQ"
#define kszMOSCQ                    "MOS-CQ"
#define kszNetworkPacketLossRate    "NetworkPacketLossRate"
#define kszEndSystemDelay           "EndSystemDelay"
#define kszInterarrivalJitter       "InterarrivalJitter"
#define kszRemoteIPP                "RemoteIP"
#define kszLocalPort                "LocalPort"
#define kszRemotePort               "RemotePort"
#define kszCodec                    "Codec"
#define kszLoaclCodec               "LocalCodec"
#define kszRemoteCodec              "RemoteCodec"
#define kszJitterBufferMax          "JitterBufferMax"

// voice quality report data
struct SVQReportData
{
    yl::string  strLocalNumber;
    yl::string  strRemoteNumber;

    yl::string  strStartTime; // Start Time
    yl::string  strCurrentTime; // Current Time
    yl::string  strRemoteIP;
    yl::string  strLocalPort;
    yl::string  strRemotePort;
    yl::string  strCodecName;

    yl::string  strJitter; //Jitter
    yl::string  strJitterBufferMax;     //JitterBuffer       - JitterBufferMax
    yl::string  strPktLost;       // Received Packets lost
    yl::string  strNetworkPacketLossRate;         // PacketLoss - NetworkPacketLossRate
    yl::string  strMOSLQ;       // QualityEstimates - MOS-LQ
    yl::string  strMOSCQ;       // QualityEstimates - MOS-CQ
    yl::string  strRoundTripDelay;      // RoundTripDelay
    yl::string  strEndSystemDelay;      // Delay - EndSystemDelay
    yl::string  strSymmOneWayDelay;     // SymmOneWayDelay
    yl::string  strInterarrivalJitter;      // Delay  - InterarrivalJitter

    SVQReportData()
    {
        Reset();
    }

    void Reset()
    {
        strLocalNumber.clear();
        strRemoteNumber.clear();
        strStartTime.clear();
        strCurrentTime.clear();
        strJitter.clear();
        strPktLost.clear();
        strSymmOneWayDelay.clear();
        strRoundTripDelay.clear();
        strMOSLQ.clear();
        strMOSCQ.clear();
        strNetworkPacketLossRate.clear();
        strEndSystemDelay.clear();
        strInterarrivalJitter.clear();
        strRemoteIP.clear();
        strCodecName.clear();
        strLocalPort.clear();
        strRemotePort.clear();
        strJitterBufferMax.clear();
    }

    yl::string GetAttributeValueById(const yl::string& strName)
    {
        if (kszLocalNumber == strName)
        {
            return strLocalNumber;
        }
        else if (kszRemoteNumber == strName)
        {
            return strRemoteNumber;
        }
        else if (kszStartTime == strName)
        {
            return strStartTime;
        }
        else if (kszCurrentTime == strName)
        {
            return strCurrentTime;
        }
        else if (kszJitter == strName)
        {
            return strJitter;
        }
        else if (kszPktLost == strName)
        {
            return strPktLost;
        }
        else if (kszSymmOneWayDelay == strName)
        {
            return strSymmOneWayDelay;
        }
        else if (kszRoundTripDelay == strName)
        {
            return strRoundTripDelay;
        }
        else if (kszMOSLQ == strName)
        {
            return strMOSLQ;
        }
        else if (kszMOSCQ == strName)
        {
            return strMOSCQ;
        }
        else if (kszNetworkPacketLossRate == strName)
        {
            return strNetworkPacketLossRate;
        }
        else if (kszEndSystemDelay == strName)
        {
            return strEndSystemDelay;
        }
        else if (kszInterarrivalJitter == strName)
        {
            return strInterarrivalJitter;
        }
        else if (kszRemoteIPP == strName)
        {
            return strRemoteIP;
        }
        else if (kszLoaclCodec == strName
                 || kszRemoteCodec == strName)
        {
            return strCodecName;
        }
        else if (kszJitterBufferMax == strName)
        {
            return strJitterBufferMax;
        }
        else
        {
            return "";
        }
    }
};

// RTCP message definition.
enum RTCPMessageDefine
{
    RTCP_MESSAGE_BEGIN = INNER_MSG_RTCP_MESSAGE_BEGIN,

    // WPARAM: 0/update 1/exit.
    RTCP_UDPATE_WND = RTCP_MESSAGE_BEGIN + 1,

    RTCP_MESSAGE_END = INNER_MSG_RTCP_MESSAGE_END
};


#endif // __VQRTCP_DATA_INC_H__
