//***************************************************************
//module.h
//
// 功能:版本信息、编译条件定义、重大BUG记录
//
// Copyright(c) 2003-2009Xiamen Yealink Network Technology Co,.Ltd
// By:xyx			2009-02-16
//***************************************************************/*


/********************************************************************
	修改时间: 2009.03.21
	原版本号: 0.0.0.1
	新版本号: 0.0.0.2
	by: xyx 

	(1)更改内容: url 解析函数
	错误现象:解析url时，如果url中没有端口号，即用默认
					的80 端口，解析时得不到路径值
	错误原因: 开发后期，为在解析的url 路径前加'/', 改了代码，
					测试时没测默认端口的情况。
	更改措施: 在路径前强加'/'
	编译条件: HTTP_URL_PATH
	

********************************************************************/
#define  HTTP_URL_PATH 1

/********************************************************************
	修改时间: 2009.03.24
	原版本号: 0.0.0.2
	新版本号: 0.0.0.3
	by: xyx 

	(1)更改内容:  
	错误现象:http_recv_to_file函数要是打开一个不存在的路径，
					    最后函数返回时调用fclose 出错，程序退出
	错误原因: bug
	更改措施: 
	编译条件: HTTP_FILE_CLOSE

	(2)更改内容:  http头的User-Agent 字段值从配置文件读取
	错误现象:
	错误原因: 客户需求
	更改措施: 
	编译条件: HTTP_USER_AGENT
	

********************************************************************/
#define HTTP_FILE_CLOSE 1
#define  HTTP_USER_AGENT 1

/********************************************************************
	修改时间: 2009.04.10
	原版本号: 0.0.0.3
	新版本号: 0.0.0.4
	by: xyx 

	(1)更改内容:  增加https 功能
	错误现象:
	错误原因: 
	更改措施: 
	编译条件: HTTPS_FUNC
********************************************************************/
#define  HTTPS_FUNC 1

/********************************************************************
	修改时间: 2009.05.8
	原版本号: 0.0.0.4
	新版本号: 0.0.0.5
	by: wxf

	(1)更改内容:  增加区别振铃功能
	错误现象:
	错误原因: 
	更改措施: 
	编译条件: HTTPS_DISTINCTIVE_RINGER

	(2)更改内容:  下载超时退出
	错误现象: 下载超时，程序不退出
	错误原因: select返回为0时程序超时，程序对返回0没有处理
	更改措施: 
	编译条件: HTTPS_TIME_OUT

	(3)更改内容:  增加导入web 服务器证书接口
	错误现象: 
	错误原因: 
	更改措施: 
	编译条件: HTTPS_WEB_SERVER_CA

********************************************************************/
#define  HTTPS_DISTINCTIVE_RINGER 1
#define HTTPS_TIME_OUT 1
#define HTTPS_WEB_SERVER_CA 1

/********************************************************************
	修改时间: 2009.06.08
	原版本号: 0.0.0.5
	新版本号: 0.0.0.6
	by: xyx 

	(1)更改内容: 打开 HTTP_USER_AGENT 编译条件
	错误现象:
	错误原因: 0.0.0.4版本时关闭了HTTP_USER_AGENT 编译条件
	更改措施: 
	编译条件: 
********************************************************************/

/********************************************************************
	修改时间: 2009.06.13
	原版本号: 0.0.0.6
	新版本号: 0.0.0.7
	by: xyx 

	(1)更改内容: http请求头增加Host 域
	错误现象:
	错误原因: TIPEL客户需求
	更改措施: 
	编译条件: 
********************************************************************/
#define HTTP_TIPEL_HOST 1

/********************************************************************
	修改时间: 2009.06.29
	原版本号: 0.0.0.7
	新版本号: 0.0.0.8
	by: xyx 

	(1)更改内容: /yealink/config/certs目录中无server 文件夹时，
						自动创建该文件夹
	错误现象: 无法导入web服务器证书
	错误原因: 
	更改措施: 
	编译条件: 
********************************************************************/
#define HTTP_MKDIR_SERVER 1


