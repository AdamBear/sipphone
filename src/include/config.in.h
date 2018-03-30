/************************
CMake configure 文件，文件给源代码传递CMake变量
CMake configure 头文件，传入一些配置变量
*************************/

#ifndef __CMAKE_CONFIG_H__
#define __CMAKE_CONFIG_H__

#define ON                                  1
#define OFF                                 0

#define HAVE_STDCPP_LIB                     0@HAVE_STDCPP_LIB@

#define HAVE_STDINT_H                       0@HAVE_STDINT_H@

#define ENABLE_BACKTRACE                    @ENABLE_BACKTRACE@      //是否启用BackTrace

#define IS_LATTICE                          0@IS_LATTICE@           //是否为黑白屏

#define IS_COLOR                            0@IS_COLOR@             //是否为彩屏

#define IS_MEMCHECK                         0@IS_MEMCHECK@          //是否打开了MEMCHECK

#define IS_CODE_TEST                        0@IS_CODE_TEST@         //否是编译成测试代码

#define IS_COVER_TEST                       0@IS_COVER_TEST@        //是否为覆盖率测试

#define ENABLE_PROF                         @ENABLE_PROF@

/******************
 *    功能宏      *
 ******************/
#define FEATURE_BROADTOUCH_UC               0@FEATURE_BROADTOUCH_UC@

#define FEATURE_T2X_LINE_FLOAT              0@FEATURE_T2X_LINE_FLOAT@ //是否启用Line类型Dsskey的Float功能

#define FEATURE_AUDIO_LOOP_TEST             0@FEATURE_AUDIO_LOOP_TEST@

#define FEATURE_METASWITCH                  0@FEATURE_METASWITCH@

#define IF_FEATURE_GBBOOK                   0@FEATURE_GBBOOK@
/****************************************************************************************************************/
///////////////////////////////////////////////////// 加入中性的宏 ///////////////////////////////////////////////
/****************************************************************************************************************/
/* 加入中性的宏，放在这个区域 */

//dsskey内容对齐配置
#define  IF_FEATURE_DSSKEY_ALIGN_MODE                           1
//开机不关掉电源灯
#if IF_ATLINK_SPECIFIC
// AtLink电源灯控制和T2x不同
#define IF_POWER_LIGHT_ALWAYS_ON                                0
#else
#define IF_POWER_LIGHT_ALWAYS_ON                                1
#endif

//是否处理Record和URL设置Dsskey的录音按键
#define IF_RECORD_AND_URL                                       1

//运行脚本的时间段内的按键不处理
#define IF_NETWORK_RUNNING                                      1

//是否添加answer only的功能
#define IF_ANSWER_ONLY                                          1

// 是否在非TalkLogic界面处理EXP按键
#define IF_PROCESS_EXP_UNTALKLOGIC                              1

// PreTran界面显示Softkey:Send取代切换输入法按键
#define IF_PRETRAN_SEND_KEY                                     1

// 问题：Tiptel-Yealink T28/T26/T22/T20 拨号时pool界面退出问题
// http://192.168.1.99/Bug.php?BugID=14583
#define TIPTEL_BUGFREE_14583                                    1

// Nablecomm-SKBB T22/T26/T28 支持韩语输入法，并添加液晶和网页韩语
// http://192.168.1.99/Bug.php?BugID=15601
// 修改:
// 1.   移植韩语输入法.
// 2.   SMS大小限制为80 bytes.
// 3.   液晶配置界面Register name, User name, SIP server显示为*号并不可修改.
#define NABLECOMM_SKBB_BUGFREE_15601                            1

// Nablecomm-Samsung SDS T26/T28 添加Wrap-up状态显示
// http://192.168.1.99/Bug.php?BugID=16773
// 修改：收到ACD的Wrap-up状态时需要显示,直到收到非Wrap-up状态
#define NABLECOMM_BUGFREE_16773                                 1

// http://192.168.1.99/Bug.php?BugID=20588
// 问题：support邮箱 T28  当T28 接到一路新来电，而此时T28 按call park retrieve键，T28 退出两路通话
// 修改：如果有来电状态再按callpark键，则新呼出一路，而不是fwd到新来电
#define IF_BUG_20588                                            1

// 问题：Tecnonet T2x  软件升级到V70
// http://192.168.1.99/Bug.php?BugID=20579
#define  IF_BUG_20000                                           1

// http://192.168.1.99/Bug.php?BugID=20643
// 问题：888VOIP-star2star  T2X 接收来电时，提手柄应答，通话没有立刻建立。
// 修改：当收到一个建立通话的消息时，先等待200毫秒，200毫秒内再次收到建立通话的消息，则立即建立通话，200毫秒内未再次收到建立通话的消息，则200毫秒后立即建立通话
// 特殊消息处理，默认关闭
#define  IF_BUG_20643                                           0

/**************************************************************
http://192.168.1.99/Bug.php?BugID=20900
问题：Yealink SIP-T20P IP Phone缓冲区溢出漏洞
修改：进入测试模式启动macd进程，退出测试模式时，如果mac后六位不为0则杀掉macd进程，否则不杀。
PS：测试部在脚本里去掉开启macd进程的语句
**************************************************************/
//该宏关闭，不开启，开启会导致连续反复进入退出测试默认，导致挂机
#define IF_BUG_20900                                            0

/**************************************************************
http://192.168.1.99/Bug.php?BugID=20944
问题：support邮箱 T20  与ININ平台软电话客户端的兼容性问题，话机无法通过软电话hold
原因：客户使用软电话上的hlod键来实现hold对方，sip向gui发送消息时没有带上音频参数数据，
      结果GUI按正常处理流程没有通知vpm不发送rtp包。
修改：参照T2X-V61版本，在通话中，GUI收到 被hold 消息PHONE_MSG_CALL_REMOTE_HOLD，获取不到对方发送的音频参数时，
     对当前音频参数设置为只接收RTP包，然后把新的音频参数数据传递给VPM.
**************************************************************/
#define IF_BUG_20944                                            1

