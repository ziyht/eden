# =====================================================================================
#
#       Filename:  EMakeParseArguments.cmake
#
#    Description:  parse arguments for cmake functions or macros, unlike cmake_parse_arguments
#                  EMakeParseArguments can identify "" in ARGN
#
#        Version:  1.0
#        Created:  2018.05.28 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

function(EMakeParseArguments prefix options ones multis)

    set(_args   "${ARGN}")
    set(options "${options}")
    set(ones    "${ones}")
    set(multis  "${multis}")

    # -- 重复检查
    foreach(_option ${options})

        list(FIND ones "${_option}" _found)
        if(_found GREATER -1)
            EMakeErrF("reduplicative arg name '${_option}' checked in option_args and one_args")
        endif()

        list(FIND multis "${_option}" _found)
        if(_found GREATER -1)
            EMakeErrF("reduplicative arg name '${_option}' checked in option_args and multi_args")
        endif()

    endforeach()

    foreach(_one ${ones})

        list(FIND multis "${_one}" _found)
        if(_found GREATER -1)
            EMakeErrF("reduplicative arg name '${_one}' checked in one_args and multi_args")
        endif()

    endforeach()

    # -- 初始化默认值
    foreach(_name IN LISTS ones multis)
        set(${prefix}_${_name} PARENT_SCOPE)
    endforeach()

    foreach(_option ${options})
        set(${prefix}_${_option} FALSE PARENT_SCOPE)
    endforeach()

    set(${prefix}_UNPARSED_ARGUMENTS PARENT_SCOPE)

    # -- 开始解析
    set(_cur_arg_name)
    set(_cur_arg_type)
    set(_cur_arg_list)
    set(_unparsed)

    foreach(_arg IN LISTS _args)

        list(FIND options "${_arg}" _is_option)
        if(_is_option GREATER -1)

            if("${_cur_arg_type}" STREQUAL "ONE")
                # do nothing
            elseif("${_cur_arg_type}" STREQUAL "MULTI" AND DEFINED _cur_arg_list)
                set(${prefix}_${_cur_arg_name} "${_cur_arg_list}" PARENT_SCOPE)
                set(_cur_arg_list)
            endif()

            set(_cur_arg_name)

            set(${prefix}_${_arg} TRUE PARENT_SCOPE)

        else()
            list(FIND ones "${_arg}" _is_one)
            if(_is_one GREATER -1)

                if("${_cur_arg_type}" STREQUAL "ONE")
                    # do nothing
                elseif("${_cur_arg_type}" STREQUAL "MULTI" AND DEFINED _cur_arg_list)
                    set(${prefix}_${_cur_arg_name} "${_cur_arg_list}" PARENT_SCOPE)
                    set(_cur_arg_list)
                endif()

                set(_cur_arg_name ${_arg})
                set(_cur_arg_type "ONE")

            else()
                list(FIND multis "${_arg}" _is_multi)
                if(_is_multi GREATER -1)

                    if("${_cur_arg_type}" STREQUAL "ONE")
                        # do nothing
                    elseif("${_cur_arg_type}" STREQUAL "MULTI" AND DEFINED _cur_arg_list)
                        set(${prefix}_${_cur_arg_name} "${_cur_arg_list}" PARENT_SCOPE)
                        set(_cur_arg_list)
                    endif()

                    set(_cur_arg_name ${_arg})
                    set(_cur_arg_type "MULTI")

                else()

                    if("${_cur_arg_type}" STREQUAL "ONE")

                        set(${prefix}_${_cur_arg_name} "${_arg}" PARENT_SCOPE)

                        set(_cur_arg_name)
                        set(_cur_arg_type)
                    elseif("${_cur_arg_type}" STREQUAL "MULTI")
                        if(DEFINED _cur_arg_list)
                            set(_cur_arg_list "${_cur_arg_list};${_arg}")
                        else()
                            set(_cur_arg_list "${_arg}")
                        endif()
                    else()
                        if(DEFINED _unparsed)
                            set(_unparsed     "${_unparsed};${_arg}")
                        else()
                            set(_unparsed     "${_arg}")
                        endif()
                    endif()

                endif()

            endif()

        endif()

    endforeach()

    if("${_cur_arg_type}" STREQUAL "MULTI"  AND DEFINED _cur_arg_list)
        set(${prefix}_${_cur_arg_name} "${_cur_arg_list}" PARENT_SCOPE)
    endif()

    if(DEFINED _unparsed)
        set(${prefix}_UNPARSED_ARGUMENTS "${_unparsed}" PARENT_SCOPE)
    endif()

endfunction()

return()

# testing
if(0)

    EMakeParseArguments(M "OPT1;OPT2;OPT3" "ONE1;ONE2;ONE3;ONE4" "MUL1;MUL2;MUL3;MUL4;MUL5"
        OPT1 OPT2
        ONE1 ONE2 "" ONE3 " " ONE4 1 2 3 ONE5 "one5"
        MUL1 MUL2 "" MUL3 " " MUL4 "mul4" MUL5 1 2 3 4
        )

    foreach(_arg OPT1 OPT2 OPT3 ONE1 ONE2 ONE3 ONE4 ONE5 MUL1 MUL2 MUL3 MUL4 MUL5 MUL6 UNPARSED_ARGUMENTS)

        set(_defined 0)
        if(DEFINED M_${_arg})
            set(_defined 1)
        endif()

        set(_true    0)
        if(M_${_arg})
            set(_true 1)
        endif()

        EMakeInfF("------ ${_arg}: | defined:${_defined} | true:${_true} | [${M_${_arg}}]")
    endforeach()

endif()

