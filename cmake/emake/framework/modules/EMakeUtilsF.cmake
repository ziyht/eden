# =====================================================================================
#
#       Filename:  EMakeUtilsF.cmake
#
#    Description:  some brief tools
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
# !\ 获取主项目根目录
#   根据是否含有标志性文件 ./CMakeLists.txt 和 ./cmake/EMakeUtilsF.cmake 进行判断
#
function(EMakeGetProjectDirF)

    cmake_parse_arguments(M "MAIN_PROJECT_ROOT_DIR;MAIN_PROJECT_DIR" "" "" ${ARGN})

    if(NOT M_MAIN_PROJECT_ROOT_DIR AND NOT M_MAIN_PROJECT_DIR)
        return()
    endif()

    if(M_MAIN_PROJECT_ROOT_DIR)
        EMakeGetGlobalPropertyM(MAIN_PROJECT_ROOT_DIR _o_dir NO_CHECK)
        if(${_o_dir})
            set(MAIN_PROJECT_ROOT_DIR ${_o_dir} PARENT_SCOPE)
        else()
            set(MAIN_PROJECT_ROOT_DIR_NEEDED 1)
        endif()
    endif()

    if(M_MAIN_PROJECT_DIR)
        EMakeGetGlobalPropertyM(MAIN_PROJECT_DIR _o_dir NO_CHECK)
        if(${_o_dir})
            set(MAIN_PROJECT_DIR ${_o_dir} PARENT_SCOPE)
        else()
            set(MAIN_PROJECT_DIR_NEEDED 1)
        endif()
    endif()

    if(NOT MAIN_PROJECT_ROOT_DIR_NEEDED AND NOT MAIN_PROJECT_DIR_NEEDED)
        return()
    endif()

    set(_check_1 "")
    set(_check_2 "cmake/emake/framework/modules/EmakeFramework.cmake")

    set(_root_dir)      # 主项目根目录
    set(_project_dir)   # 主项目目录
    set(_src_dir)       # 代码目录
    set(_parent_dir)    # 本项目父级目录

    set(_path ${CMAKE_CURRENT_SOURCE_DIR})

    set(PROJECT_DIR_FOUND 1)
    set(_project_dir ${CMAKE_CURRENT_SOURCE_DIR})

    while(EXISTS ${_path})

        #mLogDbg(---------------${_path})
        if(EXISTS ${_path}/${_check_1} AND EXISTS ${_path}/${_check_2} )
            set(_root_dir ${_path})
            set(ROOT_DIR_FOUND    1)

            if(_parent_dir AND NOT _src_dir)
                set(_src_dir ${_path})
            endif()

            break()
        endif()

        string(REGEX REPLACE "([A-Z]:|/([^/]*))$" "" _path ${_path})

        emakeinff(${_path})

        if(_parent_dir AND NOT _src_dir)
            set(_src_dir ${_path})
        endif()

        if(NOT _parent_dir)
            set(_parent_dir ${_path})
        endif()

    endwhile()

    if(NOT ROOT_DIR_FOUND)
        set(_root_dir ${_project_dir})
        set(ROOT_DIR_FOUND    1)
    endif()

    if(ROOT_DIR_FOUND)
        EMakeSetGlobalPropertyM(MAIN_PROJECT_ROOT_DIR VAR ${_root_dir})

        set(MAIN_PROJECT_ROOT_DIR ${_root_dir} CACHE STRING "main project root dir scaned by emake" FORCE)

        if(MAIN_PROJECT_ROOT_DIR_NEEDED)
            set(MAIN_PROJECT_ROOT_DIR ${_root_dir} PARENT_SCOPE)
            set(MAIN_PROJECT_ROOT_DIR_NEEDED)
        endif()

    endif()

    if(PROJECT_DIR_FOUND)
        EMakeSetGlobalPropertyM(MAIN_PROJECT_DIR VAR ${_project_dir})

        set(MAIN_PROJECT_DIR ${_project_dir} CACHE STRING "main project dir, this dir have the main CMakeLists.txt file of current project" FORCE)

        if(MAIN_PROJECT_DIR_NEEDED)
            set(MAIN_PROJECT_DIR ${_project_dir} PARENT_SCOPE)
            set(MAIN_PROJECT_DIR_NEEDED)
        endif()

    endif()

    if(MAIN_PROJECT_ROOT_DIR_NEEDED)
        EMakeErrF("can not found MAIN_PROJECT_ROOT_DIR, it needs '${_check_1}' and '${_check_2}' in it, please check you files")
    endif()

    if(MAIN_PROJECT_DIR_NEEDED)
        #EMakeErrF("can not found MAIN_PROJECT_DIR, it needs '${_check_3}' in it, please check you files")
    endif()