/********************************************************************
	修改时间: 2009.09.18
	原版本号: 0.0.0.8
	新版本号: 0.0.0.12
	by: xyx 

	(1)更改内容: 增加ftp 和tftp 接口
	错误现象:
	更改原因: jxz要用到tftp 接口，将tftp和ftp接口加入到
					通用库中，方便调用
	更改措施: 
	编译条件: HTTP_ADD_FTP_TFTP
	
	(2)更改内容: tftp，ftp，http，https的每个接口函数增加
						超时参数设置
	错误现象: 
	更改原因: 应用需求
	更改措施: 每个接口函数增加设置超时的参数
	编译条件: HTTP_ADD_TIMEOUT

	
	(3)更改内容: tftp，ftp，http，https的每个接口函数限制最大的
						文件下载字节数
	错误现象: 
	更改原因: 应用需求
	更改措施: 每个接口函数增加设置最大下载字节数的参数
	编译条件: HTTP_DOWNLOAD_MAX

	(4)更改内容: http的user agent 的格式改成:
						Yealink SIP-T28 2.X.0.X 00:15:65:xx:xx:xx
						
	错误现象: 
	错误原因:   客户需求
	更改措施:   Yealink是厂商标识，先从
					/yealink/factory/Setting/Setting.cfg->[http]-Useragent读取，读不到
					时默认是yealink
					
					SIP-T28是产品型号，先从
					/yealink/config/Setting/Setting.cfg ->[PhoneSetting]->ProductName读取，
					读不到从
					/yealink/factory/Setting/Setting.cfg ->[PhoneSetting]->ProductName读，再
					读不到默认"SIP-T28"

					2.X.0.X是固件版本号

					00:15:65:xx:xx:xx是mac地址
	编译条件: HTTP_USER_AGENT_1

	(5)更改内容: 把输出信息写到syslog
	错误现象: 
	错误原因: 
	更改措施:
	编译条件:HTTP_SYSLOG
********************************************************************/
#define HTTP_ADD_FTP_TFTP 1
#define HTTP_ADD_TIMEOUT 1
#define HTTP_DOWNLOAD_MAX 1
#define HTTP_SYSLOG 1

/********************************************************************
	修改时间: 2009.09.24
	原版本号: 0.0.0.12
	新版本号: 0.0.0.13
	by: xyx 

	(1)更改内容: 如果没有设置dns服务器，即dns的个数
					为0，会导致计算dns超时时间错误
	错误现象: 导致调用该接口函数的进程退出
	更改措施: 默认dns服务器的个数是1
	编译条件: HTTP_DNS_COUNT
********************************************************************/
#define HTTP_DNS_COUNT 1

/********************************************************************
	修改时间: 2009.09.29
	原版本号: 0.0.0.13
	新版本号: 0.0.0.14
	by: xyx 

	(1)更改内容: 如果没有设置网关，在对异步connect操作
						的返回结果判断出错，误判为connect成
						功
	错误现象: 导致调用该接口函数的进程退出
	更改措施: 增加对connect返回的错误码的判断
	编译条件: HTTP_CONNECT_ERROR
********************************************************************/
#define HTTP_CONNECT_ERROR 1

/********************************************************************
	修改时间: 2009.11.24
	原版本号: 0.0.0.14
	新版本号: 0.0.0.16
	by: xyx 

	(1)更改内容: https增加摘要认证功能
	错误现象: V42,V43需求
	错误原因: 
	更改措施: 
	编译条件: HTTPS_DIGEST
********************************************************************/
#define HTTPS_DIGEST 1

/********************************************************************
	修改时间: 2009.12.01
	原版本号: 0.0.0.16
	新版本号: 0.0.0.17
	by: xyx 

	(1)更改内容: 区别振铃文件下载的大小限制成
						DR_CACHE_BUFFER_SIZE
	错误现象: wildix:从服务器下载区别振铃失败，从syslog显
					示：http_client.c(978): remote file size:200058, max:51200 Nov 
					12 17:08:06 syslog: error.c(35): Errcode=0x01ff, desc:exceed max 
					file size。
					同样的铃声在V41上可以正常下载，其他机
					型的V42软件也有同样的问题。
	错误原因:  下载接口增加对下载文件大小限制时,将区
					别振铃文件的大小限制在50k
	更改措施: 
	编译条件: HTTPS_DR_SIZE
********************************************************************/
#define HTTPS_DR_SIZE 1

/********************************************************************
	修改时间: 2010.01.25
	原版本号: 0.0.0.17
	新版本号: 0.0.0.19
	by: xyx 

	(1)更改内容: 对http_recv_to_ram等函数的执行结果的判断依据
					   错误
						
	错误现象: http认证失败时再次返回401，由于对执行结果
					的判断依据错误，导致误认为下载成功
					
	错误原因:  用函数的返回结果作为判断依据
	更改措施: 
	编译条件: HTTPS_RET_ERROR
********************************************************************/
#define HTTPS_RET_ERROR 1

