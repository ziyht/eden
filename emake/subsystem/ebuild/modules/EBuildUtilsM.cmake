# =====================================================================================
#
#       Filename:  EBuildUtilsM.cmake
#
#    Description:  helper for build a target
#
#        Version:  1.0
#        Created:  2017.12.19 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

#
# \! 设置 policy
#
macro(_CompatPolicyM)

    if(${CMAKE_VERSION} VERSION_EQUAL 3.10 OR ${CMAKE_VERSION} VERSION_GREATER 3.10)
        cmake_policy(SET CMP0071 OLD)
    endif()

endmacro()

#
# \! 根据传入的列表获取目标的属性
#
macro(EBuildGetTargetPropertiesM i_kit_name)

    foreach(_var_name ${ARGN})
        string(STRIP ${_var_name} _var_name)
        EMakeGetTargetPropertyM(${i_kit_name} ${_var_name} ${_var_name} NO_CHECK)
    endforeach()

endmacro()

#
# \! 所有传入都将被至空
#
macro(EBuildClearLocalBuffer)

    foreach(_var_name ${ARGN})
        string(STRIP ${_var_name} _var_name)
        set(${_var_name})
    endforeach()

endmacro()

#
# \! 以 dbg 的形式打印出一系列 变量列表, 若 EMakeLogLevel 不为 1 时将不显示
#
macro(EBuildShowVariablesInDbg)

    cmake_parse_arguments(M "" "" "PREFIX" ${ARGN})

    if(KIT_DEBUG)
        EMakeLogLevelPush(1)
    endif()

    foreach(_var_name ${M_UNPARSED_ARGUMENTS})
        string(STRIP ${_var_name} _var_real_name)

        if(${_var_real_name})
            EMakeDbgF("${M_PREFIX}${_var_name} : [ ${${_var_real_name}} ]")
        else()
            EMakeDbgF("${M_PREFIX}${_var_name} :")
        endif()

    endforeach()

    if(KIT_DEBUG)
        EMakeLogLevelPop()
        set(KIT_DEBUG)
    endif()

endmacro()

#
# \! 处理 moc 源
#
macro(_SetupMocSrcsM o_var)
    cmake_parse_arguments(M "" "" "SRCS" ${ARGN})

    EMakeGetTargetPropertyM(${KIT_NAME} KIT_IS_QT_PROJECT KIT_IS_QT_PROJECT NO_CHECK)

    set(${o_var})

    if(M_SRCS)
        # this is a workaround for Visual Studio. The relative include paths in the generated
        # moc files can get very long and can't be resolved by the MSVC compiler.
        if(QT_VERSION VERSION_GREATER "4")
            foreach(_moc_src ${M_SRCS})
                qt5_wrap_cpp(${o_var} ${_moc_src} OPTIONS -f${_moc_src} OPTIONS -DHAVE_QT5)
            endforeach()
        else()
            foreach(_moc_src ${M_SRCS})
                QT4_WRAP_CPP(${o_var} ${_moc_src} OPTIONS -f${_moc_src})
            endforeach()
        endif()
    elseif(KIT_IS_QT_PROJECT)
        set(CMAKE_AUTOMOC ON)
    elseif()
        set(CMAKE_AUTOMOC OFF)
    endif()

endmacro()

#
# \! 处理 UI 源
#
macro(_SetupUIFormsM o_var)
    cmake_parse_arguments(M "" "" "SRCS" ${ARGN})

    if(M_SRCS)

        if(QT_VERSION VERSION_GREATER "4")
            if(Qt5Widgets_FOUND)
                qt5_wrap_ui(${o_var} ${M_SRCS})
            elseif(M_SRCS)
                EMakeWrnF("Argument UI_FORMS ignored because Qt5Widgets module was not specified")
            endif()
        else()
            QT4_WRAP_UI(${o_var} ${M_SRCS})
        endif()

    endif()

endmacro()

