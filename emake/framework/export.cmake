# =====================================================================================
#
#       Filename:  export.cmake
#
#    Description:  a instruction of some globel vars and APIs that can used by user
#                  directly
#
#        Version:  1.0
#        Created:  2018-08-24 23:40:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
return()
_(
# =====================================================================================

# =====================================
# cmake properties
#
# see: https://cmake.org/cmake/help/v2.8.12/cmake.html
#

# =====================================
# emake API
_()
EMakeParseArguments(prefix options ones multis)         # 参数检查，类似 CMakeParseArguments
EMakeCheckExistsF(path)                                 # 检查路径是否存在，不存在直接退出 path: 路径或路径变量
EMakeCheckDefinedF(var_name [PRE_DOC])                  # 检查变量是否定义，没定义直接退出

# =====================================
# emake properties1
#
#  重要： 请不要在 cmake 中设置这些值
#
PROPERTY                    NOTE
--------                    ----
EMAKE_FRAMEWORK_VERSION     EMAKE 版本


# =====================================
# emake properties2
#
#  重要： 请不要在 cmake 中设置这些值
#

PROPERTY     VALUE                            NOTE
----------   ------------------------------   ----
             windows       |  linux
             x86     x64   |  x86     x64
----------   -----------------------------    ----
BUILD_ARCH   win32   win64    x86     x64     ...


# =====================================
# definitions in build env

DEFINITIONS                 PLATFORM
-----------                 ----------------
_E_X86                      win32|linux_32
_E_X64                      win64|linux_64
_CRT_SECURE_NO_WARNINGS     win32|win64

MAIN_PROJECT_ROOT_DIR       all             # 当前主项目目录（包含 cmake/emake 框架的目录）, 已弃用
MAIN_PROJECT_DIR            all             # 当前 project 目录（顶级 CMakeLists.txt 所在目录）


)
