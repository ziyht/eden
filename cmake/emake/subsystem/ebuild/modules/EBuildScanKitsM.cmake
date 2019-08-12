# =====================================================================================
#
#       Filename:  EBuildScanKitsM.cmake
#
#    Description:  auto detective the internal kits and exported kits
#
#        Version:  1.0
#        Created:  2017.12.17 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================


#
# \! 查找内部含有源码的 kit
#   (用了回调，不要改成 macro)
#
function(_ScanInternalKitsF i_type i_path)

    if(NOT i_path)
        EMakeCheckDefinedF(${i_type}S_DIR)
        set(_root_dir ${${i_type}S_DIR})
    else()
        set(_root_dir ${i_path})
    endif()

    file(GLOB _list "${_root_dir}/*")

    foreach(_path ${_list})

        if(EXISTS ${_path}/CMakeLists.txt )

            if(${i_type} STREQUAL "TYPE")
                file(RELATIVE_PATH _kit ${PRODS_DIR} ${_path})
                string(REGEX REPLACE "/.+$" "" _prod ${_kit})
                string(REGEX REPLACE "^.+/" "" _type ${_kit})
                EMakeSetTargetPropertyM(${_kit} BLONG_TO  VAR ${_prod})
                EMakeSetTargetPropertyM(${_kit} TYPE_NAME VAR ${_type})
            else()
                file(RELATIVE_PATH _kit ${${i_type}S_DIR} ${_path})
            endif()

            EMakeInfF("-- scan internal ${i_type}: ${_kit}" PREFIX "   ")

            EMakeSetGlobalPropertyM(KNOWN_${i_type}S_KITS       VAR ${_kit}             APPEND)
            EMakeSetTargetPropertyM(${_kit}    KIT_IS_INTERNAL  VAR 1)
            EMakeSetTargetPropertyM(${_kit}    OPTION_NAME      VAR ${i_type}_${_kit}   DEFAULT)
            EMakeSetTargetPropertyM(${_kit}    KIT_TYPE         VAR ${i_type})
            EMakeSetTargetPropertyM(${_kit}    KIT_SOURCES_DIR  VAR ${_path})
        endif()

        # 查找下一层级
        if(IS_DIRECTORY ${_path} AND ${i_type} STREQUAL "PROD")
            _ScanInternalKitsF("TYPE" ${_path} ${i_root})
        endif()

    endforeach()

endfunction()

function(_GetExportedLibrariesF o_libraries)
    cmake_parse_arguments(M "" "" "FILES" ${ARGN})

    set(_libraries)

    foreach(_file ${M_FILES})

        if(${_file} MATCHES "\\.(lib|a|dll|dll.a)$")
            EMakeGetLibnameFromFilenameM(${_file} _library)

            #EMakeDbgF(" get lib name: ${_library}")

            list(APPEND _libraries ${_library})
        endif()

    endforeach()

    if(_libraries)
        list(REMOVE_DUPLICATES  _libraries)
    endif()
    set(${o_libraries} ${_libraries} PARENT_SCOPE)

endfunction()

#
# \! 查找有导出的 kit
#   (用了回调，不要改成 macro)
#
function(_ScanExportedKitsF i_type i_path)

    if(NOT i_path)
        EMakeCheckDefinedF(${i_type}S_EXPORTS_DIR)
        set(_root_dir ${${i_type}S_EXPORTS_DIR})
    else()
        set(_root_dir ${i_path})
    endif()

    file(GLOB _list "${_root_dir}/*")

    foreach(_path ${_list})

        if(EXISTS ${_path}/setupManifist.cmake)

            set(_exe_path ${_path}/${BUILD_ARCH})
            file(GLOB _exports RELATIVE ${_exe_path} "${_exe_path}/*.dll" "${_exe_path}/*.a" "${_exe_path}/*.lib" "${_exe_path}/*.exe")

            list(LENGTH _exports _cnt)

            if(_cnt GREATER 0)
                file(RELATIVE_PATH _kit ${${i_type}S_EXPORTS_DIR} ${_path})
                _GetExportedLibrariesF(_libraries FILES ${_exports})

                EMakeInfF("-- scan exported ${i_type}: ${_kit}" PREFIX "   ")

                EMakeSetGlobalPropertyM(KNOWN_${i_type}S_KITS            VAR ${_kit}            APPEND)
                EMakeSetTargetPropertyM(${_kit}    KIT_IS_EXPORTED       VAR 1)
                EMakeSetTargetPropertyM(${_kit}    OPTION_NAME           VAR ${i_type}_${_kit}  DEFAULT)
                EMakeSetTargetPropertyM(${_kit}    KIT_TYPE              VAR ${i_type})
                EMakeSetTargetPropertyM(${_kit}    KIT_EXPORTS_DIR       VAR ${_path})
                EMakeSetTargetPropertyM(${_kit}    KIT_EXPORTS_LIBS LIST_VAR ${_libraries})
            endif()

        endif()

        # 查找下一层级
        if(IS_DIRECTORY ${_path})
            _ScanExportedKitsF(${i_type} ${_path})
        endif()

    endforeach()

endfunction()

macro(EBuildScanKitsM)

    EMakeInfF("----------- Scanning -----------")

    _ScanInternalKitsF(EXT     "")
    _ScanInternalKitsF(LIB     "")
    _ScanInternalKitsF(PLG     "")
    _ScanInternalKitsF(APP     "")
    _ScanInternalKitsF(PROD    "")

    _ScanExportedKitsF(EXT     "")
    _ScanExportedKitsF(LIB     "")
    _ScanExportedKitsF(PLG     "")
    _ScanExportedKitsF(APP     "")

    foreach(_type EXTS LIBS PLGS APPS TYPES PRODS)
        EMakeGetGlobalPropertyM(KNOWN_${_type}_KITS KNOWN_${_type}_KITS NO_CHECK)

        if(KNOWN_${_type}_KITS)
            list(REMOVE_DUPLICATES KNOWN_${_type}_KITS)
            EMakeSetGlobalPropertyM(KNOWN_${_type}_KITS  LIST_VAR ${KNOWN_${_type}_KITS})
            EMakeSetGlobalPropertyM(KNOWN_KITS           LIST_VAR ${KNOWN_${_type}_KITS} APPEND)
            EMakeSetGlobalPropertyM(KNOWN_TARGETS        LIST_VAR ${KNOWN_${_type}_KITS} APPEND)
        endif()
    endforeach()



endmacro()