/****************************************************
http://192.168.1.99/Bug.php?BugID=20555
问题：3CX T2x  话机恢复出厂时，不发送Un-register信息给服务器来注销话机
原因：客户要求
修改：恢复出厂设置前，通知SIP向服务器注销所有帐号。待所有帐号在服务器端注销
      完毕再去恢复出厂。若15S内SIP还未在服务器端将所有帐号注销完毕，则直接恢复
      出厂。
****************************************************/
#define IF_BUG_20555                                            1

/********************************************************************
http://192.168.1.99/Bug.php?BugID=21893
问题：888VOIP-Globalinx T2X 需要支持用户手动修改的配置不再被Autop修改
修改内容：修改ConfigServer支持用户配置的保留。
修改GUI增加高级设置项的重置菜单。
********************************************************************/
#define IF_BUG_21893                                            1

/**************************************************************
192.168.1.99/Bug.php?BugID=19286
Nology-南非电信-T2X  Zero Active的用户名与密码界面跳出前检测网络是否可用
**************************************************************/
#define IF_BUG_19286                                            1

// http://192.168.1.99/Bug.php?BugID=22686
// call control 获取信息流程变更 加入中性
#define IF_BUG_22686                                            1

// http://192.168.1.99/Bug.php?BugID=22977
// call control 密码弹出框提示 加入中性
#define IF_BUG_22977                                            1

// 客户：StarFace特制
// SIP Image 加入中性
#define IF_BUG_22404                                            1

// 客户：Headon特制宏
// http://192.168.1.99/Bug.php?BugID=22272
#define IF_BUG_22272                                            1

//IT-Logiq Thinkrosystem T46G 需要支持UploadSystemInfo XML语句
//http://192.168.1.99/Bug.php?BugID=23718
#define IF_BUG_23718                                            1

//Tadiran T42G 通过BLF按键transfer到PageQ失败
//http://192.168.1.99/Bug.php?BugID=22780
#define IF_BUG_22416_PageQ_BLF                                  1

// T46 BLF同步异常
// http://192.168.1.99/Bug.php?BugID=23131
#define IF_BUG_23131_BLF_LIST                                   1

// 同步开启时忽略本地DND
#define IF_BUG_23511_SYNC_IGNORE_LOCAL_DNDFWD                   1

// Dsskey切换账号优化
#define IF_BUG_22849_SWICTH_ACCOUNT_BY_DSSKEY                   1

// http://192.168.1.99/Bug.php?BugID=22995
// Joyce GTS T46G V71 话机反应严重缓慢
// 延迟呼出,22995
#define IF_DELAY_INVITE_22995                                   1

//http://192.168.1.99/Bug.php?BugID=26760
//http://192.168.1.99/Bug.php?BugID=23094
//增加HeadSet、HeadFree两种Action URL类型
#define IF_BUG_23094                                            1    // 加入中性

//Joyce T2X-V61 通话记录中没有Pickup通话的记录
#define IF_BUG_19894                                            1

// http://192.168.1.99/Bug.php?BugID=23826
// 问题：需要支持Autop URL 用户名和密码LCD验证
#define IF_BUG_23826                                            1

// http://192.168.1.99/Bug.php?BugID=24363
// Lydis RoutIT T46G V71 话机空闲几个小时后ACD无法登入登出
#define IF_BUG_24363                                            1

// http://192.168.1.99/Bug.php?BugID=25529
// Ergocom T2X  V72特质遗漏：Intercom+Pickup功能
#define  IF_BUG_25529                                           1 // 加入中性

// 中性加入Via的显示
#define M_FORWARD_DIVERSION                                     1

//http://192.168.1.99/Bug.php?BugID=29284
//关闭的话收到SIP_CMD_CALL_ACCEPT默认都是用免提接听
#define IF_BUG_28019                                            1

/*
1. 对于像阿拉伯语和希伯来语等从右向左的显示的语言的显示优化
2. 黑白屏话机阿拉伯语支持连写,要新增字体
3. 彩屏机阿拉伯语开启之后禁用希伯来语输入法（世仇）
*/
#define IF_FEATURE_ARABIC

// C5 system的call record功能, t48未完善 // http://192.168.1.98/bugfree3/bug/1005
//#define IF_FEATURE_C5_SYS_VOICE_RECORD


#define IF_SUPPORT_CHANG_UC_PRESENCE                    1

// BroadSoft Visual Voicemail
#define IF_FEATURE_BROADSOFT_VOICEMAIL          1

