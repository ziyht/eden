# =====================================================================================
#
#       Filename:  EBuildSetupKitsM.cmake
#
#    Description:  读取每个子项目的 setupManifist.cmake 文件，并设置目标的属性
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

#
# \! 相关默认值设置
#

if(WIN32)
    set(_DF_LIB_ARCHIVE_EXTENTION ".lib")
    set(_DF_LIB_RUNTIME_EXTENTION ".dll")
    set(_DF_LIB_PREFIX                  )
    set(_DF_APP_RUNTIME_EXTENTION ".exe")
else()
    set(_DF_LIB_ARCHIVE_EXTENTION ".a"  )
    set(_DF_LIB_RUNTIME_EXTENTION ".so" )
    set(_DF_LIB_PREFIX            "lib" )
    set(_DF_APP_RUNTIME_EXTENTION)
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" _CUR_BUILD_TYPE_UPPER)
string(TOLOWER "${CMAKE_BUILD_TYPE}" _CUR_BUILD_TYPE_LOWER)

set(_DF_FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
set(_DF_DIR_PERMISSIONS  ${_DF_FILE_PERMISSIONS})
set(_DF_BIN_PERMISSIONS  ${_DF_FILE_PERMISSIONS} OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)

set(_DF_LIB_PERMISSIONS  ${_DF_FILE_PERMISSIONS})
set(_DF_PLG_PERMISSIONS  ${_DF_FILE_PERMISSIONS})
set(_DF_APP_PERMISSIONS  ${_DF_BIN_PERMISSIONS})

if(PROJECT_VERSION)
    set(_VERSION _${PROJECT_VERSION})
else()
    set(_VERSION)
endif()

set(_EXT_POSTFIX_INHERIT
    -DCMAKE_DEBUG_POSTFIX:STRING=${EXT_DEBUG_POSTFIX}
    -DCMAKE_RELEASE_POSTFIX:STRING=${EXT_RELEASE_POSTFIX}
    -DCMAKE_RELWITHDEBINFO_POSTFIX:STRING=${EXT_RELWITHDEBINFO_POSTFIX}
    -DCMAKE_MINSIZEREL_POSTFIX:STRING=${EXT_MINSIZEREL_POSTFIX}
)

#
# \! 检查 KIT_TARGET 名称是否合法，若未设置，设置为 ${KIT_NAME}
#
macro(_CheckTargetNameM i_kit i_target_)

    if("${i_target_}" STREQUAL "")
        set(KIT_TARGET ${i_kit})
    endif()

    list(FIND _G_KOWN_TARGET_NAMES_ ${KIT_TARGET} _o_index)

    # already have ?
    if(${_o_index} GREATER -1)
        EMakeGetTargetPropertyM(${KIT_TARGET} BELONG_TO   _kit_name NO_CHECK)
        EMakeGetTargetPropertyM(${_kit_name}  TARGET_TYPE _kit_type NO_CHECK)
        EMakeErrF("the KIT_TARGET '${KIT_TARGET}' set for ${i_kit} is already in using(used by '${_kit_type}:${_kit_name}'), please set another one")
    endif()

    list(APPEND _G_KOWN_TARGET_NAMES_ ${KIT_TARGET})
    EMakeSetTargetPropertyM(${KIT_TARGET} BELONG_TO VAR ${i_kit})

endmacro()



#
# \! 值检查
#   1. 若 值设置的不合法，终止 cmake 并输出错误
#   2. 若 值未设置，则设置成传入的默认值
#

macro(_CheckValM i_kit i_val i_regex i_default)

    cmake_parse_arguments(M "" "NOTE" "" "${ARGN}")

    if("${${i_val}}" STREQUAL "")
        set(${i_val} ${i_default})
    endif()

    if(NOT "${i_regex}_" STREQUAL "_")

        if(NOT "${${i_val}}" MATCHES "${i_regex}")
            EMakeErrF("the val of '${i_val}' you set for '${i_kit}' is invalid: ${M_NOTE}")
        endif()

    endif()

endmacro()


#
#   清空本文件中使用的变量缓存
#
set(_dirty_args__
    KIT_DEBUG KIT_NAME _KIT_NAME KIT_DIR KIT_TARGET KIT_LIBRARY_TYPE KIT_PREFIX
    KIT_PREFIX KIT_DOWNLOAD_DIR KIT_SOURCE_DIR KIT_BINARY_DIR KIT_INSTALL_DIR KIT_POSTFIX
    KIT_DOWNLOADS KIT_UPDATE_COMMAND KIT_PATCH_COMMAND
    KIT_CONFIGURE_COMMAND KIT_CMAKE_COMMAND KIT_CMAKE_GENERATOR KIT_CMAKE_ARGS KIT_CMAKE_CACHE_ARGS
    KIT_BUILD_COMMAND KIT_BUILD_IN_SOURCE
    KIT_TEST_COMMAND KIT_TEST_BEFORE_INSTALL KIT_TEST_AFTER_INSTALL
    KIT_INSTALL_COMMAND
    KIT_SRCS KIT_MOC_SRCS KIT_UI_FORMS KIT_RESOURCES
    KIT_DEPENDS KIT_LIBRARIES_SYS
    KIT_EXPORT KIT_EXPORT_DIR
    KIT_EXPORT_INCLUDE_DIRS KIT_EXPORT_LINK_DIRS KIT_EXPORT_LIBS KIT_EXPORT_MODULES
    EBuildAssignTargetDestF_ARGS KIT_SRCS_PLUGIN KIT_SRCS_MAIN
    )
macro(_ClearSetupKitBuffer)

    foreach(_arg ${_dirty_args__})
        set(${_arg})
    endforeach()

endmacro()

# \! 解析 文件后缀
#
#    通过设定以下值来输出
#       O_DEBUG_POSTFIX
#       O_RELEASE_POSTFIX
#       O_RELWITHDEBINFO_POSTFIX
#       O_MINSIZEREL_POSTFIX
#       O_CUR_POSTFIX
#
#
macro(_ParsePostfix)

    EMakeParseArguments(M "" "DEBUG;RELEASE;RELWITHDEBINFO;MINSIZEREL" "" "${ARGN}")

    set(_args "${ARGN}")
    list(LENGTH _args _args_cnt)

    #EMakeInfF("----- ${KIT_TYPE}: ${KIT_NAME}")

    # 初始化
    foreach(_mode DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
        set(O_${_mode}_POSTFIX ${${KIT_TYPE}_${_mode}_POSTFIX})
        #EMakeInfF("O_${_mode}_POSTFIX: ${O_${_mode}_POSTFIX}")
    endforeach()

    # 参数检查
    set(_set_by_args)
    if(DEFINED M_DEBUG OR DEFINED M_RELEASE OR DEFINED M_RELWITHDEBINFO OR DEFINED M_MINSIZEREL)

        if(DEFINED M_UNPARSED_ARGUMENTS)
            EMakeErrF("invalid format of KIT_POSTFIX set for '${KIT_NAME}', unkonw arguments checked '[${M_UNPARSED_ARGUMENTS}]'")
        endif()

        set(_set_by_args 1)
    endif()

    # 更新值
    if(_set_by_args)
        if(DEFINED M_DEBUG)
            set(O_DEBUG_POSTFIX             ${M_DEBUG})
        endif()

        if(DEFINED M_RELEASE)
            set(O_RELEASE_POSTFIX           ${M_RELEASE})
        endif()

        if(DEFINED M_RELWITHDEBINFO)
            set(O_RELWITHDEBINFO_POSTFIX    ${M_RELWITHDEBINFO})
        endif()

        if(DEFINED M_MINSIZEREL)
            set(O_MINSIZEREL_POSTFIX        ${M_MINSIZEREL})
        endif()

    elseif(${_args_cnt})
        if(${_args_cnt} GREATER 0)
            list(GET _args 0 O_DEBUG_POSTFIX)
        endif()
        if(${_args_cnt} GREATER 1)
            list(GET _args 1 O_RELEASE_POSTFIX)
        endif()
        if(${_args_cnt} GREATER 2)
            list(GET _args 2 O_RELWITHDEBINFO_POSTFIX)
        endif()
        if(${_args_cnt} GREATER 3)
            list(GET _args 3 O_MINSIZEREL_POSTFIX)
        endif()
    endif()

    # 确定值
    foreach(_mode DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)

        if(O_${_mode}_POSTFIX MATCHES "^ {1,}$")
            set(O_${_mode}_POSTFIX )
        elseif(O_${_mode}_POSTFIX MATCHES "inherit|INHERIT")
            set(O_${_mode}_POSTFIX ${CMAKE_${_mode}_POSTFIX})
        endif()
        #EMakeInfF("O_${_mode}_POSTFIX: ${O_${_mode}_POSTFIX}")

        if(${_CUR_BUILD_TYPE_UPPER} MATCHES ${_mode})
            set(O_CUR_POSTFIX ${O_${_mode}_POSTFIX})
        endif()

    endforeach()

    #EMakeInfF("O_CUR_POSTFIX: ${O_CUR_POSTFIX}\n")

endmacro()

# 此宏为 _AddFilesParseArgs 内部使用宏，解析相关参数
#
#  内部定义变量进行输出
#       _o_dest ：目标位置
#       _o_files: 文件列表
#       _o_dirs : 目录列表
#
macro(_AddFilesParseArgs)

    cmake_parse_arguments(M "" "POSTFIX" "FILES;DIRS" ${ARGN})

    if(NOT M_FILES AND NOT M_DIRS)
        EMakeErrF("invalid args, can not find any files or dirs in args in 'EBuildAddFilesF()' for kit '${KIT_NAME}': [${ARGN}]")
    endif()

    # 获取目标位置
    set(_o_dest ${M_DESTINATION_PREFIX}/${M_POSTFIX}/)    # M_DESTINATION_PREFIX 在 EBuildAddFilesF 中设置
    string(REGEX REPLACE "///|//" "/" _o_dest ${_o_dest})

    # 检查文件
    set(_o_files)
    foreach(_file ${M_FILES})

        if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${_file})

            if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${_file})
                EMakeErrF("the file '${_file}' in '${CMAKE_CURRENT_LIST_DIR}' is not a file which set in args in 'EBuildAddFilesF()' for kit '${KIT_NAME}':\n [ ${ARGN} ]")
            else()
                list(APPEND _o_files ${CMAKE_CURRENT_LIST_DIR}/${_file})
                EMakeSetTargetPropertyM(${CMAKE_CURRENT_LIST_DIR}/${_file} BLONG_TO   VAR      ${KIT_NAME} APPEND_NO_DUP)
            endif()

        elseif(EXISTS ${_file})

            if(IS_DIRECTORY ${_file})
                EMakeErrF("the file '${_file}' is not a file which set in args in 'EBuildAddFilesF()' for kit '${KIT_NAME}':\n [ ${ARGN} ]")
            else()
                list(APPEND _o_files ${_file})
                EMakeSetTargetPropertyM(${_file} BLONG_TO   VAR      ${KIT_NAME} APPEND_NO_DUP)
            endif()

            list(APPEND _o_files ${_file})
        else()
            EMakeErrF("the file '${_file}' not exist which set in args in 'EBuildAddFilesF()' for kit '${KIT_NAME}':\n [ ${ARGN} ]")
        endif()

    endforeach()

    # 检查目录
    set(_o_dirs)
    foreach(_dir ${M_DIRS})

        if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${_dir})

            if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${_dir})
                string(REGEX REPLACE "/$" "" _dir ${_dir})
                list(APPEND _o_dirs ${CMAKE_CURRENT_LIST_DIR}/${_dir})
                EMakeSetTargetPropertyM(${CMAKE_CURRENT_LIST_DIR}/${_dir} BLONG_TO   VAR      ${KIT_NAME} APPEND_NO_DUP)
            else()
                EMakeErrF("the dir '${_dir}' in '${CMAKE_CURRENT_LIST_DIR}' is not a directory which set in args in 'EBuildAddFilesF()' for kit '${KIT_NAME}':\n [ ${ARGN} ]")
            endif()

        elseif(EXISTS ${_dir})

            if(IS_DIRECTORY ${_dir})
                EMakeSetTargetPropertyM(${_dir} BLONG_TO   VAR      ${KIT_NAME} APPEND_NO_DUP)
                string(REGEX REPLACE "/$" "" _dir ${_dir})
                list(APPEND _o_dirs ${_dir})
            else()
                EMakeErrF("the dir '${_dir}' is not a directory which set in args in 'EBuildAddFilesF()' for kit '${KIT_NAME}':\n [ ${ARGN} ]")
            endif()

        else()
            EMakeErrF("the dir '${_dir}' not exist which set in args in 'EBuildAddFilesF()' for kit '${KIT_NAME}':\n [ ${ARGN} ]")
        endif()

    endforeach()

