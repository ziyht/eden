# =====================================================================================
#
#       Filename:  EOprtAddM.cmake
#
#    Description:  API to add operations for a kit
#
#        Version:  1.0
#        Created:  2017.12.25 04:38:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
# =====================================================================================


set(EXTS_POSITIONS PRE_TARGET PRE_DOWNLOAD POST_DOWNLOAD PRE_UPDATE POST_UPDATE PRE_PATCH POST_PATCH PRE_CONFIGURE POST_CONFIGURE PRE_BUILD POST_BUILD PRE_TEST POST_TEST PRE_INSTALL POST_INSTALL )
set(LIBS_POSITIONS PRE_TARGET POST_BUILD PRE_EXPORT POST_EXPORT)
set(PLGS_POSITIONS ${LIBS_POSITIONS})
set(APPS_POSITIONS ${LIBS_POSITIONS})

set(TYPES_POSITIONS PRE_EXPORT POST_EXPORT)
set(PRODS_POSITIONS PRE_EXPORT POST_EXPORT)

set(ALL_POSITIONS  ${EXTS_POSITIONS} ${LIBS_POSITIONS} ${PLGS_POSITIONS} ${APPS_POSITIONS} ${TYPES_POSITIONS} ${PRODUCTS_POSITIONS})
list(REMOVE_DUPLICATES ALL_POSITIONS)

macro(_oprtCheckKitNameM)

    if(NOT M_KIT_NAME)
        set(_KIT_NAME ${KIT_NAME})
        if(${_KIT_NAME} STREQUAL "")
            EMakeErrF("you should set the param 'KIT_NAME'")
        endif()
    else()
        set(_KIT_NAME ${M_KIT_NAME})
    endif()

    EMakeGetGlobalPropertyM(ADDED_KITS ADDED_KITS NO_CHECK)

    list(FIND ADDED_KITS ${_KIT_NAME} _o_index)
    if(_o_index EQUAL -1)
        EMakeErrF("the kit '${_KIT_NAME}' you want to set the operation is not in the build system")
    endif()

endmacro()

macro(_oprtCheckArgStepM)

    set(_check_list)

    foreach(_pos ${ALL_POSITIONS})
        if(M_${_pos})
            list(APPEND _check_list ${_pos})
        endif()
    endforeach()

    list(LENGTH _check_list _len)

    if(_len EQUAL 0)
        EMakeErrF("you should set a position in one of [ ${ALL_POSITIONS} ]")
    elseif(_len GREATER 1)
        EMakeErrF("you setted more than one postions: [ ${_check_list} ]")
    endif()

    list(GET _check_list 0 M_POSTION)

endmacro()

macro(_oprtCheckPositionM)

    list(FIND ${_KIT_TYPE}S_POSITIONS ${M_POSTION} _o_index)
    if(_o_index EQUAL -1)
        EMakeErrF("the position '${M_POSTION}' you setted for '${_KIT_NAME}' is invalid, you can only set one of [${${_KIT_TYPE}_POSITIONS}]")
    endif()

endmacro()

macro(_oprtAddOperationForExtTargetsM i_post_step i_pre_step)

    EMakeSetTargetPropertyM(${_KIT_NAME} ${i_post_step}_${i_pre_step} VAR __ APPEND)
    EMakeGetTargetPropertyM(${_KIT_NAME}  ${i_post_step}_${i_pre_step} _cache_steps)

    list(LENGTH _cache_steps INDEX)

    ExternalProject_Add_Step(${_KIT_NAME} ${i_post_step}_${i_pre_step}${INDEX}
        ${_COMMAND}
        ${_COMMENT}
        ${_WORKING_DIRECTORY}
        DEPENDEES ${i_post_step}
        DEPENDERS ${i_pre_step}
      )

endmacro()

macro(_oprtAddOperationInPreTargetM)

    if(NOT TARGET ${_KIT_NAME}_pre)
        add_custom_target(${_KIT_NAME}_pre)
        add_dependencies(${_KIT_NAME} ${_KIT_NAME}_pre)
    endif()

    add_custom_command(TARGET ${_KIT_NAME}_pre POST_BUILD
            ${_COMMAND}
            ${_COMMENT}
            ${_WORKING_DIRECTORY}
            ${_VERBATIM}
        )

endmacro()

macro(_oprtAddOperationInPostBuildM)

    if(${_KIT_TYPE}S MATCHES "^LIBS$|^PLGS$|^APPS$")

        add_custom_command(TARGET ${_KIT_NAME} PRE_BUILD
                ${_COMMAND}
                ${_COMMENT}
                ${_WORKING_DIRECTORY}
                ${_VERBATIM}
            )
    else()
        EMakeErrF("internal err: position 'POST_BUILD' not supported for ${_KIT_TYPE}S")
    endif()

endmacro()