// 统一action URL、CSTA发送事件接口
#define IF_FEATURE_EVENT_MODULE                 1
// XML自定义Softkey
#define IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY    1
// Provision Monthly需求
#define IF_FEATURE_PROVISIONING_MONTHLY         1
// XML Browser用Execute语句点亮扩展台的LED异常
#define IF_BUG_XMLBROWSER_EXP_LED               1
// 瑞士电信 T41/T42 V80 TR069支持收到Check-Sync后多种机制配置话机
#define IF_BUG_30385                            1
// 输入法autop可配  注:黑白屏需要去掉menuconfig里面的默认输入法
#define IF_FEATURE_AUTOP_IME                    1
// Telstra V80升级 更改右下角DSS Key展开和折叠按键的翻译
#define IF_BUG_30875                            1
// T48 Dsskey展开壁纸
#define IF_FEATURE_T48_DSSKEY_WALLPAPER         1
// 显示DND大图标
#define IF_FEATURE_DND_LARGE_ICON               1
// 通话界面，当处于hold或者held状态时,左上角的通话title改为"hold"或者"held"
#define IF_BUG_26666                            1
// 客户定制Softkey
#define IF_FEATURE_CUSTOM_SOFTKEY               1
// T48 default account在line key上的辨识
#define IF_BUG_25487                            1
// Call Waiting要能跟服务器同步
#define IF_FEATURE_BROADSOFT_CALL_WAITING
// 通话中若监控放状态为来电则按下BLF为截答而不是转接
#define IF_BUG_30886                            1
// 支持来电的时候按其它linekey进行新的去电
#define IF_FEATURE_INCOMEING_SUPPORT_NEWCALL    1
// hold之后按数字键进入拨号 NFON特制
#define IF_FEATURE_DIAL_ON_HOLD                 1
// Lydis TELE2 T46 V80 支持多路forward功能
#define IF_BUG_30107                            1
// BLF按键transfer到Page_Q,BLF灯会delay1.5-2s才亮起
#define IF_BUG_29887                            1  // 对应bug:29953
// Record需要等服务器回200OK才去亮灯
#define IF_BUG_22457_Record                     1
// 支持发送DTMF E按键
#define IF_FEATURE_DTMF_FLASH                   1
// 使用XML Browser进行autop配置的时候，如果处于通话状态，则等通话结束才发送消息给autop更改配置
#define IF_BUG_25845                            1
// 客户在通过服务器修改话机Account 1 Label时会导致话机掉线重新注册
#define IF_BUG_26237                            1
// 禁用forward历史记录
#define IF_FEATURE_FORBID_FORWARD_CALLLOG       1
// Lydis 话机的Hoteling功能与服务器网页端功能配置同步
#define IF_BUG_25622                            1
// Lydis TELE2 T46 V80 开启BroadWorks XSI功能时菜单项重复
#define IF_BUG_30078                            1
//RPS过程中输入用户名和密码需要保存在话机
#define IF_BUG_30940                            1
// Tadiran 客户特制加中性方案 支持模拟按键PHONE_KEY_FULLDSSKEY_BACK
#define IF_BUG_32044                            1
//hot desking支持删掉指定的账号信息，而不是删掉所有账号信息
#define IF_BUG_25349                            1
// Softkey显示不下时，使用...而不截断
#define IF_BUG_27597                            1
// PLCM 联系人
#define IF_BUG_PLCMDIR                          1
//Mobility
#define IF_FEATURE_BROADSOFT_MOBILITY
//Grop Night Forward
#define IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
// Call Forward Selective
//#define IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE

#if 1//IS_COLOR 默认开启，代码中动态屏蔽黑白屏
// Starface 150 门禁系统
#define IF_FEATURE_ENTRANCE_GUARD
#endif

// 获取余额 http://192.168.1.99/Bug.php?BugID=10337
#define IF_FEATURE_GETBALANCE

//Belnet T46G 通过Notify控制按键亮灯异常
#define IF_BUG_32429                            1
// 当接起通话时，先不去切换通道，通话接起后再去切换通道
#define IF_BUG_26732                            0
// 先建立通话再去停止铃声
#define IF_FEATURE_NFON_RTP                     0
// telstra 手动自动混合autoset分配方案
#define IF_FEATRUE_MANUAL_AUTOSET
// 通话错误自定义提示
#define IF_FEATURE_TALK_ERROR_OPT

#define IF_FEATURE_LONG_PRESS_DIGIT
//paging功能
#define IF_FEATURE_PAGING                   1
// xml info 加中性
#define IF_BUG_31595                            1
// 禁止用IP直播呼出非IP直播号码
#define IF_BUG_26015                            0
// http://192.168.1.99/Bug.php?BugID=30689
// QLOOG T48G 使用brekeke服务器的SCA功能时，无法将通话transfer出去
#define IF_BUG_30689                            1
// GPickUp 添加到MsgBox弹出框里面
#define IF_BUG_30016                            1
// 黑白屏扩展Dsskey
#define FEATURE_DSSKEY_EXTEND                   1
// xml browser Status自动刷新
#define IF_BUG_32431                            1
// 要求不显示来电
#define IF_BUG_29709                            1

#define IF_FEATURE_MUTI_CALLLOG                 1

#define IF_FEATURE_FAVORITE_DIR                 1

#define IF_FEATURE_34162                        1

#define IF_FEATURE_T46_LINEKEY_LABEL            1

#define IF_FEATURE_DND_SYNC                     1
// XML Info
#define IF_BUG_31595                            1
// 壁纸模式设置:拉伸、居中、适应、填充
#define IF_FEATURE_WALLPAPER_MODE               1
// XML Browser Hoteling
#define IF_FEATURE_XMLBROWSER_HOTELING          1
// dotWidgeLabel滚动流程调整：解决idle下弹窗提示滚动异常问题
#define IF_FEATURE_NEW_LABEL_SCROLL             1
// 新增Power Saving 模式下LED灯的开启关闭选项
#define IF_FEATURE_37754                        1
// 国际号码来电时无法显示姓名
#define IF_BUG_36497                            1
//在话机上显示配置文件的版本号
#define IF_BUG_34374                            1

#define IF_FEATURE_XMLB_SCREENSAVER 1

// 黑白屏开机Zero体验与彩屏一致
#define IF_FEATURE_POWER_ON_ZERO                1
// 开机若RPS开启则进入RPS界面等待
#define IF_FEATURE_POWER_ON_RPS                 0
// 开放MissCall dsskey到中性,客户需要的话要配置dsskey_custom.xml
#define IF_BUG_14856                            1

#define IF_FEATURE_LOOPBACK                     1

//#define IF_FEATURE_FAVORITE_DIR                 1
// XML Browser PhoneBook
#define IF_FEATURE_XMLDIRECTORY                 1
// 来电号码正则表达式匹配替换
#define IF_BUG_38979                            1
// BSFT redsky E911
#define IF_FEATURE_EMERGENCY_HELD               1
// T48默认账号lable字幕滚动功能
#define IF_BUG_26252                            1
// 话机有两路通话时，Transfer活跃通话后选中Hold状态的通话，几秒钟后话机自动呼出
#define IF_BUG_39709                            1

#define IF_FEATURE_GENBAND_E911                 1
// 禁止用户拒接来电需求说明书
#define IF_BUG_38956                            1
// 状态栏显示StationName
#define IF_FEATURE_STATION_NAME                 1
// Blue Contact在Directory中的顺序可配置
#define IF_BUG_39755                            1
// 因订阅Line-Seize失败而无法呼出
#define IF_BUG_39041                            1
// 解决挂机半咨询转失败问题
#define IF_BUG_40237                            1
// 长按BLF能查看通话信息
#define IF_FEATURE_SHOW_BLF_CALLID              1
// TR069监控通话状态
#define IF_FEATURE_TALKINFO_REPORT              1

