cmake_minimum_required(VERSION 3.1)

project(emake_fw_test)

# 加载框架
include(../../../emake/import.cmake)

# ------------------------------------
# -- 在这里添加 外部库 kit
# EBuildAddExtM(lib_name doc ON/OFF [LOGICAL_EXPR])
#
EBuildAddExtM(extdemo1 ON "" )
EBuildAddExtM(extdemo2 ON "" )

# ------------------------------------
# -- 在这里添加 内部库 kit
# EBuildAddLibM(lib_name doc ON/OFF [LOGICAL_EXPR])
#
#EBuildAddLibM(libdemo1 ON "")                 # 一个独立的内部库 static
#EBuildAddLibM(libdemo2 ON "")                 # 一个独立的内部库 shared

#EBuildAddLibM(libdemo3 ON "")                # 依赖一个外部库，依赖于 extdemo1::Add，有自己的文件
#EBuildAddLibM(libdemo4 ON "")                # 依赖一个外部库，依赖于 extdemo1::Sub
#EBuildAddLibM(libdemo5 ON "")                # 依赖一个外部库，依赖于 extdemo1

#EBuildAddLibM(libdemo6 ON "")                # 依赖多个外部库，依赖于 extdemo1::Add extdemo2::Mul
#EBuildAddLibM(libdemo7 ON "")                # 依赖多个外部库，依赖于 extdemo1::Sub extdemo2::Div
#EBuildAddLibM(libdemo8 ON "")                # 依赖多个外部库，依赖于 extdemo1 extdemo2

#EBuildAddLibM(libdemo9  ON "")               # 依赖一个内部库，依赖于 libdemo1
#EBuildAddLibM(libdemo10 ON "")               # 依赖多个内部库，依赖于 libdemo1 libdemo2
#EBuildAddLibM(libdemo11 ON "")               # 依赖多层级内部库，依赖于 libdemo9

#EBuildAddLibM(libdemo12  ON "")              # 同时依赖外部库和内部库，依赖于 extdemo1 libdemo1
#EBuildAddLibM(libdemo13  ON "")              # 同时依赖外部库和内部库，依赖于 extdemo1 extdemo2 libdemo1 libdemo2
#EBuildAddLibM(libdemo14  ON "")              # 同时依赖外部库和多层级内部库，依赖于 extdemo1 libdemo9
#EBuildAddLibM(libdemo15  ON "")              # 同时依赖外部库和多层级内部库，依赖于 extdemo1 extdemo2 libdemo9


#EBuildAddLibM(libtestdemo1  "" OFF)

#EBuildAddLibM(libset/libsetdemo1 ON "")      # todo
#EBuildAddLibM(libset/libsetdemo2 ON "")      # todo
#EBuildAddLibM(libset2/libsetdemo1 ON "")     # todo
#EBuildAddLibM(libset2/libsetdemo2 ON "")     # todo

# ------------------------------------
# -- 在这里添加 插件 kit
# EBuildAddPlgM(plg_name doc ON/OFF [LOGICAL_EXPR])
#
#EBuildAddPlgM(plgdemo1 ON "")                 # 一个独立的插件
#EBuildAddPlgM(plgdemo2 ON "")                # 一个独立的插件

#EBuildAddPlgM(plgdemo3 ON "")                # 依赖一个外部库，依赖于 extdemo1::Add，有自己的文件
#EBuildAddPlgM(plgdemo4 ON "")                # 依赖一个外部库，依赖于 extdemo1::Sub
#EBuildAddPlgM(plgdemo5 ON "")                # 依赖一个外部库，依赖于 extdemo1

#EBuildAddPlgM(plgdemo6 ON "")                # 依赖多个外部库，依赖于 extdemo1::Add extdemo2::Mul
#EBuildAddPlgM(plgdemo7 ON "")                # 依赖多个外部库，依赖于 extdemo1::Sub extdemo2::Div
#EBuildAddPlgM(plgdemo8 ON "")                # 依赖多个外部库，依赖于 extdemo1 extdemo2

#EBuildAddPlgM(plgdemo9 ON "")                # 依赖一个内部库，依赖于 libdemo1
#EBuildAddPlgM(plgdemo10 ON "")               # 依赖多个内部库，依赖于 libdemo1 libdemo2
#EBuildAddPlgM(plgdemo11 ON "")               # 依赖多层级内部库，依赖于 libdemo9

