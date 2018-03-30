# GUI

[![build status](http://appgit.yealink.com:10080/sip-phone/gui/badges/develop/build.svg)](http://appgit.yealink.com:10080/sip-phone/gui/commits/develop)
[![coverage report](http://appgit.yealink.com:10080/sip-phone/gui/badges/develop/coverage.svg)](http://appgit.yealink.com:10080/sip-phone/gui/commits/develop)

该项目是SIP Phone话机端运行的图形用户界面（GUI），实现话机端的界面交互与应用逻辑。

## 子模块列表及说明

* 基础库`baselibs`
    - 路径：`src/baselibs`
    - 项目地址：[baselibs-dist](http://appgit.yealink.com:10080/sip-phone/baselibs-dist)
    - 对应分支：`develop`
    - 说明：用于基础功能的实现，包括通用模板、线程池等。子模块中包含头文件和库文件。
* CMake包含库`cmakeinclude`
    - 路径：`cmakeinclude`
    - 项目地址：[cmakeinclude](http://appgit.yealink.com:10080/sip-phone/cmakeinclude)
    - 对应分支：`master`
    - 说明：用于CMake指定各机型对应的交叉编译链及编译选项。
* 单元测试框架`googletest`
    - 路径：`src/3rdlib/googletest`
    - 项目地址：[Google Test](https://github.com/google/googletest)
    - 说明：用于单元测试，仅包含官方源码文件，编译单元测试程序时同步编译链接。

* 配置ID定义`configiddefine`
    - 路径：`src/include/configiddefine`
    - 项目地址：[configiddefine](http://appgit.yealink.com:10080/sip-phone/configiddefine)
    - 说明：统一定义话机配置ID。
* 共用定义头文件`commondefine`
    - 路径：`src/include/commondefine`
    - 项目地址：[commondefine](http://appgit.yealink.com:10080/sip-phone/commondefine)
    - 说明：公共头文件定义，主要用于避免不同项目的对外接口或消息定义冲突。

## 常用CMake参数说明

* 机型`host`
    - 必选，会影响编译平台和机型功能宏。
* 平台`PLATFORM`
    - 可选，强制修改机型对应的编译平台，影响交叉编译工具链。
    - 可选值包括`arm`/`armv6jel`/`armdspg`/`arago`/`x86`/`win32`/`win64`。
* 编译目标安装到项目根目录开关`INSTALL_TO_ROOT_DIR`
    - 可选，开启后指定编译目标安装到项目根目录的`dist_${host}`文件夹中。
* 单元测试开关`ENABLE_TEST`
    - 可选，开启后可以编译单元测试程序。
* 单元测试覆盖率开关`ENABLE_COVER_TEST`
    - 可选，开启后可以针对单元测试程序收集覆盖率信息。

## 生成VS解决方案

该项目目前已经支持在Windows系统通过CMake生成VS解决方案。

需要提前安装`CMake`和`VS`，然后在项目根目录运行如下命令：

```CommandLine
REM 创建并进入编译目录，避免对源码造成干扰
mkdir build
cd build
REM 指定机型为T23并生成VS解决方案
cmake .. -Dhost=t23
```

## MSVC平台参数
```CommandLine
# 可以手动指定VS版本，如"Visual Studio 8 2005"、 "Visual Studio 10 2010" 、"Visual Studio 14 2015"等
-DCMAKE_GENERATOR="Visual Studio 14 2015" 
# win32平台相关的实现均放在win32子目录下，对于非活动模块通过添加fake模块替代以便于IDE编辑和显示，
# 如果需要关闭非活动代码在IDE中的显示，使用参数ENABLE_IDE_MODE，该选项默认开启
-DENABLE_IDE_MODE=OFF
```

生成的VS解决方案位于`build\dskphone.sln`，双击打开即可。

## 编译说明

需要准备配置交叉编译环境的Linux系统，在项目根目录运行如下命令：

```bash
# 创建并进入编译目录，避免对源码造成干扰
mkdir build && cd build
# 指定编译机型为T23
cmake .. -Dhost=t23
# 使用make命令编译，其中`-j4`指定编译并发数为4，建议与CPU逻辑核心数一致
make -j4 install
```

编译结果位于`build/output`目录中。

## 单元测试说明

该项目已配置好单元测试。

```bash
# 创建并进入编译目录，避免对源码造成干扰
mkdir build.test && cd build.test
# 指定编译机型为T23，编译平台为x86，启用单元测试与单元测试覆盖率统计功能
cmake .. -Dhost=t23 -DPLATFORM=x86 -DENABLE_TEST=1 -DENABLE_COVER_TEST=1
# 使用make命令编译单元测试程序并运行测试，其中`-j4`指定编译并发数为4，建议与CPU逻辑核心数一致
make -j4 check
```

运行单元测试结果如：

```
Test project /home/user/gui/build.test/src/dskphone/logic/lamp/powersaving
    Start 1: powersaving_TEST
1/1 Test #1: powersaving_TEST .................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.01 sec
```

收集单元测试覆盖率：

```bash
# 收集单元测试覆盖率信息
lcov -c -d src/dskphone/ -o ../coverage.info
# 移除单元测试覆盖率的干扰信息
lcov -r ../coverage.info '/usr/*' '*/googletest/*' '*/test/*' '*/baselibs/*' -o ../coverage.info
```

单元测试覆盖率输出结果如：

```
Summary coverage rate:
  lines......: 35.3% (123 of 348 lines)
  functions..: 43.9% (18 of 41 functions)
  branches...: no data found
```

生成HTML格式单元测试覆盖率报告：

```bash
genhtml -o report ../coverage.info
```

打开`build.test/report/index.html`即可查看报告全文。

## 持续集成说明

该项目配置了两类持续集成任务，具体脚本可以参考[.gitlab-ci.yml](.gitlab-ci.yml)。

* 编译：编译机型包括T23, T42S, T46S, T48S, T54。
* 单元测试并收集单元测试覆盖率。
* 收集编译结果文件大小。
* 添加 `tag` 即可自动提交打包系统，
    - 添加 `x.82.251.1` 或 `82.251.1`：编译、提交全部机型，
    - 添加 `t23.82.251.1`：仅编译、提交指定机型，机型列表参见[gitlab-ci.yml](.gitlab-ci.yml)指定的job
    - 添加 `82.251.1(-DCMAKE_PARM1=sth1,-DCMAKE_PARM2=sth2)`：附加额外的自定义cmake参数。git 的[tag规范](https://git-scm.com/docs/git-check-ref-format)限制了字符类型，对于此项功能，cmake参数之间的空格使用 `,` 代替
