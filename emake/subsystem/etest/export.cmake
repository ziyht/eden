# =====================================================================================
#
#       Filename:  EBuildExports.cmake
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
# =====================================================================================

#
# 位置拓扑:
#
# -- [MAIN_PROJECT_ROOTDIR]
# |
# |- src
#    |- libs
#       |- lib1
#          |- CMakeLists.txt            <- #1
#          |- test1
#          |  |- CMakeLists.txt         <- #2
#          |- testing
#             |- test2
#                |- CMakeLists.txt      <- #3
#
#
#

#
#\! -- 添加测试工程              <#1>
#
#  - Dest     测试项目在当前项目中的位置, 暂不支持外部位置
#  - [ON|OFF] 开关, 默认值为 ON
#
#
ETestAdd(<Dest> [ON|OFF])

#
#\! -- 添加测试                 <#2 #3>
#
#
ETestAddTest(<TestCaseName>     # 测试的名称, 必须设置
    TYPE     [Auto|Unit|Module]    # 测试的类型, 默认值为 Auto

    CASES    <*.c|*.cpp,...>       # 测试的源码文件(不要包含路径)
    SRCS     [*.c|*.cpp,...]       # 额外的源码文件
    DEPENDS  []                    # 依赖项, 若包含 Qt 库, 将会自动启用 AUTO_MOC

    [ON|OFF]                       # 开关, 默认值为 ON
)