#
# \! 处理资源文件
#
macro(_SetupResourcesM o_var)
    cmake_parse_arguments(M "" "" "SRCS" ${ARGN})

    if(M_SRCS)

        if(KNOWN_QT_PACKEGES)

            if(QT_VERSION VERSION_GREATER "4")
                qt5_add_resources(${o_var} ${M_SRCS})
            else()
                QT4_ADD_RESOURCES(${o_var} ${M_SRCS})
            endif()
        else()

            EmakeErrF(" _SetupResourcesM() not available now for NON_QT_PROJECT")

        endif()

    endif()
endmacro()

#
# \! 为 Visual Studio 设置 Group
#
macro(_SetupGroupForVSM)

    cmake_parse_arguments(M "" "" "RESOURCES;GENERATED" ${ARGN})

    source_group("Resources" FILES ${M_RESOURCES})
    source_group("Generated" FILES ${M_GENERATED})

endmacro()

#
# \! 为目标获取要导出的文件
#
macro(_GenerateExportedFiles o_binary_list o_header_list)

    set(_binary_list)
    set(_header_list)

    #EMakeGetTargetPropertyM(${KIT_NAME} KIT_RUNTIME_NAME _runtime_name)

    set(_output_name ${KIT_NAME}${KIT_POSTFIX})

    # 添加生成的文件
    if(WIN32)
        if(${KIT_LIBRARY_TYPE} MATCHES "STATIC")
            list(APPEND _binary_list "${KIT_OUTPUT_DIR}/${_output_name}.lib")
        elseif("${KIT_LIBRARY_TYPE}" STREQUAL "SHARED" OR "${KIT_LIBRARY_TYPE}" STREQUAL "MODULE")
            list(APPEND _binary_list "${KIT_OUTPUT_DIR}/${_output_name}.dll")
            #list(APPEND _binary_list "${KIT_OUTPUT_DIR}/${_output_name}.exp")
            #list(APPEND _binary_list "${KIT_OUTPUT_DIR}/${_output_name}.ilk")
            list(APPEND _binary_list "${KIT_OUTPUT_DIR}/${_output_name}.lib")
            #list(APPEND _binary_list "${KIT_OUTPUT_DIR}/${_output_name}.pdb")
        elseif(${KIT_TYPE}S MATCHES "APPS")
            list(APPEND _binary_list ${KIT_OUTPUT_DIR}/${_output_name}.exe)
        else()
            EMakeErrF("internal err: unkown type")
        endif()

    else()
        if(${KIT_LIBRARY_TYPE} MATCHES "STATIC")
            list(APPEND _binary_list "${KIT_OUTPUT_DIR}/lib${_output_name}.a")
        elseif("${KIT_LIBRARY_TYPE}" STREQUAL "SHARED" OR "${KIT_LIBRARY_TYPE}" STREQUAL "MODULE")
            list(APPEND _binary_list "${KIT_OUTPUT_DIR}/lib${_output_name}.so")
        elseif(${KIT_TYPE}S MATCHES "APPS")
            list(APPEND _binary_list "${KIT_OUTPUT_DIR}/${_output_name}")
        else()
            EMakeErrF("internal err: unkown type")
        endif()
    endif()

    # 添加 头文件 和 配置文件
    file(GLOB _header_list "${KIT_SOURCES_DIR}/*.cmake" "${KIT_SOURCES_DIR}/*.h")

    # 设置值
    set(${o_binary_list} ${_binary_list})
    set(${o_header_list} ${_header_list})

endmacro()

