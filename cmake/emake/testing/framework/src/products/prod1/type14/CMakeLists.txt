# =====================================================================================
#
#       Filename:  CMakeLists.txt
#
#    Description:  a temeplete file of CMakeLists.txt used by type in emake
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
# \! emake 版本内部构建逻辑
# ==================================================
if(NOT DEFINED EMAKE_SETUP_INCLUDED)
    include(../../../cmake/cmakeConfig/SetupConfig.cmake)
endif()

EBuildExecTypeM(1 0 0 0)

# ==================================================
# \! emake 版本属性设置
# ==================================================

# - 附加文件 -
EBuildAddFilesF(
#    DESTINATION_PREFIX  <目标位置前缀>                        // 相对路径，目标位置的前缀，相对本kit输出目录，若不设置，使用 '/'
#    FILES <filename1> ... [POSTFIX <目标位置后缀>]            // 设定文件列表及列表文件想要拷贝到的位置，可以有多行
#    DIRS  <dir_path1> ... [POSTFIX <目标位置后缀>]            // 设定目录列表及列表目录想要拷贝到的位置，可以有多行
)

# - 目标指派 -
EBuildAssignTargetDestF(
#    TARGETS <target1> [...] < DEST <path> | SKIP_COPY >     # 设定相关目标的存放位置或忽略拷贝，只能设置直接依赖的目标，可以有多行
)

# - 依赖 -
#
#   格式：S
#       目标名[|平台1|平台2|...]
#
#   支持平台：
#       WIN32 APPLE LINUX REDHAT REDHAT6 REDHAT7 CENTOS CENTOS6 CENTOS7 UBUNTU UBUNTU14
#
#   示例：
#       lib1|LINUX      将只在 LINUX 平台上依赖 lib1
#       plg1|WIN32      将只在 WIN32 平台上依赖 plg1
#
set(KIT_DEPENDS appdemo19)
