# =====================================================================================
#
#       Filename:  EMakeSetupConfigM.cmake
#
#    Description:  set config for emake framework
#
#        Version:  1.0
#        Created:  2018-08-21 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

macro(_InitConfig init)

    if(${init})


        if(NOT _LOG_LEVEL)
            set(_LOG_LEVEL 2)
        endif()

        if(NOT _BUILD_TYPE)
            set(_BUILD_TYPE Release)
        endif()

    else()

        set(_PROJECT_NAME)
        set(_PROJECT_VERSION)
        set(_LOG_LEVEL)
        set(_BUILD_TYPE)

    endif()

endmacro()

macro(EMakeSetupConfigInternalM)

    _InitConfig(1)

    if(_PROJECT_NAME AND _PROJECT_VERSION)
        EMakeProjectM(${_PROJECT_NAME} ${_PROJECT_VERSION})
    endif()

    EMakeSetLogLevelF(${_LOG_LEVEL})
    EMakeSetBuildTypeM(${_BUILD_TYPE})

    _InitConfig(0)
endmacro()