macro(_CheckCopyFiles i_dest)

    cmake_parse_arguments(M "" "" "FILES;DIRS" ${ARGN})

    if(M_FILES)

        foreach(_file ${M_FILES})
            string(REGEX REPLACE ".+/" "" _file_name ${_file})

            set(_path ${i_dest}${_file_name})

            EMakeGetTargetPropertyM(${_path} COPY_ORIGIN _origin NO_CHECK)

            if(_origin)

                if(NOT ${_origin} STREQUAL ${_file})

                    EMakeGetTargetPropertyM(${_path} COPY_ORIGIN_INFO _info)
                    EMakeGetTargetPropertyM(${_file}   BLONG_TO         _file_blong_to)
                    EMakeGetTargetPropertyM(${_origin} BLONG_TO         _origin_blong_to)

                    EMakeErrF("the file '${_file_name}' copy to ${i_dest} have two different source:
  now : ${_file} in '${KIT_NAME}' from '${_file_blong_to}'
  prev: ${_origin} in '${_info}' from '${_origin_blong_to}'")

                endif()

            else()

                EMakeSetTargetPropertyM(${_path} COPY_ORIGIN      VAR ${_file})
                EMakeSetTargetPropertyM(${_path} COPY_ORIGIN_INFO VAR ${KIT_NAME})

            endif()

        endforeach()

    endif()

    if(M_DIRS)

        foreach(_dir ${M_DIRS})

            string(REGEX REPLACE "/$"  "" _dir_name ${_dir})
            string(REGEX REPLACE ".+/" "" _dir_name ${_dir_name})

            set(_path ${i_dest}${_dir_name})

            EMakeGetTargetPropertyM(${_path} COPY_ORIGIN _origin NO_CHECK)

            if(_origin)

                if(NOT ${_origin} STREQUAL ${_dir})

                    EMakeGetTargetPropertyM(${_path}   COPY_ORIGIN_INFO _info)
                    EMakeGetTargetPropertyM(${_dir}    BLONG_TO         _dir_blong_to)
                    EMakeGetTargetPropertyM(${_origin} BLONG_TO         _origin_blong_to)

                    EMakeErrF("the dir '${_dir_name}' copy to ${i_dest} have two different source:
  now : ${_dir} in '${KIT_NAME}' from '${_dir_blong_to}'
  prev: ${_origin} in '${_info}' from '${_origin_blong_to}'")

                endif()
            else()

                EMakeSetTargetPropertyM(${_path} COPY_ORIGIN      VAR ${_dir})
                EMakeSetTargetPropertyM(${_path} COPY_ORIGIN_INFO VAR ${KIT_NAME})

            endif()

        endforeach()

    endif()

endmacro()

macro(_EBuildGetCopyFilsForKit)

    EMakeDbgF("KIT_TO_INSTALLS  :")

    foreach(_set TO_COPY_RUNTIMES TO_COPY_ARCHIVES TO_COPY_FILES TO_COPY_DIRS)

       EMakeGetTargetPropertySetM(${KIT_NAME} ${_set})

       string(REPLACE "TO_COPY_" "" _name ${_set})

       set(${_set}_OPITION ${KIT_COPY_${_name}})

       if(${_set}_PROPERTIES_)
           EMakeDbgF("    ${_name} [${${_set}_OPITION}]:")
           list(SORT ${_set}_PROPERTIES_)
       endif()

       foreach(_dest ${${_set}_PROPERTIES_})

           foreach(_val ${${_dest}_VALS_})

               EMakeGetTargetPropertyM(${_val} PERMISIONS _permissions NO_CHECK)
               EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} SKIP_COPY      _skip_copy      NO_CHECK)
               EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} TEST_DEST      _test_dest      NO_CHECK)
               EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} TEST_SKIP_COPY _test_skip_copy NO_CHECK)
               EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} TRACE          _trace          NO_CHECK)

               if(_skip_copy)
                   list(REMOVE_ITEM ${_dest}_VALS_ ${_val})
                   # EMakeDbgF("     || ${${_dest}_KEY_}  ${_val} [${_permissions}] [${_skip_copy}|${_test_skip_copy}|${_test_dest} ] [${_trace}]")
               else()
                   EMakeDbgF("     |- ${${_dest}_KEY_}  ${_val} [${_permissions}] [${_skip_copy}|${_test_skip_copy}|${_test_dest} ] [${_trace}]")
               endif()

           endforeach()

       endforeach()

    endforeach()

