# =====================================================================================
#
#       Filename:  EBuildExtAddExportM.cmake
#
#    Description:
#
#        Version:  1.0
#        Created:  2018.01.11 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

macro(_AutoGenArchiveName i_arch o_arch)

    if(NOT ${i_arch} MATCHES ".[so|a|dll|lib]^")

        if(WIN32)
            set(${o_arch} ${i_arch}.lib)
        else()
            set(${o_arch} lib${i_arch}.a)
        endif()
    else()
        set(${o_arch} ${i_arch})
    endif()

endmacro()

macro(_AutoGenLibraryName i_lib o_run o_link)

    if(NOT ${i_lib} MATCHES ".[so|a|dll|lib]^")

        if(WIN32)
            set(${o_run}  ${i_lib}.dll)
            set(${o_link} ${i_lib}.lib)
        else()
            set(${o_run}  lib${i_lib}.so)
            set(${o_link} lib${i_lib}.so)
        endif()

    else()

        if(WIN32)
            set(${o_run}  ${i_lib})
            set(${o_link} )
        else()
            set(${o_run}  ${i_lib})
            set(${o_link} ${i_lib})
        endif()

    endif()

endmacro()

macro(_AutoGenRuntimeName i_run o_run)

    if(NOT ${i_run} MATCHES ".[so|a|dll|lib]^")

        if(WIN32)
            set(${o_run} ${i_lib}.dll)
        else()
            set(${o_run} lib${i_lib}.so)
        endif()
    else()
        set(${o_run} ${i_lib})
    endif()

endmacro()

macro(_ExtAddExportMakeDir i_dir1 i_dir2s o_dirs)

    set(${o_dirs})

    foreach(_dir ${i_dir2s})

        if(${_dir} MATCHES "^/|^[A-Za-z]::")
            list(APPEND ${o_dirs} ${i_dir2})
        else()
            list(APPEND ${o_dirs} ${i_dir1}/${i_dir2})
        endif()

    endforeach()

endmacro()

macro(_ExtAddExportCheckAndGetDirs)

    if(M_PREFIX)

        if(M_INCLUDE_DIRS)
            _ExtAddExportMakeDir("${M_PREFIX}" "${M_INCLUDE_DIRS}" _include_dirs)
        else()
            set(_include_dirs ${M_PREFIX})
        endif()

        if(M_ARCHIVE_DIRS)
            _ExtAddExportMakeDir("${M_PREFIX}" "${M_ARCHIVE_DIRS}" _archive_dirs)
        else()
            set(_archive_dirs ${M_PREFIX})
        endif()

        if(M_LIBRARY_DIRS)
            _ExtAddExportMakeDir("${M_PREFIX}" "${M_LIBRARY_DIRS}" _library_dirs)
        else()
            set(_library_dirs ${M_PREFIX})
        endif()

        if(M_RUNTIME_DIRS)
            _ExtAddExportMakeDir("${M_PREFIX}" "${M_RUNTIME_DIRS}" _runtime_dirs)
        else()
            set(_runtime_dirs ${M_PREFIX})
        endif()
    endif()

    if(M_ARCHIVES)

        list(LENGTH M_ARCHIVES    _archive_len)
        list(LENGTH _archive_dirs _archive_dir_len)

        if(NOT _archive_dir_len EQUAL 1 AND NOT _archive_dir_len EQUAL _archive_len)
            EMakeErrF("the len(${_archive_dir_len}) of ARCHIVE_DIRS is not equal to the len(${_archive_len}) of ARCHIVES")
        endif()

    else()
        set(_archive_dirs)
    endif()

    if(M_LIBRARIES)

        list(LENGTH M_LIBRARIES   _library_len)
        list(LENGTH _archive_dirs _library_dir_len)

        if(NOT _library_dir_len EQUAL 1 AND NOT _library_dir_len EQUAL _library_len)
            EMakeErrF("the len(${_library_dir_len}) of LIBRARY_DIRS is not equal to the len(${_library_len}) of LIBRARIES")
        endif()

    else()
        set(_library_dirs)
    endif()

    if(M_RUNTIMES)

        list(LENGTH M_RUNTIMES    _runtime_len)
        list(LENGTH _runtime_dirs _runtime_dir_len)

        if(NOT _runtime_dir_len EQUAL 1 AND NOT _runtime_dir_len EQUAL _runtime_len)
            EMakeErrF("the len(${_runtime_dir_len}) of RUNTIME_DIRS is not equal to the len(${_runtime_len}) of RUNTIMES")
        endif()

    else()
        set(_runtime_dirs)
    endif()

    set(_link_dirs ${_archive_dirs} ${_library_dirs})

