# =====================================================================================
# \! 项目配置设置
#
#   在这里可以使用的框架内部变量（部分），注意不要重新设置这些变量：
#       MAIN_PROJECT_ROOT_DIR    主项目根目录
#       PROJECT_ROOT_DIR         本项目根目录
#       KIT_DIR                  当前子项目目录，即当前文件所在目录
#       KIT_NAME                 当前子项目名称
#

set(KIT_LIBRARY_TYPE        STATIC)     # 库类型    未设置则使用默认值, STATIC（默认）|SHARED|MODULE，
set(KIT_EXPORT              ON)         # 导出开关  未设置则使用默认值, ON|OFF（默认）
set(KIT_EXPORT_DIR          )           # 导出目录  未设置则使用 ${COMMONS_DIR}/libs/${KIT_NAME}
set(KIT_EXPECT_DIR          )           # 期望目标

# 源文件
set(KIT_SRCS
    para_mainwindow.cpp
    para_mainwindow.h
    para_mainwindow_p.cpp
    para_mainwindow_p.h
    )

# moc 文件
set(KIT_MOC_SRCS
    para_mainwindow.h
    para_mainwindow_p.h
    )

# UI文件
set(KIT_UI_FORMS
)

# 资源文件
set(KIT_RESOURCES
)

# 输出后缀
#   设置 DEBUG RELEASE RELWITHDEBINFO MINSIZEREL 模式下的后缀名
#       设置方式1： d INHERIT rd " "  （INHERIT 表示继承来自主项目的设置, " " 表示为空）
#       设置方式2： DEBUG d [RELEASE " " RELWITHDEBINFO rd ... ]
#       (两种方式不能混合使用，默认值均为 INHERIT，可以使用 " " 来设置空值但不要使用 "")
set(KIT_POSTFIX )

# 附加文件
EBuildAddFilesF(
#       DESTINATION_PREFIX  <目标位置前缀>                        // 相对路径，目标位置的前缀，相对本kit输出目录，若不设置，使用 '/'
#       FILES <filename1> ... [POSTFIX <目标位置后缀>]            // 设定文件列表及列表文件想要拷贝到的位置，可以有多行
#       DIRS  <dir_path1> ... [POSTFIX <目标位置后缀>]            // 设定目录列表及列表目录想要拷贝到的位置，可以有多行
)

# 设置依赖
#
#   格式：
#       [产品名:]目标名[|平台1|平台2|...]
#
#   支持设置的平台：
#       WIN32 APPLE LINUX REDHAT REDHAT6 REDHAT7 CENTOS CENTOS6 CENTOS7 UBUNTU UBUNTU14
#
#   示例：
#       Qt5::Core|LINUX      将只在 LINUX 平台上依赖 Qt5::Core
#       Qt5::Core|WIN32      将只在 WIN32 平台上依赖 Qt5::Core
#
set(KIT_DEPENDS Qt5::Core Qt5::Gui Qt5::Widgets)

