# =====================================================================================
#
#       Filename:  EBuildBuildProdM.cmake
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

macro(EBuildExecProdM)
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
#    set(_property_list  "KIT_TARGET       "
#                        "KIT_TYPE         "
#                        "KIT_LIBRARY_TYPE "
#                        "KIT_SRCS         "
#                        "KIT_MOC_SRCS     "
#                        "KIT_UI_FORMS     "
#                        "KIT_RESOURCES    "
#                        "KIT_DEPENDS      "
#                        "KIT_LIBRARIES    "
#                        "KIT_LIBRARIES_SYS"
#                        "KIT_SOURCES_DIR  "
#                        "KIT_INCLUDE_DIRS "
#                        "KIT_LINK_DIRS    "
#                        "KIT_OUTPUT_DIR   "
#                        "KIT_EXPORT       "
#                        "KIT_EXPORT_DIR   "
#        )
#    EBuildGetTargetPropertiesM(${KIT_NAME} ${_property_list})
#    EBuildShowVariablesInDbg(${_property_list})
    #set(KIT_NAME ${KIT_TARGET})

    # --------
    # 设置编译环境


    # --------
    # 添加目标


    # --------
    # 相关目标属性


    # --------
    # 设置生成目录

    # --------
    # 添加导出规则

endif()
endmacro()