/********************************************************************
	修改时间: 2010.04.27
	原版本号: 0.0.0.19
	新版本号: 0.0.0.21
	by: xyx 

	(1)更改内容: https增加客户端认证功能
						
	错误现象:  joyce需求
					
	错误原因:  
	更改措施: 启用openssl的客户端认证功能，客户端
					证书和web服务器的证书相同。
					
	编译条件: HTTPS_CLENT_CERT

	(2)更改内容: https_recv_to_ram函数中增加对nReturnCode变量的
						赋值
						
	错误现象:  https_recv_to_file下载后没有存放到指定的文件中
					
	错误原因:  https_recv_to_file中对https_recv_to_ram返回值做了判断，
					而https_recv_to_ram没有对nReturnCode变量赋值，导致
					返回值错误
	更改措施:  https_recv_to_ram增加对nReturnCode变量赋值
					
	编译条件: HTTPS_ADD_RET_CODE
********************************************************************/
#define HTTPS_CLENT_CERT 1
#define HTTPS_ADD_RET_CODE 1


/********************************************************************
	修改时间: 2010.06.11
	原版本号: 0.0.0.21
	新版本号: 0.0.0.22
	by: lzh

	错误现象:  
		三通科技反馈使用Remote Xml Phonebook功能的时候,
		如果url下载的是他们的PHP文件,会出现提示“Load remote phone book error”，	
				
	错误原因:  
		使用客户的url调试，发现该服务器返回的200 OK是这样的：
		HTTP/1.1 200 HTTP
		X-Powered-By: PHP/4.4.9
		content-type:text/xml
		Connection: close
		<YealinkIPPhoneDirectory>
		<DirectoryEntry>
		<Name>test</Name>
		<Telephone>101</Telephone>
		</DirectoryEntry>
		</YealinkIPPhoneDirectory>	
		没有content-length头域表示xml文件的长度,导致下载失败	
	
	更改措施: 
		在该种情况下将xml文件的长度设置为允许接收的文件最大长度
					
	编译条件: HTTP_200OK_WITHOUT_CONTENT_LENGTH
********************************************************************/
#define HTTP_200OK_WITHOUT_CONTENT_LENGTH		0


/********************************************************************
	修改时间: 2010.06.17
	原版本号: 0.0.0.22
	新版本号: 0.0.0.23
	by: lzh

	错误现象:
		在0.0.0.21版本中增加了对Https客户端证书的支持,但是在如下场景
		不能正常使用
		1 https服务器不要求认证客户端
        2 设备上没有客户端证书/yealink/config/certs/server/WebServer.pem
	错误原因:
		实现该功能是未处理该场景
	更改措施:
		增加对该场景的处理					
	编译条件: HTTPS_CLENT_CERT_EXT
********************************************************************/
#define		HTTPS_CLENT_CERT_EXT		1


/********************************************************************
	修改时间: 2010.06.21
	原版本号: 0.0.0.23
	新版本号: 0.0.0.24
	by: lzh

	(1)更改内容: 
	客户需求:
		Wildix反馈当HTTP的端口不是用默认80的时候，话机不会在HTTP 'Host'
		header中发送HTTP的端口号，SNOM的话机可以。
	错误现象:
		比如Url为http://10.1.1.183:8080/y000000000000.cfg
		则当前http库发出的http请求是这样的:
		GET /y000000000000.cfg HTTP/1.0
		Host: 10.1.1.183
		User-Agent: yealink SIP-T28P 2.50.0.40 00:15:65:11:4c:33
		而Snom及IE发出的http请求是这样的:
		GET /y000000000000.cfg HTTP/1.0
		Host: 10.1.1.183:8080
		User-Agent: yealink SIP-T28P 2.50.0.40 00:15:65:11:4c:33
	错误原因: 
		实现该功能的时候没有考虑该种情况
	更改措施: 
		参照Snom及IE对该种情况进行处理
	编译条件: 
********************************************************************/
#define HTTP_HOST_HEADER_PORT 1

/********************************************************************
	修改时间: 2010.06.24
	原版本号: 0.0.0.24
	新版本号: 0.0.0.25
	by: lzh

	(1)更改内容: 
	客户需求:
		客户Sistek测试V50发现的问题：远程地址薄无法加载超过81条记录的
		地址薄。客户提供了他们使用的地址薄回来，第一个可以直接按OK进入
		，显示81条记录，第二个无法直接进入：
		1. http://www.sistek.cl/search.php?name=#SEARCH
		2. http://www.sistek.cl/search2.php?name=#SEARCH
	错误分析:
		两个地址返回的200 OK只有data的长度有区别,经过测试发现如果数据比较
		长，就需要在给Http的GET请求加上头域accept和Connection
	更改措施: 
		HTTP请求增加accept和Connection头域
	编译条件: 
		HTTP_LONG_DATA
********************************************************************/
#define HTTP_LONG_DATA		1

