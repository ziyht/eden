# =====================================================================================
#
#       Filename:  ETestEngine.cmake
#
#    Description:  engine for the etest
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

enable_testing()

set(ETEST_SYSTEM_DIR   ${SUB_SYSTEM_DIR})
set(ETEST_MODULE_DIR   ${SUB_SYSTEM_DIR}/cmake)
set(ETEST_TEMPLATE_DIR ${SUB_SYSTEM_DIR}/template)

include(${ETEST_TEMPLATE_DIR}/CMakeLists.txt)

macro(ETestEngineInitM)
    if(NOT TARGET etest)
        add_custom_target(etest)
        #set_target_properties(etest PROPERTIES UnitTestOption ON ModuleTestOption ON)

    endif()

    if(${M_TYPE} MATCHES "^Benchmark$")
        if(NOT TARGET benchmark)
            add_custom_target(benchmark)
        endif()
    endif()

    if(${M_TYPE} MATCHES "^Tutorial$")
        if(NOT TARGET tutorial)
            add_custom_target(tutorial)
        endif()
    endif()

    enable_testing()
    add_definitions(-DETEST_OK=0 -DETEST_ERR=1)
endmacro()

#
# 将会生成如下变量:
#    M_DEST 疏理过的传入路径
#    M_FULL 疏理过的完整路径
#
macro(ETestEngineCheckPath i_dest)

    set(M_DEST ${i_dest})
    set(M_FULL ${CMAKE_CURRENT_LIST_DIR}/${M_DEST})

    if(EXISTS ${M_FULL})
        EMakeValidatePath(${M_DEST} M_DEST)
        EMakeValidatePath(${M_FULL} M_FULL)
    elseif(EXISTS ${M_DEST})
        EMakeValidatePath(${M_DEST} M_DEST)
        set(M_FULL ${M_DEST})
    elseif(M_CREATE)
        file(MAKE_DIRECTORY ${M_FULL})
    else()
        EMakeErrF("the DEST '${i_dest}' not exsit when adding a test for ${KIT_NAME}")
    endif()

    if(NOT EXISTS ${M_FULL}/CMakeLists.txt)
        if(M_CREATE)
            file(COPY ${ETEST_TEMPLATE_DIR}/CMakeLists.txt DESTINATION ${M_FULL})
        else()
            EMakeErrF("the file '${M_FULL}/CMakeLists.txt' not exist, please check you testing project files\n"
                      "NOTE: you can using 'CREATE' option in 'ETestAdd()' to create a template file ")
        endif()
    endif()

    set(M_DEST ${M_DEST} PARENT_SCOPE)
    set(M_FULL ${M_FULL} PARENT_SCOPE)

endmacro()

#
# 须预先设置以下值:
#   M_TEST      测试名称
#   M_TYPE      测试类型
#   M_CASES     测试用例(*.c|*.cpp)
#   M_SRCS      额外文件
#   M_DEPENDS   依赖
#   M_ON        开关
#
function(ETestEngineAddTest i_target)

    # 检查 开关
    if(M_ON)
        EMakeInfF("  |- [ON ] Test: ${i_target}" PREFIX "   ")
    else()
        EMakeInfF("  |- [OFF] Test: ${i_target}" PREFIX "   ")
        return()
    endif()

    set(_main_cxx ${i_target}_main.cxx)
    set(_head_h   ${i_target}_main.h)

    ETestEngineInitM()

    # 生成 ctest 主函数(main)文件
    create_test_sourcelist(_src_list ${_main_cxx} ${M_CASES} EXTRA_INCLUDE ${_head_h})

    # 自动创建 用例 文件（如果不存在）
    ETestGenCaseFiles(${CMAKE_CURRENT_LIST_DIR} M_CASES)

    # 添加测试可执行文件生成
    project(${i_target})
    add_executable(${i_target} ${_src_list} ${M_SRCS})

    # 检查和设置依赖
    EBuildCheckDependsM(${i_target} ${M_DEPENDS})
    target_link_libraries(${i_target} ${KIT_NAME_LIB} ${KIT_DEPENDS} ${KIT_LIBRARIES_SYS} ${KIT_LINK_DIRS})

    if(KIT_IS_QT_PROJECT)
        set(CMAKE_AUTOMOC ON)
    else()
        set(CMAKE_AUTOMOC OFF)
    endif()

    # 为 测试 添加自动测试步骤 并 筛选 gcc 的 case
    set(_add_to_cmake_test)
    if(${M_TYPE} MATCHES "^Unit|Module|Auto$")
        set(_add_to_cmake_test 1)
    endif()

    set(_gcc_cases)
    set(_id 1)
    foreach(_case ${M_CASES})

        set(_is_gcc)
        if(${_case} MATCHES "(.c|.cpp|.cxx)$")
            set(_is_gcc 1)
        endif()

        string(REGEX REPLACE "(.c|.cpp|.cxx)$" "" _case ${_case})

        if(_is_gcc)
            list(APPEND _gcc_cases ${_case})
        endif()

        if(_add_to_cmake_test)
            add_test(NAME "${i_target}:${_id}.${_case}" COMMAND ${i_target} ${_case})
        endif()

        math(EXPR _id "${_id} + 1")
    endforeach()

    # 为 gcc 的 case 创建头文件
    if(_gcc_cases)

        ETestGenHeadFile(${PROJECT_BINARY_DIR}/${_head_h} _gcc_cases)

    endif()

    set(M_TEST)

endfunction()
