# =====================================================================================
#
#       Filename:  setup.cmake
#
#    Description:  set the configuration of emake frame work or subsystem
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

# --------------------------------------
# BASIC CONFIG
# --------------------------------------
set(_PROJECT_NAME       etools)
set(_PROJECT_VERSION    1 0 0 1)        # major minor patch [build]
set(_LOG_LEVEL          2)              # 0: 关闭，1：dbg，2：inf，3：wrn
set(_BUILD_TYPE         )               # Debug|Release|MinSizeRel|RelWithDebInfo [FORCE]

# --------------------------------------
# BUILD FLAGS
# --------------------------------------
if(APPLE)

elseif(MSVC)

    set(CMAKE_EXE_LINKER_FLAGS          "/NODEFAULTLIB:libcmtd.lib")

else()

    set(_W_FLAGS  "-Wall -Wextra -Wcast-align -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wuninitialized -Wno-switch -Wno-strict-aliasing")

    set(CMAKE_C_FLAGS_DEBUG             "-g -O0 -pedantic -std=gnu99 -fPIC ${_W_FLAGS}")
    set(CMAKE_C_FLAGS_RELEASE           "   -O2 -pedantic -std=gnu99 -fPIC ${_W_FLAGS}")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO    "   -O2 -pedantic -std=gnu99 -fPIC ${_W_FLAGS}")
    set(CMAKE_C_FLAGS_MINSIZEREL        "   -O2 -pedantic -std=gnu99 -fPIC ${_W_FLAGS}")

    set(CMAKE_CXX_FLAGS_DEBUG           "-g -O0 -pedantic -fPIC ${_W_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE         "   -O2 -pedantic -fPIC ${_W_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "   -O2 -pedantic -fPIC ${_W_FLAGS}")
    set(CMAKE_CXX_FLAGS_MINSIZEREL      "   -O2 -pedantic -fPIC ${_W_FLAGS}")
endif()




