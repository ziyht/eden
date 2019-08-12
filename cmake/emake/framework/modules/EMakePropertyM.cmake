# =====================================================================================
#
#       Filename:  EMakePropertyF.cmake
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

#
#
# \! 获取全局属性:
# EMakeGetGlobalPropertyM(name           # name(in ): 要获取的属性的名称
#                        val            # val (out): 输出的值存放在这里，若没有将至为空
#                        [NO_CHECK]     # NO_CHECK : 选项，设置后将不检查相关属性是否被设置（默认检查，若检查到未设置将终止cmake）
#                        )
#
macro(EMakeGetGlobalPropertyM i_name o_val)

    cmake_parse_arguments(M "PARENT_SCOPE;NO_CHECK" "" "" ${ARGN})

    get_property(_v_set GLOBAL PROPERTY _GP_${i_name}_ SET)
    get_property(_o_val GLOBAL PROPERTY _GP_${i_name}_)

    if(NOT _v_set AND NOT M_NO_CHECK)
        EMakeErrF("the global perperty '${i_name}' you want to get is not setted")
    endif()

    if(M_PARENT_SCOPE)
        set(${o_val} ${_o_val} PARENT_SCOPE)
    else()
        set(${o_val} ${_o_val})
    endif()

endmacro()

#
#
# \!获取目标属性
# EMakeGetTargetPropertyM(target     # target(in ): 要获取的属性所属目标
#                        name       # name  (in ): 要获取的属性的名称
#                        val)       # val   (out): 获取的值存放在这里，若没有将至为空
#                        [NO_CHECK] # NO_CHECK   : 选项，设置后将不检查相关属性是否被设置（默认检查，若检查到未设置将终止cmake）
#                        )
#
macro(EMakeGetTargetPropertyM i_target i_name o_val)

    cmake_parse_arguments(M "PARENT_SCOPE;NO_CHECK" "" "" ${ARGN})

    get_property(_v_set GLOBAL PROPERTY _TP_${i_target}_${i_name}_ SET)
    get_property(_o_val GLOBAL PROPERTY _TP_${i_target}_${i_name}_)

    if(NOT _v_set AND NOT M_NO_CHECK)
        EMakeErrF("the target ${i_target}'s perperty '${i_name}' you want to get is not setted")
    endif()

    if(M_PARENT_SCOPE)
        set(${o_val} ${_o_val} PARENT_SCOPE)
    else()
        set(${o_val} ${_o_val})
    endif()
endmacro()




# --------------------------------------------------
#
# 设置全局属性
#
#   EMakeSetGlobalPropertyM(<i_name>         # 属性名称
#        VAR      <var>                     # 值   （和 LIST_VAR 不能同时使用）
#        LIST_VAR <var1 var2 ...>           # 列表值（和 VAR      不能同时使用）
#        [APPEND]                           # 是否添加当前值到对应的属性中
#        [APPEND_NO_DUP]                    # 是否添加当前值到对应的属性中，如果已经存在，则不添加
#        [DEFAULT])                         # 当前是否在设置默认值，若属性已设置，则本次操作不做任何处理
#
#
macro(EMakeSetGlobalPropertyM i_name)

    cmake_parse_arguments(M "APPEND;APPEND_NO_DUP;DEFAULT;REMOVE_DUPLICATES" "VAR" "LIST_VAR" ${ARGN})

    if(M_VAR AND M_LIST_VAR)
        EMakeErrF("you can only using 'VAR' or 'LIST_VAR' individually to set the global property")
    endif()

    get_property(_v_set GLOBAL PROPERTY _GP_${i_name}_ SET)

    if(M_DEFAULT AND _v_set)
    else()

        if(NOT M_VAR AND NOT M_LIST_VAR AND NOT M_REMOVE_DUPLICATES AND NOT M_APPEND AND NOT M_APPEND_NO_DUP)
            if(M_UNPARSED_ARGUMENTS)
                EMakeErrF("please using 'VAR' or 'LIST_VAR' to set the global property")
            endif()
            set_property(GLOBAL        PROPERTY _GP_${i_name}_)
        endif()

        if(DEFINED M_VAR OR DEFINED M_LIST_VAR)
            set(_i_vals ${M_VAR} ${M_LIST_VAR})

            if(M_APPEND)
                set_property(GLOBAL APPEND PROPERTY _GP_${i_name}_ ${_i_vals})

            elseif(M_APPEND_NO_DUP)

                get_property(_o_vals GLOBAL PROPERTY _GP_${i_name}_)

                list(REMOVE_DUPLICATES _i_vals)
                foreach(_i_val ${_i_vals})
                    list(FIND _o_vals ${_i_val} _o_cnt)

                    if(_o_cnt EQUAL -1)
                        set_property(GLOBAL APPEND PROPERTY _GP_${i_name}_ ${_i_val})
                    endif()
                endforeach()

            else()
                set_property(GLOBAL        PROPERTY _GP_${i_name}_ ${_i_vals})
            endif()
        elseif(M_REMOVE_DUPLICATES)
        else()

            set_property(GLOBAL PROPERTY _GP_${i_name}_ 0)

        endif()

        if(M_REMOVE_DUPLICATES)
            get_property(_o_vals GLOBAL PROPERTY _GP_${i_name}_)

            if(_o_vals)
                list(REMOVE_DUPLICATES _o_vals)
                set_property(GLOBAL  PROPERTY _GP_${i_name}_ ${_o_vals})
            endif()

        endif()

    endif()
