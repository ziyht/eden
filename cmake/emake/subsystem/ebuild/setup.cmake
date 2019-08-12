# =====================================================================================
#
#       Filename:  setup.cmake
#
#    Description:  set the configuration of emake frame work or subsystem
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

macro(EMakeSetupConfigM)

    # 设置源码目录
    set(EXTS_DIR            "${MAIN_PROJECT_DIR}/src/exts")            # 外部库 所在目录
    set(LIBS_DIR            "${MAIN_PROJECT_DIR}/src/libs")            # 内部库 所在目录
    set(PLGS_DIR            "${MAIN_PROJECT_DIR}/src/plugins")         # 插件   所在目录
    set(APPS_DIR            "${MAIN_PROJECT_DIR}/src/apps")            # 应用   所在目录
    set(PRODS_DIR           "${MAIN_PROJECT_DIR}/src/products")        # 产品   所在目录

    # 设置导出位置
    set(COMMONS_DIR         "${MAIN_PROJECT_DIR}/common")              # 中间文件位置
    set(EXPORTS_DIR         "${MAIN_PROJECT_DIR}/exports")             # 产品导出位置

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

    set(EXT_DEBUG_POSTFIX             )      # 若不设置，则继承 CMAKE 配置，注意，对于外部库，外部库的内部配置可能覆盖这些配置
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

    # 自动拷贝机制
    set(AUTO_COPY                   ON)

endmacro()
