# =====================================================================================
#
#       Filename:  EMakeSetupBuildEnvM.cmake
#
#    Description:  setup build environment
#
#        Version:  1.0
#        Created:  16/11/2017 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

macro(_SetupBuildArch)

    if( CMAKE_SIZEOF_VOID_P EQUAL 8 )

        set(ARCH 64)

        if(WIN32)
            set(BUILD_ARCH "win64")
        else()
            set(BUILD_ARCH "x64")
        endif()

        add_definitions(-D_E_X64)

    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)

        set(ARCH 86)

        if(WIN32)
            set(BUILD_ARCH "win32")
        else()
            set(BUILD_ARCH "x86")
        endif()

        add_definitions(-D_E_X86)

    else()

        EMakeErrF("the build architecture is not 64 or 32, please check you system env, this should not happen")

    endif()

    if(WIN32)

        string(TOLOWER ${CMAKE_SYSTEM_NAME} _sys_name)
        set(INSTALL_POSTFIX ${_sys_name}${CMAKE_SYSTEM_VERSION}_x${ARCH})

    else()

        set(_checkos_sh ${EMAKE_DIR}/framework/sbin/checkos.sh)

        set(_os)

        execute_process(
            COMMAND "bash" ${_checkos_sh}
            RESULT_VARIABLE     RESULT_VAR
            OUTPUT_VARIABLE     _os
            ERROR_VARIABLE      _error
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if(RESULT_VAR)
            EMakeErrF("Failed to obtain dependence path of ${_kit}.\n${RESULT_VAR}\n${PROJECT_BINARY_DIR}\n${_error}")
        endif()

        if(_os)
            set(INSTALL_POSTFIX ${_os}_x${ARCH})
        else()
            string(TOLOWER ${CMAKE_SYSTEM_NAME} _sys_name)
            set(INSTALL_POSTFIX ${_sys_name}${CMAKE_SYSTEM_VERSION}_x${ARCH})
        endif()

    endif()

endmacro()


macro(_SetupBuildFlags)


    if(    ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")

        set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS}          ${CMAKE_C_FLAGS_DEBUG}")
        set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        ${CMAKE_CXX_FLAGS_DEBUG}")

    elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")

        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

        set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS}          ${CMAKE_C_FLAGS_RELEASE}")
        set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        ${CMAKE_CXX_FLAGS_RELEASE}")

    elseif(${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")

        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}")

        set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS}          ${CMAKE_C_FLAGS_MINSIZEREL}")
        set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        ${CMAKE_CXX_FLAGS_MINSIZEREL}")

    elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")

        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")

        set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS}          ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
        set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

    else()

        EMakeErrF("unknown build type '${CMAKE_BUILD_TYPE}' checked, not supported, quit")

    endif()


    # C++ flags
    include(CheckCXXCompilerFlag)

    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

    if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    else()
         EMakeInfF("Warning: The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif()

    # compat static QtCore
    if(g_QtCore_lib_type STREQUAL "STATIC")
        set(CompilerFlags
                CMAKE_CXX_FLAGS
                CMAKE_CXX_FLAGS_DEBUG
                CMAKE_CXX_FLAGS_RELEASE
                CMAKE_C_FLAGS
                CMAKE_C_FLAGS_DEBUG
                CMAKE_C_FLAGS_RELEASE
                )
        foreach(CompilerFlag ${CompilerFlags})
            string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
        endforeach()
    endif()

endmacro()

macro(_SetupBuildPlatform)

    # -- 支持平台
    set(PLATFORMS WIN32 APPLE LINUX) # REDHAT REDHAT6 REDHAT7 CENTOS CENTOS6 CENTOS7 UBUNTU UBUNTU14

    # -- 当前平台
    if(WIN32)
        set(CUR_PLATFORM WIN32)
    elseif(APPLE)
        set(CUR_PLATFORM APPLE)
    else()
        set(CUR_PLATFORM LINUX)
    endif()

endmacro()

macro(EMakeSetupBuildEnvM)

    _SetupBuildArch()
    _SetupBuildFlags()
    _SetupBuildPlatform()

    EMakeGetProjectDirF(MAIN_PROJECT_ROOT_DIR MAIN_PROJECT_DIR)

    add_definitions(-DMAIN_PROJECT_ROOT_DIR="${MAIN_PROJECT_ROOT_DIR}/")
    add_definitions(-DMAIN_PROJECT_DIR="${MAIN_PROJECT_DIR}/")

    if(MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif()

endmacro()