endmacro()

#
# -- 添加文件
#
# EBuildAddFilesF(
#       DESTINATION_PREFIX  <目标位置前缀>                        // 相对路径，目标位置的前缀，相对本kit输出目录，若不设置，使用 '/'
#       FILES <filename1> ... [POSTFIX <目标位置后缀>]            // 设定文件列表及列表文件想要拷贝到的位置
#       FILES <filename1> ... [POSTFIX <目标位置后缀>]            // 设定文件列表及列表文件想要拷贝到的位置
#       DIRS  <dir_path1> ... [POSTFIX <目标位置后缀>]            // 设定目录列表及列表目录想要拷贝到的位置
#       DIRS  <dir_path1> ... [POSTFIX <目标位置后缀>]            // 设定目录列表及列表目录想要拷贝到的位置
#       ...
#)
# ----
#   - DESTINATION_PREFIX： 目标位置的前缀，相对路径（相对输出位置）
#   - FILES             ： 文件列表，可以是一个或多个，可以是绝对路径和相对路径（相对于当前kit的源码目录）
#       - POSTFIX       ： 后缀，最终的相对位置为 /${DESTINATION_PREFIX}/{POSTFIX}/
#   - DIRS              ： 目录列表，可以是一个或多个，可以是绝对路径和相对路径（相对于当前kit的源码目录）
#       - POSTFIX       ： 后缀，最终的相对位置为 /${DESTINATION_PREFIX}/{POSTFIX}/
#
#   注： 被添加的文件或目录必须存在
#
function(EBuildAddFilesF)

    CONFIGURING_SETUP_CHECK()

