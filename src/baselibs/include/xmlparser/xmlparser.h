#ifndef __XML_PARSER_H__
#define __XML_PARSER_H__

/*******************************************************************
*  create by hlx
*  2012-10-20 version 1.0
*******************************************************************/
#ifndef __cplusplus
#ifndef WIN32
#include <stdbool.h>
#endif

#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* 文件类型 */
typedef struct
{
    void * pBuffer; /* 记录pDoc地址，用于xml_doc_close中释放 */
} PG_XMLDocument;

#ifndef WIN32

typedef PG_XMLDocument XMLDocument;
#endif

/* 节点类型 */
typedef struct
{
    void * pNode; /* XMLNode相当于一个节点的handle */
} XMLNode;

/************************************************************************/
/* load & close                                                         */
/************************************************************************/
/* 从文件加载 注：必须和xml_doc_close配对使用 */
PG_XMLDocument * xml_load_file(char * szFile);
/* 从buffer中加载 注：必须和xml_doc_close配对使用 */
PG_XMLDocument * xml_load_buffer(char * szBuffer);
/* 创建新的doc（用于save） 注：必须和xml_doc_close配对使用 */
PG_XMLDocument * xml_create_empty_doc();
/* 释放doc占用的buffer 注：必须和xml_load_file或xml_load_buffer配对使用 */
void xml_doc_close(PG_XMLDocument * pDoc);

/* 获取node的name */
char * xml_node_name(XMLNode * pNode);


/************************************************************************/
/* access                                                               */
/************************************************************************/
/* doc是否为空*/
bool xml_doc_empty(PG_XMLDocument * pDoc);
/* 打印doc下的内容 */
void xml_doc_print(PG_XMLDocument * pDoc);
/* 打印node下的内容 */
void xml_node_print(XMLNode * pNode);
/* 根据名字获取doc下的child node */
XMLNode xml_doc_child_by_name(char * szName, PG_XMLDocument * pDoc);
/* 根据名字获取node下的child node */
XMLNode xml_node_child_by_name(char * szName, XMLNode * pNode);
/* 获取第一个children */
XMLNode xml_node_first_child(XMLNode * pNode);
/* 获取下一个sibling */
XMLNode xml_node_next_sibling(XMLNode * pNode);
/* node是否为空*/
bool xml_node_empty(XMLNode * pNode);
/* 获取属性值 */
char * xml_node_attribute(char * szName, XMLNode * pNode);
/* 获取node的text */
char * xml_node_text(XMLNode * pNode);

/************************************************************************/
/* modify                                                               */
/************************************************************************/
/* doc添加子节点 */
XMLNode xml_doc_append_child(char * szName, PG_XMLDocument * pDoc);
/* doc添加declaration */
XMLNode xml_doc_prepend_declaration(PG_XMLDocument * pDoc);
/* node添加子节点 */
XMLNode xml_node_append_child(char * szName, XMLNode * pNode);
/* node添加属性 */
void xml_node_append_attribute(char * szName, char * szValue, XMLNode * pNode);

/************************************************************************/
/* save                                                                 */
/************************************************************************/
/* 保存到xml文件中， save之后再调用xml_doc_close */
bool xml_doc_save_file(char * szFile, PG_XMLDocument * pDoc);

//// C interface
//int xml_set_log_level(int mode, int level);

typedef void * xml_doc_h;
typedef void * xml_node_h;
typedef void * xml_attr_h;
typedef void * pgxml_handle;

/************************************************************************/
/* load & close                                                         */
/************************************************************************/

/* 从文件加载 注：必须和xml_doc_close配对使用 */
xml_doc_h  pgxml_load_file(const char * file);
/* 从buffer中加载 注：必须和xml_doc_close配对使用 */
xml_doc_h  pgxml_load_buffer(const char * buf);
/* 创建新的doc（用于save） 注：必须和xml_doc_close配对使用 */
xml_doc_h  pgxml_create_empty_doc(void);
/* 释放doc占用的buffer 注：必须和xml_load_file或xml_load_buffer配对使用 */
void pgxml_doc_close(xml_doc_h doc);

/************************************************************************/
/* access                                                               */
/************************************************************************/

/* doc是否为空*/
bool    pgxml_doc_empty(xml_doc_h doc);
/* 打印doc下的内容 */
void    pgxml_doc_print(xml_doc_h doc);
/* 打印node下的内容 */
void    pgxml_node_print(xml_node_h node);

