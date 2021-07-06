# =====================================================================================
#
#       Filename:  mGenerateBuild.cmake
#
#    Description:  generate the cmake build files, then you can build the whole project
#
#        Version:  1.0
#        Created:  17/11/2017 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

macro(EBuildGenerateM)

    EMakeGetProjectDirF(MAIN_PROJECT_DIR)
    EMakeInff(ProjectDir: ${MAIN_PROJECT_DIR})

    EBuildScanKitsM()
    EBuildSetupKitsM()
    EBuildValidateKitsM()

    EMakeInfF("----------- Generating -----------")

    CONFIGURING_BUILD_SET(1)

    # 外部库
    set(_tag  "EXT")
    set(_dir  ${EXTS_DIR})
    set(_kits ${_G_EXTS_})
    if(_kits)
        include(ExternalProject)
        EMakeInfF(${_tag}S)
    endif()
    foreach(KIT_NAME ${_kits})

        EMakeGetTargetPropertyM(${KIT_NAME} OPTION_NAME _option_name)

        if(${_option_name})

                EMakeInfF(" |- ${KIT_NAME} ON")

                EMakeGetTargetPropertyM(${KIT_NAME} BUILD_IN_SINGLE_MODE                BUILD_IN_SINGLE_MODE                 NO_CHECK)
                EMakeGetTargetPropertyM(${KIT_NAME} BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY  NO_CHECK)

                if(BUILD_IN_SINGLE_MODE)
                    add_subdirectory(${_dir}/${KIT_NAME} ${PROJECT_BINARY_DIR}/self-build)
                elseif(BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY)
                    add_subdirectory(${_dir}/${KIT_NAME} ${PROJECT_BINARY_DIR}/${KIT_NAME}-build)
                else()
                    add_subdirectory(${_dir}/${KIT_NAME})
                endif()
        else()

            EMakeGetTargetPropertyM(${KIT_NAME} KIT_IS_INTERNAL KIT_IS_INTERNAL NO_CHECK)

            if(KIT_IS_INTERNAL)
                EMakeInfF(" |- ${KIT_NAME} OFF")
            else()
                EMakeInfF(" |- ${KIT_NAME} OFF (exported)")
            endif()

        endif()

    endforeach()

    # 内部库
    set(_tag  "LIB")
    set(_dir  ${LIBS_DIR})
    set(_kits ${_G_LIBS_})
    if(_kits)
        EMakeInfF(${_tag}S)
    endif()
    foreach(KIT_NAME ${_kits})

        EMakeGetTargetPropertyM(${KIT_NAME} OPTION_NAME _option_name)

        if(${_option_name})

                EMakeInfF(" |- ${KIT_NAME} ON")

                EMakeGetTargetPropertyM(${KIT_NAME} BUILD_IN_SINGLE_MODE                BUILD_IN_SINGLE_MODE                 NO_CHECK)
                EMakeGetTargetPropertyM(${KIT_NAME} BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY  NO_CHECK)

                if(BUILD_IN_SINGLE_MODE)
                    set(BINARY_DIR ${PROJECT_BINARY_DIR}/self-build)
                elseif(BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY)
                    set(BINARY_DIR ${PROJECT_BINARY_DIR}/${KIT_NAME}-build)
                else()
                    set(BINARY_DIR)
                endif()

                add_subdirectory(${_dir}/${KIT_NAME} ${BINARY_DIR})

        else()

            EMakeGetTargetPropertyM(${KIT_NAME} KIT_IS_INTERNAL KIT_IS_INTERNAL NO_CHECK)

            if(KIT_IS_INTERNAL)
                EMakeInfF(" |- ${KIT_NAME} OFF")
            else()
                EMakeInfF(" |- ${KIT_NAME} OFF (exported)")
            endif()

        endif()

    endforeach()

    # 插件
    set(_tag  "PLG")
    set(_dir  ${PLGS_DIR})
    set(_kits ${_G_PLGS_})
    foreach(KIT_NAME ${_kits})

        EMakeGetTargetPropertyM(${KIT_NAME} OPTION_NAME _option_name)

        if(${_option_name})

                EMakeInfF("build ${_tag} ${KIT_NAME} ON")

                EMakeGetTargetPropertyM(${KIT_NAME} BUILD_IN_SINGLE_MODE                BUILD_IN_SINGLE_MODE                 NO_CHECK)
                EMakeGetTargetPropertyM(${KIT_NAME} BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY  NO_CHECK)

                if(BUILD_IN_SINGLE_MODE)
                    add_subdirectory(${_dir}/${KIT_NAME} ${PROJECT_BINARY_DIR}/self-build)
                elseif(BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY)
                    add_subdirectory(${_dir}/${KIT_NAME} ${PROJECT_BINARY_DIR}/${KIT_NAME}-build)
                else()
                    add_subdirectory(${_dir}/${KIT_NAME})
                endif()
        else()

            EMakeGetTargetPropertyM(${KIT_NAME} KIT_IS_INTERNAL KIT_IS_INTERNAL NO_CHECK)

            if(KIT_IS_INTERNAL)
                EMakeInfF("build ${_tag} ${KIT_NAME} OFF")
            else()
                EMakeInfF("build ${_tag} ${KIT_NAME} OFF (exported)")
            endif()

        endif()

    endforeach()

    # 应用
    set(_tag  "APP")
    set(_dir  ${APPS_DIR})
    set(_kits ${_G_APPS_})
    foreach(KIT_NAME ${_kits})

        EMakeGetTargetPropertyM(${KIT_NAME} OPTION_NAME _option_name)

        if(${_option_name})

                EMakeInfF("build ${_tag} ${KIT_NAME} ON")

                EMakeGetTargetPropertyM(${KIT_NAME} BUILD_IN_SINGLE_MODE                BUILD_IN_SINGLE_MODE                 NO_CHECK)
                EMakeGetTargetPropertyM(${KIT_NAME} BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY  NO_CHECK)

                if(BUILD_IN_SINGLE_MODE)
                    add_subdirectory(${_dir}/${KIT_NAME} ${PROJECT_BINARY_DIR}/self-build)
                elseif(BUILD_AS_SINGLE_MODE_KIT_DEPENDENCY)
                    add_subdirectory(${_dir}/${KIT_NAME} ${PROJECT_BINARY_DIR}/${KIT_NAME}-build)
                else()
                    add_subdirectory(${_dir}/${KIT_NAME})
                endif()
        else()

            EMakeGetTargetPropertyM(${KIT_NAME} KIT_IS_INTERNAL KIT_IS_INTERNAL NO_CHECK)

            if(KIT_IS_INTERNAL)
                EMakeInfF("build ${_tag} ${KIT_NAME} OFF")
            else()
                EMakeInfF("build ${_tag} ${KIT_NAME} OFF (exported)")
            endif()

        endif()

    endforeach()

    # 版本
    set(_tag  "PROD")
    set(_dir  ${PRODS_DIR})
    set(_kits ${_G_PRODS_})

    add_custom_target(install_clean)

    foreach(_prod ${_kits})

        EMakeGetTargetPropertyM(${_prod} OPTION_NAME _prod_option_name)
        EMakeGetTargetPropertyM(${_prod} TYPES       _types            NO_CHECK)

        foreach(_type ${_types})

            set(KIT_NAME ${_prod}/${_type})

            EMakeGetTargetPropertyM(${KIT_NAME} OPTION_NAME _option_name)
            EMakeGetTargetPropertyM(${KIT_NAME} KIT_TYPE    _type_name)

            if(${_option_name})
                EMakeInfF("build TYPE ${KIT_NAME} ON")
                add_subdirectory(${_dir}/${KIT_NAME})
            endif()

        endforeach()

        EMakeGetTargetPropertyM(${_prod} KIT_INSTALL_DIR _prod_dir)
        add_custom_command(TARGET install_clean
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${_prod_dir}
            COMMENT "Clean up Install Files for Kit '${_prod}'"
            )

        set(KIT_NAME ${_prod})

    endforeach()

    CONFIGURING_BUILD_SET(0)

endmacro()
