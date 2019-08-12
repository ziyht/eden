EMake Framework
====

emake 是一个基于 cmake 的跨平台工程管理框架，用尽量简单的方式管理大规模的项目。

## 使用

创建一个项目目录，拷贝 emake 目录到此目录下（一般建议拷贝到独立的 cmake 文件夹下），然后拷贝 emake 目录下的 CMakeLists.txt 到主项目即可，就像这样：
   
    [PROJECT_DIR]
        |- cmake/emake/ 
        |- CMakeLists.txt

### 模板文件内容示例：

    cmake_minimum_required(VERSION 3.1)

    # ------------------------------------
    # -- 加载 emake 框架
    include(cmake/emake/import.cmake)

    # ------------------------------------
    # -- 添加 kit
    #
    #EBuildAddExtM(ext_name  ON  "" CREATE)
    #EBuildAddLibM(lib_name  ON  "" CREATE)
    #EBuildAddPlgM(plg_name  ON  "" CREATE)
    #EBuildAddAppM(app_name  ON  "" CREATE)

    # ------------------------------------
    # -- 生成构建规则
    EBuildGenerateM()

## 配置

如有必要，可在如下文件中 查看 或 修改 配置：
* emake/framework/setup.cmake
* emake/subsystem/ebuild/setup.cmake


## tutorial

可以直接打开 emake/testing 中的项目查看，建议使用 Qt