/* *******************************************************************
    修改时间: 2010.07.01
    原版本号: 0.0.0.25
    新版本号: 0.0.0.26
    by: lzh

    (1)更改内容:
    客户需求:
        Wildix反馈使用区别振铃的时候，话机不会播放铃声会当机3-4秒钟
    错误现象:
        TalkLogic退出
    错误原因:
        在0.0.0.22版本中增加对特殊格式的200 OK的时候，没有考虑下载
        区别振铃格式的情况，导致出错
    更改措施:
        修改该问题
    编译条件:
******************************************************************* */
#define    HTTP_200OK_WITHOUT_CONTENT_LENGTH_OPT 1
/* *******************************************************************
    修改时间: 2010.09.06
    原版本号: 0.0.0.26
    新版本号: 0.0.0.27
    by: lzh

    (1)更改内容: tftp增加下载到内存的接口函数。

    错误现象:  tftp下载固件时出现内存不足

    错误原因:  tftp之前只提供下载到文件的接口,autop用
                    tftp下载固件到/tmp下的文件中,然后再读
                    出放到内存中,这样占用了两份内存,导
                    制内存不足
    更改措施:  T2X的flash是8 M,申请8M的内存用于存放下载
                    的固件
    编译条件: T2X_TFTP_RAM
******************************************************************* */
#define T2X_TFTP_RAM                 1

/* *******************************************************************
    修改时间: 2010.09.21
    原版本号: 0.0.0.27
    新版本号: 0.0.0.28
    by: fwj

    (1)更改内容: tftp下载时，如果外部专递的内存指针不为空则使用外部提供的内存！

    错误现象:

    错误原因:  新版的rom写入方式，使用共享内存传递rom内容给flasher写入工具

    更改措施:  增加传入内存指针空检查
    编译条件: T2X_EXTERN_TFTP_RAM
******************************************************************* */
#define T2X_EXTERN_TFTP_RAM                 1

/* *******************************************************************
    修改时间: 2010.09.07
    原版本号: 0.0.0.28
    新版本号: 0.0.0.29
    by: wcz

    (1)更改内容:
    客户需求:
        传入的URL字符串中文件名可能也含有 @ 字符
    错误现象:
        使得不好判断用户名与密码
    错误原因:

    更改措施:
        将原先通过从URL字符串中获取用户名密码文件名的方式，
        改为通过传入结构体FILE_INFO_ST来获取这些信息。
    编译条件:
******************************************************************* */
#define    HTTP_CHANGE_GET_FILEINFO_METHOD 1

/* *******************************************************************
    修改时间: 2010.09.07
    原版本号: 0.0.0.29
    新版本号: 0.0.0.30
    by: wcz

    (1)更改内容:
    客户需求:

    错误现象:

    错误原因:
            不支持IPV6
    更改措施:
            增加对IPV6的支持
            注:0.0.0.26的版本(更早的没试过)将HTTP_ADD_TIMEOUT
                    置0会导致编译不过(即有些需要加该编译条件的地方没加)
                    我没把我的更改加到HTTP_ADD_TIMEOUT为0的地方，所以要将HTTP_ENABLE_IPV6，
                    HTTP_CHANGE_GET_FILEINFO_METHOD置1，得同时将HTTP_ADD_TIMEOUT置1.
    编译条件:
******************************************************************* */

/* *******************************************************************
    修改时间: 2010.10.25
    原版本号: 0.0.0.30
    新版本号: 0.0.0.31
    by: wcz

    (1)更改内容:
    客户需求:tiptel客户需求

    错误现象:

    错误原因:目前url最长为256，客户需要增加至512

    更改措施: 增加buf的长度

    编译条件: HTTP_INC_URL_LEN
******************************************************************* */
#define    HTTP_INC_URL_LEN 1
/* *******************************************************************
    修改时间: 2010.11.09
    原版本号: 0.0.0.31
    新版本号: 0.0.0.32
    by: wcz

    (1)更改内容:
    客户需求:

    错误现象:
            libhttp库，DNS解析时间较长
    错误原因:

    更改措施:
            修改libhttp库，当本机是ipv4地址时，不去请求AAAA。如果本机是ipv6地址，则不去请求A
    编译条件: HTTP_ENABLE_IPV6
******************************************************************* */
/* *******************************************************************
    修改时间: 2010.11.09
    原版本号: 0.0.0.32
    新版本号: 0.0.0.33
    by: wcz

    (1)更改内容:
    客户需求:

    错误现象:

    错误原因: 未对302返回码进行处理

    更改措施: 处理302返回码

    编译条件: HTTP_302
******************************************************************* */
#define HTTP_302 1
/* *******************************************************************
    修改时间: 2010.11.15
    原版本号: 0.0.0.33
    新版本号: 0.0.0.34
    by: wcz

    (1)更改内容:
    客户需求:

    错误现象:1、两个证书不一样，却判断为同一个证书
                    2、不能从客户的https服务器上下文件

    错误原因:1、 原先是通过判断两个证书的issuer是否一样，来判断是否是同一个证书
                    2、客户的https服务器使用TLSv1，而https是使用SSLv3。

    更改措施:1、通过判断issuer和serial的hash值判断是否是两个相同的证书
                    2、将meth=SSLv3_method()改为meth=TLSv1_method()

    编译条件: HTTP_CERT_TLSV1
******************************************************************* */
#define    HTTP_CERT_TLSV1 1
/* *******************************************************************
    修改时间: 2010.11.15
    原版本号: 0.0.0.34
    新版本号: 0.0.0.35
    by: wcz

    (1)更改内容:
    客户需求:

    错误现象:服务器返回的消息中不带content-length时升级固件失败


    错误原因:
                    http_remain_to_ram函数里面回去检测下载文件大小，由于没有content-length
                    无法知道文件大小，所以http_remain_to_ram返回失败

    更改措施:
                    当nFileSize为最大值8*1024*1024时，不去检测下载到的文件大小是否正确
                    并且返回实际接收到的数据
    编译条件: HTTP_WITHOUT_CONTENT_LENGTH_CHECKFILESIZE
******************************************************************* */
#define    HTTP_WITHOUT_CONTENT_LENGTH_CHECKFILESIZE 1