#    EMakeGetTargetPropertyM(${KIT_NAME} OPTION_NAME      _option_name)
#    if(NOT ${_option_name})
#        return()
#    endif()

    # 解析 DESTINATION_PREFIX
    cmake_parse_arguments(M "" "DESTINATION_PREFIX" "" ${ARGN})

    set(M_DESTINATION_PREFIX "/${M_DESTINATION_PREFIX}")

    set(_dests)

    if(NOT KIT_NAME)
        set(KIT_NAME template)
    endif()

    # 解析后续参数
    set(_args)
    foreach(_arg ${M_UNPARSED_ARGUMENTS})

        if(${_arg} MATCHES "FILES|DIRS")

            list(LENGTH _args _args_len)

            if(_args_len GREATER 0)
                _AddFilesParseArgs(${_args})
                # EMakeInfF("${_o_dest} : [${_o_files} ${_o_dirs}]")

                if(_o_files)
                    EMakeSetTargetPropertySetM(${KIT_NAME} TO_COPY_FILES ${_o_dest} VARS ${_o_files} APPEND_NO_DUP)
                endif()

                if(_o_dirs)
                    EMakeSetTargetPropertySetM(${KIT_NAME} TO_COPY_DIRS  ${_o_dest} VARS ${_o_dirs} APPEND_NO_DUP)
                endif()

                set(_args)
                list(APPEND _args ${_arg})

            else()
                list(APPEND _args ${_arg})
            endif()

        else()
            list(APPEND _args ${_arg})
        endif()

    endforeach()

    list(LENGTH _args _args_len)
    if(_args_len GREATER 0)
        _AddFilesParseArgs(${_args})
        # EMakeInfF("${_o_dest} : [${_o_files} ${_o_dirs}]")

        if(_o_files)
            EMakeSetTargetPropertySetM(${KIT_NAME} TO_COPY_FILES ${_o_dest} VARS ${_o_files} APPEND_NO_DUP)
        endif()

        if(_o_dirs)
            EMakeSetTargetPropertySetM(${KIT_NAME} TO_COPY_DIRS  ${_o_dest} VARS ${_o_dirs} APPEND_NO_DUP)
        endif()

    endif()

    # -- test output
#    EMakeGetTargetPropertySetM(${KIT_NAME} TO_COPY_FILES)
#    EMakeGetTargetPropertySetM(${KIT_NAME} TO_COPY_DIRS)

#    foreach(_to_copy ${TO_COPY_FILES_PROPERTIES_} )
#        EMakeInfF("    ${${_to_copy}_KEY_}: [ ${${_to_copy}_VALS_} ]")
#    endforeach()
#    foreach(_to_copy ${TO_COPY_DIRS_PROPERTIES_} )
#        EMakeInfF("    ${${_to_copy}_KEY_}: [ ${${_to_copy}_VALS_} ]")
#    endforeach()

endfunction()

