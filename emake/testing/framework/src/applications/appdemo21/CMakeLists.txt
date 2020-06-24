# =====================================================================================
#
#       Filename:  CMakeLists.txt
#
#    Description:  a temeplete file of CMakeLists.txt used by apps in emake
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
# \! emake 应用内部构建逻辑
# ==================================================
if(NOT DEFINED EMAKE_SETUP_INCLUDED)
    include(../../../cmake/cmakeConfig/SetupConfig.cmake)
endif()

EBuildExecAppM(1 0 0 0)

# ==================================================
# \! emake 应用属性设置
# ==================================================
set(KIT_EXPORT              )           # 导出开关  未设置则使用默认值, ON|OFF（默认）
set(KIT_EXPORT_DIR          )           # 导出目录  未设置则使用 ${COMMONS_DIR}/libs/${KIT_NAME}
set(KIT_EXPECT_DIR          )           # 期望位置  相对于被依赖目标输出位置的相对路径，默认为空

# - 源文件 -
file(GLOB _SRCS_ ${KIT_DIR}/*.h ${KIT_DIR}/*.cpp ${KIT_DIR}/*.c)
set(KIT_SRCS
    ${_SRCS_}
    )

# - 源文件: 入口 -
set(KIT_SRCS_MAIN
    )

# moc 文件，目前弃用
set(KIT_MOC_SRCS
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

# - 后缀 -
#   设置 DEBUG RELEASE RELWITHDEBINFO MINSIZEREL 模式下的后缀名
#       设置方式1： d INHERIT rd ""                              # 4项或少于4项，分别设置上述4种模式下的后缀
#       设置方式2： DEBUG d [RELEASE "" RELWITHDEBINFO rd ... ]
#    使用 INHERIT 进行占位，表示继承系统设置，无内容字串（"", " "...）表示设置为空
#    两种方式不能混合使用，默认值均为 INHERIT
set(KIT_POSTFIX)

# - 附加文件 -
EBuildAddFilesF(
#       DESTINATION_PREFIX  <目标位置前缀>                        // 相对路径，目标位置的前缀，相对本kit输出目录，若不设置，使用 '/'
#       FILES <filename1> ... [POSTFIX <目标位置后缀>]            // 设定文件列表及列表文件想要拷贝到的位置，可以有多行
#       DIRS  <dir_path1> ... [POSTFIX <目标位置后缀>]            // 设定目录列表及列表目录想要拷贝到的位置，可以有多行

    FILES appdemo21_1.txt
          appdemo21_2.txt

    DIRS  appdemo21_dir1
          appdemo21_dir2
)

# - 目标指派 -
EBuildAssignTargetDestF(
#    TARGETS <target1> [...] < DEST <path> [SKIP_COPY] [TEST_DEST <path>] [TEST_SKIP_COPY] >     # 设定相关目标的存放位置或忽略拷贝，只能设置直接依赖的目标，可以有多行
    TARGETS libdemo3 DEST libs
    TARGETS plgdemo3 DEST plugins
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
#   支持设置的平台：
#       WIN32 APPLE LINUX REDHAT REDHAT6 REDHAT7 CENTOS CENTOS6 CENTOS7 UBUNTU UBUNTU14
#
#   示例：
#       Qt5::Core|LINUX      将只在 LINUX 平台上依赖 Qt5::Core
#       Qt5::Core|WIN32      将只在 WIN32 平台上依赖 Qt5::Core
#
set(KIT_DEPENDS Qt5::Core Qt5::Gui libdemo3 plgdemo3)

# ==================================================
# \! emake 外部库项目其它操作项配置
# ==================================================

# - 添加测试 -
# ETestAdd([NAME] [ON|OFF] [DEST Path] [CREATE])
#