/* *******************************************************************
    修改时间: 2010.11.22
    原版本号: 0.0.0.35
    新版本号: 0.0.0.36
    by: wcz

    (1)更改内容:
    客户需求:

    错误现象:

    错误原因:

    更改措施: 处理302码时，增加http对用户名，密码的处理

    编译条件: HTTP_302
******************************************************************* */
/* *******************************************************************
        修改时间: 2010.11.22
        原版本号: 0.0.0.36
        新版本号: 0.0.0.37
        by: wcz

        (1)更改内容:
        客户需求:

        错误现象:

        错误原因:

        更改措施: 在0.0.0.36的基础上打开HTTP_ENABLE_IPV6编译条件

        编译条件:
******************************************************************* */

/* *******************************************************************
    修改时间: 2010.12.10
    原版本号: 0.0.0.37
    新版本号: 0.0.0.38
    by: Jacky(wcl)

    更改内容:  
        目前sip user agent的格式是：Well model_name hardware_version firmware_version MAC custom_value。
        Telefonica需要http/https的user agent也是这种格式的。
        其中的custom value可以通过auto provisioning进行修改的！
    错误现象:
    错误原因:  Joyce Telefonica T22/T26 需要http/https user agent支持custom value
    更改措施:  
        在/yealink/factory/Setting/Setting.cfg(T6X /phone/config/system.ini)的[http]下添加
        UseragentDynm = Yealink SIP-T28P $HW_VER $FW_VER $MAC custom_value
        并增加代码对其进行判断和替换。
    编译条件: HTTP_USER_AGENT_DYNM
******************************************************************* */
#define  HTTP_USER_AGENT_DYNM   1

/* *******************************************************************
    修改时间: 2010.12.22
    原版本号: 0.0.0.38
    新版本号: 0.0.0.39
    by: Jacky(wcl)

    客户需求:
        客户要求话机当收到401消息后直接跳出错误提示，
        而不要连续的请求。
    错误现象:
        当密码错误后话机收到401消息话机会连续的去request, 
        而当服务器收到连续5次错误的密码请求后会将账号锁住。
    错误原因: 
    更改措施:  
        控制当使用密码发送GET后还会收到401就去通知GUI认证
        错误，之后GUI会弹出帐号错误提示框要求重新输入密码。
    编译条件: HTTP_AUTH_ERR_GUI
******************************************************************* */
#define  HTTP_AUTH_ERR_GUI   1

/* *******************************************************************
    修改时间: 2011.1.20
    原版本号: 0.0.0.39
    新版本号: 0.0.0.40
    by: lxy
	更改原因:
		在v60版本中autop统一将url 的地址更改为ip 地址的形式，
		将其传入下载库，因为http和https在填充某些头域时
		需要的url，导致下载库下载失败。
    更改方法:
	   autop中不进行域名解析，将url传给下载库，由下
	   载库来完成域名解析
    编译条件: 
      HTTP_DNS_TO_IP
******************************************************************* */
#if M_VIDEPHONE
#define  HTTP_DNS_TO_IP   0
#else
#define  HTTP_DNS_TO_IP   1
#endif
/* *******************************************************************
    修改时间: 2011.1.24
    原版本号: 0.0.0.40
    新版本号: 0.0.0.41
    by: wzt
    客户需求:  
    错误现象:
        客户用T22（.51.0.10）测试，用HTTPS服务器auto-provision,
        当客户没上传证书时，可以升级成功，但当客户上传证书后，
        就升级不成功了。
    错误原因: 
         当客户向Https服务器上传证书时，会调用install_ca函数计算出证书的
         subject hash值，并以hash值重命名证书，但install_ca函数调用了错误的
         函数来计算hash值，使得autop程序在加载本地信任证书库时无法成功加载证书，
         导致在认证时无法认证https服务器，进而无法下载升级配置文件，以致
         升级失败。
    更改措施:  
        在install_ca函数中，改调用X509_subject_name_hash函数来计算证书的
         subject hash值。
    编译条件: HTTP_CERT_SUBJECT_HASH
******************************************************************* */
#define HTTP_CERT_SUBJECT_HASH 1

