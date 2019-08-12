# =====================================================================================
#
#       Filename:  CMakeLists.txt
#
#    Description:  a template file used by etest projects
#
#        Version:  1.0
#        Created:  2018.08.30 21:29:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

cmake_minimum_required(VERSION 2.8.1)

#
#\! -- 添加测试集
#
# ETestAddTestSet([Name] [ON|OFF]    # 测试集的名称 / 开关, 默认值为 ON
#     TYPE     [Auto|Unit|Module]    # 测试的类型, 默认值为 Auto
#
#     CASES    <*.c|*.cpp,...>       # 测试用例文件名, 注意: 不要包含额外路径
#     SRCS     [*.c|*.cpp,...]       # 额外的源码文件
#     DEPENDS  []                    # 依赖项, 若包含 Qt 库, 将会自动启用 AUTO_MOC
#                                    #  依赖的格式可以是 pthread|LINUX 的格式
#                                    #  表示只在 LINUX 下依赖 pthread
# )
#

ETestAddTestSet(esl
    TYPE    Unit

    CASES   test_basic.c
    )

ETestAddTestSet(esl
    TYPE    Benchmark
    CASES   esl_benchmark.c
    )

