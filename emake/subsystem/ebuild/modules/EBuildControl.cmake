# =====================================================================================
#
#       Filename:  EBuildConfigureControl.cmake
#
#    Description:
#
#        Version:  1.0
#        Created:  2018.09.01 21:58:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================


set(__EBUILD_CONFIGURING_SETUP )
set(__EBUILD_CONFIGURING_BUILD )

macro(CONFIGURING_SETUP_SET OPTION)
    set(__EBUILD_CONFIGURING_SETUP ${OPTION})
endmacro()

macro(CONFIGURING_BUILD_SET OPTION)
    set(__EBUILD_CONFIGURING_BUILD ${OPTION})
endmacro()

macro(CONFIGURING_SETUP_CHECK)
    if(NOT __EBUILD_CONFIGURING_SETUP OR NOT KIT_NAME)
        return()
    endif()
endmacro()

macro(CONFIGURING_BUILD_CHECK)
    if(NOT __EBUILD_CONFIGURING_BUILD OR NOT KIT_NAME)
        return()
    endif()
endmacro()