/******************************************************************************************************************/
////////////////////////////////////////////////////// 客户特制的宏 ///////////////////////////////////////////////
/******************************************************************************************************************/
/*为了和其它宏区分统一格式: IF_CUSTOMER_SUPPORT_特制号_客户名*/

#define IF_CUSTOMER_SUPPORT_0_YUK_BSFT                          0@IF_CUSTOMER_SUPPORT_0_YUK_BSFT@
#define IF_CUSTOMER_SUPPORT_0_FreePBX                           0@IF_CUSTOMER_SUPPORT_0_FreePBX@
#define IF_CUSTOMER_SUPPORT_0_8x8                               0@IF_CUSTOMER_SUPPORT_0_8x8@
#define IF_CUSTOMER_SUPPORT_0_Vonage                            0@IF_CUSTOMER_SUPPORT_0_Vonage@
#define IF_CUSTOMER_SUPPORT_9_Joyce                             0@IF_CUSTOMER_SUPPORT_9_Joyce@
#define IF_CUSTOMER_SUPPORT_14_UMD                              0@IF_CUSTOMER_SUPPORT_14_UMD@
#define IF_CUSTOMER_SUPPORT_16_Ipmatika_Rostelecom              0@IF_CUSTOMER_SUPPORT_16_Ipmatika_Rostelecom@
#define IF_CUSTOMER_SUPPORT_18_Ergocom                          0@IF_CUSTOMER_SUPPORT_18_Ergocom@
#define IF_CUSTOMER_SUPPORT_30_Joyce_Telefonica                 0@IF_CUSTOMER_SUPPORT_30_Joyce_Telefonica@
#define IF_CUSTOMER_SUPPORT_64_Joyce_TMobile                    0@IF_CUSTOMER_SUPPORT_64_Joyce_TMobile@
#define IF_CUSTOMER_SUPPORT_65_Joyce_TMobile_Simence            0@IF_CUSTOMER_SUPPORT_65_Joyce_TMobile_Simence@
#define IF_CUSTOMER_SUPPORT_75_Intelbras                        0@IF_CUSTOMER_SUPPORT_75_Intelbras@
#define IF_CUSTOMER_SUPPORT_132_Zultys                          0@IF_CUSTOMER_SUPPORT_132_Zultys@
#define IF_CUSTOMER_SUPPORT_142_CSC                             0@IF_CUSTOMER_SUPPORT_142_CSC@
#define IF_CUSTOMER_SUPPORT_149_QLOOG                           0@IF_CUSTOMER_SUPPORT_149_QLOOG@
#define IF_CUSTOMER_SUPPORT_160_TYN_Jazztel                     0@IF_CUSTOMER_SUPPORT_160_TYN_Jazztel@
#define IF_CUSTOMER_SUPPORT_163_TYN_BT                          0@IF_CUSTOMER_SUPPORT_163_TYN_BT@
#define IF_CUSTOMER_SUPPORT_169_Genesys                         0@IF_CUSTOMER_SUPPORT_169_Genesys@
#define IF_CUSTOMER_SUPPORT_175_Ergocom_012                     0@IF_CUSTOMER_SUPPORT_175_Ergocom_012@
#define IF_CUSTOMER_SUPPORT_185_Nology_IS                       0@IF_CUSTOMER_SUPPORT_185_Nology_IS@
#define IF_CUSTOMER_SUPPORT_186_Netfotris                       0@IF_CUSTOMER_SUPPORT_186_Netfotris@   //特制号186,YUK Genband的
#define IF_CUSTOMER_SUPPORT_192_TYN_TGS                         0@IF_CUSTOMER_SUPPORT_192_TYN_TGS@
#define IF_CUSTOMER_SUPPORT_188_Lydis                           0@IF_CUSTOMER_SUPPORT_188_Lydis@
#define IF_CUSTOMER_SUPPORT_193_Comhem                          0@IF_CUSTOMER_SUPPORT_193_Comhem@
#define IF_CUSTOMER_SUPPORT_199_Swisscom                        0@IF_CUSTOMER_SUPPORT_199_Swisscom@
#define IF_CUSTOMER_SUPPORT_196_Matrix                          0@IF_CUSTOMER_SUPPORT_196_Matrix@
#define IF_CUSTOMER_SUPPORT_182_CNT                             0@IF_CUSTOMER_SUPPORT_182_CNT@
#define IF_CUSTOMER_SUPPORT_198_Fortinet                        0@IF_CUSTOMER_SUPPORT_198_Fortinet@
#define IF_CUSTOMER_SUPPORT_183_ZhongGuoPingAn                  0@IF_CUSTOMER_SUPPORT_183_ZhongGuoPingAn@
#define IF_CUSTOMER_SUPPORT_150_Starface                        0@IF_CUSTOMER_SUPPORT_150_Starface@
#define IF_CUSTOMER_SUPPORT_173_Ergocom_Cellcom                 0@IF_CUSTOMER_SUPPORT_173_Ergocom_Cellcom@
#define IF_CUSTOMER_SUPPORT_0_Wurzburg_College_Project          0@IF_CUSTOMER_SUPPORT_0_Wurzburg_College_Project@
#define IF_CUSTOMER_SUPPORT_0_YiXun                             0@IF_CUSTOMER_SUPPORT_0_YiXun@
#define IF_CUSTOMER_SUPPORT_0_Ringcentral                       0@IF_CUSTOMER_SUPPORT_0_Ringcentral@
#define IF_CUSTOMER_SUPPORT_184_YUK_BTGS_Genband                0@IF_CUSTOMER_SUPPORT_184_YUK_BTGS_Genband@  // Genband的基准版
#define IF_CUSTOMER_SUPPORT_19_Tadiran                          0@IF_CUSTOMER_SUPPORT_19_Tadiran@
#define IF_CUSTOMER_SUPPORT_68_Nablecomm                        0@IF_CUSTOMER_SUPPORT_68_Nablecomm@
#define IF_CUSTOMER_SUPPORT_189_NetFortris                      0@IF_CUSTOMER_SUPPORT_189_NetFortris@
#define IF_CUSTOMER_SUPPORT_0_XieCheng                          0@IF_CUSTOMER_SUPPORT_0_XieCheng@
#define IF_CUSTOMER_SUPPORT_98_NFON                             0@IF_CUSTOMER_SUPPORT_98_NFON@
#define IF_CUSTOMER_SUPPORT_190_Sekortel                        0@IF_CUSTOMER_SUPPORT_190_Sekortel@
#define IF_CUSTOMER_SUPPORT_183_ZhongGuoPingAn_Genesys          0@IF_CUSTOMER_SUPPORT_183_ZhongGuoPingAn_Genesys@
#define IF_CUSTOMER_SUPPORT_0_TaiWanShunYuan                    0@IF_CUSTOMER_SUPPORT_0_TaiWanShunYuan@
#define IF_CUSTOMER_SUPPORT_193_DT                              0@IF_CUSTOMER_SUPPORT_193_DT@       //德电
#define IF_CUSTOMER_SUPPORT_179_YUK_BT                          0@IF_CUSTOMER_SUPPORT_179_YUK_BT@
#define IF_CUSTOMER_SUPPORT_193_Telstra                         0@IF_CUSTOMER_SUPPORT_193_Telstra@
#define IF_CUSTOMER_SUPPORT_197_M2                              0@IF_CUSTOMER_SUPPORT_197_M2@
#define IF_CUSTOMER_SUPPORT_0_YingLiXun                         0@IF_CUSTOMER_SUPPORT_0_YingLiXun@
#define IF_CUSTOMER_SUPPORT_17_IPmatika_Iskrauraltel            0@IF_CUSTOMER_SUPPORT_17_IPmatika_Iskrauraltel@
#define IF_CUSTOMER_SUPPORT_67_YUK_CG                           0@IF_CUSTOMER_SUPPORT_67_YUK_CG@
#define IF_CUSTOMER_SUPPORT_135_NWT                             0@IF_CUSTOMER_SUPPORT_135_NWT@
#define IF_CUSTOMER_SUPPORT_0_XieCheng_Freeswitch               0@IF_CUSTOMER_SUPPORT_0_XieCheng_Freeswitch@
#define IF_CUSTOMER_SUPPORT_76_PCCW                             0@IF_CUSTOMER_SUPPORT_76_PCCW@
#define IF_CUSTOMER_SUPPORT_0_HUAWEI_OEM                        0@IF_CUSTOMER_SUPPORT_0_HUAWEI_OEM@
#define IF_CUSTOMER_SUPPORT_181_Fonality                        0@IF_CUSTOMER_SUPPORT_181_Fonality@