endmacro()


#
# -/ 为 cmake target 添加编译时要拷贝的文件
#
#   i_target - cmake target
#   FROM     - kit，本框架的 kit
#   DEST     - 设定输出位置，若不设置，将使用 ${PROJECT_BINARY_DIR}
#   SETS     - 从 kit 的哪些集合中获取，目前可用：TO_COPY_RUNTIMES TO_COPY_ARCHIVES TO_COPY_FILES TO_COPY_DIRS
#              若不设置，将使用 TO_COPY_RUNTIMES
#   MODE     - 拷贝模式，可用为 BUILD 和 TEST
#                   BUILD 模式，获取 kit 集合中的默认值进行添加
#                   TEST  模式，获取 kit 集合中的测试设置进行添加
#                   若不设置，将使用 BUILD 模式
#
#
function(EBuildAddCopyForTarget i_target)

    cmake_parse_arguments(M "" "FROM;DEST;MODE" "SETS" ${ARGN})

    if(NOT TARGET ${i_target})
        EMakeErrF("${i_target} is not a cmake target")
    endif()

    if(NOT M_FROM)
        EMakeInfF("no input kit set in param FROM")
    else()

        list(FIND _known_kits ${M_FROM} _o_index)

        if(_o_index EQUAL -1)
            EMakeWrnF("the kit '${M_FROM}' you set in param FROM can not be found")
        endif()

    endif()

    if(NOT M_DEST)
        set(M_DEST ${PROJECT_BINARY_DIR})
    endif()

    if(NOT M_SETS)
        set(M_SETS TO_COPY_RUNTIMES)
    endif()

    if(NOT M_MODE)
        set(M_MODE BUILD)
    endif()

    set(_dest_list)
    set(_copy_list)

    if("${AUTO_COPY}")
        set(_copy "ON ")
    else()
        set(_copy "OFF")
    endif()

    if(M_MODE STREQUAL "BUILD")

        EMakeDbgF("KIT_BUILD_TO_COPYS: (${i_target})")

        foreach(_set ${M_SETS})

            EMakeGetTargetPropertySetM(${M_FROM} ${_set})

            string(REPLACE "TO_COPY_" "" _name ${_set})

            if(${_set}_PROPERTIES_)
                EMakeDbgF("    ${_name} [${_copy}]: (${M_DEST})")
                list(SORT ${_set}_PROPERTIES_)
            endif()

            foreach(_dest ${${_set}_PROPERTIES_})

                foreach(_val ${${_dest}_VALS_})

                    EMakeGetTargetPropertyM(${_val} PERMISIONS _permissions NO_CHECK)
                    EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} SKIP_COPY      _skip_copy      NO_CHECK)
                    EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} TRACE          _trace          NO_CHECK)

                    if(_skip_copy)
                        list(REMOVE_ITEM ${_dest}_VALS_ ${_val})
                        # EMakeDbgF("     || ${${_dest}_KEY_}  ${_val} [${_permissions}] [${_skip_copy}|${_test_skip_copy}|${_test_dest} ] [${_trace}]")
                    else()
                        EMakeDbgF("     |- ${${_dest}_KEY_}  ${_val} [${_permissions}] [${_skip_copy}|${_test_skip_copy}|${_test_dest} ] [${_trace}]")
                    endif()

                endforeach()

                list(APPEND _dest_list ${${_dest}_KEY_})
                set(${${_dest}_KEY_}_files ${${_dest}_VALS_})

            endforeach()

        endforeach()

    elseif(M_MODE STREQUAL "TEST")

        EMakeDbgF("KIT_TEST_TO_COPYS : (${i_target})")

        foreach(_set ${M_SETS})

            EMakeGetTargetPropertySetM(${M_FROM} ${_set})

            string(REPLACE "TO_COPY_" "" _name ${_set})

            if(${_set}_PROPERTIES_)
                EMakeDbgF("    ${_name} [${_copy}]: (${M_DEST})")
                list(SORT ${_set}_PROPERTIES_)
            endif()

            foreach(_dest ${${_set}_PROPERTIES_})

                foreach(_val ${${_dest}_VALS_})

                    EMakeGetTargetPropertyM(${_val} PERMISIONS _permissions NO_CHECK)
                    EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} TEST_DEST      _test_dest      NO_CHECK)
                    EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} TEST_SKIP_COPY _test_skip_copy NO_CHECK)
                    EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} TRACE          _trace          NO_CHECK)

                    if(NOT _test_skip_copy)

                        if(_test_dest)

                            list(APPEND _dest_list          ${_test_dest})
                            list(APPEND ${_test_dest}_files ${_val})

                            EMakeDbgF("     |- ${_test_dest}  ${_val} [${_permissions}] [${_skip_copy}|${_test_skip_copy}|${_test_dest} ] [${_trace}]")

                        else()

                            list(APPEND _dest_list              ${${_dest}_KEY_})
                            list(APPEND ${${_dest}_KEY_}_files  ${_val})

                            EMakeDbgF("     |- ${${_dest}_KEY_}  ${_val} [${_permissions}] [${_skip_copy}|${_test_skip_copy}|${_test_dest} ] [${_trace}]")

                        endif()

                    endif()

                endforeach()

                list(REMOVE_DUPLICATES _dest_list)
                foreach(_dest ${_dest_list})
                    list(REMOVE_DUPLICATES ${_dest}_files)
                endforeach()

            endforeach()

        endforeach()

    elseif(M_MODE)

        EMakeErrF("invald property '${M_MODE}' for argument MODE, you can only set BUILD(default) or TEST ")

    endif()

    if(_copy)

        set(COMMAND_INFO)

        foreach(_dest ${_dest_list})

            if(${_dest}_files)
                list(APPEND COMMAND_INFO COMMAND ${CMAKE_COMMAND} -E make_directory    ${M_DEST}${_dest})                      # 先创建文件夹
                list(APPEND COMMAND_INFO COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${_dest}_files} ${M_DEST}${_dest})    # 再拷贝文件
            endif()

            set(${_dest}_files)

        endforeach()

        if(COMMAND_INFO)

            add_custom_command(TARGET ${i_target} POST_BUILD
                    ${COMMAND_INFO}
                    COMMENT "[ ...] Copy dlls for target '${i_target}' (${M_MODE})"
                )

        endif()

    endif()