endmacro()

macro(_ExtAddLibs i_lib)

    cmake_parse_arguments(M "DEFAULT" "TYPE" "DIR" ${ARGN})

    if(NOT M_TYPE)
        EMakeErrF("internal err: must set a TYPE when add a lib")
    endif()

    if(NOT M_DIR)
        EMakeErrF("internal err: must set a DIR when add a lib")
    endif()

    list(FIND _${_KIT_NAME}_LIBS ${i_lib} _o_index)

    # 这个库还没有被添加
    if(_o_index EQUAL -1)

        if(NOT ${M_TYPE} MATCHES "^ARCHIVE$|^LIBRARY$|^RUNTIME$")
            EMakeErrF("internal err: unknown TYPE[${M_TYPE}] for lib '${i_lib}'")
        endif()

        # 设置此lib值
        list(APPEND _${_KIT_NAME}_LIBS ${i_lib})
        set(_${_KIT_NAME}_LIB_${i_lib}_DIR  ${M_DIR})
        set(_${_KIT_NAME}_LIB_${i_lib}_TYPE ${M_TYPE})

        EMakeSetTargetPropertyM(${_KIT_NAME}_${i_lib} TYPE VAR ${M_TYPE})
        EMakeSetTargetPropertyM(${_KIT_NAME}_${i_lib} DIR  VAR ${M_DIR} )

    else()

        # 检查 TYPE
        if(NOT ${_${_KIT_NAME}_LIB_${i_lib}_TYPE} STREQUAL ${M_TYPE})
            EMakeErrF("the TYPE[${M_TYPE}] set for lib '${i_lib}' now is not matches the TYPE[${_${_KIT_NAME}_LIB_${i_lib}_TYPE}] you setted before")
        endif()

        if(NOT M_DEFAULT AND NOT ${_${_KIT_NAME}_LIB_${i_lib}_DIR} STREQUAL ${M_DIR})
            EMakeErrF("the DIR[${M_DIR}] set for lib '${i_lib}' now is not matches the DIR[${_${_KIT_NAME}_LIB_${i_lib}_DIR}] you setted before")
        endif()

    endif()

endmacro()

#
##
##  使用前需先定义 _KIT_NAME
##
#macro(EBuildExtAddExport i_name )

#    set(_options   AUTO_GEN)
#    set(_one_args  PREFIX)
#    set(_list_args ARCHIVES;LIBRARIES;RUNTIMES;INCLUDE_DIRS;LINK_DIRS;ARCHIVE_DIRS;LIBRARY_DIRS;RUNTIME_DIRS)
#    set(_all_args  ${_options} ${_one_args} ${_list_args})

#    cmake_parse_arguments(M "${_options}" "${_one_args}" "${_list_args}" ${ARGN})

#    # 检查 kit 的类型是否为 外部库
#    EMakeGetTargetPropertyM("${_KIT_NAME}" KIT_TYPE _kit_type)
#    if(NOT "${_kit_type}" STREQUAL "EXT")
#        EMakeErrF("Please using EBuildExtAddExport() in external libs")
#    endif()

#    # 检查是否传入了 i_name
#    list(FIND _all_args ${i_name} _o_index)
#    if(_o_index GREATER -1)
#        EMakeErrF("Please set a export name")
#    endif()

#    # 检查传入的 i_name 是否已被使用
#    list(FIND _${_KIT_NAME}_TARGETS_ ${i_name} _o_index)
#    if(_o_index GREATER -1)
#        EMakeErrF("the export name '${i_name}' you set for KIT '${_KIT_NAME}' has been used already")
#    endif()


#    set(_r_libs)

#    # -- archives
#    foreach(_archive ${M_ARCHIVES})
#        _AutoGenArchiveName(${_archive} _r_archive)

#        if(_archive_dir_len EQUAL 1)
#            _ExtAddLibs(${_r_archive} TYPE "ARCHIVE" DIR "${_archive_dirs}" DEFAULT)
#        else()
#            list(FIND M_ARCHIVES    ${_archive} _o_index)
#            list(GET  _archive_dirs ${_o_index} _o_dir)

