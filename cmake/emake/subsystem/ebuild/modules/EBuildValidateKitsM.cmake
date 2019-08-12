# =====================================================================================
#
#       Filename:  EBuildValidateKitsM.cmake
#
#    Description:  make sure the enabled/disabled libraries, plugins or applications
#                  are coherent
#
#        Version:  1.0
#        Created:  17/11/2017 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

# 设置 DGRAPH 位置，本 cmake 框架内部需要
set(DGRAPH_DIR          "${SUB_SYSTEM_DIR}/utilities/DGraph")

#
# \! 尝试编译 DGgraph
#
macro(_BuildDGraphM)

    EMakeCheckExistsF(DGRAPH_DIR)

    try_compile(RESULT_VAR ${PROJECT_BINARY_DIR}/utilities/DGraph ${DGRAPH_DIR}
                  DGraphi
                  CMAKE_FLAGS
                    -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
                    -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}
                    -DCMAKE_OSX_SYSROOT:STRING=${CMAKE_OSX_SYSROOT}
                    -DCMAKE_VERBOSE_MAKEFILE:BOOL=FALSE
                  OUTPUT_VARIABLE output)

    if(NOT RESULT_VAR)
        message(FATAL_ERROR "Failed to compile DGraph application.\n${output}")
    endif()

    find_program(DGraph_EXECUTABLE DGraph
        "${PROJECT_BINARY_DIR}/utilities/DGraph/"
        "${PROJECT_BINARY_DIR}/utilities/DGraph/bin/"
        "${PROJECT_BINARY_DIR}/utilities/DGraph/Debug/"
        "${PROJECT_BINARY_DIR}/utilities/DGraph/Release/")

    #mark_as_advanced(DGraph_EXECUTABLE)

endmacro()

#
# \! 生成 DGraph 的输入文件，该文件内容包含了所有目标及该目标的依赖库
#
#   1. 没有源文件的 target 将不会作为目标写入文件，但它依然会作为其它项目的依赖
#   2. 文件内容为：
#
#       顶点数 边数
#       目标1 [依赖1] [依赖2] [...]
#       目标2 [依赖1] [依赖2] [...]
#       ...
#
#
function(_GenerateDGraphInputF i_outdir)

    cmake_parse_arguments(M "WITH_OPTION;WITH_EXTERNALS" "" "" ${ARGN})

    set(_dgraph_list )
    set(_edges)
    set(_vertices)
    set(_isolated_vertex_candidates)

    EMakeGetGlobalPropertyM(KNOWN_KITS _kits NO_CHECK)

    # 查找 顶点 和 边
    foreach(_kit ${_kits})

        EMakeGetTargetPropertyM(${_kit} KIT_DEPENDS _depends NO_CHECK)

        set(_include_dep 1)
        if(M_WITH_OPTION)
            set(_include_dep ${${_option_name}})
        endif()

        if(${_include_dep})

            #
            # if(M_WITH_EXTERNALS)
            # endif()

            if(_depends)
                list(APPEND _vertices ${_kit})
            else()
                # 添加候选
                list(APPEND _isolated_vertex_candidates ${_kit})
            endif()

            # Generate XML related to the dependencies
            foreach(_dependency ${_depends})
                list(APPEND _edges ${_dependency})
                set(_dgraph_list ${_dgraph_list} "${_kit} ${_dependency}\n")
                list(APPEND _vertices ${_dependency})
            endforeach()

        endif()

    endforeach()

    # 检查未加入的定点
    foreach(_isolated_vertex_candidate ${_isolated_vertex_candidates})
        set(_found 0)
        foreach(_dgraph_entry ${_dgraph_list})
            string(REPLACE "\n" ""  _dgraph_entry "${_dgraph_entry}")
            string(REPLACE " "  ";" _dgraph_entry "${_dgraph_entry}")
            list(FIND _dgraph_entry ${_isolated_vertex_candidate} _index)
            if(_index GREATER -1)
                set(_found 1)
                break()
            endif()
        endforeach()

        if(NOT _found)
            list(APPEND _vertices ${_isolated_vertex_candidate})
            set(_dgraph_list "${_isolated_vertex_candidate}\n" ${_dgraph_list})
        endif()
    endforeach()

    # 去重 以及 获取 顶点 和 边 的 个数
    if(_vertices)
        list(REMOVE_DUPLICATES _vertices)
    endif()
    list(LENGTH _vertices _numberOfVertices)
    list(LENGTH _edges    _numberOfEdges)

    set(dgraph_list ${_dgraph_list} PARENT_SCOPE)   # 仅供外部测试
    set(_dgraph_list "${_numberOfVertices} ${_numberOfEdges}\n" ${_dgraph_list})

    # 设置文件名
    if(M_WITH_OPTION)
        set(_filename "${i_outdir}/DGraphInput.txt")
    elseif(M_WITH_EXTERNALS)
        set(_filename "${i_outdir}/DGraphInput-alldep-withext.txt")
    else()
        set(_filename "${i_outdir}/DGraphInput-alldep.txt")
    endif()

    # 写文件
    file(WRITE ${_filename} ${_dgraph_list})
    #EMakeInfF("Generated: ${_filename}")