/******************************************************************************************************************/
////////////////////////////////////////////////////// 华丽的分割线 ///////////////////////////////////////////////
/******************************************************************************************************************/
#if FEATURE_METASWITCH
#undef IF_FEATURE_METASWITCH
#undef IF_FEATURE_METASWITCH_CALLLOG
#undef IF_FEATURE_METASWITCH_DIRECTORY
#undef IF_FEATURE_METASWITCH_ACD
#undef IF_FEATURE_METASWITCH_VOICEMAIL
#undef IF_FEATRUE_PTT
// 开放话机被Hold时不显示conference键的限制
#undef IF_BUG_35828
#undef IF_FEATURE_METASWITCH_DND
#undef IF_FEATURE_METASWITCH_CALLPARK
// Dsskey排列模式
#undef IF_FEATURE_DSSKEY_ALIGN_MODE

#define IF_FEATURE_METASWITCH                               1
#define IF_FEATURE_METASWITCH_CALLLOG                       1
#define IF_FEATURE_METASWITCH_DIRECTORY                     1
#define IF_FEATURE_METASWITCH_ACD                           1
#define IF_FEATURE_METASWITCH_VOICEMAIL                     1
#define IF_FEATRUE_PTT                                      1
#define IF_BUG_35828                                        1
#define IF_FEATURE_METASWITCH_DND                           1
#define IF_FEATURE_METASWITCH_CALLPARK                      1

#define IF_FEATURE_DSSKEY_ALIGN_MODE                        1
#endif

#if IF_CUSTOMER_SUPPORT_14_UMD

#endif

// 基于IF_CUSTOMER_SUPPORT_14_UMD来出版本
#if IF_CUSTOMER_SUPPORT_16_Ipmatika_Rostelecom

#endif

#if IF_CUSTOMER_SUPPORT_185_Nology_IS
#ifndef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
#define IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
#endif
#endif

#if IF_CUSTOMER_SUPPORT_0_8x8
#undef IF_BUG_31951

#define IF_BUG_31951                                        1
#endif

#if IF_CUSTOMER_SUPPORT_0_Ringcentral

#endif


#if IF_CUSTOMER_SUPPORT_75_Intelbras
#undef  IF_BUG_11498            //添加missCall灯
#undef  IF_BUG_27029
#undef  IF_BUG_4090             //T61 history按键改为Delete

#define IF_BUG_11498                                        1
#define IF_BUG_27029                                        1
#define IF_BUG_4090                                         1
#endif

#if IF_CUSTOMER_SUPPORT_142_CSC
#ifndef IF_FEATURE_C5_SYS_VOICE_RECORD
#define IF_FEATURE_C5_SYS_VOICE_RECORD
#endif
#endif

#if IF_CUSTOMER_SUPPORT_175_Ergocom_012
#undef  IF_BUG_17535
#undef  IF_BUG_21722
#undef  IF_BUG_27501
//#undef  IF_BUG_24921//去掉用Autop IME 替代
#undef  IF_BUG_27776
#undef   IF_BUG_34776