#
# 供 _AssignTargetDestParseArgs 调用
#
#   设置以下值来输出信息：
#       _o_targets : 所有的目标，在第一次调用本 API 前需清空此值
#
#   对于每一个 _o_targets 中的 target，通过使用后缀 _dest_ 来获取目标，如：
#   foreach(_o_target ${_o_targets})
#       EMakeInfF("  ${_o_target}: ${${_o_target}_dest_}")
#   endforeach()
#
#
macro(_AssignTargetDestParseArgsL2)

    cmake_parse_arguments(M "SKIP_COPY;TEST_SKIP_COPY" "DEST;TEST_DEST" "TARGETS" "${ARGN}")

    if(M_UNPARSED_ARGUMENTS)
        EMakeErrF("unparsed arguments checked in which set in [ ${ARGN} ], please using TARGETS to set the targets and DEST to set the destinaion")
    endif()

    if(NOT M_DEST AND NOT M_SKIP_COPY AND NOT M_TEST_DEST AND NOT M_TEST_SKIP_COPY)
        EMakeErrF("no DEST or OPERATIONS set in param [ ${ARGN} ]")
    endif()

    # EMakeInfF("-------------[ ${KIT_NAME} ${ARGN}] [${M_TEST_DEST}]")

    foreach(_target ${M_TARGETS})

        # 判断是否为直接依赖，只有直接依赖项才能被设置
        list(FIND KIT_DEPENDS ${_target} _o_id)
        if(${_o_id} EQUAL -1 )
            EMakeErrF("the Target '${_target}' is not derectly depended by '${KIT_NAME}' which you set in:
                 [ ${ARGN} ] ")
        endif()

        # 判断是否已设置，若已设置且重复设置的dest相同，输出警告，不同输出错误
        list(FIND _o_targets ${_target} _o_id)
        if(${_o_id} GREATER -1)

            EMakeInfF("********************************")

            if(${_target}_dest_)
                EMakeInfF("*******************************1")
                if("${${_target}_dest_}" STREQUAL "${M_DEST}")
                    EMakeWrnF("reduplicative DEST set checked for target '${_target}' in param: [ ${ARGN} ]
   previous set in: [ ${${_target}_args_} ]")
                else()
                    EMakeErrF("reduplicative DEST set checked for target '${_target}' in param: [ ${ARGN} ]
   previous set in: [ ${${_target}_args_} ]")
                endif()
            endif()

            if(${_target}_test_dest_)
                EMakeInfF("*******************************2")
                if("${${_target}_test_dest_}" STREQUAL "${M_TEST_DEST}")
                    EMakeWrnF("reduplicative TEST_DEST set checked for target '${_target}' in param: [ ${ARGN} ]
   previous set in: [ ${${_target}_args_} ]")
                else()
                    EMakeErrF("reduplicative TEST_DEST set checked for target '${_target}' in param: [ ${ARGN} ]
   previous set in: [ ${${_target}_args_} ]")
                endif()
            endif()

        endif()

        list(APPEND _o_targets ${_target})

        if(M_DEST)
            set(${_target}_dest_      ${M_DEST})
        endif()

        if(M_TEST_DEST)
            set(${_target}_test_dest_  ${M_TEST_DEST})
        endif()

        if(M_SKIP_COPY)
            set(${_target}_skip_copy_  1)
        endif()

        if(M_TEST_SKIP_COPY)
            set(${_target}_test_skip_copy_  1)
        endif()

        set(${_target}_args_ ${ARGN})

    endforeach()

    if(_o_targets)
        list(REMOVE_DUPLICATES _o_targets)
    endif()

endmacro()

#
# 用以解析 EBuildAssignTargetDestM 设置的参数
#
#
function(_AssignTargetDestParseArgs)

    CONFIGURING_SETUP_CHECK()

    # -- 清空可能的缓存
    set(_o_targets)
    set(_args)

    foreach(_arg ${EBuildAssignTargetDestF_ARGS})

        if(${_arg} MATCHES "TARGETS")

            list(LENGTH _args _args_len)

            if(_args_len GREATER 0)

                _AssignTargetDestParseArgsL2(${_args})

                set(_args)
                list(APPEND _args ${_arg})
            else()
                list(APPEND _args ${_arg})
            endif()

        else()
            list(APPEND _args ${_arg})

        endif()
    endforeach()

    list(LENGTH _args _args_len)
    if(_args_len GREATER 0)
        _AssignTargetDestParseArgsL2(${_args})

    endif()

    foreach(_target ${_o_targets})

        if(${_target}_dest_)
            set(_dest      /${${_target}_dest_})
            string(REGEX REPLACE "///|//" "/" _dest ${_dest})
            EMakeSetTargetPropertySetM(${KIT_NAME} ASSIGN ${_target}_dest       VARS ${_dest})
        endif()
        EMakeSetTargetPropertySetM(${KIT_NAME} ASSIGN ${_target}_skip_copy VARS ${${_target}_skip_copy_})

        if(${_target}_test_dest_ )
            set(_dest  /${${_target}_test_dest_})
            string(REGEX REPLACE "///|//" "/" _dest ${_dest})
            EMakeSetTargetPropertySetM(${KIT_NAME} ASSIGN ${_target}_test_dest  VARS ${_dest})
        endif()
        EMakeSetTargetPropertySetM(${KIT_NAME} ASSIGN ${_target}_test_skip_copy VARS ${${_target}_test_skip_copy_})

        # EMakeInfF("----------${KIT_NAME} ${_target} ---------: [${${_target}_dest_}|${${_target}_skip_copy_}|${${_target}_test_dest_}|${${_target}_test_skip_copy_} ]")

        # 清空缓存
        set(${_target}_dest_)
        set(${_target}_skip_copy_)
        set(${_target}_test_dest_)
        set(${_target}_test_skip_copy_)
    endforeach()

endfunction()

#
# -- 设置目标的存放位置
#
#  EBuildAssignTargetDestF(
#   TARGETS <target1> [...] DEST <path>     # 设定相关目标的存放位置，只能设置直接依赖的目标，可以有多行
#   TARGETS <target1> [...] DEST <path>
#   )
#
function(EBuildAssignTargetDestF)

    CONFIGURING_SETUP_CHECK()

    set(EBuildAssignTargetDestF_ARGS "${ARGN}" PARENT_SCOPE)
endfunction()

function(_SetupExtM)
    cmake_parse_arguments(M "" "EXT_NAME" "" ${ARGN})

    if(NOT M_EXT_NAME)
        EMakeErrF("internal err, you must set param 'EXT_NAME'")
    endif()

    # 设置 _KIT_NAME 供 EBuildExtAddExport() 使用
    set(_KIT_NAME ${M_EXT_NAME})
    set(KIT_NAME  ${M_EXT_NAME})
    set(KIT_DIR   ${EXTS_DIR}/${KIT_NAME})
    EMakeCheckExistsF(${KIT_DIR}/CMakeLists.txt)
    EMakeGetTargetPropertyM(${KIT_NAME} KIT_IS_INTERNAL _is_internal NO_CHECK)

    # 优先设置目录的默认值
    _CheckValM(${KIT_NAME} KIT_PREFIX       "" ${PROJECT_BINARY_DIR}/libs_external/${KIT_NAME})
    _CheckValM(${KIT_NAME} KIT_DOWNLOAD_DIR "" ${KIT_PREFIX}/src)
    _CheckValM(${KIT_NAME} KIT_SOURCE_DIR   "" ${KIT_PREFIX}/src/${KIT_NAME})
    _CheckValM(${KIT_NAME} KIT_BINARY_DIR   "" ${KIT_PREFIX}/build)
    _CheckValM(${KIT_NAME} KIT_INSTALL_DIR  "" ${KIT_PREFIX}/)

    # 配置 属性
    CONFIGURING_SETUP_SET(1)
    if(_is_internal)
        include(${EXTS_DIR}/${KIT_NAME}/CMakeLists.txt)
    else()
        EMakeErrF("internal err, this function is todo")
        include(${EXTS_EXPORTS_DIR}/${KIT_NAME}/setupManifist.cmake)
    endif()
    CONFIGURING_SETUP_SET(0)

    set(KIT_NAME ${M_EXT_NAME})

    # 检查和设置
    _CheckTargetNameM (${KIT_NAME} "${KIT_TARGET}" )

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DEBUG           VAR      "${KIT_DEBUG}" DEFAULT)
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_TARGET          VAR      ${KIT_TARGET})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_PREFIX          VAR      ${KIT_PREFIX})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DOWNLOAD_DIR    VAR      ${KIT_DOWNLOAD_DIR})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_SOURCE_DIR      VAR      ${KIT_SOURCE_DIR})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_BINARY_DIR      VAR      ${KIT_BINARY_DIR})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_INSTALL_DIR     VAR      ${KIT_INSTALL_DIR})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DOWNLOADS           LIST_VAR ${KIT_DOWNLOADS})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_UPDATE_COMMAND      LIST_VAR ${KIT_UPDATE_COMMAND})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_PATCH_COMMAND       LIST_VAR ${KIT_PATCH_COMMAND})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_CONFIGURE_COMMAND   LIST_VAR ${KIT_CONFIGURE_COMMAND})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_CMAKE_COMMAND       LIST_VAR ${KIT_CMAKE_COMMAND})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_CMAKE_GENERATOR     LIST_VAR ${KIT_CMAKE_GENERATOR})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_CMAKE_ARGS          LIST_VAR ${KIT_CMAKE_ARGS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_CMAKE_CACHE_ARGS    LIST_VAR ${KIT_CMAKE_CACHE_ARGS} ${_EXT_POSTFIX_INHERIT})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_BUILD_COMMAND       LIST_VAR ${KIT_BUILD_COMMAND})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_BUILD_IN_SOURCE     LIST_VAR ${KIT_BUILD_IN_SOURCE})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_TEST_COMMAND        LIST_VAR ${KIT_TEST_COMMAND})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_TEST_BEFORE_INSTALL VAR      ${KIT_TEST_BEFORE_INSTALL})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_TEST_AFTER_INSTALL  VAR      ${KIT_TEST_AFTER_INSTALL})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_INSTALL_COMMAND     LIST_VAR ${KIT_INSTALL_COMMAND})

    # 配置 导出
    set(CONFIGURING_EXPORT 1)
    if(_is_internal)
        include(${EXTS_DIR}/${KIT_NAME}/CMakeLists.txt)
    else()
        EMakeErrF("internal err, this function is todo")
        include(${EXTS_EXPORTS_DIR}/${KIT_NAME}/setupManifist.cmake)
    endif()
    set(CONFIGURING_EXPORT)

    set(KIT_NAME ${M_EXT_NAME})


    # 清空变量缓存
    #_ClearSetupKitBuffer()
