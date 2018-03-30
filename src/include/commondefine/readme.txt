接口文件提供给嵌入式使用，由于历史定义有冲突等问题，因此
拆分service_dsk_interface.h文件，将对外的定义拷贝到service_dsk_interface_extension.h文件。
如果出现外部模块编译不过，需要将部分service_dsk_interface.h的定义合入。

提供外部接口时，请提供：
service_dsk_interface_extension.h（外部使用的定义全部合到该文件，包括消息及结构体）

2017/11/8
新划分消息段供外部使用：
#define DSK_MSG_APP		(DSK_MSG_SIZE*64)		// section of service:application section
#define DSK_MSG_TALK	(DSK_MSG_SIZE*68)		// section of service:talk section
#define DSK_MSG_CONTACT	(DSK_MSG_SIZE*72)		// section of service:contact section
#define DSK_MSG_MENU	(DSK_MSG_SIZE*76)		// section of service:menu section
// 76 - 120 扩展新业务
#define DSK_MSG_QUERY	(DSK_MSG_SIZE*120)		// section of service:query section
#define DSK_MSG_EXTENSION	(DSK_MSG_SIZE*128)	// section of extension
// 120 - 255 预留
#define DSK_MSG_MAX		(DSK_MSG_SIZE*255)		// max limit

date:2018/1/18
author: hwz
修改gui为service程序，对外提供libdskservice.so库，及接口文件service_dsk_interface_extension.h
1、外部通过调用接口读取gui信息
2、外部通过注册notify接收消息

date：2018/3/12
author：hwz
1、网页需要开放接口的超时时间
2、同步vcs对外开放的参数
3、由于安卓要将c++接口转换成java接口，拆分service_config.h文件：将SETTING_TYPE结果独立成setting_type_define.h

