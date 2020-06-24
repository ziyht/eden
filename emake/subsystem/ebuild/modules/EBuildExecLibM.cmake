# =====================================================================================
#
#       Filename:  EBuildBuildLibM.cmake
#
#    Description:  Generate the rule of lib build
#
#        Version:  1.0
#        Created:  2017.12.18 03:58:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

macro(_CheckLibSingBuildAndExecM)

    EMakeGetTargetPropertyM(${KIT_NAME} BUILD_ADDED _build_added NO_CHECK)
    EMakeGetTargetPropertyM(${KIT_NAME} BUILD_IN_SINGLE_MODE BUILD_IN_SINGLE_MODE NO_CHECK)

    if(NOT _build_added)

        if(BUILD_IN_SINGLE_MODE)

            EMakeSetTargetPropertyM(${KIT_NAME} BUILD_ADDED VAR 1)

            message(-- " ----------- build ${KIT_NAME} in single mode -----------")

            EBuildAddLibM(${KIT_NAME} "" ON)

            EBuildGenerateM()

            return()
        endif()

    endif()

endmacro()

macro(EBuildExecLibM)
if(__EBUILD_CONFIGURING_BUILD)

    # --------
    # 清空缓存
    EBuildClearLocalBuffer(_property_list ${_property_list})

    _CompatPolicyM()

    # --------
    # 单独编译检查并执行
    _CheckLibSingBuildAndExecM()

    # --------
    # 获取本目标的相关属性
    set(_show_list      "KIT_TARGET       "
                        "KIT_TYPE         "
                        "KIT_LIBRARY_TYPE "
                        "KIT_SRCS         "
                        "KIT_MOC_SRCS     "
                        "KIT_UI_FORMS     "
                        "KIT_RESOURCES    "
                        "KIT_DEPENDS      "
                        "KIT_LIBRARIES    "
                        "KIT_LIBRARIES_SYS"
                        "KIT_SOURCES_DIR  "
                        "KIT_INCLUDE_DIRS "
                        "KIT_LINK_DIRS    "
                        "KIT_OUTPUT_DIR   "
                        "KIT_POSTFIX      "
                        "KIT_EXPORT       "
                        "KIT_EXPORT_DIR   "
                        "KIT_EXPECT_DIR   "
        )
    set(_property_list  "KIT_DEBUG        "
                        ${_show_list}
        )
    EBuildGetTargetPropertiesM(${KIT_NAME} ${_property_list})
    EBuildShowVariablesInDbg(${_show_list})
    #set(KIT_NAME ${KIT_TARGET})

    # --------
    # 设置编译环境
    include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/Resources/UI ${KIT_INCLUDE_DIRS})
    link_directories(${KIT_LINK_DIRS})

    _SetupMocSrcsM  (_MOC_CPP  SRCS ${KIT_MOC_SRCS})
    _SetupUIFormsM  (_UI_CPP   SRCS ${KIT_UI_FORMS})
    _SetupResourcesM(_QRC_SRCS SRCS ${KIT_RESOURCES})

    _SetupGroupForVSM(RESOURCES ${KIT_RESOURCES} ${KIT_UI_FORMS}
                      GENERATED ${_MOC_CPP} ${_UI_CPP} ${_QRC_SRCS})

    # --------
    # 添加目标
    add_library(${KIT_NAME} ${KIT_LIBRARY_TYPE} ${KIT_SRCS} ${_UI_CPP} ${_MOC_CPP} ${_QRC_SRCS})
    target_link_libraries(${KIT_NAME} ${KIT_LIBRARIES} ${KIT_LIBRARIES_SYS})

    if(${KIT_LIBRARY_TYPE} MATCHES "STATIC|SHARED")
        target_compile_definitions(${KIT_NAME} PRIVATE -D${KIT_NAME}_LIBRARY)
    endif()

    set(KIT_NAME_LIB ${KIT_NAME} ${KIT_LIBRARIES} ${KIT_LIBRARIES_SYS})

    # --------
    # 相关目标属性
    set_target_properties(${KIT_NAME} PROPERTIES LABELS ${KIT_NAME})
    if(KIT_LIBRARY_TYPE STREQUAL "STATIC")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT WIN32) # 64-bit
            set_target_properties(${KIT_NAME} PROPERTIES COMPILE_FLAGS "-fPIC")
        endif()
    endif()

    if(KIT_POSTFIX)
        set_target_properties(${KIT_NAME} PROPERTIES ${_CUR_BUILD_TYPE_UPPER}_POSTFIX ${KIT_POSTFIX})
    endif()

    # --------
    # 设置生成目录
    set_target_properties(${KIT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${KIT_OUTPUT_DIR}) # .dll
    set_target_properties(${KIT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${KIT_OUTPUT_DIR}) # .a 文件，注意在 windows 下面，总会有 .a 文件
    set_target_properties(${KIT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${KIT_OUTPUT_DIR}) # ...

    # --------
    # 添加构建时自动拷贝机制
    EBuildAddCopyForTarget(${KIT_NAME} FROM ${KIT_NAME} DEST ${KIT_OUTPUT_DIR})

    # --------
    # 添加导出规则
    if(KIT_EXPORT)
        _GenerateExportedFiles(_binary_list _header_list)

        add_custom_command(TARGET ${KIT_NAME} POST_BUILD
            COMMAND     ${CMAKE_COMMAND} -E make_directory    ${KIT_EXPORT_DIR}/${BUILD_ARCH}
            COMMAND     ${CMAKE_COMMAND} -E copy_if_different ${_header_list} ${KIT_EXPORT_DIR}
            COMMAND     ${CMAKE_COMMAND} -E copy_if_different ${_binary_list} ${KIT_EXPORT_DIR}/${BUILD_ARCH}
            DEPENDS     ${KIT_NAME}
            COMMENT     "[ ...] Exporting target ${KIT_NAME}"
            VERBATIM)

    endif()

    # --------
    # Components
    EMakeGetTargetPropertyM(${KIT_NAME} COMPONENTS COMPONENTS NO_CHECK)

    set(TOP_KIT ${KIT_NAME})
    foreach(comp ${COMPONENTS})

        set(KIT_NAME ${comp})

        EMakeGetTargetPropertyM(${KIT_NAME} NAME name)

        add_subdirectory(${LIBS_DIR}/${TOP_KIT}/${name})

    endforeach()

endif()
endmacro()