/* *******************************************************************
	修改时间: 2011.2.15
    原版本号: 0.0.0.41
    新版本号: 0.0.0.42
    by: lxy	
    客户需求:  
	    (1) http/https时发送的User-Agent的值有两种形式，当UseragentDynm 存在时，
	能根据UseragentDynm中包含对应的$HW_VER, $FW_VER ,$MAC 进行相应的替换，
	同时还能在其后添加随机的custom value 。否则使用原有格式，将读
	取Useragent 的值，再在其后添加ProductModel, Version, Mac 作为最终的User-Agent。		
	
    更改措施:  
    (1) 目前sip user agent的格式是根据：
	yealink/factory/Setting/Setting.cfg
	[http]
	UseragentDynm = Yealink SIP-T28P $HW_VER $FW_VER $MAC custom_value
	其中的custom value可以通过auto provisioning进行修改的！
	如: UseragentDynm = Yealink $MAC, 则最终输出地User-Agent: Yealink 00:15:65:c0:11:00
	(2) 假如yealink/factory/Setting/Setting.cfg中没有UseragentDynm，那么将使用第二种格式
	yealink/factory/Setting/Setting.cfg
	[http]
	Useragent = Yealink 
	最终输出的User-Agent: Useragent ProductModel Version Mac 
	
    编译条件:		无
******************************************************************* */

/* *******************************************************************
    修改时间: 2011.2.18
    原版本号: 0.0.0.42
    新版本号: 0.0.0.43
    by: xww
    客户需求:  
    错误现象:
        读取文件配置错误。
    错误原因: 
         机型不同，配置目录结构不同。
    更改措施:  
        修改配置
    编译条件: PHONE_CONFIG_PATH
******************************************************************* */
#define PHONE_CONFIG_PATH 1

/********************************************************************
	修改时间: 2011.02.20
	原版本号: 0.0.0.43
	新版本号: 0.0.0.44
	by: xww

	(1)更改内容: 
	客户需求:
		
	错误现象:
	        HTTPS 无法下载文件
			
	错误原因: 
			1. https_401_process 函数中调用 http_parse_url_2 时第三个参数出错，应该是Err_st结构的指针。
			2. http_parse_url_2 函数中FILE_INFO_ST结构体pcFileUrl成员中如果包含有用户名和密码，则会影响端口的获取。
	更改措施: 
			1. 修改为正确的调用参数。
			   编译条件: PARSE_URL_ERR_ST

			2. 跳过pcFileUrl 成员中用户名密码的信息（如果有的话）。	
			   编译条件: IGNORE_AUTH_FIELD
********************************************************************/
#define	PARSE_URL_ERR_ST 1
#define	IGNORE_AUTH_FIELD 1

/* *******************************************************************
	修改时间: 2011.2.24
    原版本号: 0.0.0.44
    新版本号: 0.0.0.45
    by: lxy	
    注意: 本版本仅适用与T18 
	(该版本号以下面的版本出现重复，因为以前不同平台维护导致现在统一平台)
客户需求:  
	    (1) 添加t18 支持对302 消息的处理
更改措施:  
		(1) 现T2X 已经有对302消息的处理，由于T18 没有SSL ，所以
		需要关闭HTTPS 及相关联的库。需要关闭HTTP_DNS_TO_IP, 同时
		makefile  需要在V51的版本上去掉libcrypto.so 和libssl.so
		(2) T18的内存是4M ， T2X的内存是8M ，需要作相应的修改。
	    
编译条件:		HTTP_T18_ADD_302(仅对T18开启)
******************************************************************* */
#define HTTP_T18_ADD_302      0