#define  IF_BUG_17535                                       1
#define  IF_BUG_21722                                       1
#define  IF_BUG_27501                                       1
//#define  IF_BUG_24921                                     1
#define  IF_BUG_27776                                       1
#define  IF_BUG_34776                                       1
#endif

#if IF_CUSTOMER_SUPPORT_18_Ergocom
#undef IF_BUG_17535
//#undef IF_BUG_24921//去掉用Autop IME 替代

#define IF_BUG_17535                                        1
//#define IF_BUG_24921                                      1
#endif

#if IF_CUSTOMER_SUPPORT_173_Ergocom_Cellcom
#undef IF_BUG_17535
//#undef IF_BUG_24921//去掉用Autop IME 替代
#undef IF_BUG_30016

#define IF_BUG_17535                                        1
//#define IF_BUG_24921                                      1
#define IF_BUG_30016                                        1
#endif

#if IF_CUSTOMER_SUPPORT_0_FreePBX

#endif

#if IF_CUSTOMER_SUPPORT_169_Genesys

#endif

#if IF_CUSTOMER_SUPPORT_30_Joyce_Telefonica
#undef  JOYCE_TEL_BUGFREE_19348
#undef  IF_BUG_29939
//#undef IF_BUG_1204 //T46 Label显示9位数字

#define  JOYCE_TEL_BUGFREE_19348                            1
#define  IF_BUG_29939                                       1
//#define    IF_BUG_1204                                        1
#endif

#if IF_CUSTOMER_SUPPORT_64_Joyce_TMobile
#undef IF_BUG_21754
#undef IF_BUG_33401_LOGONWIZARD_AUTOPAUTH

#define IF_BUG_21754                                        1
#define IF_BUG_33401_LOGONWIZARD_AUTOPAUTH                  1
#endif

#if IF_CUSTOMER_SUPPORT_65_Joyce_TMobile_Simence
#undef IF_BUG_21754

#define IF_BUG_21754                                        1
#endif

#if IF_CUSTOMER_SUPPORT_9_Joyce
//#undef IF_FEATURE_100_DIANNOW
//#undef IF_BUG_1204 //T46 Label显示9位数字

//#define IF_BUG_1204                                           1
#endif

#if IF_CUSTOMER_SUPPORT_186_Netfotris

#endif

#if IF_CUSTOMER_SUPPORT_163_TYN_BT
#undef IF_BUG_30788

#define IF_BUG_30788                                        1
#endif

#if IF_CUSTOMER_SUPPORT_160_TYN_Jazztel
#undef IF_FEATURE_CALLLIST_SHOW_METHOD

#define IF_FEATURE_CALLLIST_SHOW_METHOD                     1
#endif

// TYN Telefonica Global Solutions
#if IF_CUSTOMER_SUPPORT_192_TYN_TGS

#endif

#if IF_CUSTOMER_SUPPORT_0_YUK_BSFT
#endif

#if IF_CUSTOMER_SUPPORT_132_Zultys
#undef IF_BUG_27800
#undef IF_BUG_27796
#undef IF_FEATURE_ZULTYS_LOCALCONF
#undef IF_BUG_30958

#define IF_BUG_27800                                        1
#define IF_BUG_27796                                        1
#define IF_FEATURE_ZULTYS_LOCALCONF                         1
#define IF_BUG_30958                                        1
#endif //IF_CUSTOMER_SUPPORT_132_Zultys 

#if IF_CUSTOMER_SUPPORT_188_Lydis
// Lydis Routit T46 话机autop的时候，电源灯能够闪烁
#undef IF_BUG_24814
// XML Info
// RPS请求界面下的荷兰语翻译改为英语
#undef IF_BUG_32779
// XML Info Type类型由78变更为77
#undef IF_BUG_33083

#define IF_BUG_24814 1
#define IF_BUG_32779 1
#define IF_BUG_33083 1
#endif //_CUSTOMER_Lydis

#if IF_CUSTOMER_SUPPORT_193_Comhem
//该客户特制用中性的IF_FEATURE_CUSTOM_SOFTKEY可满足
#endif

#if IF_CUSTOMER_SUPPORT_196_Matrix
#undef IF_BUG_26015

#define IF_BUG_26015                        1
#endif

#if IF_CUSTOMER_SUPPORT_149_QLOOG
// 时间日期格式变换
#undef IF_BUG_28909
#undef IF_BUG_31081
#undef IF_BUG_31829

#define IF_BUG_28909                                        1
#define IF_BUG_31081                                        1
#define IF_BUG_31829                                        1
#endif

#if IF_CUSTOMER_SUPPORT_182_CNT
#undef IF_BUG_29144
#undef IF_BUG_23673

#define IF_BUG_29144                                        1
#define IF_BUG_23673                                        1
#endif

#if IF_CUSTOMER_SUPPORT_198_Fortinet
//http://192.168.1.99/Bug.php?BugID=27050
//Fortinet T66G 修改WEB和LCD的布局
#undef IF_BUG_27050
//http://192.168.1.99/Bug.php?BugID=28571
//Fortinet T66 需要长按OK键实现重启功能,长按Hold实现reset
#undef IF_BUG_28571
//http://192.168.1.99/Bug.php?BugID=27029
//Fortinet T66G 修改Redial Key的功能
#undef IF_BUG_27029
//http://192.168.1.99/Bug.php?BugID=27040
//Fortinet T66G 修改按键行为
#undef IF_BUG_27040
//http://192.168.1.99/Bug.php?BugID=27045
//Fortinet T66G 需要在液晶界面添加outgoing Access code（Area code）
#undef IF_BUG_27045
//不显示黑名单
#undef IF_FEATURE_SHOW_BLACKLIST
//http://192.168.1.99/Bug.php?BugID=27048
//Fortinet T66G 需要修改BLF灯的模式
//BLFStatusSpecific = 3
//http://192.168.1.99/Bug.php?BugID=27047
//Fortinet T66G 需要可以修改BLF list中的value值
#undef IF_BUG_27047
// Telstra T48 手动配置BLF List的时候可以选择特定的用户来监控
#undef IF_BUG_32812
//http://192.168.1.99/Bug.php?BugID=27033
//Fortinet T66G 修改Pickup DSSKey的行为
//http://192.168.1.99/Bug.php?BugID=27032
//Fortinet T66G 修改Paging DSSKey的行为
//http://192.168.1.99/Bug.php?BugID=27030
//Fortinet T66G 修改Intercom DSSKey的行为
#undef IF_BUG_27033
//http://192.168.1.99/Bug.php?BugID=27027
//Fortinet T66G 需要添加DND同步功能
#undef IF_BUG_27027
//Menu中加入Reboot选项
#undef IF_BUG_29376
//Dsskey 滚动
#undef IF_BUG_29361
//Record 灯
#undef IF_BUG_29368
//Un Park dsskey
#undef IF_BUG_30965
//Fortinet T66 无法page一个包含自己的group
#undef IF_BUG_30963
//Fortinet T66 需要加入FON Attendant Transfer功能
#undef IF_BUG_30961
//Fortinet T66 通话park两次后自动挂断
#undef IF_BUG_22416_PageQ_BLF
//Fortinet T66 需要支持通过BLF键输入
#undef IF_BUG_33235