endfunction()

#
# -/ 添加安装规则（内部会自动创建清理时用的target）
#
#   i_kit    - 框架中的 kit
#   FROM     - 来源kit，框架的 kit，如果不设置，将使用 i_kit
#   DEST     - 设定输出位置
#   SETS     - 从 kit 的哪些集合中获取，目前可用：TO_COPY_RUNTIMES TO_COPY_ARCHIVES TO_COPY_FILES TO_COPY_DIRS
#              若不设置，将使用 TO_COPY_RUNTIMES，TO_COPY_FILES，TO_COPY_DIRS
#   MODE     - 拷贝模式，可用为 BUILD 和 TEST
#                   BUILD 模式，获取 kit 集合中的默认值进行添加
#                   TEST  模式，获取 kit 集合中的测试设置进行添加
#                   若不设置，将使用 BUILD 模式
#   ENABLE_ARCHIVE - 在未设置 SETS 时，开启 archieve 的安装添加
#   ENABLE_OPTION  - 开启 kit 的 option 验证，KIT_COPY_RUNTIMES,KIT_COPY_ARCHIVES,KIT_COPY_FILES,KIT_COPY_DIRS
#
#
function(EBuildAddInstallForKit i_kit)

    set(_option "ENABLE_ARCHIVE;ENABLE_OPTION")
    set(_one    "FROM;DEST;MODE")
    set(_multi  "SETS")
    set(_all    ${_option} ${_one} ${_multi})
    cmake_parse_arguments(M "${_option}" "${_one}" "${_multi}" ${ARGN})

    list(FIND _all ${i_kit} _o_id)
    if(${_o_id} GREATER -1)
        EMakeErrF("please pass in a i_kit")
    endif()

    if(NOT M_DEST)
        EMakeErrF("please set the 'DEST' when add install steps")
    endif()

    if(NOT M_FROM)
        set(M_FROM ${i_kit})
    endif()

    if(NOT M_SETS)
        if(M_ENABLE_ARCHIVE)
            set(M_SETS TO_COPY_RUNTIMES TO_COPY_ARCHIVES TO_COPY_FILES TO_COPY_DIRS)
        else()
            set(M_SETS TO_COPY_RUNTIMES TO_COPY_FILES TO_COPY_DIRS)
        endif()
    endif()

    EMakeDbgF("KIT_TO_INSTALLS   :")

    foreach(_set ${M_SETS})

        if(${_set} MATCHES "DIRS$")
            set(_f_type DIRECTORY)
            set(_c_type DIRS)
        else()
            set(_f_type FILES)
            set(_c_type FILES)
        endif()

        EMakeGetTargetPropertySetM(${KIT_NAME} ${_set})

        string(REPLACE "TO_COPY_" "" _name ${_set})

        if(M_ENABLE_OPTION)
            set(_${_set}_enable ${KIT_COPY_${_name}})
        else()
            set(_${_set}_enable ON)
        endif()


        if(${_set}_PROPERTIES_)
            EMakeDbgF("    ${_name} [${_${_set}_enable}]:")
            list(SORT ${_set}_PROPERTIES_)
        endif()

        foreach(_dest ${${_set}_PROPERTIES_})

            # 去除不需要的文件
            foreach(_val ${${_dest}_VALS_})
                EMakeGetTargetPropertyM(${_val} PERMISIONS _permissions NO_CHECK)
                EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} SKIP_COPY      _skip_copy      NO_CHECK)
                EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} TEST_DEST      _test_dest      NO_CHECK)
                EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} TEST_SKIP_COPY _test_skip_copy NO_CHECK)
                EMakeGetTargetPropertyM(${KIT_NAME}_${${_dest}_KEY_}_${_val} TRACE          _trace          NO_CHECK)

                if(_skip_copy)
                    list(REMOVE_ITEM ${_dest}_VALS_ ${_val})
                    #EMakeDbgF("     || ${${_dest}_KEY_}  ${_val} [${_permissions}] [${_skip_copy}|${_test_skip_copy}|${_test_dest} ] [${_trace}]")
                else()
                    EMakeDbgF("     |- ${${_dest}_KEY_}  ${_val} [${_permissions}] [${_skip_copy}|${_test_skip_copy}|${_test_dest} ] [${_trace}]")
                endif()

            endforeach()

            # 检查文件冲突
            _CheckCopyFiles(${M_DEST}${${_dest}_KEY_} ${_c_type} ${${_dest}_VALS_})

            # 添加安装规则
            if(_${_set}_enable)

                foreach(_val ${${_dest}_VALS_})

                    EMakeGetTargetPropertyM(${_val} PERMISIONS _permissions NO_CHECK)

                    if(_permissions)
                        install(${_f_type} ${_val} DESTINATION ${M_DEST}${${_dest}_KEY_} PERMISSIONS ${_permissions})
                    else()
                        install(${_f_type} ${_val} DESTINATION ${M_DEST}${${_dest}_KEY_})
                    endif()

                endforeach()

            endif()

        endforeach()

    endforeach()

    # 添加清理步骤
    string(REPLACE "/" "_" _name ${KIT_NAME})
    add_custom_target(install_clean_${_name}
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${M_DEST}
            COMMENT "Clean up Install Files for Kit ${KIT_NAME}"
        )

    #add_dependencies(install_clean install_clean_${_name})

endfunction()
