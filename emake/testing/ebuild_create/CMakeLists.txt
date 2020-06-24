cmake_minimum_required(VERSION 3.1)

project(emake_fw_test)

# 加载框架
include(../../../emake.setup.cmake)

# 基本配置
EMakeSetLogLevelF(2)                     # 0: 关闭，1：dbg，2：inf，3：wrn
EMakeSetVersionM(emake_fw_test 1 0 0 0)
#EMakeSetBuildTypeM(Release)

# ------------------------------------
# -- 在这里设置所有选项，这些选项将对显示添加的 kit 有效
set(BUILD_ALL_EXTS      OFF)
set(BUILD_ALL_LIBS      OFF)
set(BUILD_ALL_PLGS      OFF)
set(BUILD_ALL_APPS      OFF)
set(BUILD_ALL_PRODUCTS  OFF)

set(OPRT CREATE)
set(OPRT DELETE)

EBuildAddExtM(extdemo1 "" ON ${OPRT})
EBuildAddLibM(libdemo1 "" ON ${OPRT})
EBuildAddPlgM(plgdemo1 "" ON ${OPRT})
EBuildAddAppM(appdemo1 "" ON ${OPRT})

EBuildAddProdM(ab:c "" ON ${OPRT})
EBuildAddProdM(ab:d "" ON ${OPRT})

# ------------------------------------
# -- 生成所需 kit 构建规则
EBuildGenerateM()


