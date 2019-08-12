# =====================================================================================
#
#       Filename:  EMakeFramework.cmake
#
#    Description:  to include emake framework modules
#
#        Version:  1.2
#        Created:  2018-09-15 00:50:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

set(EMAKE_FRAMEWORK_VERSION "1.2.5")

get_filename_component(EMAKE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../ REALPATH)

#! 加载系统模块
include(CMakeParseArguments)

#! 加载内部模块
set(_ ${CMAKE_CURRENT_LIST_DIR})

include(${_}/EMakePropertyM.cmake)          # 内部属性
include(${_}/EMakeLogF.cmake)               # 日志打印助手
include(${_}/EMakeUtilsF.cmake)             # 一些帮助小工具

include(${_}/EMakeParseArguments.cmake)     # 参数解析

include(${_}/EMakeSetupBuildEnvM.cmake)     # 设置基本的编译环境

include(${_}/EMakeSubSystemM.cmake)         # 子系统

include(${_}/EMakeProjectM.cmake)           # 设置项目或工程的版本信息
include(${_}/EMakeSetBuildTypeM.cmake)      # 设置项目编译类型，Debug、Release、...

include(${_}/EMakeSetupConfigM.cmake)       # 初始化配置

unset(_)

# 2019-06-22 13:28 ---------------------------------------
# emake 1.2.3:
#    1. 添加 Component 支持，仅 libs
#    2. 添加快速 debug 入口
#    3. 添加输入文件查找及去除指定文件，并支持指定平台

# 2018-09-15 13:28 ---------------------------------------
# emake 1.2.0:
#    1. 分离 setup.cmake, 提取 import.cmake
#    2. 移除 testing/framework 中测试用例中 setupManifist.cmake

# 2018-08-25 23:26 ---------------------------------------
# emake 1.1.1(3):
#    1. 更新 EBuild, 移除使用 setupManifist.cmake

# --------------------------------------------------------
# emake 1.1.1(2):
#    1. 重构外部库构建

# --------------------------------------------------------
# emake 1.1.1(1):
#    1. 添加 后缀设置适配
#    2. 添加 创建时的模板文件自动拷贝

# --------------------------------------------------------
# emake 1.1.1(0):
#    1. 为自动拷贝添加独有的 API 进行设置
#    2. 添加测试和构建的自动拷贝机制
#    3. 为测试系统添加额外的选项控制拷贝机制，解决测试和编译以及产品导出的拷贝冲突
#      (目前编译拷贝和产品导出采用同一套拷贝机制，自定义文件暂时不支持)
#    4. 在 概要信息中 添加 trace 输出，可以追溯各文件和库的最终由来

# --------------------------------------------------------
# emake 1.1.0(1):
#    修复 版本中有产品名时生成错误的输出路径

# --------------------------------------------------------
# emake 1.1.0(0)：
#    1. 添加自己 参数解析函数 可以支持解空字串 ""
#    2. 添加产品版本定义
#	    包括 4 中类型的文件 runtimes archives files dirs
#	    runtimes 来自依赖中的动态库
#	    archives 来自依赖中的静态库 （目前暂不拷贝）
#	    files    来自自定义的附加文件  或依赖目标中的附加文件
#	    dirs     来自自定义的附加文件夹或依赖目标中的附加文件夹
#    3. 目标都可以定义自己的附加文件或文件夹（目前安装拷贝时使用）
#    4. 目标可以指定直接依赖的目标的存放位置
#    5. 更新外部库导出 API，设置项发生变化
#	  NAME 用以指定导出目标的文件基本名，由此名称生成最后的库名
#	  TYPE 用以指定导出目标的库类型，STATIC SHARED MODULE
#	  LINK_DIRS_<config> 指定不同构建模式下的链接目录
#	  POSTFIX_<config>   指定不同构建模式下的链接目标后缀
#	  LOCATION           指定文件位置，若不设置，将使用 LINK_DIRS 中的第一项
#	  EXPECT_DIR         相对于依赖本目标的目标输出位置的相对输出位置

