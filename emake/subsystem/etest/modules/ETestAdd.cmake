# =====================================================================================
#
#       Filename:  ETestAdd.cmake
#
#    Description:  add test to the project
#
#        Version:  1.0
#        Created:  2018.08.28 21:29:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

#
#\! -- 添加测试
#
# ETestAdd([Name]  [ON|OFF]     # 测试项目的名称 / 开关, 默认值为 ON
#     DEST      [path]          #
#     [CREATE]
# )
#
# 注:
#   1. Name 和 DEST 至少设置一个, 若 DEST 未设置, 则认为设置的 Name 为 DEST, Name 自动清空
#   2. 优先检查当前工程目录路径下的文件
#
function(ETestAdd i_name)

    CONFIGURING_BUILD_CHECK()

#    if(NOT PROJECT_NAME)
#        EMakeErrF("you must set a project name before adding a test")
#    endif()

#    if(KIT_NAME)
#        if(NOT "${PROJECT_NAME}" STREQUAL "${KIT_NAME}")
#            EMakeWrnF("current kit_name '${KIT_NAME}' is no match to the current project name '${PROJECT_NAME}'")
#        endif()
#    endif()

    set(_args ON OFF CREATE DEST)

    # 设置 M_NAME
    set(M_NAME ${i_name})
    list(FIND _args "${M_NAME}" _o_id)
    if(_o_id GREATER -1)
        set(_argn "${M_NAME};${ARGN}")
        set(M_NAME)         # 用户没有设置, 清空
    else()
        set(_argn "${ARGN}")
    endif()

    EMakeParseArguments(M "ON;OFF;CREATE" "DEST" "" "${_argn}")
    if(DEFINED M_UNPARSED_ARGUMENTS)
        EMakeErrF("ETestAdd(): invalid arguments checked: '[${M_UNPARSED_ARGUMENTS}]'")
    endif()

    if(NOT M_DEST)
        if(NOT M_NAME)
            EMakeErrF("ETestAdd(): invalid arguments checked: please set at least a test name or path var 'DEST'")
        else()
            set(M_DEST ${M_NAME})
            set(M_NAME)
        endif()
    endif()

    # 检查 路径
    ETestEngineCheckPath(${M_DEST})

    # 检查 开关
    EMakeParseArguments(M "ON;OFF" "" "" "${ARGN}")
    if(NOT M_ON AND NOT M_OFF)
        set(M_ON 1)
    endif()

    if(M_NAME)
        set(M_NAME -${M_NAME})
    endif()

    if(M_ON)
        EMakeSetTargetPropertyM(${M_FULL} PROJECT_NAME VAR ${KIT_NAME} DEFAULT)

        add_subdirectory(${M_DEST})
    endif()

endfunction()


#
#\! -- 添加测试集
#
# ETestAddTestSet([Name] [ON|OFF]    # 测试集的名称 / 开关, 默认值为 ON
#     TYPE     [Auto|Unit|Module]    # 测试的类型, 默认值为 Auto
#
#     CASES    <*.c|*.cpp,...>       # 测试用例文件名, 注意: 不要包含额外路径
#     SRCS     [*.c|*.cpp,...]       # 额外的源码文件
#     DEPENDS  []                    # 依赖项, 若包含 Qt 库, 将会自动启用 AUTO_MOC
#
# )
#
macro(ETestAddTestSet i_name)

    set(options        ON OFF)
    set(oneValueArgs   TYPE DEST)
    set(multiValueArgs CASES SRCS DEPENDS)
    set(allChecks      ${options} ${oneValueArgs} ${multiValueArgs})

    EMakeGetTargetPropertyM(${CMAKE_CURRENT_LIST_DIR} PROJECT_NAME M_PROJ_NAME NO_CHECK)
    if(NOT M_PROJ_NAME)
        EMakeErrF("please using 'ETestAdd()' to add tests ")
    endif()

    # 设置 M_SET <- i_name
    set(M_SET ${i_name})
    list(FIND allChecks "${M_SET}" _o_id)
    if(_o_id GREATER -1)
        #EMakeErrF("Please set a test name when adding a test for project '${M_PROJ_NAME}'")
        set(_argn "${M_SET};${ARGN}")
        set(M_SET)
    else()
        set(_argn "${ARGN}")
    endif()
    if(M_SET)
        set(M_SET_SRC ${M_SET})
        set(M_SET     -${M_SET})
    else()
        set(M_SET_SRC "(NO_SET_NAME_SET)")
    endif()

    EMakeParseArguments(M "${options}" "${oneValueArgs}" "${multiValueArgs}" "${_argn}")

    if(DEFINED M_UNPARSED_ARGUMENTS)
        EMakeErrF("ETestAddTest(): invalid arguments checked: '[${M_UNPARSED_ARGUMENTS}]'")
    endif()

    # 检查 type
    set(_type Auto)
    if(NOT M_TYPE)
    elseif(NOT ${M_TYPE} MATCHES "^Unit|Module|Auto|Benchmark|Tutorial$")
        EMakeErrF("ETestAddTestSet(): invalid type of '${M_TYPE}', you can only set 'Auto', 'Unit', 'Module', 'Benchmark' or 'Tutorial' of TYPE when adding a test")
    else()
        set(_type ${M_TYPE})
    endif()
    set(M_TYPE ${_type})

    if(M_TYPE MATCHES "^Benchmark|Tutorial$")
        set(M_TARGET ${M_PROJ_NAME}${M_NAME}-${M_TYPE}${M_SET})
    else()
        set(M_TARGET ${M_PROJ_NAME}${M_NAME}-${M_TYPE}Test${M_SET})
    endif()
    string(REPLACE "/"  "_" M_TARGET ${M_TARGET})
    string(REPLACE "::" "_" M_TARGET ${M_TARGET})

    # 检查 name 是否已被使用
    if(TARGET ${M_TARGET})
        EMakeGetTargetPropertyM(${M_TARGET} ADD_FILE _file)
        EMakeErrF("'${i_test}' is been used already for '${M_TARGET}' which is added in ${_file}")
    else()
        EMakeSetTargetPropertyM(${M_TARGET} ADD_FILE VAR ${CMAKE_CURRENT_LIST_FILE})
    endif()

    if(NOT M_ON AND NOT M_OFF)
        set(M_ON 1)
    endif()

    # 警告没有 case 的情况
    if(NOT M_CASES)
        EMakeWrnF("you not set any cases when adding a test in name of '${M_TARGET}' , this test will be skipped")
    else()
        ETestEngineAddTest(${M_TARGET})
    endif()

endmacro()
