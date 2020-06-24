# =====================================================================================
#
#       Filename:  EMakeSetVersion.cmake
#
#    Description:  an easier macro to set version
#
#        Version:  1.0
#        Created:  15/11/2017 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================


#
# 本宏用以简便地设置项目（也可以是其它）版本信息，并添加到全局的宏定义中：
#
#   若传入 (PROJ 1 2 3)
#   那么将生成：
#       PROJ_MAJOR_VERSION 1
#       PROJ_MINOR_VERSION 2
#       PROJ_PATCH_VERSION 3
#       PROJ_VERSION "1.2.3"
#
#   若传入 (PROJ 1 2 3 4)
#   那么将生成：
#       PROJ_MAJOR_VERSION 1
#       PROJ_MINOR_VERSION 2
#       PROJ_PATCH_VERSION 3
#       PROJ_BUILD_VERSION 4
#       PROJ_VERSION "1.2.3(4)"
#
macro(EMakeProjectM proj major minor patch)

    string(FIND ${proj} "-" _find)
    if(_find GREATER -1 )
        EMakeErrF("you can not hava a '-' in a project name when set version base on it, because '-' will break the macro in cmdline")
    endif()

    string(TOUPPER ${proj} _PROJ)

    if(${proj}_VERSION)
        EMakeErrF("'${proj}_VERSION' has already been setted to '${${proj}_VERSION}', you must have run 'EMakeProjectM()' with pass in the same 'PRROJECT_NAME' more than once, we forbiden this type of using in EMake")
    endif()

    project(${proj})

    if(${_PROJ} STREQUAL "CMAKE")
        EMakeErrF("invalid name '${proj}', you can not set a name just matches 'cmake', please set another name")
    endif()

    if(${ARGN})
        set(${_PROJ}_BUILD_VERSION ${ARGN})
        set(${_PROJ}_VERSION "${${_PROJ}_VERSION}(${ARGN})")
        add_definitions(-D${_PROJ}_MAJOR_VERSION=${major}
                        -D${_PROJ}_MINOR_VERSION=${minor}
                        -D${_PROJ}_PATCH_VERSION=${patch}
                        -D${_PROJ}_BUILD_VERSION=${ARGN}
                        -D${_PROJ}_VERSION="${${_PROJ}_VERSION}"
            )
    else()
        add_definitions(-D${_PROJ}_MAJOR_VERSION=${major}
                        -D${_PROJ}_MINOR_VERSION=${minor}
                        -D${_PROJ}_PATCH_VERSION=${patch}
                        -D${_PROJ}_VERSION="${major}.${minor}.${patch}"
            )
    endif()

    set(${proj}_MAJOR_VERSION ${major})
    set(${proj}_MINOR_VERSION ${minor})
    set(${proj}_PATCH_VERSION ${patch})
    set(${proj}_VERSION "${major}.${minor}.${patch}(${ARGN})")

    set(${_PROJ}_MAJOR_VERSION ${major})
    set(${_PROJ}_MINOR_VERSION ${minor})
    set(${_PROJ}_PATCH_VERSION ${patch})
    set(${_PROJ}_VERSION "${major}.${minor}.${patch}(${ARGN})")

    set(PROJECT_MAJOR_VERSION ${major})
    set(PROJECT_MINOR_VERSION ${minor})
    set(PROJECT_PATCH_VERSION ${patch})
    set(PROJECT_BUILD_VERSION ${ARGN})
    set(PROJECT_VERSION       "${major}.${minor}.${patch}(${ARGN})")

    EMakeInfF("Project   : ${proj} @ ${PROJECT_VERSION}")

    set(_PROJ)

endmacro()