#define IF_BUG_27050 1
#define IF_BUG_28571 1
#define IF_BUG_27029 1
#define IF_BUG_27040 1
#define IF_BUG_27045 1
#define IF_BUG_27047 1
#define IF_BUG_32812 1
#define IF_BUG_27033 1
#define IF_BUG_27027 1
#define IF_BUG_29376 1
#define IF_BUG_29361 1
#define IF_BUG_29368 1
#define IF_BUG_30965 1
#define IF_BUG_30963 1
#define IF_BUG_30961 1
#define IF_BUG_22416_PageQ_BLF 1
#define IF_BUG_33235 1
#endif //IF_CUSTOMER_SUPPORT_198_Fortinet

#if IF_CUSTOMER_SUPPORT_183_ZhongGuoPingAn
#undef IF_BUG_24554
#undef IF_BUG_22024_LogonWizard_AutopURL
#undef IF_BUG_23550_Pingan_Menu
#undef IF_BUG_29534

#define IF_BUG_24554                        1
#define IF_BUG_22024_LogonWizard_AutopURL   1
#define IF_BUG_23550_Pingan_Menu            1
#define IF_BUG_29534                        1
#endif

#if IF_CUSTOMER_SUPPORT_150_Starface
#endif // IF_CUSTOMER_SUPPORT_150_Starface

#if IF_CUSTOMER_SUPPORT_0_Wurzburg_College_Project
#undef IF_FEATURE_XMLDIRECTORY

#define IF_FEATURE_XMLDIRECTORY                 1
#endif

#if IF_CUSTOMER_SUPPORT_0_YiXun

#endif

#if IF_CUSTOMER_SUPPORT_184_YUK_BTGS_Genband
#undef IF_BUG_30062

#define IF_BUG_30062                            1
#ifndef IF_FEATURE_GENBAND
#define IF_FEATURE_GENBAND
#endif
#endif

#if IF_CUSTOMER_SUPPORT_19_Tadiran
#undef IF_BUG_34413
#undef IF_BUG_34565
#undef IF_BUG_34694
#undef IF_BUG_34339
#undef IF_BUG_35381
#undef IF_BUG_34393
#undef IF_BUG_36591

#define IF_BUG_34413                      1
#define IF_BUG_34565                      1
#define IF_BUG_34694                      1
#define IF_BUG_34339                      1
#define IF_BUG_35381                      1
#define IF_BUG_34393                      1
#define IF_BUG_36591                      1
#endif

#if IF_CUSTOMER_SUPPORT_67_YUK_CG

#endif

#if IF_CUSTOMER_SUPPORT_68_Nablecomm

#endif //IF_CUSTOMER_SUPPORT_68_Nablecomm

#if IF_CUSTOMER_SUPPORT_189_NetFortris
#ifndef IF_FEATURE_GENBAND
#define IF_FEATURE_GENBAND
#endif

#endif //IF_CUSTOMER_SUPPORT_189_NetFortris

#if IF_CUSTOMER_SUPPORT_0_XieCheng
#undef IF_FEATURE_CTRIP_CTI
#undef IF_FEATURE_FORBIDDEN_FWD_TRAN
#undef IF_BUG_26469//该宏不需要,修改配置DialToneTimeout=60

#define IF_FEATURE_CTRIP_CTI                    1
#define IF_FEATURE_FORBIDDEN_FWD_TRAN           1
#define IF_BUG_26469                            1
#endif

#if IF_CUSTOMER_SUPPORT_0_XieCheng_Freeswitch
#undef IF_BUG_31642
#undef IF_FEATURE_CTRIP_CTI

#define IF_BUG_31642                            1
#define IF_FEATURE_CTRIP_CTI                    1
#endif

#if IF_CUSTOMER_SUPPORT_98_NFON
// 如果话机收到的Invite报文包含Diversion Header，屏幕显示不正常
#undef IF_BUG_26914
// T46G 需要完善Call completion功能
#undef IF_BUG_23548
// 需要在通话过程中可调出XML Phonebook
#undef IF_BUG_28455
// 隐藏历史记录详情的Line
#undef IF_FEATURE_HIDE_CALLLOG_DETAIL_LINE
// 禁用Dsskey Line功能
#undef IF_FEATURE_BLOCK_LINE
// T46 LineKey Label长度
#undef IF_FEATURE_T46_LINEKEY_LABEL
// Call History多种显示方式的需求
#undef IF_FEATURE_CALLLIST_SHOW_METHOD

#define IF_BUG_26914                            1
#define IF_BUG_23548                            1
#define IF_BUG_28455                            1
#define IF_FEATURE_HIDE_CALLLOG_DETAIL_LINE     1
#define IF_FEATURE_BLOCK_LINE                   1
#define IF_FEATURE_T46_LINEKEY_LABEL            1
#define IF_FEATURE_CALLLIST_SHOW_METHOD         1
#endif