#            _ExtAddLibs(${_r_archive} TYPE "ARCHIVE" DIR "${_o_dir}")
#        endif()

#        list(APPEND _r_libs ${_r_archive})
#    endforeach()

#    # -- libraries
#    foreach(_library ${M_LIBRARIES})
#        _AutoGenLibraryName(${_library} _r_library _r_link)

#        if(_library_dir_len EQUAL 1 )
#            _ExtAddLibs(${_r_library} TYPE "LIBRARY" DIR "${_library_dirs}" DEFAULT)
#        else()
#            list(FIND M_LIBRARIES   ${_library} _o_index)
#            list(GET  _library_dirs ${_o_index} _o_dir)

#            _ExtAddLibs(${_r_library} TYPE "LIBRARY" DIR "${_o_dir}")
#        endif()

#        list(APPEND _r_libs ${_r_library})
#    endforeach()

#    # -- runtimes
#    foreach(_runtime ${M_RUNTIMES})
#        _AutoGenLibraryName(${_runtime} _r_runtime)

#        if(_runtime_dir_len EQUAL 1 )
#            _ExtAddLibs(${_r_runtime} TYPE "RUNTIME" DIR "${_runtime_dirs}" DEFAULT)
#        else()
#            list(FIND M_RUNTIMES    ${_runtime} _o_index)
#            list(GET  _runtime_dirs ${_o_index} _o_dir)

#            _ExtAddLibs(${_r_library} TYPE "RUNTIME" DIR "${_o_dir}")
#        endif()

#        list(APPEND _r_libs ${_r_runtime})
#    endforeach()

#    EMakeSetTargetPropertyM(${_KIT_NAME} KIT_EXPORT_TARGETS VAR ${i_name} APPEND)

#    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_name} HAVE         VAR         1)
#    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_name} TYPE         VAR         EXTS)
#    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_name} LIBS         LIST_VAR    ${_r_libs})
#    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_name} INCLUDE_DIRS LIST_VAR    ${_include_dirs})
#    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_name} LINK_DIRS    LIST_VAR    ${_link_dirs})
#    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_name} OWNER        VAR         ${_KIT_NAME})

#    EMakeSetGlobalPropertyM(KNOWN_TARGETS        LIST_VAR ${_KIT_NAME}::${i_name} APPEND)

#    list(APPEND _${_KIT_NAME}_LIBS          ${_r_libs})
#    list(APPEND _${_KIT_NAME}_INCLUDE_DIRS  ${_include_dirs})
#    list(APPEND _${_KIT_NAME}_LINK_DIRS     ${_link_dirs})



#endmacro()

