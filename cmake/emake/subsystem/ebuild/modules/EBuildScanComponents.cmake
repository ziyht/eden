# =====================================================================================
#
#       Filename:  EBuildScanComponents.cmake
#
#    Description:  add sub kit tartget to a top target
#
#        Version:  1.0
#        Created:  2019.06.21 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

function(_CheckSeparateKit dir _ret)

    set(ret 0)

    if(EXISTS ${dir}/CMakeLists.txt)

        file(READ ${dir}/CMakeLists.txt line LIMIT 20)

        if("${line}" MATCHES "separate_kit|separate kit")
            set(ret 1)
        endif()

    endif()

    set(${_ret} ${ret} PARENT_SCOPE)

endfunction()

function(_ScanSeperateDirs dir _dirs)

    set(dirs)

    file(GLOB _list "${dir}/*")

    foreach(_path ${_list})

        _CheckSeparateKit(${_path} is_separate)

        if(is_separate)

            file(RELATIVE_PATH _name ${dir} ${_path}  )

            list(APPEND dirs ${_name})

        endif()

    endforeach()

    set(${_dirs} ${dirs} PARENT_SCOPE)

endfunction()

function(_AddSeparateKit TOP_KIT s_kit s_kit_dir)


    set(KIT_NAME ${s_kit})
    set(KIT_DIR  ${s_kit_dir})

    CONFIGURING_SETUP_SET(1)
    include(${s_kit_dir}/CMakeLists.txt)
    CONFIGURING_SETUP_SET(0)

    EMakeParseInputFiles("${KIT_SRCS}"      KIT_SRCS       ${s_kit_dir}/CMakeLists.txt)
    EMakeParseInputFiles("${KIT_UI_FORMS}"  KIT_UI_FORMS   ${s_kit_dir}/CMakeLists.txt)
    EMakeParseInputFiles("${KIT_RESOURCES}" KIT_RESOURCES  ${s_kit_dir}/CMakeLists.txt)

    # -- 生成目标，并为目标创建相关属性

    EBuildCheckDependsM(${TOP_KIT}::${s_kit} "${KIT_DEPENDS}")

    EMakeSetTargetPropertyM(${TOP_KIT}::${s_kit} KIT_TYPE     VAR         Component)
    EMakeSetTargetPropertyM(${TOP_KIT}::${s_kit} HAVE         VAR         1)
    EMakeSetTargetPropertyM(${TOP_KIT}::${s_kit} TYPE         VAR         Component)
    EMakeSetTargetPropertyM(${TOP_KIT}::${s_kit} OWNER        VAR         ${TOP_KIT})
    EMakeSetTargetPropertyM(${TOP_KIT}::${s_kit} BELONG_TO    VAR         ${TOP_KIT})
    EMakeSetTargetPropertyM(${TOP_KIT}::${s_kit} NAME         VAR         ${s_kit})

    EMakeSetTargetPropertyM(${TOP_KIT} KIT_SRCS            LIST_VAR ${KIT_SRCS}      APPEND_NO_DUP)
    EMakeSetTargetPropertyM(${TOP_KIT} KIT_MOC_SRCS        LIST_VAR ${KIT_MOC_SRCS}  APPEND_NO_DUP)
    EMakeSetTargetPropertyM(${TOP_KIT} KIT_UI_FORMS        LIST_VAR ${KIT_UI_FORMS}  APPEND_NO_DUP)
    EMakeSetTargetPropertyM(${TOP_KIT} KIT_RESOURCES       LIST_VAR ${KIT_RESOURCES} APPEND_NO_DUP)
    EMakeSetTargetPropertyM(${TOP_KIT} KIT_DEPENDS         LIST_VAR ${KIT_DEPENDS}   APPEND_NO_DUP)
    EMakeSetTargetPropertyM(${TOP_KIT} KIT_INCLUDE_DIRS    LIST_VAR ${s_kit_dir}     APPEND_NO_DUP)
    EMakeSetTargetPropertyM(${TOP_KIT} KIT_LINK_DIRS       LIST_VAR ${KIT_LINK_DIRS} APPEND_NO_DUP)

    EMakeSetTargetPropertyM(${TOP_KIT} COMPONENTS  LIST_VAR ${TOP_KIT}::${s_kit} APPEND_NO_DUP)

    EMakeSetGlobalPropertyM(KNOWN_TARGETS        LIST_VAR ${TOP_KIT}::${s_kit} APPEND)

endfunction()

function(_ScanSeparateKit TOP_KIT KIT_DIR)

    _ScanSeperateDirs(${KIT_DIR} s_kits  )

    if(NOT s_kits)
        return()
    endif()

    foreach(s_kit ${s_kits})

        _AddSeparateKit(${TOP_KIT} ${s_kit} ${KIT_DIR}/${s_kit})

    endforeach()

endfunction()