macro(_oprtAddOperationInPreExportM)

    if(${_KIT_TYPE}S STREQUAL "EXTS")
        EMakeErrF("not supported now")
    elseif(${_KIT_TYPE}S MATCHES "^LIBS$|^PLGS$|^APPS$|^TYPES$|^PRODS$")
        add_custom_command(TARGET ${_KIT_NAME} PRE_LINK
                ${_COMMAND}
                ${_COMMENT}
                ${_WORKING_DIRECTORY}
                ${_VERBATIM}
            )
    else()
        EMakeErrF("internal err: position 'PRE_EXPORT' not supported for ${_KIT_TYPE}S")
    endif()

endmacro()

macro(_oprtAddOperationInPostExportM)

    if(${_KIT_TYPE}S MATCHES "^LIBS$|^PLGS$|^APPS$|^TYPES$|^PRODS$")
        add_custom_command(TARGET ${_KIT_NAME} POST_BUILD
                ${_COMMAND}
                ${_COMMENT}
                ${_WORKING_DIRECTORY}
                ${_VERBATIM}
            )
    else()
        EMakeErrF("internal err: position 'POST_EXPORT' not supported for ${_KIT_TYPE}S")
    endif()

endmacro()

macro(_oprtAddOperationForExtsM)
    if    (${M_POSTION} STREQUAL "PRE_TARGET")
        _oprtAddOperationInPreTargetM()
    elseif(${M_POSTION} STREQUAL "PRE_DOWNLOAD")
        _oprtAddOperationForExtTargetsM("" download)
    elseif(${M_POSTION} STREQUAL "POST_DOWNLOAD")
        _oprtAddOperationForExtTargetsM(download update)
    elseif(${M_POSTION} STREQUAL "PRE_UPDATE")
        _oprtAddOperationForExtTargetsM(download update)
    elseif(${M_POSTION} STREQUAL "POST_UPDATE")
        _oprtAddOperationForExtTargetsM(update patch)
    elseif(${M_POSTION} STREQUAL "PRE_PATCH")
        _oprtAddOperationForExtTargetsM(update patch)
    elseif(${M_POSTION} STREQUAL "POST_PATCH")
        _oprtAddOperationForExtTargetsM(patch configure)
    elseif(${M_POSTION} STREQUAL "PRE_CONFIGURE")
        _oprtAddOperationForExtTargetsM(patch configure)
    elseif(${M_POSTION} STREQUAL "POST_CONFIGURE")
        _oprtAddOperationForExtTargetsM(configure build)
    elseif(${M_POSTION} STREQUAL "PRE_BUILD")
        _oprtAddOperationForExtTargetsM(configure build)
    elseif(${M_POSTION} STREQUAL "POST_BUILD")

        if(_TEST_BEFORE_INSTALL)            # _TEST_BEFORE_INSTALL setted in EMakeBuildExtM
            if(TARGET ${_KIT_NAME}-test)
                _oprtAddOperationForExtTargetsM(build test)
            else()
                _oprtAddOperationForExtTargetsM(build install)
            endif()
        else()
            _oprtAddOperationForExtTargetsM(build install)
        endif()
    elseif(${M_POSTION} STREQUAL "PRE_TEST")
        if(NOT TARGET ${_KIT_NAME}-test)
            EMakeErrF("you can not add operations in PRE_TEST for '${_KIT_NAME}', because this kit haven't a 'test' step, please set it first\n"
                "(you can set 'KIT_TEST_COMMAD' to set the 'test' step)")
        endif()

        if(_TEST_BEFORE_INSTALL)
            _oprtAddOperationForExtTargetsM(build test)
        else()
            _oprtAddOperationForExtTargetsM(install test)
        endif()

    elseif(${M_POSTION} STREQUAL "POST_TEST")
        if(NOT TARGET ${_KIT_NAME}-test)
            EMakeErrF("you can not add operations in POST_TEST for '${_KIT_NAME}', because this kit haven't a 'test' step, please set it first\n"
                "(you can set 'KIT_TEST_COMMAD' to set the 'test' step)")
        endif()

        if(_TEST_BEFORE_INSTALL)
            _oprtAddOperationForExtTargetsM(test install)
        else()
            _oprtAddOperationForExtTargetsM(test "")
        endif()

    elseif(${M_POSTION} STREQUAL "PRE_INSTALL")

        if(_TEST_BEFORE_INSTALL)

            if(TARGET ${_KIT_NAME}-test)
                _oprtAddOperationForExtTargetsM(test install)
            else()
                _oprtAddOperationForExtTargetsM(build install)
            endif()

        else()
             _oprtAddOperationForExtTargetsM(build install)
        endif()

    elseif(${M_POSTION} STREQUAL "POST_INSTALL")

        if(_TEST_BEFORE_INSTALL)
            _oprtAddOperationForExtTargetsM(install "")
        else()

            if(TARGET ${_KIT_NAME}-test)
                _oprtAddOperationForExtTargetsM(install test)
            else()
                _oprtAddOperationForExtTargetsM(install "")
            endif()

        endif()
    else()
        EMakeErrF("internal err: unknown position '${M_POSTION}' in _oprtAddOperationForExtsM()")
    endif()
endmacro()

