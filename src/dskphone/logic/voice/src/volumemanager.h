#ifndef __VOLUME_MANAGER_H__
#define __VOLUME_MANAGER_H__

class CVolumeManager
{
    friend class CVoiceManager;

public:
    CVolumeManager();
    ~CVolumeManager();

    // 获取指定类型的音量大小，VT_AUTO表示选择当前通道
    int GetVolume(VOLUME_TYPE eType = VT_AUTO);
    // 设置当前音量
    void SetVolume(bool bStartTalk = FALSE, int iCallID = -1);
    // 更改音量(bWriteToFile表示是否写入配置中)
    void SetVolume(int iVolume, VOLUME_TYPE eType, bool bWriteToFile = FALSE);
    // 音量是否可以改变
    bool IsVolumeCanChange();

    int ChangeForceVoice(yl::string & strVoice);

    // 重置通话音量
    void ResetTalkVolume();

private:
    // 获取当前音量类型
    VOLUME_TYPE GetVolumeType(bool bStartTalk = FALSE, int iCallID = -1);
    // 根据通道判断音量类型
    VOLUME_TYPE GetVolumeTypeByChannel(bool bTalk, bool bColorRingback);
    // 纠正逻辑音量值
    void ValidLogicVolume(int & iVolume);
    // 纠正发送给VPM的音量值
    void ValidVolumeToVPM(int & iVolume);
    // 调用VPM接口设置音量
    void SetVolumeToVPM(VOLUME_TYPE eType, int iVolume);
    // 对播放铃声，铃声音量为0时的特殊处理
    void ProcessZeroVolumeOfRing(int iLogicVolume);

    ///////////////////////从配置文件中获取或设置音量///////////////////////
    // 获取spk音量
    int GetSpkVolume(VOLUME_TYPE eType, int iDefault = DEFALUT_VOL);
    // 设置spk音量
    void SetSpkVolume(VOLUME_TYPE eType, int iVolume);

private:
    // 记录当前的音量类型
    VOLUME_TYPE       m_eCurrentType;
    // 记录当前的音量
    int               m_iCurrentVolume;
};

#endif // __VOLUME_MANAGER_H__
