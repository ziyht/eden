# =====================================================================================
#
#       Filename:  EBuildDependsM.cmake
#
#    Description:  to check depends for kit or target
#
#        Version:  1.0
#        Created:  2018.09.02 00:24:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================


EMakeSetTargetPropertyM(pthread PACKAGE VAR Threads::Threads)

function(_ParseDependencyF i_dep o_kit o_target o_platforms)

    set(_kit)
    set(_target)
    set(_platforms)

    set(_dep ${i_dep})

    # 分离 product
    if(NOT ${i_dep} MATCHES "^(Q|q)(T|t)(4|5)::")
        string(REGEX REPLACE ":"  ";" _dep ${_dep})
        set(_dep ${_dep})
        list(LENGTH _dep _cnt)
        if(${_cnt} GREATER 2)
            EMakeErrF("the dependency '${i_dep}' set for '${KIT_NAME}' is in wrong format, the correct format is kit_name[:target_name][|platform1|...]")
        elseif(${_cnt} EQUAL 2)
            list(GET _dep 0 _kit)
            list(GET _dep 1 _dep)
        endif()
    endif()

    # 分离 platforms
    string(REGEX REPLACE "\\|" ";" _dep ${_dep})
    set(_dep ${_dep})
    list(LENGTH _dep _cnt)
    if(${_cnt} GREATER 1)
        list(GET       _dep 0 _target)
        list(REMOVE_AT _dep 0 )
        set(_platforms ${_dep})
    else()
        set(_target ${_dep})
    endif()

    if(_kit)
        set(_target ${_kit}::${_target})
    endif()

#    EMakeDbgF("_kit      : ${_kit}")
#    EMakeDbgF("_target   : ${_target}")
#    EMakeDbgF("_platforms: ${_platforms}")

    set(${o_kit}       ${_kit}       PARENT_SCOPE)
    set(${o_target}    ${_target}    PARENT_SCOPE)
    set(${o_platforms} ${_platforms} PARENT_SCOPE)
endfunction()

macro(_findPackage i_package)

    STRING(REGEX REPLACE "::" ";" _package ${i_package})

    emakeinff(-------1------ ${_package})

    list(LENGTH _package _len)

    if(_len EQUAL 1)
        find_package(${_package} QUIET)
    elseif(_len EQUAL 2)

        emakeinff(------2------- [${_package}])

        list(GET _package 0 __1)
        list(GET _package 0 __2)

        find_package(${__1} QUIET)

        if(NOT ${__1}_FOUND)
            emakeinff(------3------- [${_package}])
            set(${i_package}_FOUND)
        else()
            emakeinff(------4------- [${_package}])
            find_package(${i_package} QUIET)
        endif()

    else()
        EMakeErrf("err format of package name '${i_package}'")
    endif()

endmacro()

#
# \! 检查 depends
#   1. 分离 Qt 的依赖，并单独存储到目标属性 KIT_LIBRARIES_SYS 中
#
macro(EBuildCheckDependsM i_kit)

    set(_deps)
    set(_i_deps ${ARGN})
    set(_dirs )
    set(_libs )

    foreach(_dep ${_i_deps})

        # 错误格式检查
        if(${_dep} MATCHES "^[:\\||]")
            EMakeErrF("the dependency '${_dep}' set for kit '${KIT_NAME}' is in wrong format")
        endif()

        # 解析
        _ParseDependencyF(${_dep} _kit _target _platforms)

        # 检查平台依赖
        if(_platforms)

            set(_need 0)

            foreach(_platform ${_platforms})
                list(FIND PLATFORMS ${_platform} _o_index)

                if(${_o_index} EQUAL -1)
                    EMakeErrF("the platform '${_platform}' in dependency '${_dep}' set for kit '${KIT_NAME}' is not supported")
                endif()

                list(FIND CUR_PLATFORM ${_platform} _o_index)
                if(${_o_index} GREATER -1)
                    set(_need 1)
                endif()

            endforeach()
        else()
            set(_need 1)
        endif()

        if(_need)
            # 检查是否为路径
            if(${_target} MATCHES "^-L")
                string(REGEX REPLACE "^-L" "" _target "${_target}")
                list(APPEND _dirs ${_target})
                continue()
            endif()

            # 检查是否为 lib
            if(${_target} MATCHES "^-l")
                string(REGEX REPLACE "^-l" "" _target "${_target}")
                list(APPEND _libs ${_target})
                continue()
            endif()

            list(APPEND _deps ${_target})
        endif()

        # 检查依赖限定 -- todo

    endforeach()

    EMakeListRemoveRegexsItemM(_deps "^(Q|q)(T|t)(4|5)::" EXTRACT _deps_qt)
    EBuildSetupQtM(${_deps_qt})
    if(PACKAGES_NOT_FOUND)
        EMakeErrF("the kit '${i_kit}' depends on not found QT packages '[${PACKAGES_NOT_FOUND}]', please check you 'CMakeLists.txt' file")
    endif()

    # 查找指定的 lib
    foreach(_lib ${_libs})

        find_library(LIB_${_lib} NAMES ${_lib} PATHS ${_dirs})

        if(${LIB_${_lib}} STREQUAL LIB_${_lib}-NOTFOUND)
            EMakeErrF("the lib '-l${_lib}' depended by KIT '${i_kit}' can not be found")
        endif()

    endforeach()

#    set(_deps2)
#    foreach(_dep ${_deps})

#        EMakeGetTargetPropertyM(${_dep} KIT_TYPE type NO_CHECK)

#        if("${type}" STREQUAL "Component")

#            EMakeGetTargetPropertyM(${_dep} BELONG_TO top_kit)
#            list(APPEND _deps2 ${top_kit})

#        else()
#            list(APPEND _deps2 ${_dep})
#        endif()

#    endforeach()

    set(KIT_DEPENDS         ${_deps})
    set(KIT_LIBRARIES_SYS   ${_deps_qt} ${_libs})
    set(KIT_LINK_DIRS       ${_dirs} ${KIT_LINK_DIRS})

    set(KIT_IS_QT_PROJECT)
    if(_deps_qt)
        set(KIT_IS_QT_PROJECT 1)
    endif()

endmacro()
