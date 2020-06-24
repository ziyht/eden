# =====================================================================================
#
#       Filename:  CMakeLists.txt
#
#    Description:  a temeplete file of CMakeLists.txt used by product in emake
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
# \! emake 产品内部构建逻辑
# ==================================================
if(NOT DEFINED EMAKE_SETUP_INCLUDED)
    include(../../../cmake/cmakeConfig/SetupConfig.cmake)
endif()

# ==================================================
# \! emake 产品属性设置
# ==================================================

# - 依赖 -
#
#   格式：
#       [产品名:]目标名[|平台1|平台2|...]
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
set(KIT_DEPENDS)