endfunction()


#
# 在同级的 kit 之间，根据可能的依赖关系排序
#
function(_ValidateGenerationalKitsSequenceF )

    foreach(_type EXTS LIBS PLGS APPS TYPES PRODS)

        foreach(_kit ${_G_${_type}_})

            EMakeGetTargetPropertyM(${_kit} KIT_DEPENDS _deps NO_CHECK)

            # 获取此 kit 在同类中 deps
            set(_transfer)

            foreach(_dep ${_deps})
                list(FIND _G_${_type}_ ${_dep} _o_index)

                if(_o_index GREATER -1)
                    list(REMOVE_ITEM _G_${_type}_  ${_dep})
                    list(APPEND      _transfer ${_dep})
                endif()
            endforeach()

#            EMakeDbgF("--1-- ${_G_${_type}_}")
#            EMakeDbgF("--2-- ${_kit}")
#            EMakeDbgF("--3-- ${_transfer}")

            # 把这些 deps 插入到 此 kit 之前
            list(FIND _G_${_type}_ ${_kit} _i_index)
            foreach(_dep ${_transfer})
                list(INSERT _G_${_type}_ ${_i_index} ${_dep})
            endforeach()

        endforeach()

    endforeach()

endfunction()

macro(_CheckOptionForAllKits)

    foreach(_category EXT LIB PLG APP)

        foreach(_kit ${_G_${_category}S_})

            EMakeGetTargetPropertyM(${_kit} KIT_TYPE        _kit_type        NO_CHECK)
            EMakeGetTargetPropertyM(${_kit} KIT_IS_INTERNAL _kit_is_internal NO_CHECK)
            if(_kit_type AND NOT "${_kit_type}" STREQUAL "${_category}")
                EMakeErrF("the added kit '${_kit}'s type '${_category}' is not match the type '${_kit_type}' that scanned")
            endif()

            if(BUILD_ALL_${_category}S AND NOT ${_category}_${_kit} AND _kit_is_internal)
                set(${_category}_${_kit} 1)
                EMakeInfF("Enabling Internal ${_category} [${_kit}] because of [BUILD_ALL_${_category}S:${BUILD_ALL_${_category}S}] evaluates to True")
            endif()

            if(${_category}_${_kit} AND NOT _kit_is_internal)
                #set(${_category}_${_kit} 0)
                #EMakeInfF("Disabled ${_category} [${_kit}] because of this kit is not internal")

                EMakeErrF("added kit '${_kit}' can not found in main project
    please check whether these files exist:
    ${${_category}S_DIR}/${_kit}/CMakeLists.txt
you can using CREATE option to create templete files like this:
    EBuildAddLibM(libtest ON \"comment\" CREATE)
        ")

            endif()

        endforeach()

    endforeach()

    # products
    foreach(_product ${PRODS})

        EMakeErrF("Not supported now for added prods/types")

    endforeach()

endmacro()

function(_GetToCopysForKit i_kit)

    # 检查当前 kit 是否已被检查过，若检查过，则直接退出，不做重复操作
    EMakeGetTargetPropertyM(${i_kit} KIT_CHECKED_TOCOPY _checked NO_CHECK)
    if(_checked)
        return()
    endif()

    # 获取当前 kit 代表需要拷贝（不包括依赖和附加文件）
    EMakeGetTargetPropertyM(${i_kit} KIT_TYPE          _kit_type)

    if(${_kit_type} STREQUAL "EXT")
        EMakeGetTargetPropertyM(${i_kit} RUNTIME_NAME  _runtime      NO_CHECK)
        EMakeGetTargetPropertyM(${i_kit} ARCHIVE_NAME  _archive      NO_CHECK)
        EMakeGetTargetPropertyM(${i_kit} LOCATION      _output_dir   NO_CHECK)
    else()
        EMakeGetTargetPropertyM(${i_kit} KIT_RUNTIME_NAME  _runtime    NO_CHECK)
        EMakeGetTargetPropertyM(${i_kit} KIT_ARCHIVE_NAME  _archive    NO_CHECK)
        EMakeGetTargetPropertyM(${i_kit} KIT_OUTPUT_DIR    _output_dir NO_CHECK)
    endif()

    if(_runtime)
        EMakeSetTargetPropertySetM(${i_kit} "TO_COPY_RUNTIMES" "/" VARS ${_output_dir}/${_runtime} APPEND_NO_DUP)

        if(${_kit_type} STREQUAL APP)
            EMakeSetTargetPropertyM(${_output_dir}/${_runtime} PERMISIONS LIST_VAR ${_DF_APP_PERMISSIONS})
            EMakeSetTargetPropertyM(${_output_dir}/${_runtime} BLONG_TO   VAR      ${i_kit}                APPEND_NO_DUP)
        endif()
    endif()
    if(_archive)
        EMakeSetTargetPropertySetM(${i_kit} "TO_COPY_ARCHIVES" "/"  VARS ${_output_dir}/${_archive} APPEND_NO_DUP)
        EMakeSetTargetPropertyM(${_output_dir}/${_archive} BLONG_TO VAR  ${i_kit}                   APPEND_NO_DUP)
    endif()

    # 为每个依赖执行同样的检查
    EMakeGetTargetPropertyM(${i_kit} KIT_DERECT_DEPENDS   _deps  NO_CHECK)
    foreach(_dep ${_deps})
        _GetToCopysForKit(${_dep})
    endforeach()

    # 合并来自依赖的文件
    foreach(_dep ${_deps})

        set(_dest_prefix)

        # 判断 i_kit 的 assign 设置项
        EMakeGetTargetPropertySetM(${i_kit} ASSIGN PROPERTIES ${_dep}_dest ${_dep}_skip_copy ${_dep}_test_dest ${_dep}_test_skip_copy)

        # EMakeInfF("--++------${i_kit} ${_dep} ---------: [${${_dep}_dest_VALS_}|${${_dep}_skip_copy_VALS_}|${${_dep}_test_dest_VALS_}|${${_dep}_test_skip_copy_VALS_} ]")

        if(${_dep}_dest_VALS_)
            set(_dest_prefix ${${_dep}_dest_VALS_})
        else()
            # 判断 _dep 的 expect 设置项
            EMakeGetTargetPropertyM(${_dep} KIT_EXPECT_DIR _expect_dir NO_CHECK)
            if(_expect_dir)
                set(_dest_prefix ${_expect_dir})
            endif()
        endif()

        foreach(_set TO_COPY_RUNTIMES TO_COPY_ARCHIVES TO_COPY_FILES TO_COPY_DIRS)

            #
            #  kit/dep
            #   |
            #   | -- TO_COPY_RUNTIMES
            #   |       | -- TO_COPY_RUNTIMES_PROPERTIES_
            #   |              |-- ${_prop1}
            #   |              |      | -- ${_prop1}_KEY_      <- 这里存储目标位置
            #   |              |      | -- ${_prop1}_VALS_     <- 这里存储所有文件
            #   |              |      |        | -- ${val1}       <- 具体的一个文件
            #   |              |      |        | -- ${val2}
            #   |              |      |        | -- ...
            #   |              |-- ${_prop2}
            #   |              |-- ...
            #   | -- TO_COPY_ARCHIVES
            #   | -- TO_COPY_FILES
            #   | -- TO_COPY_DIRS
            #

            EMakeGetTargetPropertySetM(${_dep} ${_set})

            foreach(_prop ${${_set}_PROPERTIES_})

                # 获取目标位置
                EMakeValidatePath(${_dest_prefix}/${${_prop}_KEY_} _dest)

                # 获取当前 _dep 中的文件，添加到当前 kit 中
                EMakeSetTargetPropertySetM(${i_kit} ${_set} ${_dest} VARS ${${_prop}_VALS_} APPEND_NO_DUP)

                # 对每一个文件针对当前 kit 设置必要的属性
                foreach(_file ${${_prop}_VALS_})

                    EMakeGetTargetPropertyM(${_dep}_${${_prop}_KEY_}_${_file} SKIP_COPY      _skip_copy      NO_CHECK)
                    EMakeGetTargetPropertyM(${_dep}_${${_prop}_KEY_}_${_file} TEST_DEST      _test_dest      NO_CHECK)
                    EMakeGetTargetPropertyM(${_dep}_${${_prop}_KEY_}_${_file} TEST_SKIP_COPY _test_skip_copy NO_CHECK)
                    EMakeGetTargetPropertyM(${_dep}_${${_prop}_KEY_}_${_file} TRACE          _trace          NO_CHECK)

                    # EMakeInfF(" ${i_kit} ${_dep}: ${_file} ${_skip_copy}")

                    if(_skip_copy OR ${_dep}_skip_copy_VALS_)
                        set(_tag !)
                    else()
                        set(_tag)
                    endif()

                    EMakeGetTargetPropertyM(${i_kit}_${_dest}_${_file} TRACE     _s_trace     NO_CHECK)
                    EMakeGetTargetPropertyM(${i_kit}_${_dest}_${_file} SKIP_COPY _s_skip_copy NO_CHECK)

                    if(_s_trace AND NOT _s_skip_copy)

                        # 原始有 trace信息 但是没有 _skip_copy，说明需要拷贝，不做 _skip_copy

                    else()

                        if(_skip_copy OR ${_dep}_skip_copy_VALS_)
                            EMakeSetTargetPropertyM(${i_kit}_${_dest}_${_file} SKIP_COPY VAR 1)
                            # EMakeInfF(" ++ ${i_kit}_${_dest}_${_file} : SKIP_COPY : 1")
                        else()
                            EMakeSetTargetPropertyM(${i_kit}_${_dest}_${_file} SKIP_COPY VAR 0)
                            # EMakeInfF(" -- ${i_kit}_${_dest}_${_file} : SKIP_COPY : 0")
                        endif()

                        if(_trace)
                            EMakeSetTargetPropertyM(${i_kit}_${_dest}_${_file} TRACE VAR "-> ${_dep} ${_tag}${_trace}")
                        else()
                            EMakeSetTargetPropertyM(${i_kit}_${_dest}_${_file} TRACE VAR "-> ${_dep}")
                        endif()

                    endif()


                    if(NOT _test_dest)
                        EMakeSetTargetPropertyM(${i_kit}_${_dest}_${_file} TEST_DEST VAR ${${_dep}_test_dest_VALS_})
                        # EMakeInfF(" ++ ${i_kit}_${_dest}_${_file} : TEST_DEST : [${${_dep}_test_dest_VALS_}]")
                    else()
                        EMakeValidatePath(${${_dep}_test_dest_VALS_}/${_test_dest} _test_dest)
                        EMakeSetTargetPropertyM(${i_kit}_${_dest}_${_file} TEST_DEST VAR ${_test_dest})
                        # EMakeInfF(" ++ ${i_kit}_${_dest}_${_file} : TEST_DEST : [${_test_dest}]")
                    endif()

                    if(_test_skip_copy OR ${_dep}_test_skip_copy_VALS_)
                        EMakeSetTargetPropertyM(${i_kit}_${_dest}_${_file} TEST_SKIP_COPY VAR 1)
                        # EMakeInfF(" ++ ${i_kit}_${_dest}_${_file} : TEST_SKIP_COPY : 1")
                    endif()

                endforeach()

            endforeach()

        endforeach()


    endforeach()

    # 设置内部状态
    EMakeSetTargetPropertyM(${i_kit} KIT_CHECKED_TOCOPY VAR 1)

endfunction()


macro(_ValidateCopyFiles)

    foreach(_kit ${_build_kits})
        EMakeGetTargetPropertyM(${_kit} KIT_TYPE    _kit_type)
        list(APPEND _${_kit_type}_kits ${_kit})
    endforeach()

#    foreach(_type EXT LIB PLG APP TYPE)
#        EMakeInfF("_${_type}_kits: [${_${_type}_kits}]")
#    endforeach()

    foreach(_ext ${_EXT_kits})
        _GetToCopysForKit(${_ext})
    endforeach()

    foreach(_lib ${_LIB_kits})
        _GetToCopysForKit(${_lib})
    endforeach()

    foreach(_plg ${_PLG_kits})
        _GetToCopysForKit(${_plg})
    endforeach()

    foreach(_app ${_APP_kits})
        _GetToCopysForKit(${_app})
    endforeach()

    foreach(_type ${_TYPE_kits})
        _GetToCopysForKit(${_type})
    endforeach()

endmacro()

macro(EBuildValidateKitsM)

    EMakeInfF("----------- Validating -----------")

    _CheckOptionForAllKits()

    set(_dir ${${PROJECT_NAME}_BINARY_DIR})
    EMakeCheckExistsF(_dir)

    # 编译 DGraph 工具
    _BuildDGraphM()

    # 根据现有的目标和依赖生成供 DGraph 工具读取的源文件
    _GenerateDGraphInputF(${_dir} WITH_EXTERNALS)

    # 梳理所有 kit 间的依赖关系
    EMakeSetPathsM("${QT_INSTALLED_LIBRARY_DIR}")
    EMakeGetGlobalPropertyM(KNOWN_KITS _known_kits NO_CHECK)
    foreach(_kit ${_known_kits})

        #EMakeInfF("Validating ${_kit}...")

        execute_process(
            COMMAND "${DGraph_EXECUTABLE}" "${_dir}/DGraphInput-alldep-withext.txt" -sort ${_kit}
            WORKING_DIRECTORY   ${_dir}
            RESULT_VARIABLE     RESULT_VAR
            OUTPUT_VARIABLE     _dep_path
            ERROR_VARIABLE      _error
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(RESULT_VAR)
            EMakeErrF("Failed to obtain dependence path of ${_kit}.\n${RESULT_VAR}\n${PROJECT_BINARY_DIR}\n${_error}")
        endif()

        # 转换成 list , 并移除 kit 本身
        string(REPLACE " " "\\;" _deps ${_dep_path})
        set(_deps ${_deps})
        list(REMOVE_ITEM _deps ${_kit})
#        EMakeInfF(--1  "${_kit}")
#        EMakeInfF(--2- "${_dep_path}")
#        EMakeInfF(--3--"${_deps}")

        # 更新依赖到目标属性中
        EMakeSetTargetPropertyM(${_kit} KIT_DEPENDS LIST_VAR ${_deps})

    endforeach()

    # 开启或添加需要构建的 kit
    EMakeGetGlobalPropertyM(ADDED_KITS    _added_kits     NO_CHECK)
    EMakeGetGlobalPropertyM(KNOWN_TARGETS _known_targets  NO_CHECK)
    foreach(_kit ${_added_kits})

        EMakeGetTargetPropertyM(${_kit} OPTION_NAME _kit_option_name)

        # 如果需要构建此 kit
        if(${_kit_option_name})

            # 检查所有的依赖项
            EMakeGetTargetPropertyM(${_kit} KIT_DEPENDS          _targets            NO_CHECK)
            EMakeGetTargetPropertyM(${_kit} BUILD_IN_SINGLE_MODE _kit_in_single_mode NO_CHECK)
            foreach(_target ${_targets})

                EMakeGetTargetPropertyM(${_target} OWNER _dep NO_CHECK)
                if(NOT _dep)
                    set(_dep ${_target})
                endif()

                # 在手动添加的 kit 中查找
                list(FIND _added_kits ${_dep} _o_index)

                # 找到则强制开启此 kit 的构建
                if(_o_index GREATER -1)

                    EMakeGetTargetPropertyM(${_dep} KIT_IS_INTERNAL  _dep_is_internal   NO_CHECK )
                    EMakeGetTargetPropertyM(${_dep} OPTION_NAME      _dep_option_name)

                    if(_dep_is_internal)
                        if(NOT ${_dep_option_name})
                            set(${_dep_option_name} ON)
                            EMakeInfF("Enabling Internal [${_dep}] because of [${_kit}] depends on it")
                        endif()
                    else()
                        set(${_dep_option_name} OFF)
                        #EMakeInfF("Linking  Exported [${_dep}] because of [${_kit}] depends on it")
                    endif()


                # 没有找到则在扫描的 kit 中查找
                else()

                    list(FIND _known_kits ${_dep} _o_index)

                    # 找到则强制添加到待构建列表中
                    if(_o_index GREATER -1)
                        EMakeGetTargetPropertyM(${_dep} KIT_IS_INTERNAL  _dep_is_internal   NO_CHECK )
                        EMakeGetTargetPropertyM(${_dep} OPTION_NAME      _dep_option_name            )
                        EMakeGetTargetPropertyM(${_dep} KIT_TYPE         _dep_kit_type               )

                        list(APPEND _added_kits ${_dep})
                        list(INSERT _G_${_dep_kit_type}S_ 0 ${_dep})

                        if(_dep_is_internal)
                            set(${_dep_option_name} ON)
                            EMakeInfF("Adding   Internal ${_dep_kit_type} [${_dep}] because of [${_kit}] depends on it")
                        else()
                            set(${_dep_option_name} OFF)
                            EMakeInfF("Adding   Exported ${_dep_kit_type} [${_dep}] because of [${_kit}] depends on it")
                        endif()

                        if(_kit_in_single_mode)
                            EMakeSetTargetPropertyM(${_dep} BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY VAR 1)
                        endif()


                    # 没有找到则输出错误并终止 cmake
                    else()
                        EMakeGetTargetPropertyM(${_dep} ERR_INFO _err_info)

                        if(_err_info)
                            EMakeErrF("${_err_info}")
                        else()
                            EMakeErrF("kit '${_kit}' depends on unknown kit: ${_dep}")
                        endif()
                    endif()

                endif()

            endforeach()

        endif()

    endforeach()

    # 扫描所有需要的 kit 的 所有 依赖，根据依赖的类型设置 kit 的包含和链接属性
    foreach(_kit ${_added_kits})

        list(FIND _known_kits ${_kit} _o_index)
        if(_o_index EQUAL -1)
            EMakeGetTargetPropertyM(${_kit} KIT_TYPE _kit_type)
            EMakeErrF("added kit '${_kit}' can not found in main project
    please check whether these files exist:
    ${${_kit_type}S_DIR}/${_kit}/setupManifist.cmake
    ${${_kit_type}S_DIR}/${_kit}/CMakeLists.txt
    or
    ${${_kit_type}S_EXPORTS_DIR}/${_kit}/setupManifist.cmake
    ${${_kit_type}S_EXPORTS_DIR}/${_kit}/*.[lib|dll|a]
    ")
        endif()

        EMakeGetTargetPropertyM(${_kit} KIT_DEPENDS _targets NO_CHECK)

        foreach(_target ${_targets})
            EMakeGetTargetPropertyM(${_target} OWNER _dep NO_CHECK)
            if(NOT _dep)
                set(_dep ${_target})
            endif()

            EMakeGetTargetPropertyM(${_dep} KIT_IS_INTERNAL  _dep_is_internal   NO_CHECK)
            EMakeGetTargetPropertyM(${_dep} KIT_TYPE         _dep_type)

            if(${_dep_type}S MATCHES "^LIBS$|^PLGS$")
                EMakeGetTargetPropertyM(${_dep} KIT_LIBRARY_TYPE _dep_labrary_type)
            endif()

            if(${_dep_type}S STREQUAL "EXTS")
                EMakeGetTargetPropertyM(${_target} INCLUDE_DIRS _dep_export_include_dirs NO_CHECK)
                EMakeGetTargetPropertyM(${_target} LINK_DIRS    _dep_export_link_dirs    NO_CHECK)
                EMakeGetTargetPropertyM(${_target} LINK_NAME    _dep_export_libs         NO_CHECK)
            endif()

            # 内置含有源码的kit
            if(_dep_is_internal)

                # 若为外部库
                if(${_dep_type}S STREQUAL "EXTS")
                    EMakeSetTargetPropertyM(${_kit} KIT_INCLUDE_DIRS LIST_VAR ${_dep_export_include_dirs} APPEND)
                    EMakeSetTargetPropertyM(${_kit} KIT_LIBRARIES    LIST_VAR ${_dep_export_libs}         APPEND)
                    EMakeSetTargetPropertyM(${_kit} KIT_LINK_DIRS    LIST_VAR ${_dep_export_link_dirs}    APPEND)
                endif()

                if(${_dep_type}S MATCHES "^LIBS$|^PLGS$")
                    # 若依赖为库且为 STATIC 或 SHARED，则添加 _dep 到自己的链接库中
                    if("${_dep_labrary_type}" STREQUAL "STATIC" OR "${_dep_labrary_type}" STREQUAL "SHARED")
                        EMakeSetTargetPropertyM(${_kit} KIT_LIBRARIES VAR "${_dep}" APPEND)
                    endif()

                    # 获取 被依赖kit 的 sources_dir 并 添加到自己的 include_dir 中
                    EMakeGetTargetPropertyM(${_dep} KIT_SOURCES_DIR  _src_dir)
                    EMakeGetTargetPropertyM(${_dep} KIT_INCLUDE_DIRS _incl_dir NO_CHECK)
                    EMakeSetTargetPropertyM(${_kit} KIT_INCLUDE_DIRS LIST_VAR ${_src_dir} ${_incl_dir} APPEND)
                endif()

            # 无源码且已导出的kit
            else()
                # 若依赖为库且为 STATIC 或 SHARED，则添加 _dep 的导出库到自己的链接库中，以及导出目录到自己的链接目录列表和头文件目录列表中
                if("${_dep_labrary_type}" STREQUAL "STATIC" OR "${_dep_labrary_type}" STREQUAL "SHARED")

                    EMakeGetTargetPropertyM(${_dep} KIT_EXPORTS_LIBS _dep_exports_libs)
                    EMakeSetTargetPropertyM(${_kit} KIT_LIBRARIES LIST_VAR "${_dep_exports_libs}" APPEND)

                    # 获取被依赖 kit 的 exports_dir 并 添加到自己的 link dir 中
                    # 注意这里 KIT_EXPORTS_DIR 并不是 KIT_EXPORT_DIR
                    # KIT_EXPORTS_DIR 为编译管理系统扫描时获取的导出目录，此目录位置唯一确定
                    # KIT_EXPORT_DIR  为用户设定的导出目录
                    EMakeGetTargetPropertyM(${_dep} KIT_EXPORTS_DIR  _dep_exports_dir)
                    EMakeSetTargetPropertyM(${_kit} KIT_INCLUDE_DIRS VAR "${_dep_exports_dir}"                APPEND)
                    EMakeSetTargetPropertyM(${_kit} KIT_LINK_DIRS    VAR "${_dep_exports_dir}/${BUILD_ARCH}"  APPEND)
                endif()

            endif()
        endforeach()

    endforeach()

    # 去重
    set(_build_kits)
    foreach(_kit ${_added_kits})
        EMakeSetTargetPropertyM(${_kit} KIT_LIBRARIES    REMOVE_DUPLICATES)
        EMakeSetTargetPropertyM(${_kit} KIT_INCLUDE_DIRS REMOVE_DUPLICATES)
        EMakeSetTargetPropertyM(${_kit} KIT_LINK_DIRS    REMOVE_DUPLICATES)

        EMakeGetTargetPropertyM(${_kit} OPTION_NAME _kit_option_name)
        if(${_kit_option_name})
            list(APPEND _build_kits ${_kit})
        endif()
    endforeach()

    # 更新 ADDED_KITS, 其它子系统可能需要查询
    EMakeSetGlobalPropertyM(ADDED_KITS LIST_VAR ${_added_kits})

    # 设置同级别 kit 的构建顺序
    _ValidateGenerationalKitsSequenceF()

    # 梳理需要拷贝的文件
    _ValidateCopyFiles()
endmacro()
