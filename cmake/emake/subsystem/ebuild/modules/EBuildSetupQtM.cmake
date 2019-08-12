# =====================================================================================
#
#       Filename:  EMakeSetupQt.cmake
#
#    Description:  set qt environment
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
cmake_minimum_required(VERSION 2.8.1)

macro(_configuring_qt_core)

    if(NOT CONFIGURED_QT_CORE)

        if(QT_VERSION VERSION_GREATER "4")

            find_package(Qt5 COMPONENTS Core Gui)

            if(NOT Qt5_FOUND)
                emakeErrF("faild in configuring 'Qt5::Core': not found")
            endif()

            get_target_property(QtCore_location Qt5::Core LOCATION)
            get_target_property(QtCore_lib_type Qt5::Core IMPORTED_LIB_TYPE)
            set(g_QtCore_lib_type)
            if(NOT ${QtCore_lib_type} STREQUAL "QtCore_lib_type-NOTFOUND" AND ${QtCore_lib_type} STREQUAL "STATIC")
                if(WIN32)
                    list(APPEND QT5_COMPONENTS EMakePlatform)
                    add_definitions(-DQT_STATIC_LIB)
                    set(g_QtCore_lib_type "STATIC")
                endif()
            endif()

        else()
            emakeErrF("you need at leat Qt5 to build this project")
        endif()

        # Set the variable QT_INSTALLED_LIBRARY_DIR that contains all
        # Qt shared libraries
        if (QT_VERSION VERSION_GREATER "4")
            if(WIN32)
                get_target_property(_qt5_moc_executable Qt5::moc LOCATION)
                get_filename_component(QT_INSTALLED_LIBRARY_DIR ${_qt5_moc_executable} PATH)
            else()
                get_target_property(_qt5_core_lib Qt5::Core LOCATION)
                get_filename_component(QT_INSTALLED_LIBRARY_DIR ${_qt5_core_lib} PATH)
            endif()
        else()
            set(QT_INSTALLED_LIBRARY_DIR ${QT_LIBRARY_DIR})
            if(WIN32)
                get_filename_component(QT_INSTALLED_LIBRARY_DIR ${QT_QMAKE_EXECUTABLE} PATH)
            endif()
        endif()

        set(CONFIGURED_QT_CORE 1)
        emakeSetGlobalPropertyM(KNOWN_QT_PACKEGES LIST_VAR Qt5::Core Qt5::Gui APPEND_NO_DUP)
    endif()

endmacro()

macro(EBuildSetupQtM)

    set(QT_VERSION "5" CACHE STRING "Expected Qt version")

    set(PACKAGES_NOT_FOUND)

    EMakeGetGlobalPropertyM(KNOWN_QT_PACKEGES _known_packages NO_CHECK)

    foreach(package ${ARGN})

        set(${package}_FOUND 1)

        list(FIND _known_packages ${package} _id)

        if(_id EQUAL -1)
            _configuring_qt_core()

            # try find qt package
            # set(QT5_COMPONENTS Core Gui Xml XmlPatterns Concurrent Sql Test Widgets OpenGL UiTools)
            string(REGEX REPLACE "^(Q|q)(T|t)(4|5)::"  "" component ${package})

            find_package(Qt5 COMPONENTS ${component} QUIET)

            if(NOT Qt5_FOUND)
                set(${package}_FOUND)
                list(APPEND PACKAGES_NOT_FOUND ${package})
            else()
                EMakeSetGlobalPropertyM(KNOWN_QT_PACKEGES LIST_VAR ${package} APPEND_NO_DUP)
            endif()

        endif()

    endforeach()

#    foreach(_dep_qt ${_deps_qt})
#        list(FIND KNOWN_QT_PACKEGES ${_dep_qt} _o_index)

#        if(_o_index EQUAL -1)
#            find_package(${_dep_qt} QUIET)

#            set(_found)

#            if(NOT ${_dep_qt}_FOUND)
#                find_library(lib NAMES ${_dep_qt} PATHS ${paths})

#                if(${lib} STREQUAL "lib-NOTFOUND")

#                    if(TARGET ${_dep_qt})

#                        get_target_property(_my_PROPERTIES ${_dep_qt} IMPORTED_LOCATION_DEBUG)  # only check DEBUG now

#                        if(_my_PROPERTIES)
#                            list(APPEND KNOWN_QT_PACKEGES ${_dep_qt})
#                        else()
#                            ParaErrF("the kit '${i_kit}' depends on a QT package/library [${_dep_qt}] which can not be found, please check you 'setupManifist.cmake' file")
#                        endif()

#                    else()

#                        ParaErrF("the kit '${i_kit}' depends on a QT package/library [${_dep_qt}] which can not be found, please check you 'setupManifist.cmake' file")

#                    endif()
#                else()
#                    list(APPEND KNOWN_QT_PACKEGES ${_dep_qt})
#                endif()
#            endif()
#        endif()
#    endforeach()

endmacro()