xml_node_h pgxml_doc_child_by_name(xml_doc_h doc, const char * name);
/* 根据名字获取node下的child node */
xml_node_h pgxml_node_child_by_name(xml_node_h node, const char * name);

/* 获取第一个children */
xml_node_h pgxml_node_first_child(xml_node_h node);
xml_node_h pgxml_node_last_child(xml_node_h node);

/* 获取上一个sibling */
xml_node_h pgxml_node_prev_sibling(xml_node_h node);
xml_node_h pgxml_node_prev_sibling_by_name(xml_node_h node, const char * name);

/* 获取下一个sibling */
xml_node_h pgxml_node_next_sibling(xml_node_h node);
xml_node_h pgxml_node_next_sibling_by_name(xml_node_h node, const char * name);

/* node是否为空*/
bool pgxml_node_empty(xml_node_h node);

const char * pgxml_node_name(xml_node_h node);
/* 获取属性值 */
const char * pgxml_node_attribute(xml_node_h node, const char * name);
// 获取节点的第一个/最后一个的属性
xml_attr_h pgxml_node_first_attr(xml_node_h node);
xml_attr_h pgxml_node_last_attr(xml_node_h node);
// 获取前后属性
xml_attr_h pgxml_attr_next(xml_attr_h attr);
xml_attr_h pgxml_attr_prev(xml_attr_h attr);
// 设置属性的名称和值
bool pgxml_attr_set_name(xml_attr_h attr, const char * szName);
bool pgxml_attr_set_value(xml_attr_h attr, const char * szValue);
// 判断当前属性是否为空
bool pgxml_attr_empty(xml_attr_h attr);
// 获取属性的名称和值
const char * pgxml_attr_get_name(xml_attr_h attr);
const char * pgxml_attr_get_value(xml_attr_h attr);

/* 获取node的text */
const char * pgxml_node_text(xml_node_h node);

/************************************************************************/
/* modify                                                               */
/************************************************************************/

/* doc添加子节点 */
xml_node_h pgxml_doc_append_child(xml_doc_h doc, const char * name);
/* doc添加declaration */
xml_node_h pgxml_doc_prepend_declaration(xml_doc_h doc);
/* node添加子节点 */
xml_node_h pgxml_node_append_child(xml_node_h node, const char * name);
/* node添加属性 */
void pgxml_node_append_attribute(xml_node_h node, const char * name, const char * value);
bool pgxml_node_change_attribute_value_by_name(xml_node_h node, const char * name,
        const char * value);


int pgxml_node_set_attribute(char * szName, const char * cszValue, xml_node_h pNode);

xml_node_h pgxml_node_parent(xml_node_h objElem);

/************************************************************************/
/* save                                                                 */
/************************************************************************/
/* 保存到xml文件中， save之后再调用xml_doc_close */
bool pgxml_doc_save_file(xml_doc_h doc, const char * file);
void pgxml_doc_save_buf(xml_doc_h doc, char * buf, unsigned int len);
bool pgxml_node_save_file(xml_node_h node, const char * file);
void pgxml_node_to_buf(xml_node_h node, char * buf, unsigned int buf_len);

//日志
typedef void(*pgxml_log_callback)(int module, int level, const char * fmt, ...);

//////////////////////////////////////////////////////////////////////////
/* eg.
  //依赖于log.c的方式
  void XmlParserLogCallBack(int module, int level, const char * fmt, ...)
 {
    va_list ap;

    va_start(ap, fmt);

    LOG_VLOG(module, level, fmt, ap);

    va_end(ap);

    return;
 }

 //直接使用printf
 static void etlTrace(int module, int level, const char * fmt, ...)
 {
    va_list ap;
    va_start(ap, fmt);
    int len = -1;
    len = vsnprintf(NULL, 0, fmt, ap);

    if (len > 0)
    {
        char * pBuffer = new char[len + 1];
        if (pBuffer)
        {
            memset(pBuffer, 0, len + 1);
            vsnprintf(pBuffer, len + 1, fmt, ap);
            printf("[xmlparser] %s\n", pBuffer);
            delete[] pBuffer;
        }
    }

    va_end(ap);

    return;
 }
*/

/**
 * @brief Pgxml init log.
 *
 * @author Song
 * @date 2018/3/13
 *
 * @param mode      The mode.
 * @param call_back The call back.
 *
 * @return An int.
 */

int pgxml_init_log(int mode, pgxml_log_callback call_back);

#ifdef __cplusplus
}
#endif

#endif // __XML_PARSER_H__
