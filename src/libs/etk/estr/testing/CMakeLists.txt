cmake_minimum_required(VERSION 2.8.1)


#
#\! -- 添加测试
#
#  此 API 在位置 #2 #3 中使用
#
# ETestAddTest(<TestCaseName>     # 测试的名称, 必须设置
#     TYPE     [Auto|Unit|Module]    # 测试的类型, 默认值为 Auto
#
#     CASES    <*.c|*.cpp,...>       # 测试的源码文件(不要包含路径)
#     SRCS     [*.c|*.cpp,...]       # 额外的源码文件
#     DEPENDS  []                    # 依赖项, 若包含 Qt 库, 将会自动启用 AUTO_MOC
#
#     [ON|OFF]                       # 开关, 默认值为 ON
# )

ETestAddTestSet(estr
    CASES  test_estr.c
           test_sstr.c
           test_split.c
           test_perf.c
    )

ETestAddTestSet(cmpQString OFF

    CASES   subs_cmp.cpp

    DEPENDS Qt5::Core

    )

ETestAddTestSet(
    TYPE    Benchmark

    CASES   test_performance.c
    )