endfunction()

#
# \! 检查目录或文件是否存在，若不存在，报错并退出 cmake
#   EMakeCheckExistsF(path)          # path: 路径或路径变量
#
function(EMakeCheckExistsF i_path)

    if(NOT EXISTS ${i_path})

        if(NOT DEFINED ${i_path})
            EMakeErrF("'${i_path}' is not exist or not defined")
        elseif(NOT EXISTS ${${i_path}})
            EMakeErrF("[${i_path}:${${i_path}}] is not exist")
        endif()

    endif()

endfunction()

#
# \! 检查变量是否被定义，若未定义，报错并退出 cmake
#   EMakeCheckDefinedF(var_name [PRE_DOC])
#

function(EMakeCheckDefinedF i_var_name)

    cmake_parse_arguments(M "" "PRE_DOC" "" ${ARGN})

    if(NOT DEFINED ${i_var_name})

        if(M_PRE_DOC)
            EMakeErrF("${M_PRE_DOC} is not defined yet")
        else()
            EMakeErrF("${i_var_name} is not defined yet")
        endif()

    endif()

endfunction()

#
# 平台相关, PATH, LD_LIBRARY_PATH 或 DYLD_LIBRARY_PATH 中会添加上 ${PATH_LIST} 中的项
#
macro(EMakeSetPathsM PATH_LIST)
    foreach(p ${PATH_LIST})
        if(WIN32)
            set(ENV{PATH} "${p};$ENV{PATH}")
        elseif(APPLE)
            set(ENV{DYLD_LIBRARY_PATH} "${p}:$ENV{DYLD_LIBRARY_PATH}")
        else()
            set(ENV{LD_LIBRARY_PATH} "${p}:$ENV{LD_LIBRARY_PATH}")
        endif()
    endforeach()
endmacro()

#
# \! 确保文件内容中的目标库中只含有字串
#
macro(mCheckLibsInFileOnlyHaveStrings filepath)
    file(STRINGS "${filepath}" _stringtocheck) # 读取文件内容到 '_stringtocheck'
    string(REGEX MATCHALL "[^\\#]\\$\\{.*\\}" _incorrect_elements ${_stringtocheck})
    foreach(_incorrect_element ${incorrect_elements})
        string(REGEX REPLACE "\\$|\\{|\\}" "" _correct_element ${_incorrect_element})
        message(FATAL_ERROR "In ${_filepath}, ${_incorrect_element} should be replaced by ${_correct_element}")
    endforeach()
endmacro()

#
# \! 根据文件名称获取libname
#   libdemo.dll    -> demo
#   liblibdemo.dll -> libdemo
#
macro(EMakeGetLibnameFromFilenameM i_filename o_libname)

    if(NOT ${CMAKE_PREFIX_PATH} MATCHES "/msvc201")
        # 清除开始的 lib 字符
        if(${i_filename} MATCHES "^liblib")
            string(REGEX REPLACE "^liblib" "lib" _filename ${i_filename})
        else()
            string(REGEX REPLACE "^lib" "" _filename ${i_filename})
        endif()

    else()
        set(_filename ${i_filename})
    endif()

    # 清除 后缀
    string(REGEX REPLACE "\\.(lib|a|dll|dll.a)$" "" _filename ${_filename})

    set(${o_libname} ${_filename})
endmacro()


#
# \! 根据路径获取最底层 文件名 或 文件夹名，即取最终 / 后面的字符
#   1. 不检查路径是否实际可访问
#   2. 若最终 / 后没有名称，取最终 / 之前的名称
#   3. 若设置了 INDEX，则取 INDEX 所指定的项（以 / 或 \\ 分隔）
#
function(EMakeGetNameFromWholePathF i_path o_name)

    cmake_parse_arguments(M "" "INDEX" "" ${ARGN})

    set(${o_name})
    set(_path ${i_path})

    string(REGEX REPLACE "\\\\" ";" _path ${_path})
    string(REGEX REPLACE "/"    ";" _path ${_path})

    set(_path ${_path})

    if(M_INDEX)
        list(GET    _path ${M_INDEX} _name)
    else()
        list(GET    _path -1         _name)
    endif()

    set(${o_name} ${_name} PARENT_SCOPE)
endfunction()

