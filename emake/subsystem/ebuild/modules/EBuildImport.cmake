# =====================================================================================
#
#       Filename:  EBuildImport.cmake
#
#    Description: *Import.cmake will be included only if have,
#                 using this files to set the order of module files
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

set(_module_dir ${SUB_SYSTEM_DIR}/modules/)

include(${_module_dir}/EBuildSetupQtM.cmake)
include(${_module_dir}/EBuildSetupCompat.cmake)
include(${_module_dir}/EBuildControl.cmake)

include(${_module_dir}/EBuildUtilsM.cmake)

include(${_module_dir}/EBuildAddM.cmake)
include(${_module_dir}/EBuildExecExtM.cmake)
include(${_module_dir}/EBuildExecLibM.cmake)
include(${_module_dir}/EBuildExecPlgM.cmake)
include(${_module_dir}/EBuildExecAppM.cmake)
include(${_module_dir}/EBuildExecTypeM.cmake)
include(${_module_dir}/EBuildExecProdM.cmake)

include(${_module_dir}/EBuildExtAddExportM.cmake)

include(${_module_dir}/EBuildDependsM.cmake)
include(${_module_dir}/EBuildScanKitsM.cmake)
include(${_module_dir}/EBuildSetupKitsM.cmake)
include(${_module_dir}/EBuildValidateKitsM.cmake)
include(${_module_dir}/EBuildGenerateM.cmake)

include(${SUB_SYSTEM_DIR}/templates/ext/CMakeLists.txt)
include(${SUB_SYSTEM_DIR}/templates/lib/CMakeLists.txt)
include(${SUB_SYSTEM_DIR}/templates/plg/CMakeLists.txt)
include(${SUB_SYSTEM_DIR}/templates/app/CMakeLists.txt)
include(${SUB_SYSTEM_DIR}/templates/type/CMakeLists.txt)
include(${SUB_SYSTEM_DIR}/templates/product/CMakeLists.txt)
