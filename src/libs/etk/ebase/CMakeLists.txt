#![separate_kit]
# =====================================================================================
#
#       Filename:  CMakeLists.txt
#
#    Description:  a temeplete file of CMakeLists.txt used by libs in emake
#
#        Version:  1.0
#        Created:  2018-08-23 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================
cmake_minimum_required(VERSION 2.8.1)

# ==================================================
# \! emake 内部库属性设置
#
#   在这里可以使用的框架内部变量（部分），注意不要重新设置这些变量：
#       MAIN_PROJECT_ROOT_DIR    主项目根目录
#       PROJECT_ROOT_DIR         本项目根目录
#       KIT_DIR                  当前子项目目录，即当前文件所在目录
#       KIT_NAME                 当前子项目名称
# ==================================================


# - 源文件 -
set(KIT_SRCS
    [*]${KIT_DIR}/*.h
    [*]${KIT_DIR}/*.cpp
    [*]${KIT_DIR}/*.c
    [*]${KIT_DIR}/libs/*.[h|c]
    )

# - UI文件 -
set(KIT_UI_FORMS
)

# - 资源文件 -
set(KIT_RESOURCES
)

# - 额外 头文件目录 -
set(KIT_INCLUDE_DIRS
)

# - 额外 链接目录 -
set(KIT_LINK_DIRS
)


# - 附加文件 -
EBuildAddFilesF(
#       DESTINATION_PREFIX  <目标位置前缀>                        // 相对路径，目标位置的前缀，相对本kit输出目录，若不设置，使用 '/'
#       FILES <filename1> ... [POSTFIX <目标位置后缀>]            // 设定文件列表及列表文件想要拷贝到的位置，可以有多行
#       DIRS  <dir_path1> ... [POSTFIX <目标位置后缀>]            // 设定目录列表及列表目录想要拷贝到的位置，可以有多行
)

# - 依赖 -
#
#   格式：
#       [产品名:]目标名[|平台1|平台2|...]
#       -l库名 -L库目录
#
#   QT库(Qt5::[...])：
#       Core Gui Xml XmlPatterns Concurrent Sql Test Widgets OpenGL UiTools
#
#   支持平台：
#       WIN32 APPLE LINUX REDHAT REDHAT6 REDHAT7 CENTOS CENTOS6 CENTOS7 UBUNTU UBUNTU14
#
#   示例：
#       Qt5::Core|LINUX      将只在 LINUX 平台上依赖 Qt5::Core
#       Qt5::Core|WIN32      将只在 WIN32 平台上依赖 Qt5::Core
#
set(KIT_DEPENDS
    -lpthread|LINUX
    pthreads-win32::static|WIN32
    )


# - 测试 -
ETestAdd(testing/estr   ON)
ETestAdd(testing/evar   ON)
ETestAdd(testing/evec   ON)
ETestAdd(testing/eatom  ON)

ETestAdd(tutorial/estr  ON)