#
# \! 从 list 中移除包含有 keyword 的项
#   1. 若设置了 OUTPUT , 最终结果写入指定的变量中，否则写入原 list 中
#   2. 若设置了 EXTRACT, 被删除的项将被写入指定的变量中
#
macro(EMakeListRemoveRegexsItemM io_list i_regexs)

    cmake_parse_arguments(M "" "OUTPUT;EXTRACT" "" ${ARGN})

    set(_list         ${${io_list}})
    set(_regexs       ${i_regexs})

    set(_filter_list    )

    foreach(_item ${_list})
        foreach(_regex ${_regexs})
            if(${_item} MATCHES ${_regex})
                list(REMOVE_ITEM _list        ${_item})
                list(APPEND      _filter_list ${_item})
            endif()
        endforeach()
    endforeach()

    if(M_OUTPUT)
        set(${M_OUTPUT} ${_list})
    else()
        set(${io_list}  ${_list})
    endif()

    if(M_EXTRACT)
        set(${M_EXTRACT} ${_filter_list})
    endif()

endmacro()

#
# \! 输入一个项目所在文件夹，解析它的项目名称
#   1. 若没有 CMakeLists.txt 文件 并且没有设置 TRY_READ，报错中断
#   2. 若 CMakeLists.txt 没有设置项目名称 并且没有设置 TRY_READ，报错中断
#   3. 若解析失败，但是设置了 TRY_READ，${out_name} 将被至为 空
#
#   EMakeReadProjectNameM(target_dir out_name [TRY_READ])
#
macro(EMakeReadProjectNameM target_dir out_name)

    cmake_parse_arguments(M "TRY_READ" "" "" ${ARGN})

    if(NOT M_TRY_READ)
        # 确保文件存在
        if(NOT EXISTS ${target_dir}/CMakeLists.txt)
            message(FATAL_ERROR "Target directory ${target_dir}/CMakeLists.txt doesn't exists !")
        endif()

        # 从 CMakeLists.txt 读取项目名称
        file(STRINGS     "${target_dir}/CMakeLists.txt"              _project_string
             REGEX       "^ *(P|p)(R|r)(O|o)(J|j)(E|e)(C|c)(T|t)\\("
             LIMIT_COUNT 10)
        string(REGEX MATCH   "\\((.*)\\)"    _project_name ${_project_string})
        string(REGEX REPLACE "\\(|\\)"    "" _project_name ${_project_name})
        if(${_project_name} STREQUAL "")
            message(FATAL_ERROR "Failed to extract project name from ${target_dir}/CMakeLists.txt")
        endif()

        set(${out_name} ${_project_name})
    else()

        set(${out_name})

        if(EXISTS ${target_dir}/CMakeLists.txt)
            # 从 CMakeLists.txt 读取项目名称
            file(STRINGS     "${target_dir}/CMakeLists.txt"              _project_string
                 REGEX       "^ *(P|p)(R|r)(O|o)(J|j)(E|e)(C|c)(T|t)\\("
                 LIMIT_COUNT 10)
            string(REGEX MATCH   "\\((.*)\\)"    _project_name ${_project_string})
            string(REGEX REPLACE "\\(|\\)"    "" _project_name ${_project_name})
            if(NOT ${_project_name} STREQUAL "")
                set(${out_name} ${_project_name})
            endif()
        endif()
    endif()

endmacro()

#
# \! 疏理路径, 去除路径中 ../.. ///
#
#  不支持 windows 路径中的 \, 可以通过 file() 先转换
#
# eg:
#   /../ab  -> /../ab
#   /ab/..  -> /
#
#
function(EMakeValidatePath i_path o_path_var)

#    set(_path ${i_path})

#    while(1)

#        string(REGEX REPLACE "//+"          "/" _path2 "${_path}")   # 去除 // /// ...
#        string(REGEX REPLACE "(([^/]+\\.\\.)|(\\.\\.[^/]+)|([^/]+\\.\\..+)|([^/\\.]+))/((\\.\\./)|(\\.\\.$))" "" _path2 "${_path2}")  # 去除 ab/..
#        string(REGEX REPLACE "//+"          "/" _path2 "${_path2}")  # 去除 // /// ... , 上一步操作后可能产生 // ///

#        if("${_path2}" STREQUAL "${_path}")
#            break()
#        else()
#            set(_path ${_path2})
#        endif()

#    endwhile()

#    set(${o_path_var} ${_path2} PARENT_SCOPE)

    if(WIN32)
        file(RELATIVE_PATH i_path "c:/" "c:/${i_path}")
        if(NOT i_path)
            set(i_path "/")
        endif()
        set(${o_path_var} ${i_path} PARENT_SCOPE)
    else()
        file(RELATIVE_PATH i_path "/"   "/${i_path}")
        set(${o_path_var} ${i_path} PARENT_SCOPE)
    endif()



