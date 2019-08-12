# =====================================================================================
#
#       Filename:  import.cmake
#
#    Description:  this file is used for import framework or subsystem
#
#        Version:  1.0
#        Created:  2018.09.02 00:24:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

#! --------------------------------
#! import framework
include(${CMAKE_CURRENT_LIST_DIR}/modules/EmakeFramework.cmake)

EMakeInfF("------------------------------------------")
EMakeInfF("EMake Version: ${EMAKE_FRAMEWORK_VERSION}")

EMakeSetupConfigInternalM()
EMakeSetupBuildEnvM()