endmacro()


# --------------------------------------------------
#
# 设置目标属性（此目标并非cmake目标，可以是任意的字串，但不能为空）
#
#   EMakeSetTargetPropertyM(<i_target>       # 目标名称
#        <i_name>                           # 属性名称
#        VAR      <var>                     # 值   （和 LIST_VAR 不能同时使用）
#        LIST_VAR <var1 var2 ...>           # 列表值（和 VAR      不能同时使用）
#        [APPEND]                           # 是否添加当前值到对应的属性中
#        [APPEND_NO_DUP]                    # 是否添加当前值到对应的属性中，如果已经存在，则不添加
#        [DEFAULT])                         # 当前是否在设置默认值，若属性已设置，则本次操作不做任何处理
#
#
macro(EMakeSetTargetPropertyM i_target i_name)

    cmake_parse_arguments(M "APPEND;APPEND_NO_DUP;DEFAULT;REMOVE_DUPLICATES" "VAR" "LIST_VAR" ${ARGN})

    if(M_VAR AND M_LIST_VAR)
        EMakeErrF("you can only using 'VAR' or 'LIST_VAR' individually to set the target property")
    endif()

    get_property(_v_set GLOBAL PROPERTY _TP_${i_target}_${i_name}_ SET)

    if(M_DEFAULT AND _v_set)
    else()

        if(NOT M_VAR AND NOT M_LIST_VAR AND NOT M_REMOVE_DUPLICATES AND NOT M_APPEND AND NOT M_APPEND_NO_DUP)
            if(M_UNPARSED_ARGUMENTS)
                EMakeErrF("please using 'VAR' or 'LIST_VAR' to set the target property, checked unparsed arguments: [ ${M_UNPARSED_ARGUMENTS} ]")
            endif()

            set_property(GLOBAL        PROPERTY _TP_${i_target}_${i_name}_)
        endif()

        if(DEFINED M_VAR OR DEFINED M_LIST_VAR)
            set(_i_vals ${M_VAR} ${M_LIST_VAR})

            if(M_APPEND)
                set_property(GLOBAL APPEND PROPERTY _TP_${i_target}_${i_name}_ ${_i_vals})

            elseif(M_APPEND_NO_DUP)

                get_property(_o_vals GLOBAL PROPERTY _TP_${i_target}_${i_name}_)

                list(REMOVE_DUPLICATES _i_vals)
                foreach(_i_val ${_i_vals})
                    list(FIND _o_vals ${_i_val} _o_cnt)

                    if(_o_cnt EQUAL -1)
                        set_property(GLOBAL APPEND PROPERTY _TP_${i_target}_${i_name}_ ${_i_val})
                    endif()
                endforeach()

            else()
                set_property(GLOBAL        PROPERTY _TP_${i_target}_${i_name}_ ${_i_vals})
            endif()
        endif()

        if(M_REMOVE_DUPLICATES)
            get_property(_o_vals GLOBAL PROPERTY _TP_${i_target}_${i_name}_)

            if(_o_vals)
                list(REMOVE_DUPLICATES _o_vals)
                set_property(GLOBAL  PROPERTY _TP_${i_target}_${i_name}_ ${_o_vals})
            endif()
        endif()

    endif()

endmacro()

