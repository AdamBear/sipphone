�ӿ��ļ��ṩ��Ƕ��ʽʹ�ã�������ʷ�����г�ͻ�����⣬���
���service_dsk_interface.h�ļ���������Ķ��忽����service_dsk_interface_extension.h�ļ���
��������ⲿģ����벻������Ҫ������service_dsk_interface.h�Ķ�����롣

�ṩ�ⲿ�ӿ�ʱ�����ṩ��
service_dsk_interface_extension.h���ⲿʹ�õĶ���ȫ���ϵ����ļ���������Ϣ���ṹ�壩

2017/11/8
�»�����Ϣ�ι��ⲿʹ�ã�
#define DSK_MSG_APP		(DSK_MSG_SIZE*64)		// section of service:application section
#define DSK_MSG_TALK	(DSK_MSG_SIZE*68)		// section of service:talk section
#define DSK_MSG_CONTACT	(DSK_MSG_SIZE*72)		// section of service:contact section
#define DSK_MSG_MENU	(DSK_MSG_SIZE*76)		// section of service:menu section
// 76 - 120 ��չ��ҵ��
#define DSK_MSG_QUERY	(DSK_MSG_SIZE*120)		// section of service:query section
#define DSK_MSG_EXTENSION	(DSK_MSG_SIZE*128)	// section of extension
// 120 - 255 Ԥ��
#define DSK_MSG_MAX		(DSK_MSG_SIZE*255)		// max limit

date:2018/1/18
author: hwz
�޸�guiΪservice���򣬶����ṩlibdskservice.so�⣬���ӿ��ļ�service_dsk_interface_extension.h
1���ⲿͨ�����ýӿڶ�ȡgui��Ϣ
2���ⲿͨ��ע��notify������Ϣ

date��2018/3/12
author��hwz
1����ҳ��Ҫ���Žӿڵĳ�ʱʱ��
2��ͬ��vcs���⿪�ŵĲ���
3�����ڰ�׿Ҫ��c++�ӿ�ת����java�ӿڣ����service_config.h�ļ�����SETTING_TYPE���������setting_type_define.h

