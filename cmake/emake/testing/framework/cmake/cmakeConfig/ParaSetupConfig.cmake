# =====================================================================================
#
#       Filename:  ParaSetupConfig.cmake
#
#    Description:  设置配置信息，主项目整体编译和子项目单独编译均要使用此配置文件，以保证配置的一致性
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

macro(ParaSetupConfigM)

    # 设置 cmake 日志级别
    EMakeSetLogLevelF(2)             # 0: 关闭，1：dbg，2：inf，3：wrn

    EMakeGetProjectDirF(MAIN_PROJECT_ROOT_DIR MAIN_PROJECT_DIR)
    EMakeDbgF("MAIN_PROJECT_ROOT_DIR: ${MAIN_PROJECT_ROOT_DIR}")
    EMakeDbgF("MAIN_PROJECT_DIR     : ${MAIN_PROJECT_DIR}")

    # 设置源码目录
    set(EXTS_DIR            "${MAIN_PROJECT_DIR}/src/libs_external")            # 外部库 所在目录
    set(LIBS_DIR            "${MAIN_PROJECT_DIR}/src/libs")                     # 内部库 所在目录
    set(PLGS_DIR            "${MAIN_PROJECT_DIR}/src/plugins")                  # 插件   所在目录
    set(APPS_DIR            "${MAIN_PROJECT_DIR}/src/applications")             # 应用   所在目录
    set(PRODS_DIR           "${MAIN_PROJECT_DIR}/src/products")                 # 产品   所在目录

    # 设置导出位置
    set(COMMONS_DIR         "${MAIN_PROJECT_DIR}/common")                       # 中间文件位置
    set(EXPORTS_DIR         "${MAIN_PROJECT_DIR}/exports")                      # 产品导出位置

    # 设置构建目录  （注意，在子项目单独编译时，PROJECT_BINARY_DIR 是它自己的构建目录）
    set(OUTPUT_DIR          "${PROJECT_BINARY_DIR}/build")
    set(EXTS_OUTPUT_DIR     "${PROJECT_BINARY_DIR}/libs_external")
    set(LIBS_OUTPUT_DIR     "${OUTPUT_DIR}/libs")
    set(PLGS_OUTPUT_DIR     "${OUTPUT_DIR}/plugins")
    set(APPS_OUTPUT_DIR     "${OUTPUT_DIR}/apps")

    # 设置安装位置  （注意，在子项目单独编译时，PROJECT_BINARY_DIR 是它自己的构建目录）
    set(INSTALL_INCLUDE_DIR "${PROJECT_BINARY_DIR}/install/include")            # 头文件，暂时无效
    set(INSTALL_LIB_DIR     "${PROJECT_BINARY_DIR}/install/libs")               # 库文件
    set(INSTALL_PLG_DIR     "${PROJECT_BINARY_DIR}/install/plugins")            # 插件
    set(INSTALL_BIN_DIR     "${PROJECT_BINARY_DIR}/install/bin")                # 二进制文件(应用)

    # 目标后缀
    set(CMAKE_DEBUG_POSTFIX             ""  CACHE STRING "add a postfix, usually d "    FORCE)
    set(CMAKE_RELEASE_POSTFIX           ""  CACHE STRING "add a postfix, usually empty" FORCE)
    set(CMAKE_RELWITHDEBINFO_POSTFIX    ""  CACHE STRING "add a postfix, usually empty" FORCE)
    set(CMAKE_MINSIZEREL_POSTFIX        ""  CACHE STRING "add a postfix, usually empty" FORCE)

    set(EXT_DEBUG_POSTFIX             )    # 若不设置，则继承 CMAKE 配置
    set(EXT_RELEASE_POSTFIX           )
    set(EXT_RELWITHDEBINFO_POSTFIX    )
    set(EXT_MINSIZEREL_POSTFIX        )

    set(LIB_DEBUG_POSTFIX             )
    set(LIB_RELEASE_POSTFIX           )
    set(LIB_RELWITHDEBINFO_POSTFIX    )
    set(LIB_MINSIZEREL_POSTFIX        )

    set(PLG_DEBUG_POSTFIX             "")
    set(PLG_RELEASE_POSTFIX           "")
    set(PLG_RELWITHDEBINFO_POSTFIX    "")
    set(PLG_MINSIZEREL_POSTFIX        "")

    set(APP_DEBUG_POSTFIX             )
    set(APP_RELEASE_POSTFIX           )
    set(APP_RELWITHDEBINFO_POSTFIX    )
    set(APP_MINSIZEREL_POSTFIX        )

    # 设置 DGRAPH 位置，本 cmake 框架内部需要
    set(DGRAPH_DIR          "${MAIN_PROJECT_ROOT_DIR}/cmake/utilities/DGraph")

    # 设置 检测逻辑
    EMakeSetGlobalPropertyM(CONFIG_SETTED    VAR   1)

    # 输出提示（被包含时可能多次执行（不应该），会重置设置，在这里给出提示以快速确定问题）
    EMakeInfF("Framework Configured OK")
endmacro()

# 单独构建检查 和 执行配置设定
if(NOT DEFINED CMAKE_MODULE_PATH)

    set(_check_1 "CMakeLists.txt")
    set(_check_2 "cmake/ParaUtilsF.cmake")

    set(_path ${CMAKE_CURRENT_SOURCE_DIR})
    while(EXISTS ${_path})
        if(EXISTS ${_path}/${_check_1} AND EXISTS ${_path}/${_check_2})
            set(MAIN_PROJECT_DIR_FOUND 1)
            set(CMAKE_MODULE_PATH "${_path}/cmake" ${CMAKE_MODULE_PATH})
            break()
        endif()
        if(EXISTS ${_path}/cmake/cmakeConfig/ParaSetupConfig.cmake)
            if(NOT MAIN_CONFIG_FILE_FOUND)
                set(MAIN_CONFIG_FILE_FOUND 1)
                set(CMAKE_MODULE_PATH "${_path}/cmake/cmakeConfig" ${CMAKE_MODULE_PATH})
            endif()
        endif()
        string(REGEX REPLACE "([A-Z]:|/([^/]*))$" "" _path ${_path})
    endwhile()

    if(NOT MAIN_PROJECT_DIR_FOUND)
        message(FATAL_ERROR "------ ERROR --------\nmain project dir not found\n---------------------\n")
    endif()
    if(NOT MAIN_CONFIG_FILE_FOUND)
        message(FATAL_ERROR "------ ERROR --------\nmain config file not found\n---------------------\n")
    endif()

    include(ParaSetupFrameworkM)
    EMakeSetupSingleBuildM()

else(NOT DEFINED CONFIG_SETTED)

    EMakeGetGlobalPropertyM(CONFIG_SETTED _config_setted NO_CHECK)

    if(NOT _config_setted)
        EMakeSetupConfigM()
    endif()

endif()
