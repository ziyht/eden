# =====================================================================================
#
#       Filename:  mBuildExt.cmake
#
#    Description:  an easier macro to build external libs
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


macro(_CheckExtSingBuildAndExecM)

    EMakeGetTargetPropertyM(${KIT_NAME} BUILD_ADDED _build_added NO_CHECK)
    EMakeGetTargetPropertyM(${KIT_NAME} BUILD_IN_SINGLE_MODE BUILD_IN_SINGLE_MODE NO_CHECK)

    if(NOT _build_added)

        if(BUILD_IN_SINGLE_MODE)

            EMakeSetTargetPropertyM(${KIT_NAME} BUILD_ADDED VAR 1)

            EMakeInfF(" ----------- build ${KIT_NAME} in single mode -----------")

            EBuildAddExtM(${KIT_NAME} "" ON)

            EBuildGenerateM()

            return()
        endif()

    endif()

endmacro()

macro(EBuildExecExtM)
if(__EBUILD_CONFIGURING_BUILD)

    # --------
    # 清空缓存
    EBuildClearLocalBuffer(_property_list ${_property_list})

    # --------
    # 单独编译检查并执行
    _CheckExtSingBuildAndExecM()

    # --------
    # 获取本目标的相关属性
    set(_show_list      "KIT_DEBUG               "
                        "KIT_TARGET              "
                        "KIT_TYPE                "
                        "KIT_PREFIX              "
                        "KIT_DOWNLOAD_DIR        "
                        "KIT_SOURCE_DIR          "
                        "KIT_BINARY_DIR          "
                        "KIT_INSTALL_DIR         "
                        "KIT_DOWNLOADS           "
                        "KIT_UPDATE_COMMAND      "
                        "KIT_PATCH_COMMAND       "
                        "KIT_CONFIGURE_COMMAND   "
                        "KIT_CMAKE_COMMAND       "
                        "KIT_CMAKE_GENERATOR     "
                        "KIT_CMAKE_ARGS          "
                        "KIT_CMAKE_CACHE_ARGS    "
                        "KIT_BUILD_COMMAND       "
                        "KIT_BUILD_IN_SOURCE     "
                        "KIT_TEST_COMMAND        "
                        "KIT_TEST_BEFORE_INSTALL "
                        "KIT_TEST_AFTER_INSTALL  "
                        "KIT_INSTALL_COMMAND     "
        )

    set(_property_list  "KIT_DEBUG               "
                        ${_show_list}
        )
    set(_property_list2 "TYPE          "
                        "ARCHIVE_NAME  "
                        "RUNTIME_NAME  "
                        "LINK_NAME     "
                        "INCLUDE_DIRS  "
                        "LINK_DIRS     "
                        "LOCATION      "
                        "EXPECT_DIR    "
        )

    EBuildGetTargetPropertiesM(${KIT_NAME} ${_property_list})
    EBuildShowVariablesInDbg(${_show_list})


    # 输出导出目标
    EMakeGetTargetPropertyM(${KIT_NAME} KIT_EXPORT_TARGETS _exports NO_CHECK)
    EMakeDbgF("KIT_EXPORT_TARGETS       : [ ${_exports} ]")
    foreach(_export ${_exports})
        EBuildGetTargetPropertiesM(${_export} ${_property_list2})
        EMakeInfF("  |- Export: ${_export} " PREFIX "   ")
        EBuildShowVariablesInDbg(${_property_list2} PREFIX "     |- ")
    endforeach()

    # --
    if(KIT_TEST_BEFORE_INSTALL AND KIT_TEST_AFTER_INSTALL)
        EMakeErrF("[KIT_TEST_BEFORE_INSTALL:${KIT_TEST_BEFORE_INSTALL}] and [KIT_TEST_AFTER_INSTALL:${KIT_TEST_AFTER_INSTALL}] for kit '${KIT_NAME}' is all logically ture, you can only set one of them")
    elseif(NOT KIT_TEST_BEFORE_INSTALL AND NOT KIT_TEST_AFTER_INSTALL)
        set(_TEST_BEFORE_INSTALL 1)
        set(_TEST_AFTER_INSTALL  0)
    else()
        set(_TEST_BEFORE_INSTALL ${KIT_TEST_BEFORE_INSTALL})
        set(_TEST_AFTER_INSTALL  ${KIT_TEST_AFTER_INSTALL})
    endif()

    if(KIT_BUILD_IN_SOURCE)
        set(KIT_BINARY_DIR)
    endif()

    # --------
    # 添加外部库编译
    ExternalProject_Add(${KIT_NAME}

        # -- dir
        PREFIX              ${KIT_PREFIX}
        DOWNLOAD_DIR        ${KIT_DOWNLOAD_DIR}
        SOURCE_DIR          ${KIT_SOURCE_DIR}
        BINARY_DIR          ${KIT_BINARY_DIR}
        INSTALL_DIR         ${KIT_INSTALL_DIR}

        # -- download
        ${KIT_DOWNLOADS}

        # -- configure
        CONFIGURE_COMMAND   ${KIT_CONFIGURE_COMMAND}
        CMAKE_COMMAND       ${KIT_CMAKE_COMMAND}
        CMAKE_GENERATOR     ${KIT_CMAKE_GENERATOR}
        CMAKE_ARGS          ${KIT_CMAKE_ARGS}
        CMAKE_CACHE_ARGS    ${KIT_CMAKE_CACHE_ARGS}

        # -- build
        BUILD_IN_SOURCE     ${KIT_BUILD_IN_SOURCE}
        BUILD_COMMAND       ${KIT_BUILD_COMMAND}


        # -- test and install
        TEST_COMMAND        ${KIT_TEST_COMMAND}
        INSTALL_COMMAND     ${KIT_INSTALL_COMMAND}
        TEST_BEFORE_INSTALL ${_TEST_BEFORE_INSTALL}
        TEST_AFTER_INSTALL  ${_TEST_AFTER_INSTALL}

        DEPENDS

        STEP_TARGETS        download update patch configure build test install
    )

endif()

endmacro()