endfunction()

function(EMakePlatformsPassed platforms o_passed)

    string(REGEX REPLACE "," ";" platforms ${platforms})

    set(_need 0)

    foreach(platform ${platforms})

        set(rev 0)

        if(${platform} MATCHES "^!")

            string(REGEX REPLACE "^!" "" platform ${platform})
            set(rev 1)

        endif()

        list(FIND PLATFORMS ${platform} _o_index)

        if(${_o_index} EQUAL -1)
            set(_ITEM ${platform}     PARENT_SCOPE)
            set(_ERR  "not supported" PARENT_SCOPE)

            set(${o_passed} 0 PARENT_SCOPE)

            return()
        endif()

        if(${platform} STREQUAL ${CUR_PLATFORM})

            if(NOT rev)
                set(${o_passed} 1 PARENT_SCOPE)
                return()
            endif()

        else()

            if(rev)
                set(${o_passed} 1 PARENT_SCOPE)
                return()
            endif()

        endif()

    endforeach()

    set(${o_passed} 0 PARENT_SCOPE)

endfunction()

function(EMakeGetPlatformVar _var)

    set(var ${${_var}})

    if("${var}" MATCHES ".?\\|[^\]]+$")

        string(REGEX REPLACE "^.*\\|" ""  _platforms ${var})    # 去除 所有 *|

        EMakePlatformsPassed(${_platforms} _passed)

        if(NOT _passed)
            set(${_var} PARENT_SCOPE)
        else()
            string(REGEX REPLACE "\\|[^\]^\)]+$" ""  var ${var}) # 去除 最后一个 |*
            set(${_var} ${var} PARENT_SCOPE)

        endif()

    endif()

endfunction()

macro(__getMatchedFiles _var)

    set(var ${${_var}})

    string(REGEX REPLACE "^.*/"    ""  _reg ${var})
    string(      REPLACE "${_reg}" ""  _dir ${var})

    file(GLOB _fd_files RELATIVE ${_dir} ${_dir}*)

    set(o_files)

    foreach(_file ${_fd_files})

        if(${_file} MATCHES "${_reg}")

            list(APPEND o_files ${_dir}${_file})

        endif()

    endforeach()

endmacro()

