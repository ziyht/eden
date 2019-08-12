cmake_minimum_required(VERSION 3.1)


# =====================================================================================
# \! 项目构建设置
#
#   这里没有用户要设置的项，请不要做额外的修改，以免引发错误
#

if(NOT IMPORTING_BUILD OR IMPORTING_SETUP)
    return()
endif()

if(KIT_NAME)
    project(${KIT_NAME})
endif()

EBuildExecLibM()

# =====================================================================================
# \! 额外操作项设置
#
#   这里可以添加测试逻辑，或其它操作项
#

ParaAddCustomOperationF(KIT_NAME libdemo1 PRE_TARGET
    COMMAND echo command execute libdemo1 pre_target 1
    COMMENT "comment output libdemo1 PRE_TARGET 1"
    )
ParaAddCustomOperationF(KIT_NAME libdemo1 PRE_TARGET
    COMMAND echo command execute libdemo1 pre_target 2
    COMMENT "comment output libdemo1 PRE_TARGET 2"
    )

ParaAddCustomOperationF(KIT_NAME libdemo1 POST_BUILD
    COMMAND echo command execute libdemo1 post_build 1
    COMMENT "comment output libdemo1 POST_BUILD 1"
    )

ParaAddCustomOperationF(KIT_NAME libdemo1 PRE_EXPORT
    COMMAND echo command execute libdemo1 pre_export 1
    COMMENT "comment output libdemo1 PRE_EXPORT 1"
    )
ParaAddCustomOperationF(KIT_NAME libdemo1 POST_EXPORT
    COMMAND echo command execute libdemo1 post_export 1
    COMMENT "comment output libdemo1 POST_EXPORT 1"
    )


ParaAddCustomOperationF(KIT_NAME libdemo1 POST_BUILD
    COMMAND echo command execute libdemo1 post_build 2
    COMMENT "comment output libdemo1 POST_BUILD 2"
    )

ParaAddCustomOperationF(KIT_NAME libdemo1 PRE_EXPORT
    COMMAND echo command execute libdemo1 pre_export 2
    COMMENT "comment output libdemo1 PRE_EXPORT 2"
    )
ParaAddCustomOperationF(KIT_NAME libdemo1 POST_EXPORT
    COMMAND echo command execute libdemo1 post_export 2
    COMMENT "comment output libdemo1 POST_EXPORT 2"
    )

