# =====================================================================================
#
#       Filename:  EBuildAddM.cmake
#
#    Description:  add build to the project
#
#        Version:  1.0
#        Created:  2017.12.17 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

set(_EXT_TEMPLATES_DIR  ${SUB_SYSTEM_DIR}/templates/ext)
set(_LIB_TEMPLATES_DIR  ${SUB_SYSTEM_DIR}/templates/lib)
set(_PLG_TEMPLATES_DIR  ${SUB_SYSTEM_DIR}/templates/plg)
set(_APP_TEMPLATES_DIR  ${SUB_SYSTEM_DIR}/templates/app)
set(_TYPE_TEMPLATES_DIR ${SUB_SYSTEM_DIR}/templates/type)
set(_PROD_TEMPLATES_DIR ${SUB_SYSTEM_DIR}/templates/product)

set(TYPES_DIR ${PRODS_DIR})

#
# check the logical_expr and set enable_msg to varname
#

function(_fAddBuildLogicalToMessage varname logical_expr)

    set(enabling_msg)

    foreach(arg ${logical_expr})
        if(NOT "${${arg}}" STREQUAL "")
            set(value_as_int 0)
            if(${${arg}})
                set(value_as_int 1)
            endif()
            set(enabling_msg "${enabling_msg} ${arg}:${value_as_int}")
        else()
            set(enabling_msg "${enabling_msg} ${arg}")
        endif()
    endforeach()

    set(${varname} ${enabling_msg} PARENT_SCOPE)

endfunction()

#
#
function(_createTemplatesForKit i_name i_type)

    set(_dir ${${i_type}S_DIR}/${i_name})

    if(EXISTS ${_dir})

        if(NOT IS_DIRECTORY ${_dir})
            EMakeErrF("${_dir} exist but not a directory")
        endif()

    else()
        file(MAKE_DIRECTORY ${_dir})
    endif()

    if(EXISTS ${_dir}/CMakeLists.txt)
        EMakeInfF("created already of '${i_type}::${i_name}'")
        return()
    else()
        EMakeInfF("creating '${i_type}::${i_name}' at ${_dir}")
        file(COPY ${_${i_type}_TEMPLATES_DIR}/CMakeLists.txt DESTINATION ${_dir})
    endif()

    if(${i_type} STREQUAL "APP")

        if(EXISTS $${_dir}/${i_name}.c OR EXISTS $${_dir}/${i_name}.cpp)
        else()

            file(COPY ${_${i_type}_TEMPLATES_DIR}/main.c DESTINATION ${_dir})
            file(RENAME ${_dir}/main.c ${_dir}/${i_name}.c)

            EMakeInfF("creating '${i_type}::${i_name}' at ${_dir}")

        endif()

    endif()

endfunction()

#
# 检查两个文件是否相同
#   设置两个变量：
#       IS_SAME : 当两个文件相同时至为 1，否则为 0
#       IS_DIFF : 当两个文件不同时至为 1，否则为 0
#
function(_fileCompare i_f1 i_f2)

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E compare_files "${i_f1}" "${i_f2}"
        RESULT_VARIABLE     RESULT_VAR
        OUTPUT_VARIABLE     _is_diff
        ERROR_VARIABLE      _error
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT RESULT_VAR)
        set(IS_SAME 1 PARENT_SCOPE)
        set(IS_DIFF 0 PARENT_SCOPE)
    else()
        set(IS_SAME 0 PARENT_SCOPE)
        set(IS_DIFF 1 PARENT_SCOPE)
    endif()

endfunction()

