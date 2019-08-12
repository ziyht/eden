#
#   a file to set the properties of a lib target
#
#

set(KIT_LIBRARY_TYPE        SHARED)     # 库类型    未设置则使用默认值, STATIC（默认）|SHARED|MODULE
set(KIT_EXPORT              ON)         # 导出开关  未设置则使用默认值, ON|OFF（默认）
set(KIT_EXPORT_DIR          )           # 导出目录  未设置则使用 ${COMMONS_DIR}/libs/${KIT_NAME}

# 在这里添加 源文件
set(KIT_SRCS
    libdemo12.cpp
    libdemo12.h
    )

# 在这里添加需要moc的头文件
set(KIT_MOC_SRCS
    )

# 在这里添加 UI文件
set(KIT_UI_FORMS
)

# 在这里添加 资源文件
set(KIT_RESOURCES
)

# 在这里添加 依赖
#
#   格式：
#       [产品名:]目标名[|平台1|平台2|...]
#
#   支持设置的平台：
#       WIN32 APPLE LINUX REDHAT REDHAT6 REDHAT7 CENTOS CENTOS6 CENTOS7 UBUNTU UBUNTU14
#
#   示例：
#       Qt5::Core            在所有平台上依赖 Qt5::Core
#       Qt5::Core|LINUX      将只在 LINUX 平台上依赖 Qt5::Core
#       Qt5::Core|WIN32      将只在 WIN32 平台上依赖 Qt5::Core
#
set(KIT_DEPENDS Qt5::Core Qt5::Gui Qt5::Widgets extdemo1 libdemo1)