macro(_oprtAddOperationForLibsM)

    if    (${M_POSTION} STREQUAL "PRE_TARGET")
        _oprtAddOperationInPreTargetM()
    elseif(${M_POSTION} STREQUAL "POST_BUILD")
        _oprtAddOperationInPostBuildM()
    elseif(${M_POSTION} STREQUAL "PRE_EXPORT")
        _oprtAddOperationInPreExportM()
    elseif(${M_POSTION} STREQUAL "POST_EXPORT")
        _oprtAddOperationInPostExportM()
    else()
        EMakeErrF("internal err: unknown position '${M_POSTION}' in _oprtAddOperationForLibsM()")
    endif()

endmacro()

macro(_oprtAddOperationForPlgsM)

    if    (${M_POSTION} STREQUAL "PRE_TARGET")
        _oprtAddOperationInPreTargetM()
    elseif(${M_POSTION} STREQUAL "POST_BUILD")
        _oprtAddOperationInPostBuildM()
    elseif(${M_POSTION} STREQUAL "PRE_EXPORT")
        _oprtAddOperationInPreExportM()
    elseif(${M_POSTION} STREQUAL "POST_EXPORT")
        _oprtAddOperationInPostExportM()
    else()
        EMakeErrF("internal err: unknown position '${M_POSTION}' in _oprtAddOperationForPlgsM()")
    endif()

endmacro()

macro(_oprtAddOperationForAppsM)

    if    (${M_POSTION} STREQUAL "PRE_TARGET")
        _oprtAddOperationInPreTargetM()
    elseif(${M_POSTION} STREQUAL "POST_BUILD")
        _oprtAddOperationInPostBuildM()
    elseif(${M_POSTION} STREQUAL "PRE_EXPORT")
        _oprtAddOperationInPreExportM()
    elseif(${M_POSTION} STREQUAL "POST_EXPORT")
        _oprtAddOperationInPostExportM()
    else()
        EMakeErrF("internal err: unknown position '${M_POSTION}' in _oprtAddOperationForAppsM()")
    endif()

endmacro()

macro(_oprtAddOperationForTypesM)

    if    (${M_POSTION} STREQUAL "PRE_EXPORT")
        _oprtAddOperationInPreExportM()
    elseif(${M_POSTION} STREQUAL "POST_EXPORT")
        _oprtAddOperationInPostExportM()
    else()
        EMakeErrF("internal err: unknown position '${M_POSTION}' in _oprtAddOperationForAppsM()")
    endif()

endmacro()

macro(_oprtAddOperationForProdsM)

    if    (${M_POSTION} STREQUAL "PRE_EXPORT")
        _oprtAddOperationInPreExportM()
    elseif(${M_POSTION} STREQUAL "POST_EXPORT")
        _oprtAddOperationInPostExportM()
    else()
        EMakeErrF("internal err: unknown position '${M_POSTION}' in _oprtAddOperationForAppsM()")
    endif()

endmacro()

#
#   EOprtAddCommandF(KIT_NAME name)
#
#
#
#

function(EOprtAddCommandF)

    if(NOT __EBUILD_CONFIGURING_BUILD)
        return()
    endif()

    cmake_parse_arguments(M
        "VERBATIM;${ALL_POSITIONS}"
        "KIT_NAME;WORKING_DIRECTORY;COMMENT"
        "COMMAND" ${ARGN})

    if(M_UNPARSED_ARGUMENTS)
        EMakeErrF("unknown arguments '${M_UNPARSED_ARGUMENTS}' checked")
    endif()

    _oprtCheckKitNameM()

    # 获取必要的属性
    EMakeGetTargetPropertyM(${_KIT_NAME} KIT_TYPE _KIT_TYPE)

    _oprtCheckArgStepM()
    _oprtCheckPositionM()

    # 设置参数
    if(M_VERBATIM)
        set(_VERBATIM VERBATIM)
    endif()
    set(_COMMAND            COMMAND           ${M_COMMAND})
    set(_COMMENT            COMMENT           ${M_COMMENT})
    set(_WORKING_DIRECTORY  WORKING_DIRECTORY ${M_WORKING_DIRECTORY})


    # 添加操作
    if(${_KIT_TYPE}S STREQUAL "EXTS")
        _oprtAddOperationForExtsM()
    elseif(${_KIT_TYPE}S STREQUAL "LIBS")
        _oprtAddOperationForLibsM()
    elseif(${_KIT_TYPE}S STREQUAL "PLGS")
        _oprtAddOperationForPlgsM()
    elseif(${_KIT_TYPE}S STREQUAL "APPS")
        _oprtAddOperationForAppsM()
    elseif(${_KIT_TYPE}S STREQUAL "TYPES")
        _oprtAddOperationForTypesM()
    elseif(${_KIT_TYPE}S STREQUAL "PRODS")
        _oprtAddOperationForProdsM()
    else()
        EMakeErrF("internal err: unkown type for kit '${_KIT_NAME}': ${_KIT_TYPE}")
    endif()

endfunction()
