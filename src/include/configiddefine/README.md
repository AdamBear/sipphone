# configiddefine - 配置ID定义

该项目维护配置ID定义头文件`configiddefine.h`，并自动生成`configMapper.xml`。

新增配置时，一般先从代码开始添加，因此把配置存储路径、对应M7语句等信息以**Doxygen注释方式**添加在代码定义中，这样方便在调试代码时查阅，也方便通过Doxygen生成网页文档。

完整的配置ID定义格式举例如下（**仅用于格式举例，不代表内容正确**）：

```
/**
 * @brief SIP账号密码
 *
 * 加密存储SIP账号的密码信息
 *
 *         name | value
 * ------------:|:----------------
 *           m7 | account.%%d.password
 *       t49-m7 | account.%%d.info.password
 *         path | /config/user/voip/sipAccount%%d.cfg
 *      section | account
 *          key | password
 *      default | ""
 *  dbx-default | ******
 *  t49-default | ??????
 *      encrypt | True
 *          reg | {maxlength:99}
 */
#define kszAccountLabel "AccountLabel"
#define kszSipAccountLabel "AccountLabel"
```

* 格式说明：
    - 第1-18行是注释部分（Doxygen格式）。
        + 第2行是概要注释部分（必选），对配置ID实现的功能进行简述或命名。
        + 第4行是详细注释部分（可选），对配置ID进行详细说明。
        + 第6-17行是Markdown格式的配置信息部分（可选），如果缺少该部分则对应配置ID不会生成到`configMapper.xml`中。
            * 第6-7行是表头。
            * 第8行`m7`是配置项的M7语句，生成到dbx机型（包括T49）`configMapper.xml`的`M7`字段中。如果缺少该字段则对应配置ID不会生成到dbx机型`configMapper.xml`中。
            * 第9行`t49-m7`是配置项的（T49机型）M7语句，生成到T49`configMapper.xml`的`M7`字段中。如果缺少该字段则使用`m7`代替。
            * 第10行`path`是配置项对应的存储文件路径，生成到ini机型`configMapper.xml`的`filepath`字段中。如果缺少该字段则对应配置ID不会生成到ini机型`configMapper.xml`中。
            * 第11行`section`是配置项的存储文件区段，生成到ini机型`configMapper.xml`的`section`字段中。如果缺少该字段则对应配置ID不会生成到ini机型`configMapper.xml`中。
            * 第12行`key`是配置项的存储文件键名，生成到ini机型`configMapper.xml`的`key`字段中。如果缺少该字段则对应配置ID不会生成到ini机型`configMapper.xml`中。
            * 第13行`default`是配置项的默认值，生成到`configMapper.xml`的`defvalue`字段中。如果缺少该字段则对应配置ID不会生成到`configMapper.xml`中。
            * 第14行`dbx-default`是配置项的（dbx机型）默认值，生成到dbx机型（包括T49）`configMapper.xml`的`defvalue`字段中。如果缺少该字段则使用`default`代替。
            * 第15行`t49-default`是配置项的（T49机型）默认值，生成到T49机型`configMapper.xml`的`defvalue`字段中。如果缺少该字段则使用`dbx-default`或者`default`代替。
            * 第16行`encrypt`是配置项的加密标志，生成到ini机型`configMapper.xml`的`encrypt`字段中。如果缺少该字段则对应配置ID在生成`configMapper.xml`时不会带有`encrypt`字段。
            * 第17行`reg`是配置项的限制条件（Web使用），生成到T49机型`configMapper.xml`的`reg`字段中。如果缺少该字段则对应配置ID在生成`configMapper.xml`时不会带有`reg`字段。
    - 第19-20行是宏定义部分。

## 强制规范

为了保证代码统一规范，**强制**要求严格遵守如下规则：

* 使用UTF-8编码，LF换行（Unix格式），空格缩进。
* 行尾没有多余空格。
* 文件结尾包含一个空行。
* 注意Doxygen注释缩进规则：第2-18行行首有一个空格。
* 由于Doxygen会对注释中的`%`进行转义，因此出现在**注释中**的`%`都应该写成`%%`。此规则主要影响通过Doxygen生成的文档，通过`PyConfigUtils`生成`configMapper.xml`时兼容两种写法，不受影响。
* 由于Markdown表格格式限制，单元格不能为空，因此空值应该写为`""`。

## 建议规范

为了保证代码可读性，**建议**遵守如下规则：

* 在整个文件中，按照配置ID升序排列，同样的配置ID仅保留一份Doxygen注释，宏定义写在一起。
* 配置ID之间保留一个空行，注释中各部分（概要注释、详细注释、Markdown表格）之间保留一个空行。
* 在Markdown表格内容中（第8-17行），按照`m7`, `t49-m7`, `path`, `section`, `key`, `default`, `dbx-default`, `t49-default`, `encrypt`, `reg`顺序排列。
* 在同一个配置ID的宏定义中（第19-20行），按照宏定义升序排列。

## 问题排查

由于头文件实际通过正则表达式解析，因此无法提示准确的格式错误提示，只能提示错误发生在哪一行以后。

每次提交的改动一般不会很大，因此重点分析提交改动的部分，特别是错误提示行数之后的那一个配置项定义。

常见错误包括：

* 注释除了第一行以外，后续的`*`前面需要有一个空格。
* 行尾不要带有多余空格。
* 每个配置项定义之间有且只有一个空行。

其他错误可以参照`强制规范`进行分析。

**建议使用现代编辑器进行编辑！**现代编辑器如：`Notepad++`、`Sublime Text`、`Visual Studio Code`等。编辑时建议打开“显示所有字符”功能，避免错误地添加了空白字符。
