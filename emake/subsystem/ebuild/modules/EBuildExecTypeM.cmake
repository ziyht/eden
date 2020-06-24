# =====================================================================================
#
#       Filename:  EBuildBuildTypeM.cmake
#
#    Description:  Generate the rule of lib build
#
#        Version:  1.0
#        Created:  2018.05.22 03:58:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

macro(_CheckPlgSingBuildAndExecM)

    EMakeGetTargetPropertyM(${KIT_NAME} BUILD_ADDED _build_added NO_CHECK)
    EMakeGetTargetPropertyM(${KIT_NAME} BUILD_IN_SINGLE_MODE BUILD_IN_SINGLE_MODE NO_CHECK)

    if(NOT _build_added)

        if(BUILD_IN_SINGLE_MODE)

            EMakeSetTargetPropertyM(${KIT_NAME} BUILD_ADDED VAR 1)

            message(-- " ----------- build ${KIT_NAME} in single mode -----------")

            EBuildAddPlgM(${KIT_NAME} "" ON)

            EBuildGenerateM()

            return()
        endif()

    endif()

endmacro()

macro(EBuildExecTypeM)
if(__EBUILD_CONFIGURING_BUILD)

    # --------
    # 清空缓存
    EBuildClearLocalBuffer(_property_list ${_property_list})

    _CompatPolicyM()

    # --------
    # 单独编译检查并执行
    _CheckPlgSingBuildAndExecM()

    # --------
    # 获取本目标的相关属性
    set(_show_list      "KIT_TARGET       "
                        "KIT_TYPE         "
                        "KIT_DEPENDS      "
                        "KIT_LIBRARIES    "
                        "KIT_LIBRARIES_SYS"
                        "KIT_SOURCES_DIR  "
                        "KIT_PRODUCT_DIR  "
                        "KIT_INSTALL_DIR  "
                        "KIT_COPY_RUNTIMES"
                        "KIT_COPY_ARCHIVES"
                        "KIT_COPY_FILES   "
                        "KIT_COPY_DIRS    "
        )
    set(_property_list  "KIT_DEBUG        "
                        ${_show_list}
        )
    EBuildGetTargetPropertiesM(${KIT_NAME} ${_property_list})
    EBuildShowVariablesInDbg(${_show_list})

    # --------
    # 添加安装规则
    EBuildAddInstallForKit(${KIT_NAME} DEST ${KIT_INSTALL_DIR} ENABLE_ARCHIVE ENABLE_OPTION)

    #set(KIT_DIR ${PRODS_DIR}/${KIT_NAME} PARENT_SCOPE)

endif()
endmacro()