/* *******************************************************************
    修改时间: 2011.3.1
    原版本号: 0.0.0.44
    新版本号: 0.0.0.45
    by: Jacky(wcl)

   问题描述:
        (1) 上传证书，并把"Only Accept Trusted Certificates" 项设置为Disable，autoprovision成功。
		 (2) 上传证书，并把" Only Accept Trusted Certificates" 项 设置为Enable，autoprovision不成功。
     更改措施:
        (1) 目前验证证书的合法性如下: 比较证书的颁发对象CN 是否和
	url 的host部分相同。由于客户在做测试用的证书时，可能CN 与host 不
	一样导致证书认证失败。现去除CN 与host的比较。

    编译条件: HTTP_SSL_CHECK_CN
******************************************************************* */
#define HTTP_SSL_CHECK_CN  1 

/********************************************************************
修改时间: 2011.3.11
原版本号: 2.0.0.45
新版本号: 2.0.0.46
by: lxy

注意:此功能适用于V60 版本
更改原因: 
	(1)  添加对HTTP 301的支持，301 是永久定向，302 是临时的
重定向，现对301 与302  不作区别。
更改方法:
	(1)  HTTP 301 与302 相同处理。 	
编译条件: HTTP_301
********************************************************************/
#define	HTTP_301		1

/********************************************************************
修改时间: 2011.3.11
原版本号: 2.0.0.46
新版本号: 2.0.0.47
by: lxy

注意:此功能适用于V60 版本
更改原因: 
	(1)  之前的V60 版本中对URL的处理是，在autop 中读取配置中的信息，
将其组装为URL://username:password@hostname:port/path 。http 下载库对收到的url 再
进行解析，取出username, password ,url = hostname:port/path ；最后发送的http 请求是
url，但在取username,password 时采用的方法是: 读取到第一个@，然后以: 为界，
前面是username, 后面是password 。当遇到username或是password 中包含有@时，就
会出现错误。
更改方法:
	(1)  http从URL中取出最后一个@， 假如存在@，说明该URL 包含有用户名与
密码，那么再以:为界，取出用户名与密码。
	
	http://tiptel:123@update.yealink.de/auto/,                      user = tiptel;         password = 123 
	http://tiptel@123:test@update.yealink.de/auto/               user = tiptel@123; password = test
	http://tiptel@123:test@456@update.yealink.de/auto/        user = tiptel@123; password = test@456
	http://tiptel@123:test@456@10.2.2.21:280/                     user = tiptel@123; password = test@456

编译条件: HTTP_URL_RESOLVE
********************************************************************/
#define	HTTP_URL_RESOLVE		1

/********************************************************************
修改时间: 2011.5.29
原版本号: 2.0.0.49
新版本号: 3.0.0.0
by: pjb

注意:	此功能适用于VP2009使用版本
更改原因：
		将VP2009和T3x，T2x等下载库进行统一合并，方便维护。
更改方法：
1、统一平台代码，兼容平台接口。
2、合并http和https，共用代码，方便维护。
3、修改http中发送和响应数据包的处理逻辑。
4、修改Makefile，方便平台编译管理.
5、增加http的测试文件，方便自测。

注：当前版本只移植了http和https,且T2x的一些接口还未全部处理。

编译条件: 无
********************************************************************/


/********************************************************************
修改时间: 2011.5.30
原版本号: 3.0.0.0
新版本号: 3.0.0.1
by: pjb

注意:	
更改原因：
		增加对client DER证书格式的支持。
更改方法：
		通过读取证书文件来判断是DER还是PAM格式的证书，进而做相应处理。
编译条件: M_CA_FILE_TYPE(未发布)
********************************************************************/
#define M_CA_FILE_TYPE				0

/********************************************************************
修改时间: 2011.6.8
原版本号: 3.0.0.1
新版本号: 3.0.0.2
by: pjb

注意: 本版本仅适用T2x V61中性版，请勿编译到其他版本
1、更改原因：
		tftp在IPV6下下载文件失败
问题分析：
		当前tftp在IPV4和IPV6地址拷贝时出现出错
更改方法：
		修复该问题
编译条件: 
		M_TFTP_IPV6_DOWNLOAD
2、更改原因：
		增加T2x的相关接口
问题分析：
		T2x V61使用统一后的下载库代码download_lib
更改方法：
		按需求更改
编译条件
		无
3、更改原因：
		客户的服务器需要使用终端发送的请求包中http version为HTTP/1.0
	问题分析：
		当前话机默认都是使用HTTP/1.1的字段
	解决方法：
		增加T2x配置
		/yealink/config/Setting/Setting.cfg
		[ http ]
		http_version = 1.1(默认为1.1，给tiptel KPN客户为 1.0)
********************************************************************/
#define M_TFTP_IPV6_DOWNLOAD		1


/********************************************************************
修改时间: 2011.6.8
原版本号: 3.0.0.2
新版本号: 3.0.0.3
by: pjb

注意: 本版本仅适用T2x V61中性版，请勿编译到其他版本
更改原因：
		GUI自测发现使用http_recv_head_2接口无法下载成功
		测试部测试发现useragent和区别振铃配置文件路径错误
问题分析：
		代码bug
更改方法：
		修复该问题
编译条件: 
		无
********************************************************************/

