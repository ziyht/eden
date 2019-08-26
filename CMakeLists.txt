cmake_minimum_required(VERSION 3.1)

# ------------------------------------
# -- 加载 emake 框架
include(cmake/emake/import.cmake)

# ------------------------------------
# -- 添加 kit
#
EBuildAddExtM(libuv     ON  "")
EBuildAddLibM(etk       ON  "eden toolkit" )



# ------------------------------------
# -- 生成构建规则
EBuildGenerateM()


