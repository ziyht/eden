# =====================================================================================
#
#       Filename:  EBuildExports.cmake
#
#    Description:  a instruction of some globel vars and APIs that can used by user
#                  directly
#
#        Version:  1.0
#        Created:  2018-08-24 23:40:34 PM
#       Revision:  none
#       Compiler:  cmake
#
#         Author:  Haitao Yang, joyhaitao@foxmail.com
#        Company:
#
return()
_(
# =====================================================================================

# =====================================
# -- step1 添加 kit
#

EBuildAddExtM(KitName ON|OFF "comment" [CREATE|DELETE] [DEUBG|Debug|D|d])     # 添加 外部库
EBuildAddLibM(KitName ON|OFF "comment" [CREATE|DELETE] [DEUBG|Debug|D|d])     # 添加 内部库
EBuildAddPlgM(KitName ON|OFF "comment" [CREATE|DELETE] [DEUBG|Debug|D|d])     # 添加 插件
EBuildAddAppM(KitName ON|OFF "comment" [CREATE|DELETE] [DEUBG|Debug|D|d])     # 添加 应用

EBuildAddProdM(Prod      ON|OFF "comment" [CREATE|DELETE] [DEUBG|Debug|D|d])  # 添加 产品
EBuildAddProdM(Prod:type ON|OFF "comment" [CREATE|DELETE] [DEUBG|Debug|D|d])  # 添加 版本

# =====================================
# -- step2 Generate
#
EBuildGenerateM()

)
