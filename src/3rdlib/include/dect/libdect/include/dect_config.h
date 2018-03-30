/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2011-07-12
*
*******************************************************************/
#ifndef __DECT_CONFIG_H__
#define __DECT_CONFIG_H__

/************************************************************************/
/*                         SW config                                    */
/************************************************************************/
/*
* common config
*/ 

/*
* log config
*/
#define  DECT_CFG_LOGNAME           "DEBS"
#define  DECT_CFG_LOGCHECK          0
#define  DECT_CFG_LOGANY            0
#define  DECT_CFG_LOGDEBUG          1
#define  DECT_CFG_LOGCOLOR          1
#define  DECT_CFG_LOGPOS            1

/*
* debug config
*/
#ifdef DBG_MEM_CHECK
#define  DECT_CONFIG_MEMCHECK          1
#else
#define  DECT_CONFIG_MEMCHECK          0
#endif
#ifdef DBG_MEM_CHECK_INFO
#define  DECT_CONFIG_MEMCHECK_INFO      1
#else
#define  DECT_CONFIG_MEMCHECK_INFO      0
#endif
/*memory limit to 1M*/
#define  DECT_CONFIG_MEMLIMITES        (1*(1024*1024))

/*assert config*/
#ifdef DBG_DEBUG
#define  DECT_CONFIG_ASSERT            1
#else
#define  DECT_CONFIG_ASSERT            0
#endif

#define  DECT_CONFIG_SEMDBG            0

/*SRTP config*/
#ifdef DBG_SRTP
#define  DECT_CONFIG_SRTP              1
#else
#define  DECT_CONFIG_SRTP              0
#endif

/************************************************************************/
/*                         C config                                    */
/************************************************************************/

/************************************************************************/
/*                         HW config                                    */
/************************************************************************/

/************************************************************************/
/*                         FUNs config                                  */
/************************************************************************/
/********************************************************************
修改时间: 2012-02-22
发布者  : 
旧版本号: 
新版本号: 
更改原因: DECT_CONFIG_HAS_MATH
更改措施: 
编译条件: 
*********************************************************************/
#define  DECT_CONFIG_HAS_MATH 0

/********************************************************************
修改时间: 2012-02-22
发布者  : 
旧版本号: 
新版本号: 
更改原因: DECT_CONFIG_DEBUG_RTP
更改措施: 
编译条件: 
*********************************************************************/
#define  DECT_CONFIG_DEBUG_RTP 0
#define  DECT_CONFIG_DEBUG_DEV 0

/********************************************************************
修改时间: 2012-02-22
发布者  : 
旧版本号: 
新版本号: 
更改原因: DECT_CONFIG_SRTP_SUPPORT
更改措施: 
编译条件: 
*********************************************************************/
#define  DECT_CONFIG_SRTP_SUPPORT 1

/********************************************************************
修改时间: 2012-02-22
发布者  : 
旧版本号: 
新版本号: 
更改原因: DECT_CONFIG_NETSRV_MAX
更改措施: 
编译条件: 
*********************************************************************/
#define DECT_CONFIG_NETSRV_MAX      256

/*FIXME:how many do we need exactly??*/
#define DECT_CONFIG_PACK_MAX        1024  

/********************************************************************
修改时间: 2012-02-22
发布者  : 
旧版本号: 
新版本号: 
更改原因: DECT_CONFIG_NETLINK_PAYLOAD
更改措施: netlink payload size
编译条件: 
*********************************************************************/
#define DECT_CONFIG_NETLINK_PAYLOAD 2048
#define DECT_CONFIG_NETLINK_BUFFER  NLMSG_SPACE(DECT_CONFIG_NETLINK_PAYLOAD)

/********************************************************************
修改时间: 2012-02-22
发布者  : 
旧版本号: 
新版本号: 
更改原因: DECT_CONFIG_DECTD_PORT
更改措施: dectd utility port 
编译条件: 
*********************************************************************/
#define DECT_CONFIG_DECTD_PORT      4158
#define DECT_CONFIG_DECTD_BUFLEN    (1024)

/********************************************************************
修改时间: 2012-02-22
发布者  : 
旧版本号: 
新版本号: 
更改原因: SMS support
更改措施: 
编译条件: 
*********************************************************************/
//#define DECT_CONFIG_RDK_SMS