endfunction()

function(_SetupLibM)
    cmake_parse_arguments(M "" "LIB_NAME" "" ${ARGN})

    if(NOT M_LIB_NAME)
        EMakeErrF("internal err, you must set param 'LIB_NAME'")
    endif()

    set(KIT_NAME ${M_LIB_NAME})
    set(KIT_DIR  ${LIBS_DIR}/${KIT_NAME})
    EMakeCheckExistsF(${KIT_DIR}/CMakeLists.txt)
    EMakeGetTargetPropertyM(${KIT_NAME} KIT_IS_INTERNAL _is_internal NO_CHECK)

    # 导入配置
    CONFIGURING_SETUP_SET(1)
    if(_is_internal)
        include(${LIBS_DIR}/${KIT_NAME}/CMakeLists.txt)
    else()
        EMakeErrf("not supported now")
    endif()
    CONFIGURING_SETUP_SET(0)
    EMakeGetGlobalPropertyM(LIBS_DIR LIBS_DIR)

    set(KIT_NAME ${M_LIB_NAME})
    set(KIT_TYPE LIB)

    EMakeParseInputFiles("${KIT_SRCS}"      KIT_SRCS      ${LIBS_DIR}/${KIT_NAME}/CMakeLists.txt)
    EMakeParseInputFiles("${KIT_UI_FORMS}"  KIT_UI_FORMS  ${LIBS_DIR}/${KIT_NAME}/CMakeLists.txt)
    EMakeParseInputFiles("${KIT_RESOURCES}" KIT_RESOURCES ${LIBS_DIR}/${KIT_NAME}/CMakeLists.txt)

    # 检查
    _CheckValM(${KIT_NAME} KIT_LIBRARY_TYPE "^STATIC$|^SHARED$|^MODULE$" "STATIC"   NOTE "you can only set [STATIC|SHARED|MODULE] or a empty val")
    _CheckValM(${KIT_NAME} KIT_EXPORT       "^ON$|^OFF$"                 "OFF"      NOTE "you can only set [ON|OFF|] or a empty val")
    _CheckValM(${KIT_NAME} KIT_EXPORT_DIR   ""                           ${COMMONS_DIR}/libs/${KIT_NAME})
    _CheckTargetNameM(${KIT_NAME} "${KIT_TARGET}" )

    EBuildCheckDependsM(${KIT_NAME} "${KIT_DEPENDS}")

    _ParsePostfix("${KIT_POSTFIX}")

    # 获取必要的值
    file(RELATIVE_PATH _relative_path ${MAIN_PROJECT_DIR} ${LIBS_DIR}/${M_LIB_NAME})

    # 设置可配置的目标属性
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DEBUG           VAR      "${KIT_DEBUG}" DEFAULT)
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_TARGET          VAR      ${KIT_TARGET})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_LIBRARY_TYPE    VAR      ${KIT_LIBRARY_TYPE})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_EXPORT          VAR      ${KIT_EXPORT})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_EXPORT_DIR      VAR      ${KIT_EXPORT_DIR})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_EXPECT_DIR      VAR      ${KIT_EXPECT_DIR})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_SRCS            LIST_VAR ${KIT_SRCS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_MOC_SRCS        LIST_VAR ${KIT_MOC_SRCS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_UI_FORMS        LIST_VAR ${KIT_UI_FORMS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_RESOURCES       LIST_VAR ${KIT_RESOURCES})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DEPENDS         LIST_VAR ${KIT_DEPENDS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DERECT_DEPENDS  LIST_VAR ${KIT_DEPENDS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_LIBRARIES_SYS   LIST_VAR ${KIT_LIBRARIES_SYS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_INCLUDE_DIRS    LIST_VAR ${KIT_INCLUDE_DIRS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_LINK_DIRS       LIST_VAR ${KIT_LINK_DIRS}    APPEND)

    # 设置其它相关的目标属性
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_SOURCES_DIR     VAR      ${LIBS_DIR}/${M_LIB_NAME})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_BUILD_DIR       VAR      ${PROJECT_BINARY_DIR}/${_relative_path})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_OUTPUT_DIR      VAR      ${LIBS_OUTPUT_DIR})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_IS_QT_PROJECT   VAR      ${KIT_IS_QT_PROJECT})

    # 设置文件
    set(_RUNTIME_NAME)
    set(_ARCHIVE_NAME)
    if(DEFINED KIT_NAME)
        set(_OUTPUT_NAME ${KIT_NAME}${O_CUR_POSTFIX})

        if(${KIT_LIBRARY_TYPE} MATCHES "STATIC")
            set(_ARCHIVE_NAME ${_DF_LIB_PREFIX}${_OUTPUT_NAME}${_DF_LIB_ARCHIVE_EXTENTION})
        elseif(${KIT_LIBRARY_TYPE} MATCHES "SHARED")
            if(WIN32)
                set(_RUNTIME_NAME ${_DF_LIB_PREFIX}${_OUTPUT_NAME}${_DF_LIB_RUNTIME_EXTENTION})
                set(_ARCHIVE_NAME ${_DF_LIB_PREFIX}${_OUTPUT_NAME}${_DF_LIB_ARCHIVE_EXTENTION})
            else()
                set(_RUNTIME_NAME ${_DF_LIB_PREFIX}${_OUTPUT_NAME}${_DF_LIB_RUNTIME_EXTENTION})
                set(_ARCHIVE_NAME ${_DF_LIB_PREFIX}${_OUTPUT_NAME}${_DF_LIB_RUNTIME_EXTENTION})
            endif()
        elseif(${KIT_LIBRARY_TYPE} MATCHES "MODULE")
            set(_RUNTIME_NAME ${_DF_LIB_PREFIX}${_OUTPUT_NAME}${_DF_LIB_RUNTIME_EXTENTION})
        endif()
    endif()
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_POSTFIX         VAR ${O_CUR_POSTFIX})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_ARCHIVE_NAME    VAR ${_ARCHIVE_NAME})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_RUNTIME_NAME    VAR ${_RUNTIME_NAME})

    _AssignTargetDestParseArgs()

    _ScanSeparateKit(${KIT_NAME} ${KIT_DIR})

    # 清空变量缓存
    #_ClearSetupKitBuffer()