function(_dirCompare _dir1 _dir2)

    set(ERR_CODE  PARENT_SCOPE)
    set(ERR_INFO  PARENT_SCOPE)
    set(ERR_FILES)

    set(_dir1_exsit 1)

    if(EXISTS ${_dir1})
        if(NOT IS_DIRECTORY ${_dir1})
            set(ERR_INFO "${_dir1} exist but not a directory" PARENT_SCOPE)
            set(ERR_CODE DIR1_NOT_DIR PARENT_SCOPE)
            return()
        endif()
    else()
        set(_dir1_exsit 0)
    endif()

    if(EXISTS ${_dir2})
        if(NOT IS_DIRECTORY ${_dir2})
            set(ERR_INFO "${_dir2} exist but not a directory" PARENT_SCOPE)
            set(ERR_CODE DIR2_NOT_DIR PARENT_SCOPE)
            return()
        endif()

        if(NOT _dir1_exsit)
            set(ERR_INFO "${_dir1} is not exist" PARENT_SCOPE)
            set(ERR_CODE DIR1_NOT_EXIST PARENT_SCOPE)
            return()
        endif()

    else()
        if(NOT _dir1_exsit)
            set(ERR_INFO "${_dir1} and ${_dir2} are not exist" PARENT_SCOPE)
            set(ERR_CODE DIRS_NOT_EXIST PARENT_SCOPE)
            return()
        else()
            set(ERR_INFO "${_dir2} is not exist" PARENT_SCOPE)
            set(ERR_CODE DIR2_NOT_EXIST PARENT_SCOPE)
            return()
        endif()

    endif()

    file(GLOB _files1 RELATIVE ${_dir1} ${_dir1}/*)
    file(GLOB _files2 RELATIVE ${_dir2} ${_dir2}/*)

#    EMakeInfF("${_dir1}:[${_files1}]")
#    EMakeInfF("${_dir2}:[${_files2}]")

    list(LENGTH _files1 _cnt1)
    list(LENGTH _files2 _cnt2)

    if(NOT _cnt1 EQUAL _cnt2)
        set(ERR_INFO "
    the files count is not match
    in  src      dir: ${_dir1}
    and templete dir: ${_dir2}" PARENT_SCOPE)
        set(ERR_CODE CNT_NOT_MATCH PARENT_SCOPE)
        return()
    endif()

    foreach(_file ${_files1})

        if(NOT EXISTS ${_dir2}/${_file})
            set(ERR_INFO "${_file} in ${_dir1} is not exist in ${_dir2}" PARENT_SCOPE)
            set(ERR_CODE FILENAME_NOT_MATCH PARENT_SCOPE)
            list(APPEND ERR_FILES ${_file})
            continue()
        endif()

        _fileCompare(${_dir1}/${_file} ${_dir2}/${_file})
        if(IS_DIFF)
            set(ERR_INFO "${_file} in ${_dir1} and ${_dir2} is not match" PARENT_SCOPE)
            set(ERR_CODE FILE_NOT_MATCH PARENT_SCOPE)
            list(APPEND ERR_FILES ${_file})
            break()
        endif()
    endforeach()

    set(ERR_FILES ${ERR_FILES} PARENT_SCOPE)

endfunction()

function(_deleteTemplatesForKit i_name i_type)

    set(_can_delete)

    set(_dir ${${i_type}S_DIR}/${i_name})
    _dirCompare(${_dir} ${_${i_type}_TEMPLATES_DIR})

    if(NOT ERR_CODE)
        set(_can_delete 1)
    elseif("${ERR_CODE}" MATCHES "DIR[1|S]_NOT_EXIST")
        #EMakeInfF("deleted already of '${i_type}::${i_name}'")
    elseif("${ERR_CODE}" STREQUAL  "FILENAME_NOT_MATCH")

        if(${ERR_FILES} STREQUAL "${i_name}.c")

            _fileCompare(${_dir}/${i_name}.c ${_${i_type}_TEMPLATES_DIR}/main.c )
            if(IS_SAME)
                set(_can_delete 1)
            else()
                EMakeErrF("delete src files err for kit '${i_type}::${i_name}': [FILE_NOT_MATCH]
    src     : ${_dir}/${i_name}.c
    template: ${_${i_type}_TEMPLATES_DIR}/main.c")
            endif()
        endif()
    elseif("${ERR_CODE}" STREQUAL "CNT_NOT_MATCH" AND "${i_type}" STREQUAL "PROD")
    else()
        EMakeErrF("delete src files err for kit '${i_type}::${i_name}': ${ERR_INFO}")
    endif()

    if(_can_delete)
        file(REMOVE_RECURSE ${_dir})
        EMakeInfF("deleted '${i_type}::${i_name}' at ${_dir}")
    endif()

endfunction()

#
#
#   1. 设置 option ${category}_${name} 为 ${on_off}
#   2. 添加 ${name} 到 _G_${category}S_
#   3. 检查 ARGN, 若逻辑表达式为 true, 将强制设置 ${category}_${name} 为 ON
#
macro(_mAddBuild category name doc on_off)

    cmake_parse_arguments(M "CREATE;DELETE;DEBUG;Debug;D;d" "INDEX;PROD;" "" ${ARGN})

    set(_name ${name})
    if(${category} MATCHES "TYPE")

        if(NOT M_PROD)
            EMakeErrF("internal err: no product name setted when add a target to TYPE")
        endif()

        set(_name ${M_PROD}/${name})
        set(_prod ${M_PROD})

        EMakeSetTargetPropertyM(${_name} TYPE_NAME   VAR ${name})
        EMakeSetTargetPropertyM(${_name} BLONG_TO    VAR ${_prod})

    endif()

    if(M_DELETE)

        if(NOT ${category} STREQUAL "PROD")

            _deleteTemplatesForKit(${_name} ${category})

            if(${category} STREQUAL "TYPE")
                _deleteTemplatesForKit(${_prod} PROD)
            endif()

        endif()

    else()

        option(${category}_${_name} "${doc}" ${on_off})
        set(${category}_${_name} ${on_off})
        mark_as_advanced(${category}_${_name})

        list(FIND _G_ADDED_KIT_NAMES_ ${_name} _o_index)

        # not found
        if(_o_index EQUAL -1)
            if(M_INDEX)
                list(INSERT _G_${category}S_ ${M_INDEX} ${_name})
            else()
                list(APPEND _G_${category}S_ ${_name})
            endif()

            list(APPEND _G_ADDED_KIT_NAMES_ ${_name})

        # found -> err
        else()
            EMakeGetTargetPropertyM(${_name} KIT_TYPE _t_type)
            EMakeErrF("the target '${_name}' that you want to add to '${category}S' is added to '${_t_type}' already")
        endif()

        # copy file
        if(M_CREATE)
            _createTemplatesForKit(${_name} ${category})
        endif()

        set(_logical_expr ${M_UNPARSED_ARGUMENTS})

        if(_logical_expr AND NOT ${category}_${_name})
            if(${ARGN})
                set(${category}_${_name} ON)
                set(enabling_msg)
                _fAddBuildLogicalToMessage(enabling_msg "${ARGN}")
                EMakeInfF("Enabling [${category}_${_name}] because of [${enabling_msg}] evaluates to True")
            endif()
        endif()

        set(${category}_${_name}_SKIPCACHEVALUE 1)

        EMakeSetGlobalPropertyM(${category}S LIST_VAR ${_G_${category}S_})
        EMakeSetGlobalPropertyM(ADDED_KITS   LIST_VAR ${_G_ADDED_KIT_NAMES_})

        EMakeSetTargetPropertyM(${_name} OPTION_NAME VAR ${category}_${_name})
        EMakeSetTargetPropertyM(${_name} KIT_TYPE    VAR ${category})

        if(M_DEBUG OR M_Debug OR M_D OR M_d)
            EMakeSetTargetPropertyM(${_name} KIT_DEBUG    VAR 1)
        endif()

    endif()

endmacro()

#
# \! 添加 外部库
#   1. 设置 option EXT_${name} 为 ${on_off}
#   2. 添加 ${name} 到 EXTS 中
#
macro(EBuildAddExtM name on_off doc)

    _mAddBuild(EXT ${name} "${doc}" ${on_off} ${ARGN})

endmacro()

#
# \! 添加 lib
#   1. 设置 option LIB_${name} 为 ${on_off}
#   2. 添加 ${name} 到 LIBS 中
#
macro(EBuildAddLibM name on_off doc)

    _mAddBuild(LIB ${name} "${doc}" ${on_off} ${ARGN})

endmacro()

#
# \! 添加 plugin
#   1. 设置 option PLG_${name} 为 ${on_off}
#   2. 添加 ${name} 到 PLGS 中
#
macro(EBuildAddPlgM name on_off doc )
    _mAddBuild(PLG ${name} "${doc}" ${on_off} ${ARGN})
endmacro()

#
# \! 添加 app
#   1. 设置 option APP_${name} 为 ${on_off}
#   2. 添加 ${name} 到 APPS 中
#
macro(EBuildAddAppM name on_off doc)
    _mAddBuild(APP ${name} "${doc}" ${on_off} ${ARGN})
endmacro()

#
# \! 添加产品和版本
#   1. 根据 name 解析 prod 和 type，如：product1：type1 解析 product为 product1，type 为 type1
#   2. 设置 option PROD_${prod} 和 TYPE_${type} 为 ${on_off}
#   3. 添加 ${prod} 到 PRODS 中, 添加 ${type} 到 TYPES 中
#   4. 检查 ARGN, 若 逻辑表达式 为 true，将强制设置 PROD_${product} 和 TYPE_${type} 为 ON
#   5. 若 BUILD_ALL_PRODS 为 true，将强制设置 PROD_${prod} 和 TYPE_${type} 为 ON
#   6. 若 BUILD_ALL_TYPES 为 true，将强制设置 TYPE_${type} 为 ON
#
# \! usage:
#   添加产品：mAddProdM(product ...)
#   添加版本：mAddProdM(product:type ...)      # 若此产品不存在，将会自动创建
#
macro(EBuildAddProdM name on_off doc)

    set(_pro)
    set(_type)

    # 解析 prod 和 type
    string(REGEX REPLACE ":|/" ";" _list ${name})
    set(_list "${_list}")

    list(LENGTH _list _len)
    if(${_len} EQUAL 1)
        list(GET _list 0 _prod)
    elseif(${_len} EQUAL 2)
        list(GET _list 0 _prod)
        list(GET _list 1 _type)
    else()
        EMakeLogErrF("error format of 'name' when add prod: ${name}")
    endif()

    if(_type)

        list(FIND _G_PRODS_ ${_prod} _o_index)

        if(${_o_index} EQUAL -1)
            _mAddBuild(PROD ${_prod} "${doc}" ${on_off} ${ARGN})
        endif()

        _mAddBuild(TYPE ${_type} "${doc}" ${on_off} PROD ${_prod} ${ARGN})

        if(BUILD_ALL_PRODS AND NOT ${TYPE_${_type}})
            set(PROD_${_type} 1)
            message(STATUS "Enabling [TYPE_${_type}] because of [BUILD_ALL_PRODS:${BUILD_ALL_PRODS}] evaluates to True")
        elseif(BUILD_ALL_TYPES AND NOT ${TYPE_${_type}})
            set(PROD_${_type} 1)
            message(STATUS "Enabling [TYPE_${_type}] because of [BUILD_ALL_TYPES:${BUILD_ALL_TYPES}] evaluates to True")
        endif()

        # 添加 type 到产品 prod 的 type 列表中
        list(FIND ARGN "DELETE" DELETE)
        if(NOT DELETE)
            EMakeSetTargetPropertyM(${_prod} TYPES VAR ${_type} APPEND)
        endif()

    else()
        _mAddBuild(PROD ${_prod} "${doc}" ${on_off} ${ARGN})
        if(BUILD_ALL_PRODS AND NOT ${PROD_${_prod}})
            set(PROD_${_prod} 1)
            message(STATUS "Enabling [PROD_${_prod}] because of [BUILD_ALL_PRODS:${BUILD_ALL_PRODS}] evaluates to True")
        endif()
    endif()

endmacro()

