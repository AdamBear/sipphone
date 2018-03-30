# baselibs-dist - 基础库发布版

**develop**
[![build status](http://192.168.6.185:10080/app-common/baselibs-dist/badges/develop/build.svg)](http://192.168.6.185:10080/app-common/baselibs-dist/commits/develop)

**master**
[![build status](http://192.168.6.185:10080/app-common/baselibs-dist/badges/master/build.svg)](http://192.168.6.185:10080/app-common/baselibs-dist/commits/master)

**V82**
[![build status](http://192.168.6.185:10080/app-common/baselibs-dist/badges/release/v82/build.svg)](http://192.168.6.185:10080/app-common/baselibs-dist/commits/release/v82)

**V81**
[![build status](http://192.168.6.185:10080/app-common/baselibs-dist/badges/release/v81/build.svg)](http://192.168.6.185:10080/app-common/baselibs-dist/commits/release/v82)

**V80**
[![build status](http://192.168.6.185:10080/app-common/baselibs-dist/badges/release/v80/build.svg)](http://192.168.6.185:10080/app-common/baselibs-dist/commits/release/v82)

**dect**
[![build status](http://192.168.6.185:10080/app-common/baselibs-dist/badges/release/dect/build.svg)](http://192.168.6.185:10080/app-common/baselibs-dist/commits/release/dect)

**CP920**
[![build status](http://192.168.6.185:10080/app-common/baselibs-dist/badges/release/cp920/build.svg)](http://192.168.6.185:10080/app-common/baselibs-dist/commits/release/cp920)


该项目用于存储基础库对外提供的头文件和动态库/静态库文件。

## 持续集成流程说明

本项目已配置两个持续集成脚本，包括：

1. 更新外部项目依赖当前项目（Git子模块）版本（scripts/create_MR.sh）；
2. 把当前项目（目前主要是头文件）推送到SVN指定路径（scripts/svn_push.sh）。

具体脚本参见scripts目录。

### 持续集成：Git子模块版本更新

Git子模块与SVN不同，更新Git项目时不会自动更新子模块版本。

针对这个问题，通过持续集成自动更新Git子模块版本并创建Merge Request。

要实现该流程，步骤如下：

1. 在本项目中配置`GIT_GUI_*`变量，其中`*`是**本项目中分支名转义后的名称**，如`GIT_GUI_release_v82`。
如果需要更新多个GUI项目的Git子模块，可以指定多个项目的参数，并用半角分号`;`隔开。每个项目的配置值分为三个部分，用半角逗号`,`隔开：
    1. 需要更新子模块版本的GUI项目分支，如`develop`。
    2. 需要更新子模块版本的GUI项目ID，可以在项目设置`Settings`->`General`->`Project ID`中找到。
    3. 需要更新子模块版本的GUI项目Git URL，如`ssh://git@192.168.6.185:10022/v82-gui/gui.git`。

2. 在目标GUI项目的`.gitmodules`文件中指定正确的分支。由于该流程通过执行`git submodule update --remote src/baselibs`命令更新子模块版本，因此会更新到`.gitmodules`文件中指定分支的最新版本。
3. 目标项目添加用户`bot`作为`Developer`，用于自动提交代码并创建Merge Request。

### 持续集成：SVN推送

目前还有部分项目使用SVN进行代码管理，因此基础库的头文件需要同步到SVN指定目录。

要实现该流程，需要在本项目中配置`SVN_*`变量，其中`*`是**本项目中分支名转义后的名称**，如`SVN_release_v81`。变量值为头文件对应的SVN路径，如`https://10.3.1.88/repos/baselibs/branches/v81-release/V80/include`。