endfunction()

function(_SetupPlgM)
    cmake_parse_arguments(M "" "PLG_NAME" "" ${ARGN})

    if(NOT M_PLG_NAME)
        EMakeErrF("internal err, you must set param 'LIB_NAME'")
    endif()

    set(KIT_NAME ${M_PLG_NAME})
    set(KIT_DIR  ${PLGS_DIR}/${KIT_NAME})
    EMakeCheckExistsF(${KIT_DIR}/CMakeLists.txt)
    EMakeGetTargetPropertyM(${KIT_NAME} KIT_IS_INTERNAL _is_internal NO_CHECK)

    # 导入配置
    CONFIGURING_SETUP_SET(1)
    if(_is_internal)
        include(${PLGS_DIR}/${M_PLG_NAME}/CMakeLists.txt)
    else()
        EMakeErrf("not supported now")
    endif()
    CONFIGURING_SETUP_SET(0)

    EMakeGetGlobalPropertyM(PLGS_DIR PLGS_DIR)
    set(KIT_NAME ${M_PLG_NAME})
    set(KIT_TYPE PLG)

    EMakeParseInputFiles("${KIT_SRCS}"      KIT_SRCS      ${PLGS_DIR}/${KIT_NAME}/CMakeLists.txt)
    EMakeParseInputFiles("${KIT_UI_FORMS}"  KIT_UI_FORMS  ${PLGS_DIR}/${KIT_NAME}/CMakeLists.txt)
    EMakeParseInputFiles("${KIT_RESOURCES}" KIT_RESOURCES ${PLGS_DIR}/${KIT_NAME}/CMakeLists.txt)

    # 检查
    _CheckValM(${KIT_NAME} KIT_LIBRARY_TYPE "^STATIC$|^SHARED$|^MODULE$" "MODULE"   NOTE "you can only set [STATIC|SHARED|MODULE] or a empty val")
    _CheckValM(${KIT_NAME} KIT_EXPORT       "^ON$|^OFF$"                 "OFF"      NOTE "you can only set [ON|OFF|] or a empty val")
    _CheckValM(${KIT_NAME} KIT_EXPORT_DIR   ""                           ${COMMONS_DIR}/plugins/${KIT_NAME})
    _CheckTargetNameM(${KIT_NAME} "${KIT_TARGET}" )

    EBuildCheckDependsM(${KIT_NAME} "${KIT_DEPENDS}")

    _ParsePostfix("${KIT_POSTFIX}")

    # 获取必要的值
    file(RELATIVE_PATH _relative_path ${MAIN_PROJECT_DIR} ${PLGS_DIR}/${M_PLG_NAME})

    # 设置可配置的目标属性
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DEBUG           VAR      "${KIT_DEBUG}" DEFAULT)
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_TARGET          VAR      ${KIT_TARGET})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_LIBRARY_TYPE    VAR      ${KIT_LIBRARY_TYPE})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_EXPORT          VAR      ${KIT_EXPORT})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_EXPORT_DIR      VAR      ${KIT_EXPORT_DIR})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_PREFIX          VAR      ${KIT_PREFIX})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DEFINITIONS     VAR      ${KIT_DEFINITIONS})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_SRCS            LIST_VAR ${KIT_SRCS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_SRCS_PLUGIN     LIST_VAR ${KIT_SRCS_PLUGIN})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_MOC_SRCS        LIST_VAR ${KIT_MOC_SRCS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_UI_FORMS        LIST_VAR ${KIT_UI_FORMS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_RESOURCES       LIST_VAR ${KIT_RESOURCES})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DEPENDS         LIST_VAR ${KIT_DEPENDS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DERECT_DEPENDS  LIST_VAR ${KIT_DEPENDS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_LIBRARIES_SYS   LIST_VAR ${KIT_LIBRARIES_SYS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_INCLUDE_DIRS    LIST_VAR ${KIT_INCLUDE_DIRS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_LINK_DIRS       LIST_VAR ${KIT_LINK_DIRS}    APPEND)

    # 设置其它相关的目标属性
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_SOURCES_DIR     VAR      ${PLGS_DIR}/${M_PLG_NAME})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_BUILD_DIR       VAR      ${PROJECT_BINARY_DIR}/${_relative_path})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_OUTPUT_DIR      VAR      ${PLGS_OUTPUT_DIR})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_IS_QT_PROJECT   VAR      ${KIT_IS_QT_PROJECT})

    # 设置文件
    set(_RUNTIME_NAME)
    set(_ARCHIVE_NAME)
    if(DEFINED KIT_NAME)
        set(_OUTPUT_NAME ${KIT_NAME}${O_CUR_POSTFIX})

        set(_PREFIX ${_DF_LIB_PREFIX})
        if(KIT_PREFIX)
            set(_PREFIX ${KIT_PREFIX})
        endif()

        if(${KIT_LIBRARY_TYPE} MATCHES "STATIC")
            set(_ARCHIVE_NAME ${_PREFIX}${_OUTPUT_NAME}${_DF_LIB_ARCHIVE_EXTENTION})
        elseif(${KIT_LIBRARY_TYPE} MATCHES "SHARED")
            if(WIN32)
                set(_RUNTIME_NAME ${_PREFIX}${_OUTPUT_NAME}${_DF_LIB_RUNTIME_EXTENTION})
                set(_ARCHIVE_NAME ${_PREFIX}${_OUTPUT_NAME}${_DF_LIB_ARCHIVE_EXTENTION})
            else()
                set(_RUNTIME_NAME ${_PREFIX}${_OUTPUT_NAME}${_DF_LIB_RUNTIME_EXTENTION})
                set(_ARCHIVE_NAME ${_PREFIX}${_OUTPUT_NAME}${_DF_LIB_RUNTIME_EXTENTION})
            endif()
        elseif(${KIT_LIBRARY_TYPE} MATCHES "MODULE")
            set(_RUNTIME_NAME ${_PREFIX}${_OUTPUT_NAME}${_DF_LIB_RUNTIME_EXTENTION})
        endif()
    endif()
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_POSTFIX         VAR ${O_CUR_POSTFIX})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_ARCHIVE_NAME    VAR ${_ARCHIVE_NAME})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_RUNTIME_NAME    VAR ${_RUNTIME_NAME})

    _AssignTargetDestParseArgs()

    # 清空变量缓存
    #_ClearSetupKitBuffer()