# --------------------------------------------------
#
# 设置目标的集合属性（此目标并非cmake目标，可以是任意的字串，但不能为空）
#
#   EMakeSetTargetPropertySetM(<i_target>    # 目标名称
#        <i_set>                            # 集合名称
#        <i_property>                       # 属性名称
#        [VARS <var1> ]                     # 值，可以是列表，若为列表，则依次遍历列表中的值
#        [APPEND]                           # 值被添加到 相关属性中
#        [APPEND_NO_DUP]                    # 要添加的值不在已有属性中时才进行添加，具有更高的优先级
#        [PROPERTY <key> [<val1> ...]]      # 附加属性，针对要设置的 ${i_property}
#   )
#
macro(EMakeSetTargetPropertySetM i_target i_set i_property)

    cmake_parse_arguments(M "APPEND;APPEND_NO_DUP" "" "VARS;PROPERTY" ${ARGN})

    if(M_UNPARSED_ARGUMENTS)
        EMakeInfF("{ARGN}: [${ARGN}]")
        EMakeErrF("unparsed arguments checked: [ ${M_UNPARSED_ARGUMENTS} ]")
    endif()

    set(_append_option)
    set(_EMakeAddTargetSetProperty_added_)

    if(M_APPEND)
        set(_append_option APPEND)
    endif()

    if(M_APPEND_NO_DUP)
        set(_append_option APPEND_NO_DUP)
    endif()

    # 设置值，使用了 VALS
    if(DEFINED M_VARS)

        foreach (_arg ${M_VARS})
            # EMakeInfF(" -- ${i_target} ++ $${i_set}$_${i_property} ${_arg}")

            EMakeSetTargetPropertyM(${i_target} $${i_set}$_${i_property} LIST_VAR ${_arg}        ${_append_option})   # 设置值

            set(_EMakeAddTargetSetProperty_added_ 1)

        endforeach()

    else()

        EMakeSetTargetPropertyM(${i_target} $${i_set}$_${i_property} VAR         ${_append_option})   # 设置值

    endif()

    # 设置 property
    if(M_PROPERTY)

        set(_i_props ${M_PROPERTY})

        list(GET       _i_props 0 _i_key)
        list(REMOVE_AT _i_props 0 )

        if(${_i_key} MATCHES "KEY_|VALS_|PROPERTIES_")
            EMakeErrF("you can not use KEY_, VALS_ or PROPERTIES_ to set append property, they have been used already in internal env")
        endif()

        EMakeSetTargetPropertyM(${i_target} $${i_set}$_${i_property}_${_i_key}     LIST_VAR ${_i_props})               # 设置相关属性值
        EMakeSetTargetPropertyM(${i_target} $${i_set}$_${i_property}_$PROPERTIES_$ VAR      $${i_set}$_${i_property}_${_i_key}  APPEND_NO_DUP)  # 添加相关key

        set(_EMakeAddTargetSetProperty_added_ 1)

    endif()

    if(_EMakeAddTargetSetProperty_added_)
        EMakeSetTargetPropertyM(${i_target} ${i_set} LIST_VAR "$${i_set}$_${i_property}" APPEND_NO_DUP)
    endif()

endmacro()


# --------------------------------------------------
#
# 获取目标的集合属性
#
#   EMakeGetTargetPropertySetM(<i_target>    # 目标名称
#        <i_set>                            # 集合名称
#        [PROPERTIES <name> [...]]          # 指定要获取的属性，若设置此值，则只有集合中的指定属性会被获取
#   )
#
#   1. 获取所有属性，即不指定 PROPERTIES
#
#       获取的所有属性名存放在 ${i_set}_PROPERTIES_ 中
#
#       如果需要获取各属性的值，需要遍历 ${i_set}_PROPERTIES_，然后针对每个属性，
#   通过添加后缀 _KEY_ 和 _VALS_ 来获取具体的 属性名 和 值，如：
#
#       foreach(_prop ${${i_set}_PROPERTIES_})
#           message("-- ${${_prop}_KEY_}: [ ${${_prop}_VALS_} ] ")
#       endforeach()
#
#   2. 指定 properties，与 1 类似，对于每个属性，添加后缀 _VALS_ 来获取具体的值，若该
#      属性的值没有被设置，那么获取的值为空
#
#
macro(EMakeGetTargetPropertySetM i_target i_set)

    cmake_parse_arguments(M "" "" "PROPERTIES" ${ARGN})

    if(M_UNPARSED_ARGUMENTS)
        EMakeErrF("invalid arguments, checked unparsed_arguments: [ ${M_UNPARSED_ARGUMENTS} ]")
    endif()

    if(M_PROPERTIES)

        foreach(_prop ${M_PROPERTIES})

            EMakeGetTargetPropertyM(${i_target} $${i_set}$_${_prop}               ${_prop}_VALS_ NO_CHECK)
            EMakeGetTargetPropertyM(${i_target} $${i_set}$_${_prop}_$PROPERTIES_$ _o_appd_props  NO_CHECK)

            foreach(_appd_prop ${_o_appd_props})
                EMakeGetTargetPropertyM(${i_target} ${_appd_prop} ${_appd_prop})   # 设置 key 的值
            endforeach()

        endforeach()

    else()

        EMakeGetTargetPropertyM(${i_target} ${i_set} ${i_set}_PROPERTIES_ NO_CHECK)

        foreach(_prop ${${i_set}_PROPERTIES_})

            string(REPLACE $${i_set}$_ "" ${_prop}_KEY_ ${_prop})           # 获取 key

            EMakeGetTargetPropertyM(${i_target} ${_prop} ${_prop}_VALS_)     # 获取 vals
            EMakeGetTargetPropertyM(${i_target} ${_prop}_$PROPERTIES_$ _o_appd_props NO_CHECK)

            foreach(_appd_prop ${_o_appd_props})
                EMakeGetTargetPropertyM(${i_target} ${_appd_prop} ${_appd_prop})   # 设置 key 的值
            endforeach()

        endforeach()

    endif()

endmacro()