# 导出设置
#   必须进行导出设置才能让其它 kit 依赖本 kit 的相关资源
#
# EBuildExtAddExport(<Target>                 # 此目标将可以被其它 kit 依赖，格式为 ${KIT_NAME}::${Target}
#       NAME <name>                         # 文件名称
#       TYPE <FILE|STATIC|SHARED|MODULE>    # 目标的类型：文件，静态库，动态库，模块
#
#       INCLUDE_DIRS   <dir1> [...]         # 依赖此目标需要包含的头文件目录
#       LINK_DIRS      <dir1> [...]         # 依赖此目标需要包含的链接目录
#       [LINK_DIRS_<config> <dir1> [...]  ] # 设置各构建模式下的链接目录，当此项被设置时，在特定的构建模式下，具有更高的优先级
#                                           #   此时会覆盖掉 LINK_DIRS 中的值
#                                           #   <config> 可以是 DEBUG RELEASE RELWITHDEBINFO MINSIZEREL
#
#       [ARCHIVE_EXTENTION <AUTO|.a|.lib...>]  # 归档文件扩展名，若设为 AUTO（默认），则框架会自动根据平台设置为 .lib 或 .a
#       [RUNTIME_EXTENTION <AUTO|.so|.dll...>] # 运行时库扩展名，若设为 AUTO（默认），则框架会自动根据平台设置为 .dll 或 .so
#                                              #     在 linux 下的动态库，会以 运行时库 作为链接库
#
#       [POSTFIX_<config>]                  # 设置各构建模式下的后缀，<config> 可以是 DEBUG RELEASE RELWITHDEBINFO MINSIZEREL
#                                           #    默认值为 EXT_${<config>}_POSTFIX
#
#       [LOCATION              <dir>   ]    # 此目标的位置（目录），若不设置，将取 LINK_DIRS 中的第一项
#       [EXPECT_DIR            <path>  ]    # 期望的位置（相对路径），当某个目标a依赖本目标时，期望本目标的输出位置（相对于a的输出位置）
#                                           #   此位置可被 目标a 重新设定
#       )
function(EBuildExtAddExport i_target )

    CONFIGURING_SETUP_CHECK()

    set(options         )
    set(oneValueArgs   NAME TYPE ARCHIVE_EXTENTION RUNTIME_EXTENTION POSTFIX_DEBUG POSTFIX_RELEASE POSTFIX_RELWITHDEBINFO POSTFIX_MINSIZEREL LOCATION EXPECT_DESTINATION)
    set(multiValueArgs INCLUDE_DIRS LINK_DIRS LINK_DIRS_DEBUG LINK_DIRS_RELEASE LINK_DIRS_RELWITHDEBINFO LINK_DIRS_MINSIZEREL)
    set(allChecks      ${options} ${oneValueArgs} ${multiValueArgs})

    EMakeGetTargetPropertyM("${_KIT_NAME}" KIT_TYPE _kit_type NO_CHECK)

    # 检查
    if("${i_target}" STREQUAL "")
        return()
    endif()

    if("${_kit_type}" STREQUAL "")
        return()
    endif()

    if(NOT "${_kit_type}" STREQUAL "EXT")
        EMakeErrF("Please using EBuildExtAddExport() in external libs")
    endif()

    list(FIND allChecks "${i_target}" _o_id)
    if(_o_id GREATER -1)
        EMakeErrF("Please set a target name when setting export target for KIT '${_KIT_NAME}'")
    endif()

    EMakeParseArguments(M "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    # 检查 target 是否已被使用
    list(FIND _${_KIT_NAME}_EXPORTS_ ${i_target} _o_id)
    if(_o_id GREATER -1)
        EMakeErrF("the export name '${i_target}' you set for KIT '${_KIT_NAME}' has been used already")
    endif()

    # 检查需要被设置的项
    if(NOT M_NAME)

        if(M_TYPE STREQUAL "FILE")
            return()
        else()
            EMakeErrF("Please set NAME for you KIT target '${_KIT_NAME}::${i_target}'")
        endif()

    endif()

    if(NOT M_TYPE)
        EMakeErrF("Please set TYPE for you target '${_KIT_NAME}::${i_target}'")
    elseif(NOT ${M_TYPE} MATCHES "FILE|STATIC|SHARED|MODULE")
        EMakeErrF("invalid TYPE '${M_TYPE}' for KIT target '${_KIT_NAME}::${i_target}', you can only set one of them:
            FILE STATIC SHARED MODULE")
    elseif(${M_TYPE} MATCHES "FILE")
        EMakeErrF("invalid TYPE '${M_TYPE}' for KIT target '${_KIT_NAME}::${i_target}', may supported later")
    endif()

    if(M_LINK_DIRS_${_CUR_BUILD_TYPE_UPPER})
        set(M_LINK_DIRS ${M_LINK_DIRS_${_CUR_BUILD_TYPE_UPPER}})
    endif()

    if(M_TYPE MATCHES "STATIC|SHARED")

        if(NOT M_INCLUDE_DIRS)
            EMakeErrF("Please set INCLUDE_DIRS for you '${M_TYPE}' target '${_KIT_NAME}::${i_target}'")
        endif()

        if(NOT M_LINK_DIRS)
            EMakeErrF("Please set LINK_DIRS for you '${M_TYPE}' target '${_KIT_NAME}::${i_target}'")
        endif()

    endif()

    if(NOT M_LOCATION)

        if(M_TYPE MATCHES "FILE|MODULE" AND NOT M_LINK_DIRS)
            EMakeErrF("Please set LOCATION for you target '${_KIT_NAME}::${i_target}'")
        endif()

        list(GET M_LINK_DIRS 0 M_LOCATION)

    endif()

    # -- 获取文件后缀
    set(_name_postfix)
    if(NOT M_TYPE STREQUAL "FILE")

        if(${CMAKE_BUILD_TYPE} MATCHES "Debug|Release|MinSizeRel|RelWithDebInfo")
            if(DEFINED M_POSTFIX_${_CUR_BUILD_TYPE_UPPER})
                set(_name_postfix ${M_POSTFIX_${_CUR_BUILD_TYPE_UPPER}})
            else()
                set(_name_postfix ${EXT_${_CUR_BUILD_TYPE_UPPER}_POSTFIX})
            endif()
        else()
            EMakeErrF("invalid build type '${CMAKE_BUILD_TYPE}'")
        endif()

    endif()

    # -- 获取文件扩展名
    set(_archive_extention)
    set(_runtime_extention)

    set(_archive_extention ${_DF_LIB_ARCHIVE_EXTENTION})
    set(_runtime_extention ${_DF_LIB_RUNTIME_EXTENTION})

    if(M_RUNTIME_EXTENTION AND "${M_RUNTIME_EXTENTION}" STREQUAL "AUTO")
        set(_runtime_extention ${M_RUNTIME_EXTENTION})
    endif()

    if(M_ARCHIVE_EXTENTION AND "${M_ARCHIVE_EXTENTION}" STREQUAL "AUTO")
        set(_archive_extention ${M_ARCHIVE_EXTENTION})
    endif()

    if    (${M_TYPE} STREQUAL "STATIC")
        set(_runtime_extention)
    elseif(${M_TYPE} STREQUAL "MODULE")
        set(_archive_extention)
    endif()


    # -- 生成文件名
    set(_archive_name)
    set(_runtime_name)
    set(_link_name)

    if(_archive_extention)
        set(_archive_name ${_DF_LIB_PREFIX}${M_NAME}${_name_postfix}${_archive_extention})
        set(_link_name    ${M_NAME}${_name_postfix}${_archive_extention})
    endif()
    if(_runtime_extention)
        set(_runtime_name ${_DF_LIB_PREFIX}${M_NAME}${_name_postfix}${_runtime_extention})

        if(NOT WIN32)
            set(_link_name    ${M_NAME}${_name_postfix}${_runtime_extention})
        endif()

    endif()

    # -- 生成目标，并为目标创建相关属性
    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_target} KIT_TYPE     VAR         EXT)
    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_target} TYPE         VAR         ${M_TYPE})
    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_target} ARCHIVE_NAME VAR         ${_archive_name})
    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_target} RUNTIME_NAME VAR         ${_runtime_name})
    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_target} LINK_NAME    VAR         ${_link_name})
    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_target} INCLUDE_DIRS LIST_VAR    ${M_INCLUDE_DIRS})
    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_target} LINK_DIRS    LIST_VAR    ${M_LINK_DIRS})
    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_target} LOCATION     VAR         ${M_LOCATION})
    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_target} EXPECT_DIR   VAR         ${M_EXPECT_DIR})

    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_target} HAVE         VAR         1)
    EMakeSetTargetPropertyM(${_KIT_NAME}::${i_target} OWNER        VAR         ${_KIT_NAME})

    # -- 设置总目标的相关属性
    EMakeSetTargetPropertyM(${_KIT_NAME} KIT_EXPORT_TARGETS VAR      ${_KIT_NAME}::${i_target}     APPEND)
    EMakeSetTargetPropertyM(${_KIT_NAME} KIT_DERECT_DEPENDS VAR      ${_KIT_NAME}::${i_target}     APPEND)
    EMakeSetTargetPropertyM(${_KIT_NAME} KIT_DEPENDS        VAR      ${_KIT_NAME}::${i_target}     APPEND)
    EMakeSetTargetPropertyM(${_KIT_NAME} LINK_NAME          VAR      ${_link_name}                 APPEND_NO_DUP)
    EMakeSetTargetPropertyM(${_KIT_NAME} INCLUDE_DIRS       LIST_VAR ${_${KIT_NAME}_INCLUDE_DIRS}  APPEND_NO_DUP)
    EMakeSetTargetPropertyM(${_KIT_NAME} LINK_DIRS          LIST_VAR ${_${KIT_NAME}_LINK_DIRS}     APPEND_NO_DUP)

    # -- 更新到全局目标
    EMakeSetGlobalPropertyM(KNOWN_TARGETS        LIST_VAR ${_KIT_NAME}::${i_target} APPEND)

endfunction()
