# =====================================================================================
#
#       Filename:  CMakeLists.txt
#
#    Description:  a temeplete file of CMakeLists.txt used by emake
#
#        Version:  1.0
#        Created:  2018-08-23 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

# --------------------------------------------------------------
# 关于 cmake 的外部项目构建可以参考： https://cmake.org/cmake/help/v3.0/module/ExternalProject.html
# 关于有哪些构建器请参考： https://cmake.org/cmake/help/v2.8.8/cmake.html#section_Generators
#
# 在本文件中可以直接使用的属性:
#
#   KIT_NAME - 本 kit 的名称，它和文件夹的名称一致
#   EXTS_DIR - 所有外部库源码所在的位置，可使用 ${EXTS_DIR}/${KIT_NAME}
#              获取本 kit 所在的位置
#
#
# --------------------------------------------------------------

# ==================================================
# \! emake 外部库项目内部构建逻辑
# ==================================================
cmake_minimum_required(VERSION 3.1)

EBuildExecExtM(1 0 0)

# ==================================================
# \! emake 外部库项目属性设置
# ==================================================


# 重要： KIT_BINARY_DIR 是各步骤命令的工作目录

#set(KIT_PREFIX         ${PROJECT_BINARY_DIR}/libs_external/${KIT_NAME})    # 项目目录
#set(KIT_DOWNLOAD_DIR   ${KIT_PREFIX}/src)                                  # 下载目录
#set(KIT_SOURCE_DIR     ${KIT_PREFIX}/src/${KIT_NAME})                      # 源码目录
#set(KIT_BINARY_DIR     ${KIT_PREFIX}/build)                                # 构建目录
#set(KIT_INSTALL_DIR    ${KIT_PREFIX}/)                                     # 安装目录

set(KIT_UPDATE_COMMAND                                       )  # step2: Source work-tree update command
set(KIT_PATCH_COMMAND                                        )  # step2: Command to patch downloaded source
set(KIT_CONFIGURE_COMMAND                                    )  # step3: cmake ../src/${KIT_NAME}
set(KIT_CMAKE_COMMAND       ${CMAKE_COMMAND}                 )  # step3: cmake 可执行程序或命令
set(KIT_CMAKE_GENERATOR     ${EBUILD_EXT_DF_CMAKE_GENERATOR} )  # step3: 设置 GENERATOR
set(KIT_CMAKE_ARGS                                           )  # step3: 设置 cmake 命令行参数
set(KIT_CMAKE_CACHE_ARGS    ${EBUILD_EXT_DF_CMAKE_CACHE_ARGS})  # step3: 设置 cmake 初始化参数
set(KIT_BUILD_COMMAND       ${CMAKE_MAKE_PROGRAM}            )  # step4: 设置构建命令
set(KIT_BUILD_IN_SOURCE                                      )  # step4: 是否在源目录进行构建
set(KIT_TEST_COMMAND        ${CMAKE_COMMAND} -E echo_append  )  # step5: 设置测试命令 (lg: ${CMAKE_MAKE_PROGRAM} test)
set(KIT_TEST_BEFORE_INSTALL 1                                )  # step5: 先测试再安装
set(KIT_INSTALL_COMMAND     ${CMAKE_COMMAND} -E echo_append  )  # step6: 设置安装命令 (lg: ${CMAKE_MAKE_PROGRAM} install)

set(KIT_DOWNLOADS

    TIMEOUT             30          # 超时时间，单位为秒

    # 设置 git
    GIT_REPOSITORY      "git@git.paratera.net:ziyht/extdemo1.git"
    GIT_TAG             "master"

    # 设置 自定义下载命令
    #DOWNLOAD_COMMAND   cmd...      # Command to download source tree
    #DOWNLOAD_NAME      fname       # File name to store (if not end of URL)

    # 设置 cvs
    #CVS_REPOSITORY     cvsroot      # CVSROOT of CVS repository
    #CVS_MODULE         mod          # Module to checkout from CVS repo
    #CVS_TAG            tag          # Tag to checkout from CVS repo

    # 设置 svn
    #SVN_REPOSITORY     url          # URL of Subversion repo
    #SVN_REVISION       rev          # Revision to checkout from Subversion repo
    #SVN_USERNAME       john         # Username for Subversion checkout and update
    #SVN_PASSWORD       doe          # Password for Subversion checkout and update
    #SVN_TRUST_CERT     1            # Trust the Subversion server site certificate

    # 设置 url
    #URL                /.../src.tgz       # Full path or URL of source
    #URL_HASH           ALGO=value         # Hash of file at URL
    #URL_MD5            md5                # Equivalent to URL_HASH MD5=md5

)