#EBuildAddPlgM(plgdemo12 ON "")               # 依赖一个插件，依赖于 plgdemo1
#EBuildAddPlgM(plgdemo13 ON "")               # 依赖多个插件，依赖于 plgdemo1 plgdemo2
#EBuildAddPlgM(plgdemo14 ON "")               # 依赖多层级插件，依赖于 plgdemo12

#EBuildAddPlgM(plgdemo15 ON "")               # 同时依赖外部库和内部库，依赖于 extdemo1 libdemo9
#EBuildAddPlgM(plgdemo16 ON "")               # 同时依赖外部库和插件，依赖于 extdemo1 plgdemo12
#EBuildAddPlgM(plgdemo17 ON "")               # 同时依赖内部库和插件，依赖于 libdemo9 plgdemo12

#EBuildAddPlgM(plgdemo18 ON "")               # 跨层级依赖，依赖于 libdemo5(->extdemo1)

#EBuildAddPlgM(plgdemo20 ON "")               # 依赖 libdemo3，包含自己的文件
#EBuildAddPlgM(plgdemo21 ON "")               # 依赖 libdemo3，指派 libdemo3 的存放位置，包含自己的文件

# ------------------------------------
# -- 在这里添加 应用 kit
# EBuildAddAppM(app_name doc ON/OFF [LOGICAL_EXPR])
#
#EBuildAddAppM(appdemo1 ON "")                # 一个独立的 app
#EBuildAddAppM(appdemo2 ON "")                # 一个独立的 app

#EBuildAddAppM(appdemo3 ON "")                # 依赖一个外部库，依赖于 extdemo1::Add
#EBuildAddAppM(appdemo4 ON "")                # 依赖一个外部库，依赖于 extdemo1::Sub
#EBuildAddAppM(appdemo5 ON "")                # 依赖一个外部库，依赖于 extdemo1

#EBuildAddAppM(appdemo6 ON "")                # 依赖多个外部库，依赖于 extdemo1::Add extdemo2::Mul
#EBuildAddAppM(appdemo7 ON "")                # 依赖多个外部库，依赖于 extdemo1::Sub extdemo2::Div
#EBuildAddAppM(appdemo8 ON "")                # 依赖多个外部库，依赖于 extdemo1 extdemo2

#EBuildAddAppM(appdemo9  ON "")               # 依赖一个内部库，依赖于 libdemo1
#EBuildAddAppM(appdemo10 ON "")               # 依赖多个内部库，依赖于 libdemo1 libdemo2
#EBuildAddAppM(appdemo11 ON "")               # 依赖多层级内部库，依赖于 libdemo9

#EBuildAddAppM(appdemo12 ON "")               # 依赖一个插件，依赖于 plgdemo1
#EBuildAddAppM(appdemo13 ON "")               # 依赖多个插件，依赖于 plgdemo1 plgdemo2
#EBuildAddAppM(appdemo14 ON "")               # 依赖多层级插件，依赖于 plgdemo12

#EBuildAddAppM(appdemo15 ON "")               # 依赖多个外部库和多个内部库 extdemo1 extdemo2 libdemo1 libdemo2
#EBuildAddAppM(appdemo16 ON "")               # 依赖多个外部库和多个插件   extdemo1 extdemo2 plgdemo1 plgdemo2
#EBuildAddAppM(appdemo17 ON "")               # 依赖多个内部库和多个插件   libdemo1 libdemo2 plgdemo1 plgdemo2

#EBuildAddAppM(appdemo18 ON "")               # 跨层级依赖，依赖于 plgdemo18(->libdemo5(->extdemo1))

#EBuildAddAppM(appdemo19 ON "")               # 同时依赖外部库、内部库、插件，依赖于 extdemo1 extdemo2 libdemo9 plgdemo1 plgdemo2

#EBuildAddAppM(appdemo20 ON "")               # 依赖 libdemo3 plgdemo3，自身包含文件
#EBuildAddAppM(appdemo21 ON "")               # 依赖 libdemo3 plgdemo3，并分别指派它们的位置，自身包含文件

# ------------------------------------
# -- 在这里添加 产品|版本 kit
# EBuildAddProductM(product_name[:type_name] doc ON/OFF [LOGICAL_EXPR])
#