#
# i_list : input regex file list
# _o_list: output
# c_file : cmake file, only for print note if err occured
#
function(EMakeParseInputFiles i_list _o_list c_file )

    set(o_list)

    set(_cache)
    set(_cache_reg)
    set(_state)

    foreach(item ${i_list})

        EMakeGetPlatformVar(item)

        if(_ERR)

            EMakeErrF("the platform setted in property '${_o_list}' '${_ERR}':
    '${_ITEM}'
please check you project file:
    '${c_file}'")

        endif()

        if(item)

            if("${item}" MATCHES "^\\[\\*\\]*")

                string(REGEX REPLACE "^\\[\\*\\]" "" item "${item}")

                if(_state EQUAL 2)
                    file(GLOB _SRCS_ ${_cache})
                    list(REMOVE_ITEM o_list ${_SRCS_};"")
                    set(_cache)
                endif()

                list(APPEND _cache ${item})
                set(_state 1)

            elseif("${item}" MATCHES "^\\[[Rr]\\]*")

                string(REGEX REPLACE "^\\[[Rr]\\]" "" item "${item}")

                if(_state EQUAL 2)
                    file(GLOB _SRCS_ ${_cache})
                    list(REMOVE_ITEM o_list ${_SRCS_};"")
                    set(_cache)
                endif()

                list(APPEND _cache_reg ${item})
                set(_state 1)

            elseif("${item}" MATCHES "^\\[\\-\\]*")

                string(REGEX REPLACE "^\\[\\-\\]" "" item "${item}")

                if(_state EQUAL 1)

                    if(_cache)
                        file(GLOB _SRCS_ ${_cache})
                        list(APPEND o_list ${_SRCS_})
                        set(_cache)
                    endif()

                    if(_cache_reg)
                        foreach(_reg ${_cache_reg})

                            __getMatchedFiles(_reg)
                            list(APPEND o_list ${o_files})
                            set(_cache_reg)

                        endforeach()
                    endif()
                endif()

                list(APPEND _cache "${item}")
                set(_state 2)

            elseif(EXISTS ${item})

                if(_state EQUAL 2)
                    file(GLOB _SRCS_ ${_cache})
                    list(REMOVE_ITEM o_list ${_SRCS_};"")
                    set(_cache)
                endif()

                list(APPEND o_list ${item})

            else()

                EMakeErrF("the file setted in property '${_o_list}' not exists:
    '${item}'
please check you project file:
    '${c_file}'")

            endif()

        endif()

    endforeach()

    if(_state EQUAL 1)
        if(_cache)
            file(GLOB _SRCS_ ${_cache})
            list(APPEND o_list ${_SRCS_})
            set(_cache)
        endif()

        if(_cache_reg)
            foreach(_reg ${_cache_reg})

                __getMatchedFiles(_reg)
                list(APPEND o_list ${o_files})
                set(_cache_reg)

            endforeach()
        endif()
    elseif(_state EQUAL 2)
        file(GLOB _SRCS_ ${_cache})
        list(REMOVE_ITEM o_list ${_SRCS_};"")
    endif()

    set(${_o_list} ${o_list} PARENT_SCOPE)

endfunction()

#
# \! 解析 参数，从 ctk 移植，暂时未用
#
macro(mParseArguments prefix arg_names option_names)
    set(DEFAULT_ARGS)

    foreach(arg_name ${arg_names})
      set(${prefix}_${arg_name})
    endforeach()

    foreach(option ${option_names})
      set(${prefix}_${option} FALSE)
    endforeach()

    set(current_arg_name DEFAULT_ARGS)
    set(current_arg_list)

    foreach(arg ${ARGN})
      set(larg_names ${arg_names})
      list(FIND larg_names "${arg}" is_arg_name)
      if (is_arg_name GREATER -1)
        set(${prefix}_${current_arg_name} ${current_arg_list})
        set(current_arg_name ${arg})
        set(current_arg_list)
      else (is_arg_name GREATER -1)
        set(loption_names ${option_names})
        list(FIND loption_names "${arg}" is_option)
        if (is_option GREATER -1)
          set(${prefix}_${arg} TRUE)
        else (is_option GREATER -1)
          set(current_arg_list ${current_arg_list} ${arg})
        endif (is_option GREATER -1)
      endif (is_arg_name GREATER -1)
    endforeach()

    set(${prefix}_${current_arg_name} ${current_arg_list})

endmacro()

#
# \! 移除列表 list 中所有不符合 传入正则表达式 的 项
#
#   ctkMacroListFilter(<list> <regexp_var> [<regexp_var> ...]
#                      [OUTPUT_VARIABLE <variable>])
#
#   1. 如果 OUTPUT_VARIABLE 进行了设定，那么结果将被写入相关变量
#   2. 如果 OUTPUT_VARIABLE 没有设定，那么结果将被更新到 list 中
#
# 注意：正则表达式在传入时不能直接使用，所以我们需要使用变量的方式进行传递，而非正则表达式本身
#
macro(mListFilter)
    mParseArguments(LIST_FILTER "OUTPUT_VARIABLE" "" ${ARGV})

    # Check arguments.
    list(LENGTH LIST_FILTER_DEFAULT_ARGS LIST_FILTER_default_length)
    if(${LIST_FILTER_default_length} EQUAL 0)
        message(FATAL_ERROR "LIST_FILTER: missing list variable.")
    endif()
    if(${LIST_FILTER_default_length} EQUAL 1)
        message(FATAL_ERROR "LIST_FILTER: missing regular expression variable.")
    endif()

    # Reset output variable
    if(NOT LIST_FILTER_OUTPUT_VARIABLE)
        set(LIST_FILTER_OUTPUT_VARIABLE "LIST_FILTER_internal_output")
    endif()
    set(${LIST_FILTER_OUTPUT_VARIABLE})

    # Extract input list from arguments
    list(GET LIST_FILTER_DEFAULT_ARGS 0 LIST_FILTER_input_list)
    list(REMOVE_AT LIST_FILTER_DEFAULT_ARGS 0)
    foreach(LIST_FILTER_item ${${LIST_FILTER_input_list}})
        foreach(LIST_FILTER_regexp_var ${LIST_FILTER_DEFAULT_ARGS})
            foreach(LIST_FILTER_regexp ${${LIST_FILTER_regexp_var}})
                if(${LIST_FILTER_item} MATCHES ${LIST_FILTER_regexp})
                    list(APPEND ${LIST_FILTER_OUTPUT_VARIABLE} ${LIST_FILTER_item})
                endif()
            endforeach()
        endforeach()
    endforeach()

    # If OUTPUT_VARIABLE is not specified, overwrite the input list.
    if(${LIST_FILTER_OUTPUT_VARIABLE} STREQUAL "LIST_FILTER_internal_output")
        set(${LIST_FILTER_input_list} ${${LIST_FILTER_OUTPUT_VARIABLE}})
    endif()
endmacro(mListFilter)
