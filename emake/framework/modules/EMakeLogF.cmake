# =====================================================================================
#
#       Filename:  EMakeLogF.cmake
#
#    Description:  set cmake environment
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

if(NOT DEFINED EMakeLogLevel)
    set(EMakeLogLevel 1)
endif()

EMakeSetGlobalPropertyM(_EMakeLogLevelStack LIST_VAR )

function(EMakeDbgF)

    cmake_parse_arguments(M "" "PREFIX" "" ${ARGN})

    if(M_PREFIX)
        set(_prefix ${M_PREFIX})
    else()
        set(_prefix "   ")
    endif()

    if(EMakeLogLevel AND EMakeLogLevel LESS 2)
        message("${_prefix}" ${M_UNPARSED_ARGUMENTS})
    endif()
endfunction()

function(EMakeInfF)

    cmake_parse_arguments(M "" "PREFIX" "" ${ARGN})

    if(M_PREFIX)
        set(_prefix ${M_PREFIX})
    else()
        set(_prefix "-- ")
    endif()

    if(EMakeLogLevel AND EMakeLogLevel LESS 3)
        message("${_prefix}" ${M_UNPARSED_ARGUMENTS})
    endif()
endfunction()

function(EMakeWrnF)

    if(EMakeLogLevel AND EMakeLogLevel LESS 4)
        message(WARNING "------ WARNING --------\n" ${ARGN} "\n---------------------\n")
    endif()
endfunction()

function(EMakeErrF)
    message(FATAL_ERROR "------ ERROR --------\n" ${ARGN} "\n---------------------\n")
endfunction()


macro(_EMakeLogLevelSet i_level)

    if(i_level LESS 1)
        set(i_level 4)
    endif()

    set(EMakeLogLevel ${i_level} CACHE INTERNAL "" FORCE PARENT_SCOPE)

endmacro()

function(EMakeSetLogLevelF i_level)
    _EMakeLogLevelSet(${i_level})
endfunction()

function(EMakeLogLevelPush i_level)

    EMakeGetGlobalPropertyM(_EMakeLogLevelStack _stack NO_CHECK)

    list(GET EMakeLogLevel 0 _level)
    EMakeSetGlobalPropertyM(_EMakeLogLevelStack _stack LIST_VAR ${_level} ${_stack})

    _EMakeLogLevelSet(${i_level})
endfunction()

function(EMakeLogLevelPop)

    EMakeGetGlobalPropertyM(_EMakeLogLevelStack _stack NO_CHECK)

    if(NOT _stack)
        EMakeErrF("no stack log level left, please push first")
        return()
    endif()

    list(GET       _stack 0 _level)
    list(REMOVE_AT _stack 0)

    EMakeSetGlobalPropertyM(_EMakeLogLevelStack LIST_VAR ${_stack})
    EMakeGetGlobalPropertyM(_EMakeLogLevelStack _stack NO_CHECK)
    _EMakeLogLevelSet(${_level})

endfunction()