# ---------------------------------------------------------
# 导出设置
#   必须进行导出设置才能让其它 kit 依赖本 kit 的相关资源
#
# ParaExtAddExport(<Target>                 # 此目标将可以被其它 kit 依赖，格式为 ${KIT_NAME}::${Target}
#       NAME <name>                         # 文件名称
#       TYPE <FILE|STATIC|SHARED|MODULE>    # 目标的类型：文件，静态库，动态库，模块
#
#       INCLUDE_DIRS   <dir1> [...]         # 依赖此目标需要包含的头文件目录
#       LINK_DIRS      <dir1> [...]         # 依赖此目标需要包含的链接目录
#       [LINK_DIRS_<config> <dir1> [...]  ] # 设置各构建模式下的链接目录，当此项被设置时，在特定的构建模式下，具有更高的优先级
#                                           #   此时会覆盖掉 LINK_DIRS 中的值
#                                           #   <config> 可以是 DEBUG RELEASE RELWITHDEBINFO MINSIZEREL
#
#       [ARCHIVE_EXTENTION <AUTO|.a|.lib...>]  # 归档文件扩展名，若设为 AUTO（默认），则框架会自动根据平台设置为 .lib 或 .a
#       [RUNTIME_EXTENTION <AUTO|.so|.dll...>] # 运行时库扩展名，若设为 AUTO（默认），则框架会自动根据平台设置为 .dll 或 .so
#                                              #     在 linux 下的动态库，会以 运行时库 作为链接库
#
#       [POSTFIX_<config>]                  # 设置各构建模式下的后缀，<config> 可以是 DEBUG RELEASE RELWITHDEBINFO MINSIZEREL
#
#       [LOCATION              <dir>   ]    # 此目标的位置（目录），若不设置，将取 LINK_DIRS 中的第一项
#       [EXPECT_DIR            <path>  ]    # 期望的位置（相对路径），当某个目标a依赖本目标时，期望本目标的输出位置（相对于a的输出位置）
#                                           #   此位置可被 目标a 重新设定
#       )

EBuildExtAddExport(Add
                    NAME                    add
                    TYPE                    STATIC
                    INCLUDE_DIRS            ${KIT_SOURCE_DIR}/src
                    LINK_DIRS               ${KIT_BINARY_DIR}
                    )

EBuildExtAddExport(Sub
                    NAME                    sub
                    TYPE                    STATIC
                    INCLUDE_DIRS            ${KIT_SOURCE_DIR}/src
                    LINK_DIRS               ${KIT_BINARY_DIR}
                    )



EOprtAddCommandF( PRE_DOWNLOAD
    COMMAND echo excute pre_download
    COMMENT "-------------- PRE_DOWNLOAD 1"
    )

#EOprtAddCommandF( POST_DOWNLOAD
#    COMMAND echo excute post_download
#    COMMENT "-------------- POST_DOWNLOAD 1"
#    )
#EOprtAddCommandF( PRE_UPDATE
#    COMMAND echo excute pre_update
#    COMMENT "-------------- PRE_UPDATE 1"
#    )

#EOprtAddCommandF( POST_UPDATE
#    COMMAND echo excute post_update
#    COMMENT "-------------- POST_UPDATE 1"
#    )
#EOprtAddCommandF( PRE_PATCH
#    COMMAND echo excute pre_patch
#    COMMENT "-------------- PRE_PATCH 1"
#    )

#EOprtAddCommandF( POST_PATCH
#    COMMAND echo excute post_patch
#    COMMENT "-------------- POST_PATCH 1"
#    )
#EOprtAddCommandF( PRE_CONFIGURE
#    COMMAND echo excute pre_configure
#    COMMENT "-------------- PRE_CONFIGURE 1"
#    )

#EOprtAddCommandF( POST_CONFIGURE
#    COMMAND echo excute post_configure
#    COMMENT "-------------- POST_CONFIGURE 1"
#    )
#EOprtAddCommandF( PRE_BUILD
#    COMMAND echo excute pre_build
#    COMMENT "-------------- PRE_BUILD 1"
#    )

#EOprtAddCommandF( POST_BUILD
#    COMMAND echo excute post_build
#    COMMENT "-------------- POST_BUILD 1"
#    )
#EOprtAddCommandF( PRE_TEST
#    COMMAND echo excute pre_test
#    COMMENT "-------------- PRE_TEST 1"
#    )

#EOprtAddCommandF( POST_TEST
#    COMMAND echo excute post_test
#    COMMENT "-------------- POST_TEST 1"
#    )
#EOprtAddCommandF( PRE_INSTALL
#    COMMAND echo excute pre_install
#    COMMENT "-------------- PRE_INSTALL 1"
#    )

#EOprtAddCommandF( POST_INSTALL
#    COMMAND echo excute post_install
#    COMMENT "-------------- POST_INSTALL 1"
#    )