#EBuildAddProdM(prod1          ON "")
#EBuildAddProdM(prod1:type1    ON "")              # 依赖 libdemo1
#EBuildAddProdM(prod1:type2    ON "")              # 依赖 libdemo2
#EBuildAddProdM(prod1:type3    ON "")              # 依赖 libdemo1 libdemo2，指派 libdemo2 的存放位置
#EBuildAddProdM(prod1:type4    ON "")              # 依赖 extdemo1 extdemo2，指派 extdemo2 的存放位置
#EBuildAddProdM(prod1:type5    ON "")              # 依赖 libdemo8
#EBuildAddProdM(prod1:type6    ON "")              # 依赖 libdemo8，指派 libdemo8 的存放位置
#EBuildAddProdM(prod1:type7    ON "")              # 依赖 plgdemo1 plgdemo2
#EBuildAddProdM(prod1:type8    ON "")              # 依赖 plgdemo1 plgdemo2，指派 plgdemo1 的存放位置
#EBuildAddProdM(prod1:type9    ON "")              # 依赖 plgdemo1 plgdemo2，指派 plgdemo1 和 plgdemo2 存放于不同位置
#EBuildAddProdM(prod1:type10   ON "")              # 依赖 plgdemo10(依赖于 libdemo1(static), libdemo2(shared))， plgdemo10 指派了 libdemo2 的存放位置
#EBuildAddProdM(prod1:type11   ON "")              # 依赖 appdemo1 appdemo2
#EBuildAddProdM(prod1:type12   ON "")              # 依赖 appdemo7 appdemo10，依赖多个app，且每个app 有自己依赖的静态库和动态库
#EBuildAddProdM(prod1:type13   ON "")              # 依赖 appdemo18(->plgdemo18(->libdemo5(->extdemo1)))
#EBuildAddProdM(prod1:type14   ON "")              # 依赖 appdemo19(依赖于 extdemo1 extdemo2 libdemo9 plgdemo1 plgdemo2)

#EBuildAddProdM(prod2:type1    ON "")              # 无依赖，自身包含 六个文件拷贝 和 2 个文件夹拷贝
#EBuildAddProdM(prod2:type2    ON "")              # 依赖 libdemo3(有自己的文件)，自身包含 六个文件拷贝 和 2 个文件夹拷贝
#EBuildAddProdM(prod2:type3    ON "")              # 依赖 libdemo3(有自己的文件)，指派 libdemo3 的存放位置，自身包含 六个文件拷贝 和 2 个文件夹拷贝
#EBuildAddProdM(prod2:type4    ON "")              # 依赖 libdemo3(有自己的文件) 和 plgdemo3(有自己的文件)，自身包含 六个文件拷贝 和 2 个文件夹拷贝
#EBuildAddProdM(prod2:type5    ON "")              # 依赖 plgdemo20(有自己的文件)，plgdemo20 依赖于 libdemo3，自身包含 六个文件拷贝 和 2 个文件夹拷贝
#EBuildAddProdM(prod2:type6    ON "")              # 依赖 plgdemo21(有自己的文件)，plgdemo21 依赖于 libdemo3，plgdemo21 指派 libdemo3 的存放位置，自身包含 六个文件拷贝 和 2 个文件夹拷贝
#EBuildAddProdM(prod2:type7    ON "")              # 依赖 plgdemo21(有自己的文件)，plgdemo21 依赖于 libdemo3，plgdemo21 指派 libdemo3 的存放位置，自身包含 六个文件拷贝 和 2 个文件夹拷贝，指派 plgdemo21 的存放位置
#EBuildAddProdM(prod2:type8    ON "")              # 依赖 appdemo20，appdemo20 依赖 libdemo3 plgdemo3
#EBuildAddProdM(prod2:type9    ON "")              # 依赖 appdemo21，appdemo21 依赖 libdemo3 plgdemo3 并分别指派了他们的存放位置
#EBuildAddProdM(prod2:type10   ON "")              # 依赖 appdemo21，appdemo21 依赖 libdemo3 plgdemo3 并分别指派了他们的存放位置，指派 appdemo21 的存放位置

#EBuildAddProdM(prod3:type1    ON "")              # 包含不存在的文件
#EBuildAddProdM(prod3:type2    ON "")              # 包含不存在的文件夹
#EBuildAddProdM(prod3:type3    ON "")              # 对同一个依赖指派了不同位置
#EBuildAddProdM(prod3:type4    ON "")              # 拷贝冲突

# ------------------------------------
# -- 生成所需 kit 构建规则
EBuildGenerateM()