/********************************************************************
修改时间: 2011.8.1
原版本号: 3.0.0.3
新版本号: 3.0.0.4
by: lxy

注意: 本版本仅适用T2x V61中性版，请勿编译到其他版本
更改原因：
	    (1) 之前的版本对useragent的支持是:
	        UseragentDynm = Yealink SIP-T28P $HW_VER $FW_VER $MAC custom_value
	        现Tiptel 要求支持	"$MAC_NO_COLON"的支持。
	        "$MAC"格式是:          00:15:65:c0:11:00
	        "$MAC_NO_COLON"格式:    001565c01100
更改方法：
		按客户的要求修改。
编译条件: 
		M_HTTP_USER_AGENT_MAC_NO_COLON
********************************************************************/
#define M_HTTP_USER_AGENT_MAC_NO_COLON		1









//tiptel客户定制，中性版关闭
#define M_TIPTEL_HTTP10				0

/*V60以后的版本才需要打开HTTP_ENABLE_IPV6的宏*/
#if M_VIDEOPHONE
#define    HTTP_ENABLE_IPV6 0
#else
#define    HTTP_ENABLE_IPV6 1
#endif


#define HTTP_LIB  "3.0.0.3"





/*
注：平台选择通过make -D来实现，如编译vp使用make vp即可。
#define		M_VIDEOPHONE			1
#define		M_T2X					0
#define		M_T3X					0
#define		M_T6X					0
*/

/*配置文件路径*/
#ifdef WIN32
#define CA_PATH				"Y:\\work\\http_t3x_vp\\certs"
#define CA_SERVER_DIR		CA_PATH "\\server"
#define CA_SERVER_FILE		CA_SERVER_DIR "\\WebServer.pem"
#define SYSTEM_INI			"Y:\\work\\http_t3x_vp\\config\\system.ini"
#define FACTORY_SYSTEM_INI	SYSTEM_INI
#define HTTP_SECTION		"http"
#define HTTP_VERSION		"http_version"
#else
#if M_VIDEOPHONE
#define CA_PATH				"/mini-boot/www/certs/"
#define CA_SERVER_DIR		CA_PATH "server/"
#define CA_SERVER_FILE		CA_SERVER_DIR "WebServer.pem"
#define SYSTEM_INI			"/rundata/config/system.ini"
#define FACTORY_SYSTEM_INI	"/phone/factory/config/system.ini"
#define VERSION_INI			"/phone/resource/version.ini"
#define NETCFGFILE			"/phone/config/system.ini"
#define NETWORK_SECTION		"Network"
#define NETWORK_WAN_TYPE	"eWANType"
#define NETWORK_WAN_IP		"strWanIP"
#define HTTP_SECTION		"HttpServer"
#define HTTP_VERSION		"strHttpVersion"

#elif M_T2X
#define CA_PATH				"/yealink/config/certs/"
#define CA_SERVER_DIR		CA_PATH "server/"
#define CA_SERVER_FILE		CA_SERVER_DIR "WebServer.pem"
#define SYSTEM_INI			"/yealink/config/Setting/Setting.cfg"
#define VERSION_INI			"/yealink/version"
#define USER_INI			"/yealink/config/user.ini"
#define NETCFGFILE			"/yealink/config/Network/Network.cfg"
#define NETWORK_SECTION		"WAN"
#define NETWORK_WAN_TYPE	"WANType"
#define NETWORK_WAN_IP		"WANStaticIP"
#define FACTORY_SYSTEM_INI	"/yealink/factory/Setting/Setting.cfg"
#define HTTP_SECTION		"http"
#define HTTP_VERSION		"http_version"

#elif (M_T3X || M_T6X)
#define CA_PATH				"/phone/config/certs/"
#define CA_SERVER_DIR		CA_PATH "server/"
#define CA_SERVER_FILE		CA_SERVER_DIR "WebServer.pem"
#define SYSTEM_INI			"/phone/config/system.ini"
#define FACTORY_SYSTEM_INI	"/phone/factory/system.ini"
#define FACTORY_USER_INI	"/phone/factory/user.ini"
#define VERSION_INI			"/phone/version"
#define USER_INI			"/phone/config/user.ini"
#define NETCFGFILE			"/phone/config/system.ini"
#define NETWORK_SECTION		"Network"
#define NETWORK_WAN_TYPE	"eWANType"
#define NETWORK_WAN_IP		"strWanIP"
#define HTTP_SECTION		"HttpServer"
#define HTTP_VERSION		"strHttpVersion"
#endif
#endif




