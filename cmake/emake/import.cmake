# =====================================================================================
#
#       Filename:  import.cmake
#
#    Description:  emake 框架入口, 包含此文件即可加载 emake 框架
#
#        Version:  1.0
#        Created:  16/11/2017 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================

macro(EMakeImport _dir)

    if(EXISTS ${_dir}/export.cmake)
       include(${_dir}/export.cmake)
    endif()

    if(EXISTS ${_dir}/setup.cmake)
        include(${_dir}/setup.cmake)
    endif()

    if(NOT EXISTS ${_dir}/import.cmake)

        file(GLOB _module_list ${_dir}/modules/*.cmake)
        foreach(_module_file ${_module_list})
            include(${_module_file})
        endforeach()

        #message(FATAL_ERROR "------ ERROR --------\n 'import.cmake' not exist in dir '${_dir}', please check it\n---------------------\n")
    endif()

    include(${_dir}/import.cmake)

endmacro()

#! --------------------------------
#! import framework
#! 在 相关目录中设置 配置
EMakeImport(${CMAKE_CURRENT_LIST_DIR}/framework)

#! --------------------------------
#! import subsystem
#! 在 相关目录中设置 配置
EMakeImportSubSystemM(ebuild)
EMakeImportSubSystemM(etest)
EMakeImportSubSystemM(eoprt)
