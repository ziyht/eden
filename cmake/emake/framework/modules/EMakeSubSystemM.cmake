# =====================================================================================
#
#       Filename:  EMakeImportSubSystemM.cmake
#
#    Description:  a simple tool to import sub system for project management framework
#
#        Version:  1.0
#        Created:  17/12/2017 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================



# 内部全局变量
#
set(_G_SUB_SYSTEMS_ "" CACHE INTERNAL "" FORCE)             # 子系统列表

#macro(clearGlobel)
#    set(_G_SUB_SYSTEMS_ "" CACHE INTERNAL "" FORCE)
#endmacro()

macro(_mRegisterSubSystem i_name)
    list(APPEND _G_SUB_SYSTEMS_ ${i_name})
endmacro()


macro(_mCheckSubSystem i_name)
    list(FIND _G_SUB_SYSTEMS_ ${i_name} o_index)

    if(${o_index} GREATER -1)
        EMakeErrF("sub system '${i_name}' added already")
    endif()

endmacro()


macro(_mImportModulesFromDir i_dir)

    if(EXISTS ${i_dir})

        file(GLOB _module_list ${i_dir}/*Import.cmake)
        if(_module_list)
            foreach(_module_file ${_module_list})
                include(${_module_file})
            endforeach()
        else()
            file(GLOB _module_list ${i_dir}/*.cmake)

            foreach(_module_file ${_module_list})
                include(${_module_file})
            endforeach()
        endif()
    endif()

endmacro()

#
# !\ 导入子系统
#
#   EMakeImportSubSystemM(name)      # 子系统名称
#
# !\ 约定：
#   1. 子系统目录在 {PROJECT_ROOT_DIR}/cmake/cmakeSubSystem/${name} 下
#   2. 且该文件夹下有 ${name}Manager.cmake 文件
#
# !\ 处理：
#   1. 若子系统目录下有 cmake 文件夹，其下的所有 cmake 文件都将自动导入
#
#
#
macro(EMakeImportSubSystemM i_name)

    _mCheckSubSystem(${i_name})

    EMakeGetProjectDirF(MAIN_PROJECT_ROOT_DIR)

    set(SUB_SYSTEM_DIR ${EMAKE_DIR}/subsystem/${i_name})

    EMakeImport(${SUB_SYSTEM_DIR})

    set(SUB_SYSTEM_DIR)

    _mRegisterSubSystem(${i_name})
endmacro()

macro(EMakeExecuteSubSystemM i_name)
    EMakeGetMainProjectDirF(_o_dir)

    #include(${EMAKE_DIR}/subsystem/${i_name}/${i_name}Manager.cmake)
endmacro()