endfunction()

function(_SetupAppM)

    cmake_parse_arguments(M "" "APP_NAME" "" ${ARGN})

    if(NOT M_APP_NAME)
        EMakeErrF("internal err, you must set param 'APP_NAME'")
    endif()

    EMakeCheckExistsF(${APPS_DIR}/${M_APP_NAME}/CMakeLists.txt)

    set(KIT_NAME ${M_APP_NAME})
    set(KIT_DIR  ${APPS_DIR}/${KIT_NAME})
    #EMakeGetTargetPropertyM(${KIT_NAME} KIT_IS_INTERNAL _is_internal NO_CHECK)

    # 导入配置
    CONFIGURING_SETUP_SET(1)
    include(${APPS_DIR}/${M_APP_NAME}/CMakeLists.txt)
    CONFIGURING_SETUP_SET(0)

    EMakeGetGlobalPropertyM(APPS_DIR APPS_DIR)
    set(KIT_NAME ${M_APP_NAME})
    set(KIT_TYPE APP)

    EMakeParseInputFiles("${KIT_SRCS}"      KIT_SRCS      ${APPS_DIR}/${KIT_NAME}/CMakeLists.txt)
    EMakeParseInputFiles("${KIT_UI_FORMS}"  KIT_UI_FORMS  ${APPS_DIR}/${KIT_NAME}/CMakeLists.txt)
    EMakeParseInputFiles("${KIT_RESOURCES}" KIT_RESOURCES ${APPS_DIR}/${KIT_NAME}/CMakeLists.txt)

    # 检查
    _CheckValM(${KIT_NAME} KIT_EXPORT       "^ON$|^OFF$"                 "OFF"      NOTE "you can only set [ON|OFF|] or a empty val")
    _CheckValM(${KIT_NAME} KIT_EXPORT_DIR   ""                           ${COMMONS_DIR}/applications/${KIT_NAME})
    _CheckTargetNameM(${KIT_NAME} "${KIT_TARGET}" )

    EBuildCheckDependsM(${KIT_NAME} "${KIT_DEPENDS}")

    _ParsePostfix("${KIT_POSTFIX}")

    # 获取必要的值
    file(RELATIVE_PATH _relative_path ${MAIN_PROJECT_DIR} ${APPS_DIR}/${M_APP_NAME})

    # 设置可配置的目标属性
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DEBUG           VAR      "${KIT_DEBUG}" DEFAULT)
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_TARGET          VAR      ${KIT_TARGET})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_EXPORT          VAR      ${KIT_EXPORT})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_EXPORT_DIR      VAR      ${KIT_EXPORT_DIR})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DEFINITIONS     VAR      ${KIT_DEFINITIONS})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_SRCS            LIST_VAR ${KIT_SRCS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_SRCS_MAIN       LIST_VAR ${KIT_SRCS_MAIN})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_MOC_SRCS        LIST_VAR ${KIT_MOC_SRCS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_UI_FORMS        LIST_VAR ${KIT_UI_FORMS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_RESOURCES       LIST_VAR ${KIT_RESOURCES})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DEPENDS         LIST_VAR ${KIT_DEPENDS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DERECT_DEPENDS  LIST_VAR ${KIT_DEPENDS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_LIBRARIES_SYS   LIST_VAR ${KIT_LIBRARIES_SYS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_INCLUDE_DIRS    LIST_VAR ${KIT_INCLUDE_DIRS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_LINK_DIRS       LIST_VAR ${KIT_LINK_DIRS}    APPEND)

    # 设置其它相关的目标属性
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_SOURCES_DIR     VAR      ${APPS_DIR}/${M_APP_NAME})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_BUILD_DIR       VAR      ${PROJECT_BINARY_DIR}/${_relative_path})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_OUTPUT_DIR      VAR      ${APPS_OUTPUT_DIR})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_IS_QT_PROJECT   VAR      ${KIT_IS_QT_PROJECT})

    # 设置文件
    set(_OUTPUT_NAME ${KIT_NAME}${O_CUR_POSTFIX})
    if(WIN32)
        set(_RUNTIME_NAME ${_OUTPUT_NAME}${_DF_APP_RUNTIME_EXTENTION})
    else()
        set(_RUNTIME_NAME ${_OUTPUT_NAME}${_DF_APP_RUNTIME_EXTENTION})
    endif()
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_POSTFIX         VAR ${O_CUR_POSTFIX})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_RUNTIME_NAME    VAR ${_RUNTIME_NAME})

    _AssignTargetDestParseArgs()

    # 清空变量缓存
    #_ClearSetupKitBuffer()
endfunction()

function(_SetupTypeM)

    cmake_parse_arguments(M "" "TYPE_NAME" "" ${ARGN})

    EMakeCheckExistsF(${PRODS_DIR}/${M_TYPE_NAME}/CMakeLists.txt)

    set(KIT_NAME ${M_TYPE_NAME})
    set(KIT_DIR  ${PRODS_DIR}/${KIT_NAME})
    #EMakeGetTargetPropertyM(${KIT_NAME} KIT_IS_INTERNAL _is_internal NO_CHECK)

    # 导入配置
    CONFIGURING_SETUP_SET(1)
    include(${PRODS_DIR}/${KIT_NAME}/CMakeLists.txt)
    CONFIGURING_SETUP_SET(0)

    # 检查
