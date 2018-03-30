#ifndef _FILE_PLAYER_DEFINE_
#define _FILE_PLAYER_DEFINE_

#define FILE_PLAYER_DEFAULT_INDEX                               -1
#define FILE_PLAYER_DEFAULT_NAME                                ""

#define FILE_PLAYER_ZERO_SECOND                                 0
#define FILE_PLAYER_ONE_SECOND                                  1000

#define FILE_PLAYER_DEFAULT_SWIFT_NEXT_TIME         4000
#define FILE_PLAYER_DEFAULT_SWIFT_PRE_TIME              4000

#define FILE_PLAYER_DEFAULT_SWIFT_STARTTIME         500
#define FILE_PLAYER_DEFAULT_SWIFT_INTERVAL              500

#define FILE_PLAYER_EMPTY_PERCENT                               0
#define FILE_PLAYER_FULL_PERCENT                                    100

#define FILE_PLAYER_COLOR_BLACK                                 QColor(0, 0, 0)
#define FILE_PLAYER_COLOR_GRAY                                  QColor(137, 129, 137)
#define FILE_PLAYER_COLOR_BLUE                                  QColor(49, 139, 206)
#define FILE_PLAYER_COLOR_RED                                       QColor(255, 0, 0)

//播放操作
#define FILEPLAYER_ACTION_PLAY                              "PlayAction_Play"
#define FILEPLAYER_ACTION_PAUSE                             "PlayAction_Pause"
#define FILEPLAYER_ACTION_RESUME                            "PlayAction_Resume"
#define FILEPLAYER_ACTION_STOP                              "PlayAction_Stop"

//单纯进行停止，不进行标记已读或其他操作
#define FILEPLAYER_ACTION_STOPONLY                      "PlayAction_StopOnly"

//快速定位
#define FILEPLAYER_ACTION_SEEK_SECOND                   "PlayAction_SeekSecond"

#define FILEPLAYER_ACTION_SEEK_PERCENT_START        "PlayAction_SeekPercent_Start"
#define FILEPLAYER_ACTION_SEEK_PERCENT_MOVE     "PlayAction_SeekPercent_Move"
#define FILEPLAYER_ACTION_SEEK_PERCENT_OVER     "PlayAction_SeekPercent_Over"

#define FILEPLAYER_ACTION_SWIFT_NEXT_START          "PlayAction_SwiftNext_Start"
#define FILEPLAYER_ACTION_SWIFT_NEXT_OVER           "PlayAction_SwiftNext_Over"
#define FILEPLAYER_ACTION_SWIFT_PRE_START           "PlayAction_SwiftPre_Start"
#define FILEPLAYER_ACTION_SWIFT_PRE_OVER                "PlayAction_SwiftPre_Over"

//前后条切换
#define FILEPLAYER_ACTION_SWITCH_NEXT                   "PlayAction_Switch_Next"
#define FILEPLAYER_ACTION_SWITCH_PRE                    "PlayAction_Switch_Pre"

//文件相关操作
#define FILEPLAYER_ACTION_ENTER_PLAY                    "PlayAction_EnterPlay"
#define FILEPLAYER_ACTION_OPTION                            "PlayAction_Option"
#define FILEPLAYER_ACTION_SELECT_OPTION             "PlayAction_SelectOption"
#define FILEPLAYER_ACTION_DETAIL                                "PlayAction_Detail"

#define FILEPLAYER_ACTION_DELETE                            "PlayAction_Delete"
#define FILEPLAYER_ACTION_DELETE_ALL                        "PlayAction_DeleteAll"

#define FILEPLAYER_ACTION_ADDTO_CONTACT         "PlayAction_AddContact"
#define FILEPLAYER_ACTION_ADDTO_NET_CONTACT "PlayAction_AddNetContact"

#define FILEPLAYER_ACTION_CALL_BACK                     "PlayAction_CallBack"
#define FILEPLAYER_ACTION_CALL_VOICEMAIL                "PlayAction_Call_VoiceMail"

#define FILEPLAYER_ACTION_MARK_AS_READ              "PlayAction_MarkAsRead"
#define FILEPLAYER_ACTION_MARK_AS_UNREAD            "PlayAction_MarkAsUnread"

#define FILEPLAYER_ACTION_MARK_ALL_AS_READ      "PlayAction_MarkAllAsRead"
#define FILEPLAYER_ACTION_MARK_ALL_AS_UNREAD    "PlayAction_MarkAllAsUnread"

enum FILEPLAYER_TYPE
{
    FILEPLAYERTYPE_ERROR = 0,
    FILEPLAYERTYPE_INDEX,
    FILEPLAYERTYPE_NAME,
    FILEPLAYERTYPE_END
};

enum FILEPLAYER_STATE
{
    FILEPLAYER_STATE_ERROR = 0,
    FILEPLAYER_STATE_STOP,
    FILEPLAYER_STATE_PLAY,
    FILEPLAYER_STATE_PAUSE,
    FILEPLAYER_STATE_END
};

struct FilePlayerData
{
    FILEPLAYER_TYPE m_eFilePlayerType;
    //文件的序号，用以唯一标识文件，根据逻辑层接口决定
    int                     m_nFileIndex;
    //文件的名称，用以唯一标识文件，根据逻辑层接口决定
    yl::string              m_strFileName;

    //总共播放时长（单位秒）
    long                        m_lTotalSeconds;
    //当前播放时间（单位秒）
    long                        m_lCurrentSeconds;

    //单步快进步长（单位毫秒）
    int                     m_nNextTime;
    //单步快退步长（单位毫秒）
    int                     m_nPreTime;

    FilePlayerData()
    {
        m_eFilePlayerType = FILEPLAYERTYPE_ERROR;
        m_nFileIndex = -1;
        m_strFileName = "";
        m_lTotalSeconds = -1;
        m_lCurrentSeconds = -1;
        m_nNextTime = -1;
        m_nPreTime = -1;
    }
};

#define VVM_FILE_READ                   "Read"
#define VVM_FILE_UNREAD             "UnRead"

#endif //_FILE_PLAYER_DEFINE_