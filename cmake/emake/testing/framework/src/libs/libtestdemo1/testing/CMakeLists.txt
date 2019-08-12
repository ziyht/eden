cmake_minimum_required(VERSION 3.1)

#include(../../ParaCmakeTestModuleConfig.cmake)



#init TestModule
ParaAutoTestInitM()

#ParaAutoTestUnitTestOption(ON)
#ParaAutoTestMoudleTestOption(ON)

#add test module for PROJECT
ParaAutoTestAddProjectTestM(PROJECT ${PROJECT_NAME})

set(unitTestCases
    calcTest.cpp
    calcTest1.cpp
    )
#ParaAutoTestGetProjectTestListF(arg)
#message("CMakeList.txt -testing:${arg}")

#set(moduleTestCases
#    ParaLessModuleTest.cpp
#    )


#add testCase to UnitTest
ParaAutoTestAddUnitTestCases(PROJECT ${PROJECT_NAME} SRCS ${unitTestCases})
#add testCase to ModuleTest
#ParaPartAutoTestAddModuleTestCases(PROJECT ${PROJECT_NAME} SRCS ${moduleTestCases})

#build TestDirver
ParaAutoTestBuildTestDirver(PROJECT ${PROJECT_NAME})



#ParaAutoTestAddProjectTestM(PROJECT unittest1)

#ParaAutoTestGetProjectTestListF(arg)
#message("CMakeList.txt -testing:${arg}")
