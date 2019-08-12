#
#   a file to set the properties of a plg target
#
#

set(KIT_TARGET              )           # 构建名称  未设置则使用目标名 ${KIT_NAME}
set(KIT_LIBRARY_TYPE        MODULE)     # 库类型    未设置则使用默认值, STATIC|SHARED|MODULE(默认)，
set(KIT_EXPORT              ON)         # 导出开关  未设置则使用默认值, ON|OFF（默认）
set(KIT_EXPORT_DIR          )           # 导出目录  未设置则使用 ${COMMONS_DIR}/libs/${KIT_NAME}

# 在这里添加 源文件
set(KIT_SRCS
    plg_non_gui.cpp
    plg_non_gui.h
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

# 附加文件
EBuildAddFilesF(
#    DESTINATION_PREFIX  <目标位置前缀>                        // 相对路径，目标位置的前缀，相对本kit输出目录，若不设置，使用 '/'
#    FILES <filename1> ... [POSTFIX <目标位置后缀>]            // 设定文件列表及列表文件想要拷贝到的位置，可以有多行
#    DIRS  <dir_path1> ... [POSTFIX <目标位置后缀>]            // 设定目录列表及列表目录想要拷贝到的位置，可以有多行

    FILES plgdemo3_1.txt
          plgdemo3_2.txt

    DIRS  plgdemo3_dir1
          plgdemo3_dir2
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
#       Qt5::Core|LINUX      将只在 LINUX 平台上依赖 Qt5::Core
#       Qt5::Core|WIN32      将只在 WIN32 平台上依赖 Qt5::Core
#
set(KIT_DEPENDS Qt5::Core Qt5::Gui Qt5::Widgets extdemo1::Add)
