cmake_minimum_required(VERSION 3.1)

# --------------------------------------------
# 固有逻辑

if(IMPORTING_BUILD)

    # 项目名称
    if(KIT_NAME)
        project(${KIT_NAME})
    endif()

    # 导入配置，它将检测是否为单独编译并进行必要的环境设置
    include(../../../cmake/cmakeConfig/ParaSetupConfig.cmake)

    # 构建 kit
    ParaBuildLibM()

endif()
