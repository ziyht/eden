# =====================================================================================
#
#       Filename:  EMakeSetBuildTypeM.cmake
#
#    Description:  get property
#
#        Version:  1.0
#        Created:  2017.12.18 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    EMakeInfF("Setting build type to 'Debug' as none was specified")

    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)

    # Set the possible values of build type for cmake-gui
    mark_as_advanced(CMAKE_BUILD_TYPE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

    set(CMAKE_BUILD_TYPE_DEFAULT_SETTED 1)
endif()

#
# 设置构建类型 Debug Release MinSizeRel RelWithDebInfo
#   需传入需要设置的类型
#
#   EMakeSetBuildTypeM(Debug|Release|MinSizeRel|RelWithDebInfo [FORCE])
#
#   1. 若 CMAKE_BUILD_TYPE 没有被设置，那么传入的 type 将被设置
#   2. 若 CMAKE_BUILD_TYPE 已被设置，但是是由本文件设置的默认值，那么传入的 type 将被设置
#   3. 无论 CMAKE_BUILD_TYPE 有没有设置，只要传入 FORCE，那么传入的 type 将被设置
#   4. 其它任意情况，本宏无效
#
macro(EMakeSetBuildTypeM i_type)

    cmake_parse_arguments(M "FORCE" "" "" ${ARGN})

    if(M_FORCE OR NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE_DEFAULT_SETTED)
        set(_need_set_build_type 1)
        set(CMAKE_BUILD_TYPE_DEFAULT_SETTED)
    else()
        set(_need_set_build_type)
    endif()

    if(_need_set_build_type)
        set(_type_ok)

        foreach(_type Debug Release MinSizeRel RelWithDebInfo)
            if("${i_type}" STREQUAL "${_type}")
                set(_type_ok 1)
            endif()
        endforeach()

        if(${_type_ok})

            if(${i_type} STREQUAL ${CMAKE_BUILD_TYPE})

                EMakeInfF("Build Type: ${CMAKE_BUILD_TYPE}")

            else()
                EMakeInfF("Build Type: ${i_type} <- ${CMAKE_BUILD_TYPE}")
                set(CMAKE_BUILD_TYPE ${i_type} CACHE STRING "Choose the type of build." FORCE)
                set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
            endif()
        else()
            EMakeErrF("[${i_type}] is invalid, please using [Debug|Release|MinSizeRel|RelWithDebInfo] to set build type")
        endif()

        set(_type_ok)
    else()
        EMakeInfF("Build Type: ${CMAKE_BUILD_TYPE}")
    endif()

endmacro()
