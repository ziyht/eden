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