#if IF_CUSTOMER_SUPPORT_190_Sekortel
//Sekortel T23 transfer后 Action URL中的remote URL上报问题
#undef IF_BUG_30423

#define IF_BUG_30423                            1
#endif //IF_CUSTOMER_SUPPORT_190_Sekortel

#if IF_CUSTOMER_SUPPORT_199_Swisscom
#endif

#if IF_CUSTOMER_SUPPORT_0_Vonage
#undef IF_BUG_30625

#define IF_BUG_30625                            1
#endif

#if IF_CUSTOMER_SUPPORT_183_ZhongGuoPingAn_Genesys

#endif

#if IF_CUSTOMER_SUPPORT_0_TaiWanShunYuan

#endif

#if IF_CUSTOMER_SUPPORT_193_Telstra
#undef IF_BUG_30885
// T48 LineKey 需要支持分行显示
#undef IF_BUG_25010
// label显示优化,输入为10个纯数字,则显示的格式为xx xxxx xxxx
#undef IF_BUG_25367
// Telstra Softkey layout需求,允许Hold按Cancel退出通话
#undef IF_BUG_25055
// T42分2行显示
#undef IF_FEATURE_T42_LINEKEY_SECONDLINE
// Telstra V80升级 Dsskey类型特制:Dsskey只允许被配成： NA、Line、Speed Dial、BLF List几种
#undef IF_FEATURE_DSSKEY_LIST
// 重设blflist时，不允许重设line
#undef IF_BUG_28566
#undef IF_BUG_25058
// 隐藏呼出特征码
#undef IF_BUG_24711
// Telstra T48 手动配置BLF List的时候可以选择特定的用户来监控
#undef IF_BUG_32812


#define IF_BUG_30885                            1
#define IF_BUG_25010                            1
#define IF_BUG_25367                            1
#define IF_BUG_25055                            1
#define IF_FEATURE_T42_LINEKEY_SECONDLINE       1
#define IF_FEATURE_DSSKEY_LIST                  1
#define IF_BUG_28566                            1
#define IF_BUG_25058                            1
#define IF_BUG_24711                            1
#define IF_BUG_32812                            1

#ifndef IF_FEATRUE_NETWORKCONF_MULTROUTINE
#define IF_FEATRUE_NETWORKCONF_MULTROUTINE
#endif
#endif

#if IF_CUSTOMER_SUPPORT_193_DT
#undef IF_BUG_30940
#undef IF_BUG_23784
#undef IF_BUG_28566
#undef IF_FEATURE_32591

#define IF_BUG_30940                            1
//去除该功能
#define IF_BUG_23784                            0
#define IF_BUG_28566                            1
#define IF_FEATURE_32591                        1

#ifndef IF_FEATURE_AUTO_ANSWER_TIMER
#define IF_FEATURE_AUTO_ANSWER_TIMER
#endif

#ifndef IF_FEATURE_FEATURE_PAGE
#define IF_FEATURE_FEATURE_PAGE
#endif

#ifndef IF_FEATURE_SOFTKEY_SEND_OPT
#define IF_FEATURE_SOFTKEY_SEND_OPT
#endif

#ifndef IF_FEATURE_DIR_OPT
#define IF_FEATURE_DIR_OPT
#endif

#ifndef IF_FEATURE_FORWARD_OPT
#define IF_FEATURE_FORWARD_OPT
#endif

#ifndef IF_FEATURE_COMBOBOXLABEL
#define IF_FEATURE_COMBOBOXLABEL
#endif

#ifndef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
#define IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
#endif
#endif

#if IF_CUSTOMER_SUPPORT_0_YingLiXun
#undef IF_BUG_27560

#define IF_BUG_27560                            1
#endif

#if IF_CUSTOMER_SUPPORT_0_HUAWEI_OEM
#undef IF_FEATURE_PAGING

#define IF_FEATURE_PAGING             0
#endif

#if IF_CUSTOMER_SUPPORT_17_IPmatika_Iskrauraltel
#undef IF_BUG_31481
#define IF_BUG_31481                            1
#endif //IF_CUSTOMER_SUPPORT_17_IPmatika_Iskrauraltel

#if IF_CUSTOMER_SUPPORT_179_YUK_BT
// V81均已加入中性
#endif

#if IF_CUSTOMER_SUPPORT_197_M2
//#undef IF_BUG_28566

//#define IF_BUG_28566                             1  //v81中autop改变值发一个消息过来，所以不会存在v80的问题。
#undef IF_BUG_35285
#undef IF_FEATURE_MUTI_CALLLOG
#undef IF_BUG_MCA_PARK

#define IF_BUG_35285                            1
#define IF_FEATURE_MUTI_CALLLOG                 1
#define IF_BUG_MCA_PARK                         1 //mca park没有空闲linekey无法呼出
#endif

#if IF_CUSTOMER_SUPPORT_135_NWT
#undef IF_FEATURE_FORBIDDEN_FWD_TRAN

#define IF_FEATURE_FORBIDDEN_FWD_TRAN         1
#endif//IF_CUSTOMER_SUPPORT_135_NWT

#if IF_CUSTOMER_SUPPORT_76_PCCW
//PCCW T41P V81 软件中定制 SRTP encryption 图标和判断逻辑
#undef IF_FEATURE_SRTP
#undef IF_BUG_33301
//ACD的状态修改为2个
#undef IF_BUG_24860

#define IF_FEATURE_SRTP                         1
#define IF_BUG_33301                            1
#define IF_BUG_24860                            1
#endif

#if IF_CUSTOMER_SUPPORT_181_Fonality
//Idle界面新增Logo显示
#undef IF_BUG_33812

#define IF_BUG_33812                            1
#endif

#ifdef IF_LYNC
#define IF_TALKUI_BLUETOOTH_BT_ENABLE 0
#else
#define IF_TALKUI_BLUETOOTH_BT_ENABLE 1
#endif

#endif

