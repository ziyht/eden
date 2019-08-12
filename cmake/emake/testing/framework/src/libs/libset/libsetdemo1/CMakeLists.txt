cmake_minimum_required(VERSION 2.8.11)

if(IMPORTING_BUILD)

# 项目名称
project(${KIT_NAME})

# 单独编译配置
if(NOT DEFINED CMAKE_MODULE_PATH)

    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../CMake" ${CMAKE_MODULE_PATH})
    include(mSetupCmake)
    mSetupQt()              # this must after set project()
    mSetupSingleBuild()
    message(-- " ----------- build ${KIT_LIB_NAME} in single mode -----------")

endif()



ParaBuildLibM()

endif()