#    _CheckValM(${KIT_NAME} KIT_COPY_RUNTIMES      "^ON$|^OFF$"                 "ON"      NOTE "you can only set [ON|OFF|] or a empty val")
#    _CheckValM(${KIT_NAME} KIT_COPY_ARCHIVES      "^ON$|^OFF$"                 "OFF"     NOTE "you can only set [ON|OFF|] or a empty val")
#    _CheckValM(${KIT_NAME} KIT_COPY_FILES         "^ON$|^OFF$"                 "ON"      NOTE "you can only set [ON|OFF|] or a empty val")
#    _CheckValM(${KIT_NAME} KIT_COPY_DIRS          "^ON$|^OFF$"                 "ON"      NOTE "you can only set [ON|OFF|] or a empty val")
    _CheckTargetNameM(${KIT_NAME} "${KIT_TARGET}" )

    EBuildCheckDependsM(${KIT_NAME} "${KIT_DEPENDS}")

    # 设置属性
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DEBUG           VAR      "${KIT_DEBUG}" DEFAULT)
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DEPENDS         LIST_VAR ${KIT_DEPENDS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_DERECT_DEPENDS  LIST_VAR ${KIT_DEPENDS})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_LIBRARIES_SYS   LIST_VAR ${KIT_LIBRARIES_SYS})

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_COPY_RUNTIMES   VAR      ON)
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_COPY_ARCHIVES   VAR      OFF)
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_COPY_FILES      VAR      ON)
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_COPY_DIRS       VAR      ON)

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_SOURCES_DIR     VAR      ${PRODS_DIR}/${KIT_NAME})

    EMakeGetTargetPropertyM(${KIT_NAME} BLONG_TO  _prod)
    EMakeGetTargetPropertyM(${KIT_NAME} TYPE_NAME _type)

    EMakeSetTargetPropertyM(${KIT_NAME} KIT_PRODUCT_DIR      VAR     ${EXPORTS_DIR}/${_prod}${_ver}_${INSTALL_POSTFIX}_${_CUR_BUILD_TYPE_LOWER})
    EMakeSetTargetPropertyM(${KIT_NAME} KIT_INSTALL_DIR      VAR     ${EXPORTS_DIR}/${_prod}${_ver}_${INSTALL_POSTFIX}_${_CUR_BUILD_TYPE_LOWER}/${_type})

    EMakeSetTargetPropertyM(${_prod}    KIT_INSTALL_DIR      VAR     ${EXPORTS_DIR}/${_prod}${_ver}_${INSTALL_POSTFIX}_${_CUR_BUILD_TYPE_LOWER})

    _AssignTargetDestParseArgs()

    #_ClearSetupKitBuffer()

endfunction()

macro(_SetupProdM)

endmacro()

#
# 检查 kit 的依赖：
#   依次检查：已知的的kit
#           检查是否为 package   如果为 package，则从 KIT_DEPENDS 中删除，并添加到 KIT_LIBRARIES_SYS 中
#           检查是否为 library   如果为 library，则从 KIT_DEPENDS 中删除，并添加到 KIT_LIBRARIES_SYS 中
#
#       若全部失败，则追加错误信息到当前 kit
#
#
macro(_CheckUnkownsDependencyKits)

    EMakeGetGlobalPropertyM(KNOWN_TARGETS KNOWN_TARGETS NO_CHECK)

    set(_updated)

    foreach(_kit ${ARGN})

        EMakeGetTargetPropertyM(${_kit} KIT_DEPENDS   _deps NO_CHECK)
        EMakeGetTargetPropertyM(${_kit} KIT_LINK_DIRS _dirs NO_CHECK)

        foreach(_dep ${_deps})

            # 从已有的 target 中查找
            list(FIND KNOWN_TARGETS ${_dep} _o_index)
            if(_o_index EQUAL -1)       # 没有找到

                string(REGEX REPLACE "^-l" "" _check "${_dep}")

                if(TARGET "${_dep}")
                    list(REMOVE_ITEM _deps ${_dep})
                    set(_updated 1)

                    EMakeSetTargetPropertyM(${_kit} KIT_LIBRARIES_SYS VAR ${_dep} APPEND)
                    continue()
                endif()

                find_package(${_check} QUIET)  # 查找看是否是一个 package

                if(NOT ${_check}_FOUND)        # 不是 package

                    find_library(LIB_${_check} NAMES ${_check} PATHS ${_dirs})      # 查找看是否是一个系统库

                    if(${LIB_${_check}} STREQUAL LIB_${_check}-NOTFOUND)
                        EMakeSetTargetPropertyM(${_dep} ERR_INFO VAR "kit '${_kit}' depends on unknown target: \n    ${_dep}" APPEND)

                        continue()
                    endif()
                endif()

                list(REMOVE_ITEM _deps ${_dep})
                set(_updated 1)

                if(LIB_${_check}_FOUND)

                    EMakeSetTargetPropertyM(${_kit} KIT_LIBRARIES_SYS VAR -l${_dep} APPEND)

                endif()

            endif()

        endforeach()

        if(_updated)
            EMakeSetTargetPropertyM(${_kit} KIT_DEPENDS LIST_VAR ${_deps})
        endif()

    endforeach()

endmacro()

macro(EBuildSetupKitsM)

    _ClearSetupKitBuffer()
    EMakeGetProjectDirF(MAIN_PROJECT_DIR)

    foreach(_type EXTS LIBS PLGS APPS PRODS)
        EMakeGetGlobalPropertyM(KNOWN_${_type}_KITS KNOWN_${_type}_KITS NO_CHECK)
    endforeach()

    foreach(_ext ${KNOWN_EXTS_KITS})
        _SetupExtM(EXT_NAME ${_ext})
    endforeach()

    foreach(_lib ${KNOWN_LIBS_KITS})
        _SetupLibM(LIB_NAME ${_lib})
    endforeach()

    foreach(_plg ${KNOWN_PLGS_KITS})
        _SetupPlgM(PLG_NAME ${_plg})
    endforeach()

    foreach(_app ${KNOWN_APPS_KITS})
        _SetupAppM(APP_NAME ${_app})
    endforeach()

    foreach(_type ${KNOWN_TYPES_KITS})
        _SetupTypeM(TYPE_NAME ${_type})
    endforeach()

    foreach(_type EXTS LIBS PLGS APPS PRODS)
        _CheckUnkownsDependencyKits(${KNOWN_${_type}_KITS})
    endforeach()

endmacro()
