cmake_minimum_required(VERSION 3.1)

# ------------------------------------
# -- 加载 emake 框架
include(cmake/emake/import.cmake)

# ------------------------------------
# -- 添加 kit
#
#EBuildAddExtM(ext_name  ON  "" [CREATE] [DEBUG])
#EBuildAddLibM(lib_name  ON  "" [CREATE] [DEBUG])
#EBuildAddPlgM(plg_name  ON  "" [CREATE] [DEBUG])
#EBuildAddAppM(app_name  ON  "" [CREATE] [DEBUG])

# ------------------------------------
# -- 生成构建规则
EBuildGenerateM()


