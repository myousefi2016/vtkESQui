IF(VTK_TCL_INCLUDE_DIR)
  INCLUDE_DIRECTORIES("${PYTHON_INCLUDE_PATH}")
ENDIF(VTK_TCL_INCLUDE_DIR)

VTK_WRAP_PYTHON3(${PROJECT_NAME}${KIT}Python KitPYTHON_SRCS "${Kit_SRCS}")
ADD_LIBRARY(${PROJECT_NAME}${KIT}PythonD ${KitPYTHON_SRCS})
ADD_LIBRARY(${PROJECT_NAME}${KIT}Python MODULE ${PROJECT_NAME}${KIT}PythonInit.cxx)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}${KIT}PythonD ${PROJECT_NAME}${KIT})
FOREACH(c ${Kit_LIBS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME}${KIT}PythonD ${c}PythonD)
ENDFOREACH(c)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}${KIT}Python ${PROJECT_NAME}${KIT}PythonD)