/********************************************************************
修改时间: 2012-02-22
发布者  : 
旧版本号: 
新版本号: 
更改原因: SMS support
更改措施: 
编译条件: 
*********************************************************************/
#define DECT_CONFIG_CHAN_MAX  6

/********************************************************************
修改时间: 2013-05-21
发布者  : 
旧版本号: 
新版本号: 
更改原因: data handle
更改措施: 
编译条件: 
*********************************************************************/
#define DECT_CONFIG_DATA_HANDLE  1
#define DECT_CONFIG_MMI_SERVICE 1

/******************************************************************************
修改时间:   2012.1.11
发布者  :   
原版本号:   12.1.0.14
新版本号:   12.1.1.0
修改文件:
错误现象:
错误原因:
更改措施:
编译条件:	
******************************************************************************/

/************************************************************************/
/*                         version config                               */
/************************************************************************/
/*
0.0.0.0  -> 0.0.0.0:
1.0.8.43 -> 1.0.8.44:
1.0.8.44 -> 2.0.0.0:配合驱动实现内核模式，同时预留了内核srtp的接口
2.0.0.0  -> 2.0.0.1:A)添加blind transfer 命令;
                    B)配合驱动整理了RTP、RTCP的启动和更新流程
2.0.0.1  -> 2.0.0.2: 添加RPS_USER,RPS_PASSWD, PPPOE_USER,PPPOE_PASSWD 进入DECT SETTING LA PROP FIELD.
2.0.0.3  -> 2.0.0.4: 添加#define LA_IP_TYPE_PPPOE   0x04
2.0.0.4  -> 2.0.0.5: 添加VPN网络状态接口
2.0.0.5  -> 2.0.1.0: 配合driver和coma的RTP等接口的变更，改变较大，存在较大风险
2.0.1.0  -> 2.0.1.1: 配合调整了dect_chan_stop()的过程
2.0.1.1  -> 2.0.1.2: 添加log信息，功能没有改变
2.0.1.2  -> 2.0.1.3: 调整jib的默认值，120ms-240ms；修复rtp的poll模式选择错误bug
2.0.1.3  -> 2.0.1.4: 配合coma更改jib等设置方法，将所有jib参数全部导出到应用配置（libdect+driver+coma必须同步更改）
2.0.1.4  -> 2.0.1.5: 配合coma添加空中CODEC配置方式，分为固定方式和协商方式
2.0.1.5  -> 2.0.1.6: 更改RTCP默认间隔时间为5s，该bug在3cx上hold是会出现概率性断开
2.0.1.6  -> 2.0.1.7: 更改CFG的部分定义，统一以DECT_ xx命名
2.0.1.7  -> 2.0.1.8: 整理codec配置，更改ptype的设置由sip决定；删除不适用的定义；codec名称有更改，需要和coma同步
2.0.1.8  -> 2.0.1.9: 更正enc_payload和dec_payload配置，一般情况下也不会有问题
2.0.1.9  -> 2.0.2.0: 修改L2C和C2L的数据传送
2.0.2.0  -> 2.0.2.1: 优化L2C和C2L的数据传送
2.0.2.1  -> 2.0.2.2: 再次优化L2C和C2L的数据传送
2.0.2.2  -> 2.0.2.3: 添加BS PHONEBOOK. 多目录多分组电话本.
2.0.2.3  -> 2.0.2.4: 添加START SESSION  . LA_REJECT_PIN_CODE_REQ 错误类型.
2.0.2.4  -> 2.0.2.5: 去掉DSR_ReqLASendGenericEventNotificationLC 函数中对LIST ID 的限制.
2.0.2.5  -> 2.0.2.6: 添加bNetworkCallLog. 增加Advance Contact listId 范围到64 个.
2.0.2.6  -> 2.0.3.0: 去掉用户态srtp模块
2.0.3.0  -> 2.0.3.1: 添加CountryTone接口支持，但是notify和LA还没有实现；添加SRTP编译控制；
2.0.3.1  -> 2.0.3.2: 1,添加：呼叫外线时可选择播放彩铃 ；2,添加：外线来电时可选择铃声
2.0.3.2  -> 2.0.3.3: 添加MULTI_VOICE_MAIL,MULTI_MISSED_CALL,MULTI_LIST_CHANGE . Notify 消息定义.
2.0.3.3  -> 2.0.3.4: 1.LINE SETTING -- LineStatus Field 中添加numOfMissedCall 信息. 2. 修改LA_MAX_LIST_IDENTIFY_CODE_LEN 为4.
2.0.3.4  -> 2.0.3.5: 添加sip error code 对应的call release reason.
2.0.3.5  -> 2.0.3.6: 更改来电响铃选择的定义名称
2.0.3.6  -> 2.0.3.7: DEMO打开HS的Ringback tone 及 根据网页的配置项修改CountryTone名称
2.0.3.7  -> 2.0.3.8: 修改Country Tone 宏定义名称.
2.0.3.8  -> 2.0.3.9: 将来电响铃类型作安全限定。
2.0.3.9  -> 2.0.4.0: 按照syslog规范统一实现log。
2.0.4.0  -> 2.0.4.1: 1，修改RPS、PPPOE、BROADSOFT的PASSWD通信方式；2，将SS7 的instance申请区分为SS70_INSTANCE_AP_DECT，SS70_INSTANCE_AP_SIP；3，syslog规范模板中log.c更新
2.0.4.1  -> 2.0.4.2: 1:添加Call Log PropFiledInfo -一次传送多个field,减少空中数据11-14 Bytes ,提高传送速度. 2: 修改ALL CALL /DIALED/INCOMING CALL Get Entry 的函数，根据各自的FILED ID 定义取相应的数据.
2.0.4.2  -> 2.0.4.3: 修改CLIP UPDATE 宏定义名称.
2.0.4.3  -> 2.0.4.4: 将CLIP 和CNIP的更新整合成一个接口[Beta 版本标志.]
2.0.4.4  -> 2.0.4.5: 添加DTM 功能.1:增加linesetting LA 中dailToneId 变量.使用linestatus field 传送. 2: 增加country tone 中DAIL_SPECAIL_CONDITION，DAIL_MESSAGE_WAITING 两种声音.3.增加 LA_EVENT_TYPE_MULTI_DAIL_TONE_ID notify.
2.0.4.5  -> 2.0.4.6: 增加宏定义:DECT_CFG_REPEATER_REGISTER_DAIL_NUMBER,BASE_RESET_MODE_PARTLY. 2:添加Repeater 手动注册模式参考代码.
2.0.4.6  -> 2.0.4.7: 添加mmi服务
2.0.4.7  -> 2.0.4.8: 添加ClipCnip Update时最大长度检查
2.0.4.8  -> 2.0.4.9: 添加 Auto Intercom 功能支持. 来电时，如果ringtype 为 DECT_SS7_ALERT_AUTO_INTERCOM，那么手柄会自动接起这路通话.
2.0.4.9  -> 2.0.4.10: 恢复2.0.4.8版本的 ClipCnip Update 最大长度检查，原因是已经有做长度限制，不用返回错误。
2.0.4.10  -> 2.0.4.11:增加notify两个RES消息，以便APP判断coma的状况。
2.0.4.11  -> 3.0.0.0 :将service重新用c语言实现
3.0.0.0   -> 3.0.0.1 :去除无用的宏定义
3.0.0.1   -> 3.0.0.2 :去除部分eeprom接口
3.0.0.2   -> 3.0.0.3 :去除信号量相关的代码
3.0.0.3   -> 3.0.0.4 :修改和增加MMI接口，如DSP和音量调整等
3.0.0.4   -> 3.0.0.5 :添加DECT_CONFIG_HAS_MATH编译条件，控制libm的引用；关闭情况下会影响tone的设置功能，不过目前该子功能没有使用
3.0.0.5   -> 3.0.0.6 :修复因DECT_TALK_DIRC引起的bug
3.0.0.6   -> 3.0.0.7 :修改使用chan_update_rtp接口，解决没有RTCP发送的问题
3.0.0.7   -> 3.0.0.8 :修改使用dect_srtp_close接口，解决不能正确关闭SRTP状态的问题
3.0.0.8   -> 3.0.0.9 :添加DATA HANDLE 处理. 编译开关默认为关闭.
3.0.0.9   -> 3.0.0.10:修改jitter buffer service time至datasheet中描述的默认值, dect_srtp_close接口调用与34.0.0.9版本驱动同步
3.0.0.10  -> 3.0.0.11:修改mmi接口，用于解决mute和参数问题；删除了c++的不会使用的接口
3.0.0.11  -> 3.0.0.12:修改SRTP关闭警告log
3.0.0.12  -> 3.0.0.13:无线升级 dectlib\fwup 代码提交.默认为关闭.去除 service_Create 接口 的 dh_cb 参数.
3.0.0.13  -> 3.0.0.14:增加G726字节序翻转功能
3.0.0.14  -> 3.0.0.15:FWUP 增加set progress 接口.
3.0.0.15  -> 3.0.0.16:FWUP 修改第二次下载不能启动的问题.
3.0.0.16  -> 3.0.0.17:添加获取decode/encode 的latency的ioctl接口；需要配合coma才能取到正确的数据；在不使用该功能时可向下兼容
3.0.0.17  -> 3.0.0.18:FWUP 添加fwup_rom_get_rom_version() 接口. 增加 handset_su_event.c handset_su_cmd.c handset_fwup.c 文件,去除handset_datahandle.c 文件.
3.0.0.18  -> 3.0.0.19:FWUP 1:更新fwup_inst_set_progress() 接口,根据每个inst 分开. 2: hs_reboot 接口更改.
3.0.0.19  -> 3.0.0.20:增加Base Status Notify, 可用于发送Base的一些状态信息, 显示到手柄上, 支持4个优先级显示
3.0.0.20  -> 3.0.0.21:增加测试速度接口,增加压缩rom支持,指定数据增加重发机制,打印信息多手柄支持
3.0.0.21  -> 3.0.0.22:FWUP 1:优化压缩的ROM包的数据传送.
3.0.0.22  -> 3.0.0.23:FWUP 1.压缩算法支持全ff跳过处理.2.压缩写支持重发机制
3.0.0.23  -> 3.0.0.24:FWUP 1.多个Hs Msg数据包一起到达的处理优化.
3.0.0.24  -> 3.0.0.25:FWUP 1.添加cypher2_try_parser()函数，prepare rom 的时候预分析ROM包，判断是否出错.
3.0.0.25  -> 3.0.0.26:FWUP 1.retry 时如果碰到BUS_ERR就不retry. 2. SU CMD 要发送的时候检查该HS 是否正在升级过程中.3.升级出错时不发送REBOOT 命令，而是等DH 断开时HS会重启.
3.0.0.26  -> 3.0.0.27:FWUP 1.修改线程内存泄露问题. 2.多手柄资源释放逻辑修改. 3.增加手机断电测试模式.
3.0.0.27  -> 3.0.0.28:FWUP 1.修改POST MESSAGE HS NR 错误BUG.2.修改多手柄同时进行速度测试时速度显示出错BUG.
3.0.0.28  -> 3.0.0.29:FWUP 1.多手柄升级时CRC_VERIFY error 时多次重试，最长为15分钟.
3.0.0.29  -> 3.0.0.30:FWUP 1.ERROR CODE 整理.
3.0.0.30  -> 3.0.0.31:修改在通话一段时间后，jitter buffer会下益到0的问题。
3.0.0.31  -> 3.0.0.32:FWUP 1 增加reopen data call 来处理与手柄数据接收异常的恢复, 解决多手柄升级失败的问题。
3.0.0.32  -> 3.0.0.33:FWUP 1 增加失败重试次数到80次其中reopen data call为40次. 2解决在获取手柄版本失败后,还是返回成功的Bug. 3.优化传给CRC32函数的长度值 4.从手柄获取版本信息时,发送的最大长度减少到64, 减少出错.
3.0.0.33  -> 3.0.0.34:FWUP 1. 根据出错类型, 优化reopen data calll的门限次数.
3.0.0.34  -> 3.0.0.35:FWUP 1. 完善open data call返回值TIMEOUT及EXIST处理. 2.错开各手柄首次发送命令的时间
3.0.0.35  -> 3.0.0.36:FWUP 1. 解决升级过程中手柄重启异常处理的错误.2: 修改DFLASH VERIFY 代码缺陷.
3.0.0.36  -> 3.0.0.37:FWUP 1. 在FFS/APP写完后, 默认需要校验, 以免出错!
3.0.0.37  -> 3.0.0.38:修改 Line Name为多手柄接口
3.0.0.38  -> 3.0.0.39:增加Advanced Search功能, 通过SearchMatchCnf给手柄发送上下页信息
3.0.0.39  -> 3.0.0.40:增加来电/通话中AUTO ANSWER NOTIFY 的接口.
3.0.0.40  -> 3.0.0.41:FWUP 1. 添加send_time_out 的处理. 2:更改错误重试机制.
3.0.0.41  -> 3.0.0.42:FWUP 1. 增加升级时间打印及宏开关DECT_CONFIG_PRINT_FWUP_TIME.2.解决引起Base段错误的原因, netsrv_on_dh增加判断是否为空.
3.0.0.42  -> 3.0.0.43:FWUP 1. 出错处理增加bWaitHsResp 参数. 2. LOG 打印都添加上手柄编号的打印. 3. 添加DECT_CONFIG_FWUP_DH_OPEN_LOCK开关。默认为关闭.
3.0.0.43  -> 3.0.0.44:FWUP 1. 未压缩的HS ROM包，校验采用CRC32校验. 2: 添加FWUP_ERR_HS_NOT_ALIVE 错误返回值.
3.0.0.44  -> 3.0.0.45:FWUP 1. 手柄升级过程中其它手柄进入LA/通话, 增加对应的错误码
3.0.0.45  -> 3.0.0.46:FWUP 1. 新增NOTIFY命令, 表示已经是最新版本.
3.0.0.46  -> 3.0.0.47:FWUP 1:添加"禁止内线通话和外线来电及Paging 来电" 的接口. 2: datahandler 添加反初始化接口.3. 新增LA START FLAG 判断是否为初始化数据. 
3.0.0.47  -> 3.0.0.48:FWUP 1:更改Hs Upgrade Event Notify 结构体. 添加手柄能力成员.
3.0.0.48  -> 3.0.0.49:FWUP 1:补充提交: datahandler 添加反初始化接口. 2: 比较版本发现不用升级之后马上重启手柄.
3.0.0.49  -> 3.0.0.50:FWUP 1:去除rom version 和OEM NAME 的检查.
3.0.0.50  -> 3.0.0.51:FWUP 1:fwup_rom_get_rom_version() 添加获取DEVICE ID 接口.
3.0.0.51  -> 3.0.0.52:FWUP 1:获取手柄版本命令增加Dflash是否被破坏标志.
3.0.0.52  -> 3.0.0.53:FWUP 1:设置手柄升级进度百分比时, 对返回值进行判断.
3.0.0.53  -> 3.0.0.54:FWUP 1:BUG 手柄断电,dectlib 需要等待30s 才返回给APP 消息. 2:优化:fwup_inst_destroy() 判断inst 是否存在，防止重复释放.3: 优化:fwup_inst_get_handle() 加入MUX LOCK保护.
3.0.0.54  -> 3.0.0.55:FWUP 1:netlink payload 越界问题修改. 2: BUG ROM 版本不一样，但是APP 和FFS 版本一样不能成功升级. 3: 添加SU EVENT 定义.
3.0.0.55  -> 3.0.0.56:FWUP 1:优化多线程处理保护.
3.0.0.56  -> 3.0.0.57:FWUP 1:BUG:89%之后又显示85%.2: 增加hs_reopen 重试次数为3次. 3: 减少dectlib发送给coma 的DH消息数据量.
3.0.0.57  -> 3.0.0.58:FWUP 1:优化多线程处理保护.
3.0.0.58  -> 3.0.0.59:FWUP 1:修改OPEN 失败时的错误码修改. 2: demo 程序更新.
3.0.0.59  -> 3.0.0.60:FWUP 1:配合COMA 修改OPEN 失败重试次数. 
3.0.0.60  -> 3.0.0.61:1:配合COMA 修改,如果号码是带有字母的话会造成显示不正常
3.0.0.61  -> 3.0.0.62:增加Suppress DTMF Display功能的支持. 
3.0.0.62  -> 3.0.0.63:添加HS的信息接口（locate_type与capability），手柄断开又很快重连的情况下，APP不去发送初始化通知消息。
3.0.0.63  -> 3.0.0.64:添加coma Alive/Shutdown, Hs alive/Shutdown, get coma operation (用于coma keep alive )接口.
3.0.0.64  -> 3.0.0.65:EVENT NOTIFY RESULT 接口增加eType,u16_hsMask,hs_result[5] 的参数.
3.0.0.65  -> 3.0.0.66:外线来电RELEASE 原因类型添加. UNREACHABLE, NOT_FOUND.
3.0.0.66  -> 3.0.0.67:技术支持BugID=23520, 支持通过AutoP修改手柄配置.
3.0.0.67  -> 3.0.0.68:1.技术支持BugID=23937, 手柄增加显示证书状态.
3.0.0.68  -> 3.0.0.69:增加SCA功能.
3.0.0.69  -> 3.0.0.70:增加RPS 状态.
3.0.0.70  -> 3.0.0.71:1:增加配置项，对底层RTP包是否合法判断. 2:Codec表增加sample rate值,用于RTP包长度计算.
3.0.0.71  -> 3.0.0.72:1:DATA_HANDLE编译开关默认修改为打开. 2:添加PROP TONE IND 接口以控制手柄TONE音播放和停止.
3.0.0.72  -> 3.0.0.73:1:增加Trans_After_Conf_Enable配置接口.2:增加SS70_INF_TYPE_CONF_RELEASE 定义.
3.0.0.73  -> 3.0.0.74:1:增加Login Notify 2:增加DSR_CMD_LA_SET_HS_LA_TIMER.
3.0.0.74  -> 3.0.0.75:1:新增BASE/HS发送额外数据功能.
3.0.0.75  -> 3.0.0.76:1:EVENT PHONE_CFG 中添加bAutoIntercomToHeadset配置.
3.0.0.76  -> 3.0.0.77:1.PhoneCfg增加Reset Local Config 菜单显示开关
3.0.0.77  -> 3.0.0.78:1.修改LA_SET_LA_TIMER_START_SESSION_ID宏定义为0.
3.0.0.78  -> 3.0.0.79:1.修改BUG :自定义Dial信号音在200-7000范围外的频率不是默认的信号音.
3.0.0.79  -> 3.0.0.80:1.Jitter Buffer修改限制值最小值为60并允许最小最大值相等.
                      2.PhoncCfg增加VoiceMailTone配置开关
                      3.增加XSI相关错误码
3.0.0.80  -> 3.0.0.81:1.LA_REJECT_SIP_XSI_AUTH_FAILED修改宏定义笔误.
3.0.0.81  -> 3.0.0.82:1.PhoneCfg增加bEnableCFChannel配置开关.解决新手柄配合旧BASE概率性出现单通问题.
3.0.0.82  -> 3.0.0.83:1.检查Jitter Buffer值并对非法的参数进行修正
3.0.0.83  -> 3.0.0.84:1.增加AUTOP CODE Release Reason，手柄用于判断是否显示CALL FAILED界面
3.0.0.84  -> 3.0.0.85:1.添加NET_CONF 接口.
3.0.0.85  -> 3.0.0.86:1.添加NET_CONF_URI 标志位.
3.0.0.86  -> 3.0.0.87:1.自定义信号音频率为0的时候无效.
3.0.0.87  -> 3.0.0.88:1.添加callEndTone5s phone cfg 配置项.
3.0.0.88  -> 3.0.0.89:1.添加通话结束余额显示功能.
3.0.0.89  -> 3.0.0.90:1.技术支持: W52P 隐藏手柄Anonymous Call选项
3.0.0.90  -> 3.0.0.91:1.技术支持：Joyce Telefonica  W52P 特质移植
                                (1) 通话中按OK键发送info消息
                                (2)在PhoneCfg添加此功能的开关
3.0.0.91  -> 3.0.0.92:1.修改有关OK键发送info消息的函数名和变量名
3.0.0.92  -> 3.0.0.93:1.添加rtcpBye disable 接口.
3.0.0.93  -> 3.0.0.94:1.技术支持:Ringcentral W52P  当有新VoiceMail时没有任何提示
3.0.0.94  -> 3.0.0.95:1.技术支持:W52P Logon Wizard特制移植
3.0.0.95  -> 3.0.0.96:1.技术支持:W52P 支持PLCM phonebook service联系人功能
3.0.0.96  -> 3.0.0.97:1.技术支持:GAMMA  W52 V73 手柄注册PIN无法进行注销
3.0.0.97  -> 3.0.0.98:1.技术支持: 手柄菜单隐藏Call Waiting，Anon. Call Rejection，Anonymous Call三个配置项
3.0.0.98  -> 3.0.0.99:1.修改BUG: tone音在特定的频段和特定的持续时间，声音异常
3.0.0.99  -> 3.0.0.100:1.技术支持: YUK GAMMA W52 V73 支持# as send功能,BugID=28047
3.0.0.100 -> 3.0.0.101:1.1.技术支持:YUK GAMMA 隐藏黑名单，VM配置接口,SCA下Private Hold功能. BugID=28045
3.0.0.101 -> 3.0.0.102:1.技术支持: YUK GAMMA W52 V73 支持# as send功能完善, 增加*号支持,BugID=28047
3.0.0.102 -> 3.0.0.103:1.技术支持:MTSW的Network CallLog和Contact的功能添加
3.0.0.103 -> 3.0.0.104:添加GetLoginField等接口.
3.0.0.104 -> 3.0.0.105:添加Dect System LA StatusMask 及US_DECT_INVERT 等参数.
3.0.0.105 -> 3.0.1.0:1.添加RTCP-XR功能
3.0.1.0   -> 3.0.1.1:W56 1.增加Dial Now及PreDial接口
3.0.1.1   -> 3.0.1.2:W56 1.增加SCA接口
3.0.1.2   -> 3.0.1.3:W56 1.增加Call Park功能
3.0.1.3   -> 3.0.1.4:W52 1.技术支持: 增加手柄隐藏默认账号及设置来电权限功能.BugID=30170
3.0.1.4   -> 3.0.1.5:W56 1.增加三级权限功能
3.0.1.5   -> 25.80.0.1:W56 1. 支持W56 无线升级.
                                       2.dsr 添加 Prop9Field 定义 ， 添加 dsr 传输结构体 reserved 数据方便以后扩张.
25.80.0.1: -> 25.80.0.11:  1: 技术支持BugID=30351, 修改rtcp_mode赋值错误.
                           2: 1.增加Phone Cfg2接口用于配置一些功能开关  2.DEMO中增加一些功能的测试函数  3.三级权限项整理.
						   3: 添加Power LED灯配置信息
						   4: 添加 SS7_PropClipNameInd,
						   5: 添加搜索联系人默认输入法配置信息
						   6: 支持对W52手柄的在线升级
25.80.0.11: -> 25.80.0.13:  fwup_version_dummy_skip 跳过1.1.1.1 版本的app/ffs.
25.80.0.13: -> 25.80.0.14:  检查FFS VER 判断如果已升级，那么不再升级FFS.
25.80.0.14: -> 25.80.0.15:  1: 解决因service_Create()初始化失败,引起的GUI段错误. (Merged)
                            2:  ptime超范围时,取默认值. BugID=80513 (Merged)
							3: 修改支持iLBC和ptime区别iLBC_15_2和iLBC_13_3. BugID=81902 (Merged)
							4:修改开启SRTP反复hold-resume通话无声的问题. (BugID=81206) (Merged)
							5: 解决DSCP始终为0的问题. (BugID=80996) (Merged)
							6:dectlib 打印coma 发送出来的debug 和 统计stat 信息. 打印到syslog 供分析. (Merged)
							7:无线升级优化. 1: 添加速度测试特殊rom包识别. 3: log 信息添加. (Merged)
25.80.0.15: -> 25.80.0.16:  1、新增EnableTls 变量用于传输TLS加密状态 2、新增对应登录用户的type 3、新增 SUOTA_ENABLE 标志，用于优化升级时的Notify慢.
25.80.0.16: -> 25.80.0.17:  1.dectlib使用最新的log代码，更改对应的log.h (Merged)
							2.添加quality_ind 统计信息打印.
25.80.0.17: -> 25.80.0.18:	1:无线升级 判断DEVID = 56 之外，还增加判断ROM包的，DEVICE NAME = W56H 才允许升级 (Merged)
							2:添加 DSR_DectPageIntervalTimeMs_Set 接口. Merge from v73/v73_xr (Merged)
							3:添加统计信息,最强和最弱信号. Frame0LostIndCnt. (Merged)

25.80.0.18: -> 25.80.0.19:  1:change w56 device id from 56 to 61.


*/


#if defined(DBG_MEM_CHECK) || defined(DBG_DEBUG) || DECT_CONFIG_DEBUG_RTP || DECT_CONFIG_DEBUG_DEV
#define  DECT_CONFIG_RELEASE "debug"
#else
#define  DECT_CONFIG_RELEASE "release"
#endif


#define  DECT_CONFIG_PROGRAM "dectlib"
#define  DECT_CONFIG_VERSION "25.80.0.19"
#define  DECT_CONFIG_BUILD   DECT_CONFIG_VERSION"-"DECT_CONFIG_RELEASE"("__DATE__","__TIME__")"

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /*__DECT_CONFIG_H__*/

